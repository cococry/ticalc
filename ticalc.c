#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <leif/leif.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "config.h"
#include "eval.h"

#define MAX(a, b) a > b ? a : b
#define MIN(a, b) a < b ? a : b

typedef struct {
  uint32_t winw, winh;
  GLFWwindow* win;

  LfFont panelfont;
  
  LfInputField input;
  char inputbuf[EXPR_BUF_SIZE];

  const char* hist[HIST_MAX];
  double results[HIST_MAX];
  uint32_t histsize;
  int32_t histidx;

  LfDiv* panel;
  float panelscroll, 
  panelscroll_vel, 
  panelscroll_max; 
} state;

static state s;

static void   initwin();
static void   terminate();

static void   resizecb(GLFWwindow* win, int32_t w, int32_t h);
static void   inputinsertcb(void* input_data);

static void   drwbuttons();
static void   drwpanel();

static bool   isshortcut(uint32_t charcode);
static char   shortcuttochar(operation_shortcut shortcut);

static void   handlekeys();

static void   execop(btn_operation op);

static void   clearinput();
static void   clearentries();
static void   evalinput();
static void   evalpercent();

static void   addinput(char c);

static void   histup();
static void   histdown();

static void   clipboardresult();

void initwin() {
  memset(&s, 0, sizeof(s));
  s.winw = WIN_INIT_W;
  s.winh = WIN_INIT_H;
  assert(glfwInit() && "Failed to initialize GLFW.");

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  s.win = glfwCreateWindow(s.winw, s.winh, "ticalc", NULL, NULL);

  assert(s.win && "Failed to create GLFW window.");

  glfwMakeContextCurrent(s.win);

  glfwSetFramebufferSizeCallback(s.win, resizecb);

  lf_init_glfw(s.winw, s.winh, s.win);

  LfTheme theme = lf_get_theme();
  theme.font = lf_load_font(FONT_DIR"/"FONT".ttf", 24);
  theme.scrollbar_props.color = lf_color_brightness(MAIN_COL, 4.0f);
  theme.scrollbar_props.corner_radius = 2.0f;
  lf_set_theme(theme);

  s.panelfont = lf_load_font(FONT_DIR"/"FONT".ttf", 48);

  memset(s.inputbuf, 0, sizeof(s.inputbuf)); 
  s.input = (LfInputField){
    .buf = s.inputbuf,
    .width = s.winw - 60.0f,
    .buf_size = EXPR_BUF_SIZE,
    .max_chars = 30, 
    .placeholder = "",
    .selected = true,
    .insert_override_callback = inputinsertcb
  };
  s.win = s.win;
}

void terminate() {
  glfwDestroyWindow(s.win);
  glfwTerminate();

  for(uint32_t i = 0; i < s.histsize; i++) {
    if(s.hist[i])
      free((void*)s.hist[i]);
  }
}

void resizecb(GLFWwindow* win, int32_t w, int32_t h) {
  (void)win;
  s.winw = w;
  s.winh = h;
  glViewport(0, 0, w, h);
  lf_resize_display(w, h);
}

