/*
 * app_paint.ino
 * App "Paint Táctil": Paleta de colores, tamaños de pincel, borrador y limpiar.
 */
#include "config.h"
#include "app.h"

#define TOOLBAR_H  46
#define COLOR_W    32
#define COLOR_H    32
#define TOOL_Y     7

static uint16_t paint_colors[] = {COL_WHITE, COL_RED, COL_GREEN, COL_BLUE, COL_YELLOW, COL_MAGENTA, COL_CYAN, COL_BLACK};
static const uint8_t PALETTE_COUNT = 8;

static uint16_t pt_currentColor = COL_WHITE;
static uint8_t  pt_brushSize    = 3; // 1, 3, 6 píxeles de radio
static int16_t  pt_lastX        = -1;
static int16_t  pt_lastY        = -1;

static void drawToolbar() {
  tft.fillRect(0, 0, SCR_W, TOOLBAR_H, COL_DKGRAY);
  tft.drawFastHLine(0, TOOLBAR_H - 1, SCR_W, COL_WHITE);

  // Muestras de colores
  for (uint8_t i = 0; i < PALETTE_COUNT; i++) {
    int16_t cx = 6 + i * (COLOR_W + 4);
    tft.fillRect(cx, TOOL_Y, COLOR_W, COLOR_H, paint_colors[i]);
    tft.drawRect(cx, TOOL_Y, COLOR_W, COLOR_H, (paint_colors[i] == pt_currentColor) ? COL_WHITE : COL_GRAY);
  }

  // Botón Limpiar
  drawButtonF(315, TOOL_Y, 70, COLOR_H, "Borrar", FONT_SMALL, COL_ORANGE, COL_BLACK);

  // Botón Menú
  drawButtonF(395, TOOL_Y, 78, COLOR_H, "MENU", FONT_SMALL, COL_NAVY, COL_WHITE);
}

void paintInit() {
  pt_currentColor = COL_WHITE;
  pt_brushSize = 3;
  pt_lastX = -1;
  pt_lastY = -1;

  tft.fillScreen(COL_BLACK);
  drawToolbar();
}

void paintUpdate() {
  int16_t tx, ty;
  bool touched = readTouch(tx, ty);

  if (touched) {
    // Interacción con la barra superior
    if (ty < TOOLBAR_H) {
      pt_lastX = -1;
      pt_lastY = -1;

      // Selección de Color
      for (uint8_t i = 0; i < PALETTE_COUNT; i++) {
        int16_t cx = 6 + i * (COLOR_W + 4);
        if (tx >= cx && tx <= cx + COLOR_W) {
          pt_currentColor = paint_colors[i];
          drawToolbar();
          return;
        }
      }

      // Botón Borrar Pantalla
      if (inRect(tx, ty, 315, TOOL_Y, 70, COLOR_H)) {
        tft.fillRect(0, TOOLBAR_H, SCR_W, SCR_H - TOOLBAR_H, COL_BLACK);
        return;
      }

      // Botón Menú
      if (inRect(tx, ty, 395, TOOL_Y, 78, COLOR_H)) {
        switchApp(APP_MENU);
        return;
      }
    } 
    // Dibujo continuo en el lienzo
    else if (ty >= TOOLBAR_H + pt_brushSize) {
      if (pt_lastX > 0 && pt_lastY > TOOLBAR_H) {
        // Trazado continuo entre puntos para evitar huecos al mover rápido el dedo
        if (pt_brushSize == 1) {
          tft.drawLine(pt_lastX, pt_lastY, tx, ty, pt_currentColor);
        } else {
          for (int8_t dx = -pt_brushSize; dx <= pt_brushSize; dx++) {
            tft.drawLine(pt_lastX + dx, pt_lastY, tx + dx, ty, pt_currentColor);
          }
        }
      } else {
        tft.fillCircle(tx, ty, pt_brushSize, pt_currentColor);
      }
      pt_lastX = tx;
      pt_lastY = ty;
    }
  } else {
    pt_lastX = -1;
    pt_lastY = -1;
  }
}