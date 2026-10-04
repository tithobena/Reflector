/*
 * app_snake.ino
 * App "Snake" (La Viborita).
 */
#include "config.h"
#include "app.h"

#define BOARD_X    10
#define BOARD_Y    35
#define CELL_S     14
#define COLS       22  // Ancho tablero: 308 px
#define ROWS       19  // Alto tablero:  266 px

#define MAX_SNAKE  200

// Botones de control táctil (derecha)
#define BTN_UP_X   390
#define BTN_UP_Y   110
#define BTN_DN_X   390
#define BTN_DN_Y   230
#define BTN_LT_X   335
#define BTN_LT_Y   170
#define BTN_RT_X   445
#define BTN_RT_Y   170
#define BTN_SIZE   45

static int8_t   snk_x[MAX_SNAKE];
static int8_t   snk_y[MAX_SNAKE];
static uint8_t  snk_len;
static int8_t   snk_dirX, snk_dirY;
static int8_t   snk_nextX, snk_nextY;
static int8_t   snk_foodX, snk_foodY;
static uint16_t snk_score;
static bool     snk_gameOver;
static unsigned long snk_lastFrame;

static void spawnFood() {
  bool valid = false;
  while (!valid) {
    snk_foodX = random(0, COLS);
    snk_foodY = random(0, ROWS);
    valid = true;
    for (uint8_t i = 0; i < snk_len; i++) {
      if (snk_x[i] == snk_foodX && snk_y[i] == snk_foodY) {
        valid = false;
        break;
      }
    }
  }
  tft.fillRect(BOARD_X + snk_foodX * CELL_S + 1, BOARD_Y + snk_foodY * CELL_S + 1, CELL_S - 2, CELL_S - 2, COL_RED);
}

static void drawSnakeControls() {
  // Marco del tablero
  tft.drawRect(BOARD_X - 1, BOARD_Y - 1, COLS * CELL_S + 2, ROWS * CELL_S + 2, COL_WHITE);

  // HUD
  drawButtonF(380, 4, 95, 28, "MENU", FONT_SMALL, COL_NAVY, COL_WHITE);
  
  char buf[16];
  sprintf(buf, "Pts: %u", snk_score);
  tft.setFont(FONT_SMALL);
  tft.setTextColor(COL_YELLOW);
  tft.setCursor(335, 60);
  tft.print(buf);
  tft.setFont(NULL);

  // D-Pad táctil
  drawButtonF(BTN_UP_X, BTN_UP_Y, BTN_SIZE, BTN_SIZE, "^", FONT_LABEL, COL_DKGRAY, COL_WHITE);
  drawButtonF(BTN_DN_X, BTN_DN_Y, BTN_SIZE, BTN_SIZE, "v", FONT_LABEL, COL_DKGRAY, COL_WHITE);
  drawButtonF(BTN_LT_X, BTN_LT_Y, BTN_SIZE, BTN_SIZE, "<", FONT_LABEL, COL_DKGRAY, COL_WHITE);
  drawButtonF(BTN_RT_X, BTN_RT_Y, BTN_SIZE, BTN_SIZE, ">", FONT_LABEL, COL_DKGRAY, COL_WHITE);
}

void snakeInit() {
  snk_score = 0;
  snk_gameOver = false;
  snk_len = 4;

  snk_dirX = 1;  snk_dirY = 0;
  snk_nextX = 1; snk_nextY = 0;

  for (uint8_t i = 0; i < snk_len; i++) {
    snk_x[i] = 10 - i;
    snk_y[i] = 10;
  }

  tft.fillScreen(COL_BLACK);
  drawSnakeControls();

  // Dibujar cuerpo inicial
  for (uint8_t i = 0; i < snk_len; i++) {
    tft.fillRect(BOARD_X + snk_x[i] * CELL_S + 1, BOARD_Y + snk_y[i] * CELL_S + 1, CELL_S - 2, CELL_S - 2, COL_GREEN);
  }

  spawnFood();
  snk_lastFrame = millis();
}