void drwpanel() {
  s.input.width = s.winw - 60.0f;
  s.input.selected = true;
  {
    LfUIElementProps props = lf_get_theme().div_props;
    props.color = PANEL_COLOR;
    props.corner_radius = 8.0f;
    lf_push_style_props(props);
    s.panel = lf_div_begin_ex(((vec2s){BTN_PADDING, BTN_PADDING}), ((vec2s){s.winw - (BTN_PADDING * 2.0f), PANEL_H - BTN_PADDING}), true,
                    &s.panelscroll, &s.panelscroll_vel);
    lf_pop_style_props();
  }

  for(uint32_t i = 0; i < s.histsize; i++) {

    LfUIElementProps props = lf_get_theme().text_props;
    // Expression
    {
      props.text_color = EXPR_TEXT_COL;
      props.margin_left = 10.0f;
      lf_push_style_props(props);
      lf_text(s.hist[i]);
      lf_pop_style_props();
    }

    lf_next_line();
    // Result
    {
      char* format = "= ";
      char resultstr[EXPR_BUF_SIZE + strlen(format)];
      sprintf(resultstr, "%s"RESULT_PRECISION, format, s.results[i]);
      props.text_color = MAIN_TEXT_COL;
      lf_push_style_props(props);
      lf_text(resultstr);
      lf_pop_style_props();
    }
    lf_next_line();
  }
  {
    LfUIElementProps props = lf_get_theme().inputfield_props;
    props.text_color = MAIN_TEXT_COL;
    props.color = LF_NO_COLOR;
    props.border_width = 0.0f;
    lf_push_style_props(props),
    lf_push_font(&s.panelfont);
    lf_input_text(&s.input);
    lf_pop_style_props();
    lf_pop_font();
  }

  lf_next_line();

  s.panel->total_area.x = lf_get_ptr_x();
  s.panel->total_area.y = lf_get_ptr_y();

  float area_y = s.panel->total_area.y - s.panelscroll;
  float size_y = s.panel->aabb.size.y; 
  float maxscroll = ((area_y - size_y) * -1.0f) - s.panelfont.font_size;
  s.panelscroll_max = maxscroll;

  lf_div_end();


}
void drwbuttons() {
  const float nx = 4;
  const float ny = 5;
  const float padding  = BTN_PADDING; 
  const float availw = s.winw - (padding * (nx + 1));
  const float availh = (s.winh - PANEL_H) - (padding * (nx + 2));
  const float w = availw / nx;
  const float h = availh / ny;
  const float htotal = (s.winh - PANEL_H) - padding;

  float x = padding; 
  float y = s.winh - htotal;
  
  uint32_t iconidx = 0;

  for(uint32_t i = 0; i < ny; i++) {
    for(uint32_t j = 0; j < nx; j++) {
      lf_set_ptr_x_absolute(x);
      lf_set_ptr_y_absolute(y);
      LfUIElementProps props = lf_get_theme().button_props;
      props.padding = 0.0f;
      props.margin_left = 0.0f;
      props.margin_right = 0.0f;
      props.margin_top = 0.0f;
      props.margin_bottom = 0.0f;
      props.border_width = 0.0f;
      props.corner_radius = 5.0f;
      props.text_color = MAIN_TEXT_COL;
      props.color = BTN_COLOR;
      lf_push_style_props(props);
      if(lf_button_fixed(buttonicons[iconidx], w, h) == LF_CLICKED) {
        execop((btn_operation)iconidx);
      }
      lf_pop_style_props();
      x += w + padding;
      iconidx++;
    }
    x = padding;
    y += h + padding;
  }
}

void inputinsertcb(void* input_data) {
  LfInputField* input = (LfInputField*)input_data;
  if(isdigit(lf_char_event().charcode)) {
    lf_input_insert_char_idx(input, lf_char_event().charcode, input->cursor_index++);
  } else if(isshortcut(lf_char_event().charcode)) {
    lf_input_insert_char_idx(input, shortcuttochar(lf_char_event().charcode), input->cursor_index++);
  }
}

bool isshortcut(uint32_t charcode) {
  return (
  charcode == PLUS || 
  charcode == MINUS || 
  charcode == MULTIPLY ||
  charcode == DIVIDE || 
  charcode == DOT 
  );
}
static char shortcuttochar(operation_shortcut shortcut) {
  switch (shortcut) {
    case PLUS: return '+'; 
    case MINUS: return '-'; 
    case MULTIPLY: return '*'; 
    case DIVIDE: return '/'; 
    case DOT: return '.';
  }
}

void handlekeys() {
  if(lf_key_is_down(GLFW_KEY_LEFT_CONTROL)) return;
  if(lf_key_went_down(SHORTCUT_EVAL)) {
    evalinput();
  }
  if(lf_key_went_down(SHORTCUT_CLEAR)) {
    clearinput();
  }
  if(lf_key_went_down(SHORTCUT_CLEAR_ENTRIES)) {
    clearentries();
  }
  if(lf_key_went_down(SHORTCUT_PERCENT)) {
    evalpercent();
  }
  if(lf_key_went_down(SHORTCUT_HIST_UP)) {
    histdown(); // Down is up
  }
  if(lf_key_went_down(SHORTCUT_HIST_DOWN)) {
    histup(); // Up is down
  }
  if(lf_key_went_down(SHORTCUT_COPY_RESULT)) {
    clipboardresult();
  }
}

