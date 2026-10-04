/*
 * app_game.ino
 * App "Minijuego" Rompe-ladrillos (Control 100% Táctil).
 */
#include "config.h"
#include "app.h"

#define HUD_H       32
#define BR_COLS     8
#define BR_ROWS     5
#define BR_W        60
#define BR_H        16
#define BR_Y0       40
#define BALL        6
#define PAD_Y       292
#define PAD_H       8
#define PAD_W       70
#define MENU_BTN_X  380

#define EB_Y        180
#define EB1_X       100
#define EB2_X       260
#define EB_W        120
#define EB_H        50

static const uint16_t brickColor[BR_ROWS] = {COL_RED, COL_ORANGE, COL_YELLOW, COL_GREEN, COL_CYAN};

static uint8_t  bricks[BR_ROWS];
static uint8_t  bricksLeft;
static int16_t  bx, by, vx, vy;
static int16_t  padX;
static uint8_t  lives;
static uint16_t br_score;
static bool     br_gameOver = false;
static bool     hudDirty = false;
static unsigned long br_lastFrame = 0;

static void drawBrick(uint8_t r, uint8_t c, bool on) {
  int16_t x = c * BR_W;
  int16_t y = BR_Y0 + r * BR_H;
  if (on) tft.fillRect(x + 1, y + 1, BR_W - 2, BR_H - 2, brickColor[r]);
  else    tft.fillRect(x, y, BR_W, BR_H, COL_BLACK);
}

static void restoreBricks(int16_t x, int16_t y) {
  int16_t zoneBottom = BR_Y0 + BR_ROWS * BR_H;
  if (y + BALL <= BR_Y0 || y >= zoneBottom) return;
  int16_t ya = (y < BR_Y0) ? BR_Y0 : y;
  int16_t yb = (y + BALL - 1 >= zoneBottom) ? zoneBottom - 1 : y + BALL - 1;
  int16_t xa = (x < 0) ? 0 : x;
  int16_t xb = (x + BALL - 1 >= SCR_W) ? SCR_W - 1 : x + BALL - 1;
  for (uint8_t r = (ya - BR_Y0) / BR_H; r <= (uint8_t)((yb - BR_Y0) / BR_H); r++)
    for (uint8_t c = xa / BR_W; c <= (uint8_t)(xb / BR_W); c++)
      if (bricks[r] & (1 << c)) drawBrick(r, c, true);
}

static void br_drawHud() {
  tft.fillRect(0, 0, MENU_BTN_X - 4, HUD_H, COL_BLACK);
  char buf[32];
  sprintf(buf, "Pts: %u   Vid: %u", br_score, lives);
  tft.setFont(FONT_SMALL);
  tft.setTextColor(COL_WHITE);
  tft.setCursor(10, 22);
  tft.print(buf);
  tft.setFont(NULL);
}

static void movePaddle(int16_t nx) {
  if (nx < 0) nx = 0;
  if (nx > SCR_W - PAD_W) nx = SCR_W - PAD_W;
  if (nx == padX) return;
  if (nx > padX) tft.fillRect(padX, PAD_Y, nx - padX, PAD_H, COL_BLACK);
  else           tft.fillRect(nx + PAD_W, PAD_Y, padX - nx, PAD_H, COL_BLACK);
  padX = nx;
  tft.fillRect(padX, PAD_Y, PAD_W, PAD_H, COL_CYAN);
}

static void resetBall() {
  bx = padX + PAD_W / 2 - BALL / 2;
  by = PAD_Y - BALL - 1;
  vx = random(2) ? 3 : -3;
  vy = -4;
  tft.fillRect(bx, by, BALL, BALL, COL_WHITE);
  delay(700);
}