void snakeUpdate() {
  int16_t tx, ty;
  if (touchPressed(tx, ty)) {
    if (inRect(tx, ty, 380, 4, 95, 28)) { switchApp(APP_MENU); return; }

    if (snk_gameOver) {
      snakeInit();
      return;
    }

    // Controles direccionales
    if (inRect(tx, ty, BTN_UP_X, BTN_UP_Y, BTN_SIZE, BTN_SIZE) && snk_dirY == 0) { snk_nextX = 0;  snk_nextY = -1; }
    if (inRect(tx, ty, BTN_DN_X, BTN_DN_Y, BTN_SIZE, BTN_SIZE) && snk_dirY == 0) { snk_nextX = 0;  snk_nextY = 1;  }
    if (inRect(tx, ty, BTN_LT_X, BTN_LT_Y, BTN_SIZE, BTN_SIZE) && snk_dirX == 0) { snk_nextX = -1; snk_nextY = 0;  }
    if (inRect(tx, ty, BTN_RT_X, BTN_RT_Y, BTN_SIZE, BTN_SIZE) && snk_dirX == 0) { snk_nextX = 1;  snk_nextY = 0;  }
  }

  if (snk_gameOver) return;

  if (millis() - snk_lastFrame < 130) return; // Velocidad del juego
  snk_lastFrame = millis();

  snk_dirX = snk_nextX;
  snk_dirY = snk_nextY;

  int8_t headX = snk_x[0] + snk_dirX;
  int8_t headY = snk_y[0] + snk_dirY;

  // Colisión con paredes
  if (headX < 0 || headX >= COLS || headY < 0 || headY >= ROWS) {
    snk_gameOver = true;
    printCenteredF("FIN DEL JUEGO", BOARD_X + (COLS * CELL_S) / 2, BOARD_Y + (ROWS * CELL_S) / 2, FONT_SUB, COL_RED);
    return;
  }

  // Colisión con sí misma
  for (uint8_t i = 0; i < snk_len; i++) {
    if (snk_x[i] == headX && snk_y[i] == headY) {
      snk_gameOver = true;
      printCenteredF("FIN DEL JUEGO", BOARD_X + (COLS * CELL_S) / 2, BOARD_Y + (ROWS * CELL_S) / 2, FONT_SUB, COL_RED);
      return;
    }
  }

  // Comer manzana
  if (headX == snk_foodX && headY == snk_foodY) {
    if (snk_len < MAX_SNAKE) snk_len++;
    snk_score += 10;
    
    // Actualizar Score HUD
    tft.fillRect(335, 45, 140, 25, COL_BLACK);
    char buf[16];
    sprintf(buf, "Pts: %u", snk_score);
    tft.setFont(FONT_SMALL);
    tft.setTextColor(COL_YELLOW);
    tft.setCursor(335, 60);
    tft.print(buf);
    tft.setFont(NULL);

    spawnFood();
  } else {
    // Borrar cola
    int8_t tailX = snk_x[snk_len - 1];
    int8_t tailY = snk_y[snk_len - 1];
    tft.fillRect(BOARD_X + tailX * CELL_S + 1, BOARD_Y + tailY * CELL_S + 1, CELL_S - 2, CELL_S - 2, COL_BLACK);
  }

  // Desplazar cuerpo
  for (int16_t i = snk_len - 1; i > 0; i--) {
    snk_x[i] = snk_x[i - 1];
    snk_y[i] = snk_y[i - 1];
  }
  snk_x[0] = headX;
  snk_y[0] = headY;

  // Dibujar cabeza
  tft.fillRect(BOARD_X + headX * CELL_S + 1, BOARD_Y + headY * CELL_S + 1, CELL_S - 2, CELL_S - 2, COL_GREEN);
}