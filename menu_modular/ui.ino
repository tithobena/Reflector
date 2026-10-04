/*
 * ui.ino
 * Funciones de dibujo genericas que usa CUALQUIER app: botones, texto
 * centrado con tipografia moderna, deteccion de toque dentro de un
 * rectangulo. Nada aqui sabe nada sobre el menu, la calculadora ni
 * el juego.
 *
 * Nota sobre fuentes GFX: a diferencia de setTextSize() (que usa el
 * tipo de ancho fijo de fabrica), las fuentes .h de Adafruit_GFX son
 * proporcionales: cada letra mide distinto. Por eso para centrar un
 * texto hay que medirlo primero con getTextBounds() en vez de estimar
 * "cant. de letras x ancho fijo".
 */
#include "config.h"

bool inRect(int16_t px, int16_t py, int16_t x, int16_t y, int16_t w, int16_t h) {
  return px >= x && px < x + w && py >= y && py < y + h;
}

void printCenteredF(const char *s, int16_t cx, int16_t cy, const GFXfont *font, uint16_t color) {
  tft.setFont(font);
  tft.setTextColor(color);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor(cx - w / 2 - x1, cy - h / 2 - y1);
  tft.print(s);
  tft.setFont(NULL);   // vuelve a la fuente de fabrica para que nadie mas la herede sin querer
}

void drawButtonF(int16_t x, int16_t y, int16_t w, int16_t h,
                  const char *label, const GFXfont *font, uint16_t fill, uint16_t txt) {
  tft.fillRoundRect(x, y, w, h, 8, fill);
  tft.drawRoundRect(x, y, w, h, 8, COL_WHITE);
  printCenteredF(label, x + w / 2, y + h / 2, font, txt);
}
