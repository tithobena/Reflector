/*
 * app_flappy.ino
 * App "Flappy Bird" (Renderizado fluido e incremental sin parpadeos).
 */
#include "config.h"
#include "app.h"

#define HUD_H       32
#define MENU_BTN_X  380
#define BIRD_X      90
#define BIRD_R      10
#define PIPE_W      55
#define GAP_H       95

#define EB_Y        180
#define EB1_X       100
#define EB2_X       260
#define EB_W        120
#define EB_H        50

static float    birdY, birdVy;
static int16_t  pipeX, gapY;
static uint16_t flappy_score;
static bool     flappy_gameOver = false;
static bool     passedPipe = false;
static unsigned long flappy_lastFrame = 0;

static void flappy_drawHud() {
  tft.fillRect(0, 0, MENU_BTN_X - 4, HUD_H, COL_BLACK);
  char buf[20];
  sprintf(buf, "Puntos: %u", flappy_score);
  tft.setFont(FONT_SMALL);
  tft.setTextColor(COL_WHITE);
  tft.setCursor(15, 22);
  tft.print(buf);
  tft.setFont(NULL);
}

static void flappy_endGame(bool hitGround) {
  flappy_gameOver = true;
  tft.fillRect(70, 40, 340, 220, COL_DISPLAY);
  tft.drawRect(70, 40, 340, 220, COL_WHITE);
  printCenteredF("¡GAME OVER!", SCR_W / 2, 85, FONT_TITLE, COL_RED);

  char t[24];
  sprintf(t, "Puntos: %u", flappy_score);
  printCenteredF(t, SCR_W / 2, 135, FONT_SUB, COL_WHITE);

  drawButtonF(EB1_X, EB_Y, EB_W, EB_H, "Otra vez", FONT_LABEL, COL_GREEN, COL_BLACK);
  drawButtonF(EB2_X, EB_Y, EB_W, EB_H, "Menu", FONT_LABEL, COL_BLUE, COL_WHITE);
}

void flappyInit() {
  flappy_gameOver = false;
  flappy_score = 0;
  birdY = SCR_H / 2;
  birdVy = -5.0;
  pipeX = SCR_W;
  gapY = random(HUD_H + 30, SCR_H - GAP_H - 30);
  passedPipe = false;

  tft.fillScreen(COL_BLACK);
  drawButtonF(MENU_BTN_X, 2, SCR_W - MENU_BTN_X - 4, HUD_H - 4, "MENU", FONT_SMALL, COL_NAVY, COL_WHITE);
  flappy_drawHud();
  flappy_lastFrame = millis();
}

void flappyUpdate() {
  if (flappy_gameOver) {
    int16_t x, y;
    if (touchPressed(x, y)) {
      if (inRect(x, y, EB1_X, EB_Y, EB_W, EB_H)) flappyInit();
      else if (inRect(x, y, EB2_X, EB_Y, EB_W, EB_H)) switchApp(APP_MENU);
    }
    return;
  }

  if (millis() - flappy_lastFrame < 25) return; // ~ 40 FPS
  flappy_lastFrame = millis();

  int16_t tx, ty;
  if (touchPressed(tx, ty)) {
    if (ty < HUD_H && tx >= MENU_BTN_X) { switchApp(APP_MENU); return; }
    birdVy = -6.5; // Salto
  }

  // 1. Borrar posición anterior del pájaro
  int16_t oldBirdY = (int16_t)birdY;
  tft.fillCircle(BIRD_X, oldBirdY, BIRD_R, COL_BLACK);

  // 2. Física del pájaro
  birdVy += 0.55;
  birdY += birdVy;
  int16_t newBirdY = (int16_t)birdY;

  // 3. Renderizado incremental del tubo (sin parpadeos)
  int16_t speed = 4;

  // Borrar franja trasera del tubo (derecha)
  int16_t clearX = pipeX + PIPE_W;
  if (clearX < SCR_W && clearX + speed > 0) {
    int16_t cx = max((int16_t)0, clearX);
    int16_t cw = min((int16_t)speed, (int16_t)(SCR_W - cx));
    if (cw > 0) {
      tft.fillRect(cx, HUD_H, cw, SCR_H - HUD_H, COL_BLACK);
    }
  }

  pipeX -= speed;

  // Si el tubo salió por la izquierda, reiniciar al borde derecho
  if (pipeX < -PIPE_W) {
    tft.fillRect(0, HUD_H, speed + 4, SCR_H - HUD_H, COL_BLACK); // Limpia residuos a la izquierda
    pipeX = SCR_W;
    gapY = random(HUD_H + 30, SCR_H - GAP_H - 30);
    passedPipe = false;
  }

  // Dibujar nueva franja delantera del tubo (izquierda)
  if (pipeX < SCR_W && pipeX + speed > 0) {
    int16_t drawX = max((int16_t)0, pipeX);
    int16_t drawW = min((int16_t)speed, (int16_t)(SCR_W - drawX));
    if (drawW > 0) {
      if (gapY > HUD_H)
        tft.fillRect(drawX, HUD_H, drawW, gapY - HUD_H, COL_GREEN);
      int16_t bY = gapY + GAP_H;
      if (bY < SCR_H)
        tft.fillRect(drawX, bY, drawW, SCR_H - bY, COL_GREEN);
    }
  }

  // Sumar Puntos
  if (!passedPipe && pipeX + PIPE_W < BIRD_X) {
    flappy_score++;
    passedPipe = true;
    flappy_drawHud();
  }

  // 4. Dibujar pájaro en nueva posición
  tft.fillCircle(BIRD_X, newBirdY, BIRD_R, COL_YELLOW);

  // 5. Colisiones
  if (newBirdY - BIRD_R <= HUD_H || newBirdY + BIRD_R >= SCR_H) {
    flappy_endGame(true);
    return;
  }

  if (BIRD_X + BIRD_R >= pipeX && BIRD_X - BIRD_R <= pipeX + PIPE_W) {
    if (newBirdY - BIRD_R < gapY || newBirdY + BIRD_R > gapY + GAP_H) {
      flappy_endGame(false);
      return;
    }
  }
}