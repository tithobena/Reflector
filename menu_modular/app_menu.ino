/*
 * app_menu.ino
 * Menú Principal rediseñado con typedef explícito para evitar errores de compilación.
 */
#include "config.h"
#include "app.h"

#define GRID_COLS 4
#define GRID_ROWS 2

#define CARD_W  100
#define CARD_H  105
#define GAP_X   16
#define GAP_Y   12
#define START_X 16
#define START_Y 52



// Prototipos manuales para evitar fallos del preprocesador de Arduino
static void drawHeader();
static void drawFooter();
static void renderCard(uint8_t col, uint8_t row, const MenuCardItem &card);

// --- 2. DIBUJO DE ICONOS EN PIXEL-ART ---

static void drawIconCalc(int16_t x, int16_t y) {
  tft.fillRect(x + 30, y + 12, 40, 48, COL_DKGRAY);
  tft.drawRect(x + 30, y + 12, 40, 48, COL_WHITE);
  tft.fillRect(x + 35, y + 16, 30, 10, COL_GREEN);
  tft.fillRect(x + 35, y + 30, 8, 8, COL_BLUE);
  tft.fillRect(x + 46, y + 30, 8, 8, COL_RED);
  tft.fillRect(x + 57, y + 30, 8, 8, COL_YELLOW);
  tft.fillRect(x + 35, y + 42, 8, 8, COL_ORANGE);
  tft.fillRect(x + 46, y + 42, 19, 8, COL_CYAN);
}

static void drawIconBreakout(int16_t x, int16_t y) {
  tft.fillRect(x + 24, y + 15, 14, 7, COL_RED);
  tft.fillRect(x + 42, y + 15, 14, 7, COL_YELLOW);
  tft.fillRect(x + 60, y + 15, 14, 7, COL_BLUE);
  tft.fillRect(x + 33, y + 25, 14, 7, COL_GREEN);
  tft.fillRect(x + 51, y + 25, 14, 7, COL_MAGENTA);
  tft.fillCircle(x + 45, y + 42, 4, COL_WHITE);
  tft.fillRect(x + 32, y + 52, 32, 6, COL_CYAN);
}

static void drawIconFlappy(int16_t x, int16_t y) {
  tft.fillRect(x + 22, y + 12, 10, 48, COL_GREEN);
  tft.fillRect(x + 68, y + 12, 10, 48, COL_GREEN);
  tft.fillCircle(x + 48, y + 35, 11, COL_YELLOW);
  tft.fillRect(x + 55, y + 33, 7, 5, COL_ORANGE);
  tft.fillCircle(x + 51, y + 31, 2, COL_BLACK);
}

static void drawIconTateti(int16_t x, int16_t y) {
  tft.fillRect(x + 38, y + 14, 3, 44, COL_GRAY);
  tft.fillRect(x + 56, y + 14, 3, 44, COL_GRAY);
  tft.fillRect(x + 24, y + 28, 48, 3, COL_GRAY);
  tft.fillRect(x + 24, y + 44, 48, 3, COL_GRAY);
  tft.drawLine(x + 27, y + 16, x + 35, y + 25, COL_CYAN);
  tft.drawLine(x + 35, y + 16, x + 27, y + 25, COL_CYAN);
  tft.drawCircle(x + 48, y + 36, 5, COL_YELLOW);
}

static void drawIconPaint(int16_t x, int16_t y) {
  tft.fillCircle(x + 45, y + 35, 20, COL_DKGRAY);
  tft.drawCircle(x + 45, y + 35, 20, COL_WHITE);
  tft.fillCircle(x + 37, y + 27, 4, COL_RED);
  tft.fillCircle(x + 53, y + 27, 4, COL_BLUE);
  tft.fillCircle(x + 33, y + 39, 4, COL_YELLOW);
  tft.fillCircle(x + 45, y + 45, 4, COL_GREEN);
  tft.drawLine(x + 58, y + 16, x + 48, y + 30, COL_WHITE);
}

static void drawIconSnake(int16_t x, int16_t y) {
  tft.fillRect(x + 25, y + 45, 20, 6, COL_GREEN);
  tft.fillRect(x + 39, y + 31, 6, 20, COL_GREEN);
  tft.fillRect(x + 39, y + 25, 25, 6, COL_GREEN);
  tft.fillRect(x + 58, y + 25, 6, 18, COL_GREEN);
  tft.fillCircle(x + 61, y + 50, 5, COL_RED);
}

