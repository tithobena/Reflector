/*
 * app_saver.ino
 * Salvapantallas Rebotador estilo DVD / Clock
 */
#include "config.h"
#include "app.h"

static int16_t boxX = 50, boxY = 50;
static int8_t  dirX = 2,  dirY = 2;
static uint16_t currentColor = COL_CYAN;
static const uint8_t BOX_W = 110;
static const uint8_t BOX_H = 30;
static unsigned long lastDrawTime = 0;

static const uint16_t colors[] = {
  COL_CYAN, COL_GREEN, COL_RED, COL_YELLOW, COL_MAGENTA, COL_WHITE, COL_ORANGE
};

static void changeColor() {
  static uint8_t colorIdx = 0;
  colorIdx = (colorIdx + 1) % (sizeof(colors) / sizeof(colors[0]));
  currentColor = colors[colorIdx];
}

void saverInit() {
  tft.fillScreen(COL_BLACK);
  boxX = random(10, SCR_W - BOX_W - 10);
  boxY = random(10, SCR_H - BOX_H - 10);
  dirX = (random(0, 2) == 0) ? 2 : -2;
  dirY = (random(0, 2) == 0) ? 2 : -2;
  changeColor();
  lastDrawTime = millis();
}

void saverUpdate() {
  int16_t tx, ty;
  
  // Al tocar la pantalla en cualquier lugar, despierta y regresa al menú
  if (touchPressed(tx, ty)) {
    switchApp(APP_MENU);
    return;
  }

  // Refresco del salvapantallas (~30 FPS)
  if (millis() - lastDrawTime < 33) return;
  lastDrawTime = millis();

  // Borrar posición previa
  tft.fillRect(boxX, boxY, BOX_W, BOX_H, COL_BLACK);

  // Mover coordenadas
  boxX += dirX;
  boxY += dirY;

  // Rebote Horizontal
  if (boxX <= 0 || boxX + BOX_W >= SCR_W) {
    dirX = -dirX;
    changeColor();
  }

  // Rebote Vertical
  if (boxY <= 0 || boxY + BOX_H >= SCR_H) {
    dirY = -dirY;
    changeColor();
  }

  // Dibujar nueva posición (Caja + Texto "STANDBY")
  tft.drawRect(boxX, boxY, BOX_W, BOX_H, currentColor);
  tft.drawRect(boxX + 1, boxY + 1, BOX_W - 2, BOX_H - 2, currentColor);

  tft.setTextColor(currentColor);
  tft.setTextSize(2);
  tft.setCursor(boxX + 12, boxY + 8);
  tft.print("STANDBY");
}