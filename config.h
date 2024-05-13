#pragma once

// Theming  
#define MAIN_COL (LfColor){182, 211, 224, 255}
#define MAIN_TEXT_COL LF_BLACK 
#define EXPR_TEXT_COL (LfColor){100, 100, 100, 255} 
#define PANEL_COLOR lf_color_brightness(MAIN_COL, 1.5f)
#define BTN_COLOR lf_color_brightness(MAIN_COL, 1.6f)
#define BTN_PADDING 20.0f

// Aspects
#define WIN_INIT_W 400
#define WIN_INIT_H 600

#define PANEL_H s.winh / 4.0f 

// Buffers
#define EXPR_BUF_SIZE 64
#define HIST_MAX 512 

// Font declarationpercisio
#define FONT "RobotoMono-Bold"
#define FONT_DIR "/usr/share/fonts/TTF/"

// Feature flags 
#define AUTO_CLIPBOARD_RESULT true // Automatically copies the last result to the clipboard

// Results 
#define RESULT_PRECISION "%.3f"

// Icons of the operation buttons (in order)
static const char* buttonicons[20] = {
  "%", "C",  "CE", "/",
  "7", "8",  "9",  "*",
  "4", "5",  "6",  "-",
  "1", "2",  "3",  "+",
  " ", "0",  ".",  "="
};

// Enumarion of actions of buttons (in order)
typedef enum {
  OP_PERCENT, OP_CLEAR, OP_CLEAR_ENTRIES, OP_DIVIDE,
  OP_7,       OP_8,     OP_9,             OP_MUTIPLY,
  OP_4,       OP_5,     OP_6,             OP_MINUS,
  OP_1,       OP_2,     OP_3,             OP_PLUS,
  OP_NONE,    OP_0,     OP_DOT,           OP_EQUALS
} btn_operation;

// Mapping from operations to shortcut keys (chars)
typedef enum {
  PLUS = '+',
  MINUS = '-',
  MULTIPLY = '*',
  DIVIDE = '/',
  DOT = '.',
} operation_shortcut;

// Key Strokes (Shortcuts)
#define SHORTCUT_EVAL           GLFW_KEY_ENTER
#define SHORTCUT_CLEAR          GLFW_KEY_B
#define SHORTCUT_CLEAR_ENTRIES  GLFW_KEY_S
#define SHORTCUT_COPY_RESULT    GLFW_KEY_C
#define SHORTCUT_PERCENT        GLFW_KEY_P
#define SHORTCUT_HIST_UP        GLFW_KEY_UP
#define SHORTCUT_HIST_DOWN      GLFW_KEY_DOWN