static void br_endGame(bool win) {
  br_gameOver = true;
  tft.fillRect(70, 40, 340, 220, COL_DISPLAY);
  tft.drawRect(70, 40, 340, 220, COL_WHITE);
  printCenteredF(win ? "GANASTE!" : "FIN DEL JUEGO", SCR_W / 2, 85, FONT_TITLE, win ? COL_GREEN : COL_RED);
  char t[24];
  sprintf(t, "Puntos: %u", br_score);
  printCenteredF(t, SCR_W / 2, 135, FONT_SUB, COL_WHITE);
  drawButtonF(EB1_X, EB_Y, EB_W, EB_H, "Otra vez", FONT_LABEL, COL_GREEN, COL_BLACK);
  drawButtonF(EB2_X, EB_Y, EB_W, EB_H, "Menu", FONT_LABEL, COL_BLUE, COL_WHITE);
}

void gameInit() {
  br_gameOver = false;
  br_score = 0;
  lives = 3;
  bricksLeft = BR_ROWS * BR_COLS;
  tft.fillScreen(COL_BLACK);
  for (uint8_t r = 0; r < BR_ROWS; r++) {
    bricks[r] = 0xFF;
    for (uint8_t c = 0; c < BR_COLS; c++) drawBrick(r, c, true);
  }
  drawButtonF(MENU_BTN_X, 2, SCR_W - MENU_BTN_X - 4, HUD_H - 4, "MENU", FONT_SMALL, COL_NAVY, COL_WHITE);
  br_drawHud();
  padX = (SCR_W - PAD_W) / 2;
  tft.fillRect(padX, PAD_Y, PAD_W, PAD_H, COL_CYAN);
  resetBall();
  br_lastFrame = millis();
}

void gameUpdate() {
  if (br_gameOver) {
    int16_t x, y;
    if (touchPressed(x, y)) {
      if (inRect(x, y, EB1_X, EB_Y, EB_W, EB_H)) gameInit();
      else if (inRect(x, y, EB2_X, EB_Y, EB_W, EB_H)) switchApp(APP_MENU);
    }
    return;
  }

  if (millis() - br_lastFrame < 20) return; // ~ 50 FPS
  br_lastFrame = millis();

  int16_t tx, ty;
  bool touched = readTouch(tx, ty);

  if (touched && ty < HUD_H && tx >= MENU_BTN_X) { switchApp(APP_MENU); return; }

  if (touched) {
    movePaddle(tx - PAD_W / 2);
  }

  tft.fillRect(bx, by, BALL, BALL, COL_BLACK);
  restoreBricks(bx, by);

  bx += vx;
  by += vy;

  if (bx <= 0)                 { bx = 0;              vx = abs(vx); }
  else if (bx >= SCR_W - BALL) { bx = SCR_W - BALL;   vx = -abs(vx); }
  if (by <= HUD_H + 2)         { by = HUD_H + 2;      vy = abs(vy); }

  int16_t cx = bx + BALL / 2;
  int16_t cy = by + BALL / 2;
  if (cy >= BR_Y0 && cy < BR_Y0 + BR_ROWS * BR_H) {
    uint8_t r = (cy - BR_Y0) / BR_H;
    uint8_t c = cx / BR_W;
    if (c < BR_COLS && (bricks[r] & (1 << c))) {
      bricks[r] &= ~(1 << c);
      drawBrick(r, c, false);
      bricksLeft--;
      br_score += 10;
      vy = -vy;
      hudDirty = true;
    }
  }

  if (vy > 0 && by + BALL >= PAD_Y && by + BALL <= PAD_Y + PAD_H + vy &&
      bx + BALL >= padX && bx <= padX + PAD_W) {
    int16_t sp = 4 + br_score / 120;
    if (sp > 7) sp = 7;
    by = PAD_Y - BALL;
    vy = -sp;
    vx = ((bx + BALL / 2) - (padX + PAD_W / 2)) / 7;
    if (vx > 5) vx = 5;
    if (vx < -5) vx = -5;
    if (vx == 0) vx = random(2) ? 1 : -1;
  }

  if (by >= SCR_H - BALL) {
    lives--;
    hudDirty = false;
    br_drawHud();
    if (lives == 0) { br_endGame(false); return; }
    resetBall();
    return;
  }

  tft.fillRect(bx, by, BALL, BALL, COL_WHITE);

  if (hudDirty) { br_drawHud(); hudDirty = false; }
  if (bricksLeft == 0) br_endGame(true);
}