static void drawIconSimon(int16_t x, int16_t y) {
  tft.fillRect(x + 28, y + 16, 20, 20, COL_GREEN);
  tft.fillRect(x + 50, y + 16, 20, 20, COL_RED);
  tft.fillRect(x + 28, y + 38, 20, 20, COL_YELLOW);
  tft.fillRect(x + 50, y + 38, 20, 20, COL_BLUE);
  tft.fillCircle(x + 49, y + 37, 6, COL_BLACK);
  tft.drawCircle(x + 49, y + 37, 6, COL_WHITE);
}

static void drawIconScope(int16_t x, int16_t y) {
  tft.fillRect(x + 25, y + 14, 50, 44, COL_BLACK);
  tft.drawRect(x + 25, y + 14, 50, 44, COL_WHITE);
  tft.drawFastHLine(x + 25, y + 36, 50, COL_DKGRAY);
  tft.drawFastVLine(x + 50, y + 14, 44, COL_DKGRAY);
  int16_t prevY = y + 36;
  for (int16_t i = 0; i < 46; i += 3) {
    int16_t curY = y + 36 + (int16_t)(12.0 * sin(i * 0.25));
    tft.drawLine(x + 27 + i, prevY, x + 30 + i, curY, COL_GREEN);
    prevY = curY;
  }
}

// --- 3. ARREGLO DE TARJETAS ---

static MenuCardItem menuCards[] = {
  {APP_CALC,   "Calculadora", COL_BLUE,    drawIconCalc},
  {APP_GAME,   "Bricks",      COL_GREEN,   drawIconBreakout},
  {APP_FLAPPY, "Flappy",      COL_ORANGE,  drawIconFlappy},
  {APP_TATETI, "Ta Te Ti",    COL_MAGENTA, drawIconTateti},
  {APP_PAINT,  "Paint",       COL_CYAN,    drawIconPaint},
  {APP_SNAKE,  "Snake",       COL_RED,     drawIconSnake},
  {APP_SIMON,  "Simon",       COL_YELLOW,  drawIconSimon},
  {APP_SCOPE,  "Osciloscopio",COL_GREEN,   drawIconScope}
};

// --- 4. RENDERIZADO Y UI ---

static void drawHeader() {
  tft.fillRect(0, 0, SCR_W, 40, COL_DKGRAY);
  tft.drawFastHLine(0, 39, SCR_W, COL_CYAN);
  tft.drawFastHLine(0, 40, SCR_W, COL_BLUE);
  tft.fillCircle(25, 20, 8, COL_CYAN);
  tft.fillCircle(25, 20, 4, COL_BLACK);
  printCenteredF("SISTEMA MULTI-APP TFT", SCR_W / 2, 26, FONT_TITLE, COL_WHITE);
}

static void drawFooter() {
  tft.fillRect(0, 288, SCR_W, 32, COL_DKGRAY);
  tft.drawFastHLine(0, 287, SCR_W, COL_GRAY);
  printCenteredF("Selecciona una app o usa el monitor Serial ('?')", SCR_W / 2, 308, FONT_SMALL, COL_YELLOW);
}

static void renderCard(uint8_t col, uint8_t row, const MenuCardItem &card) {
  int16_t x = START_X + col * (CARD_W + GAP_X);
  int16_t y = START_Y + row * (CARD_H + GAP_Y);

  tft.fillRect(x, y, CARD_W, CARD_H, COL_BLACK);
  tft.drawRect(x, y, CARD_W, CARD_H, card.accentColor);
  tft.drawRect(x + 1, y + 1, CARD_W - 2, CARD_H - 2, COL_DKGRAY);

  card.drawIcon(x, y);

  tft.fillRect(x + 3, y + CARD_H - 26, CARD_W - 6, 22, COL_DKGRAY);
  printCenteredF(card.title, x + (CARD_W / 2), y + CARD_H - 11, FONT_SMALL, COL_WHITE);
}

// --- 5. FUNCIONES PRINCIPALES ---

void menuInit() {
  tft.fillScreen(COL_BLACK);
  drawHeader();

  for (uint8_t i = 0; i < 8; i++) {
    uint8_t col = i % GRID_COLS;
    uint8_t row = i / GRID_COLS;
    renderCard(col, row, menuCards[i]);
  }

  drawFooter();
}

void menuUpdate() {
  int16_t tx, ty;
  if (touchPressed(tx, ty)) {
    for (uint8_t i = 0; i < 8; i++) {
      uint8_t col = i % GRID_COLS;
      uint8_t row = i / GRID_COLS;
      int16_t x = START_X + col * (CARD_W + GAP_X);
      int16_t y = START_Y + row * (CARD_H + GAP_Y);

      if (inRect(tx, ty, x, y, CARD_W, CARD_H)) {
        switchApp(menuCards[i].appIndex);
        return;
      }
    }
  }
}