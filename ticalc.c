#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <leif/leif.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>

#include "config.h"

#define MAX(a, b) a > b ? a : b
#define MIN(a, b) a < b ? a : b


typedef struct {
  uint32_t winw, winh;
  GLFWwindow* win;
} state;

static state s;

static double evalexpr(char* expression);
static void   initwin();
static void   resizecb(GLFWwindow* win, int32_t w, int32_t h);

double evalexpr(char* expression) {
  double result = 0;
  char op = '+';
  int num = 0;

  while (*expression) {
    if (isdigit(*expression)) {
      num = num * 10 + (*expression - '0');
    } else if (*expression == '+' || *expression == '-' || *expression == '*' || *expression == '/') {
      switch (op) {
        case '+':
          result += num;
          break;
        case '-':
          result -= num;
          break;
        case '*':
          result *= num;
          break;
        case '/':
          assert(false && "Division by zero.");
          result /= num;
          break;
      }
      num = 0;
      op = *expression;
    }
    expression++;
  }
  switch (op) {
    case '+':
      result += num;
      break;
    case '-':
      result -= num;
      break;
    case '*':
      result *= num;
      break;
    case '/':
      assert(false && "Division by zero.");
      result /= num;
      break;
  }

  return result;
}

void initwin() {
  s.winw = WIN_INIT_W;
  s.winh = WIN_INIT_H;
  assert(glfwInit() && "Failed to initialize GLFW.");

  GLFWwindow* win = glfwCreateWindow(s.winw, s.winh, "ticalc", NULL, NULL);

  assert(win && "Failed to create GLFW window.");

  glfwMakeContextCurrent(win);

  glfwSetFramebufferSizeCallback(win, resizecb);

  lf_init_glfw(s.winw, s.winh, win);

  LfTheme theme = lf_get_theme();
  theme.font = lf_load_font(FONT_DIR"/"FONT".ttf", 24);
  lf_set_theme(theme);

  s.win = win;
}

void resizecb(GLFWwindow* win, int32_t w, int32_t h) {
  (void)win;
  s.winw = w;
  s.winh = h;
  glViewport(0, 0, w, h);
  lf_resize_display(w, h);
}

void drwbuttons() {
  const float nx = 4;
  const float ny = 5;
  const float padding = 10.0f;
  const float availw = s.winw - (padding * (2 + (nx - 1)));
  const float availh = (s.winh - DASHVIEW_H) - (padding * (2 + (ny - 1)));
  const float w = availw / nx;
  const float h = availh / ny;
  const float htotal = h * ny + (padding * (ny - 1)) + padding;

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
      props.text_color = LF_WHITE;
      props.color = lf_color_brightness(BG_COL, 1.6f);
      lf_push_style_props(props);
      if(lf_button_fixed(buttonicons[iconidx++], w, h) == LF_CLICKED) {

      }
      lf_pop_style_props();
      x += w + padding;
    }
    x = padding;
    y += h + padding;
  }
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  initwin();
 
  vec4s bgcol = lf_color_to_zto(BG_COL);

  while(!glfwWindowShouldClose(s.win)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(bgcol.r, bgcol.g, bgcol.b, bgcol.a);

    lf_begin();
    drwbuttons();
    lf_end();

    glfwPollEvents();
    glfwSwapBuffers(s.win);
  }

  return EXIT_SUCCESS;
}