void execop(btn_operation op) {
  switch(op) {
    case OP_0:              { addinput('0');  break; }
    case OP_1:              { addinput('1');  break; }
    case OP_2:              { addinput('2');  break; }
    case OP_3:              { addinput('3');  break; }
    case OP_4:              { addinput('4');  break; }
    case OP_5:              { addinput('5');  break; }
    case OP_6:              { addinput('6');  break; }
    case OP_7:              { addinput('7');  break; }
    case OP_8:              { addinput('8');  break; }
    case OP_9:              { addinput('9');  break; }
    case OP_PLUS:           { addinput('+');  break; }
    case OP_MINUS:          { addinput('-');  break; }
    case OP_MUTIPLY:        { addinput('*');  break; }
    case OP_DIVIDE:         { addinput('/');  break; }
    case OP_DOT:            { addinput('.');  break; }
    case OP_CLEAR:          { clearinput();   break; }
    case OP_CLEAR_ENTRIES:  { clearentries(); break; }
    case OP_EQUALS:         { evalinput();    break; }
    case OP_PERCENT:        { evalpercent();  break; }
    default:                {                 break; }
  }
}

void clearinput() {
  lf_input_field_unselect_all(&s.input);
  s.input.cursor_index = 0;
  memset(s.input.buf, 0, EXPR_BUF_SIZE);
}
void clearentries() {
  memset(s.hist, 0, sizeof(s.hist));
  memset(s.results, 0, sizeof(s.results));
  s.histsize = 0;
  s.panelscroll = 0.0f;
}
void evalinput() {
  if(!strlen(s.inputbuf)) return;
  double result;
  {
    uint32_t first;
    for(first = 0; first < strlen(s.input.buf); first++) {
      if(!isspace(s.input.buf[first])) break;
    }
    char* format = s.input.buf[first] == '/' ? "1" : "0";
    char expr[EXPR_BUF_SIZE + strlen(format)];
    sprintf(expr, "%s%s", format, s.input.buf);
    result = evalexpr(expr);
  }
  if(s.histsize + 1 <= HIST_MAX)
  {
    char *buf = s.inputbuf; 
    char *histentry; 
    histentry = (char *)malloc(strlen(buf) + 1); 
    strcpy(histentry, buf);
    s.hist[s.histsize] = histentry;
    s.results[s.histsize] = result;
    s.histsize++;
    s.histidx = s.histsize;
  }

  clearinput();

  s.panelscroll = s.panelscroll_max;
  if(AUTO_CLIPBOARD_RESULT) {
    clipboardresult();
  }
}

void evalpercent() {
  char* format = "/100";
  strcat(s.input.buf, format);
  evalinput();
}

void addinput(char c) {
  lf_input_insert_char_idx(&s.input, c, s.input.cursor_index++); 
  s.panelscroll = s.panelscroll_max + s.panelfont.font_size;
}

void histup() {
  if(s.histidx + 1 >= s.histsize) return;
  clearinput();
  s.histidx++;
  strcpy(s.input.buf, s.hist[s.histidx]);
  s.input.cursor_index = strlen(s.input.buf);
}

void histdown() {
  if(s.histidx - 1 < 0) return;
  clearinput();
  s.histidx--;
  strcpy(s.input.buf, s.hist[s.histidx]);
  s.input.cursor_index = strlen(s.input.buf);
}

void clipboardresult() {
  if(s.histsize < 1) return;
  char buf[EXPR_BUF_SIZE];
  sprintf(buf, RESULT_PRECISION, s.results[s.histsize - 1]);
  lf_set_clipboard_text(buf);
  printf("Copied last result to clipboard.\n");
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  initwin();
 
  vec4s bgcol = lf_color_to_zto(MAIN_COL);

  while(!glfwWindowShouldClose(s.win)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(bgcol.r, bgcol.g, bgcol.b, bgcol.a);

    lf_begin();
    drwpanel();
    drwbuttons();
    lf_end();

    handlekeys();

    glfwPollEvents();
    glfwSwapBuffers(s.win);
  }

  terminate();
  return EXIT_SUCCESS;
}
