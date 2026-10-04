/*
 * app_scope.ino
 * App "Graficador Analógico": Lee un pin analógico (A0-A5) y grafica el voltaje.
 */
#include "config.h"
#include "app.h"

#define GRAPH_X 40
#define GRAPH_Y 40
#define GRAPH_W 360
#define GRAPH_H 240

static uint8_t  selectedPin = 0; // A0 por defecto
static int16_t  lastGraphY = -1;
static uint16_t graphIndex = 0;
static unsigned long lastSampleTime = 0;

static void drawScopeUI() {
  tft.fillScreen(COL_BLACK);
  drawButtonF(410, 4, 65, 30, "MENU", FONT_SMALL, COL_NAVY, COL_WHITE);

  // Botones de selección de pin
  char pinName[5];
  sprintf(pinName, "A%u", selectedPin);
  drawButtonF(410, 50, 65, 35, pinName, FONT_SMALL, COL_DKGRAY, COL_YELLOW);

  // Ejes del gráfico
  tft.drawRect(GRAPH_X - 1, GRAPH_Y - 1, GRAPH_W + 2, GRAPH_H + 2, COL_WHITE);
  
  // Líneas de cuadrícula (Grid)
  for (uint16_t y = GRAPH_Y + 40; y < GRAPH_Y + GRAPH_H; y += 40) {
    tft.drawFastHLine(GRAPH_X, y, GRAPH_W, COL_DKGRAY);
  }
  for (uint16_t x = GRAPH_X + 60; x < GRAPH_X + GRAPH_W; x += 60) {
    tft.drawFastVLine(x, GRAPH_Y, GRAPH_H, COL_DKGRAY);
  }

  // Etiquetas de voltaje
  tft.setFont(FONT_SMALL);
  tft.setTextColor(COL_GRAY);
  tft.setCursor(5, GRAPH_Y + 10);        tft.print("5V");
  tft.setCursor(5, GRAPH_Y + GRAPH_H/2); tft.print("2.5V");
  tft.setCursor(5, GRAPH_Y + GRAPH_H);   tft.print("0V");
  tft.setFont(NULL);
}

void scopeInit() {
  graphIndex = 0;
  lastGraphY = -1;
  drawScopeUI();
}

void scopeUpdate() {
  int16_t tx, ty;
  if (touchPressed(tx, ty)) {
    if (inRect(tx, ty, 410, 4, 65, 30)) { switchApp(APP_MENU); return; }
    
    // Cambiar pin analógico (A0 -> A5)
    if (inRect(tx, ty, 410, 50, 65, 35)) {
      selectedPin = (selectedPin + 1) % 6;
      scopeInit();
      delay(200);
      return;
    }
  }

  if (millis() - lastSampleTime < 20) return; // Muestreo cada 20ms
  lastSampleTime = millis();

  // Leer pin
  uint16_t rawVal = analogRead(A0 + selectedPin);
  
  // Mapear valor 0-1023 a la altura del gráfico (invertido porque Y crece hacia abajo)
  int16_t curY = (GRAPH_Y + GRAPH_H) - map(rawVal, 0, 1023, 0, GRAPH_H);

  // Limpiar franja vertical actual antes de dibujar
  int16_t curX = GRAPH_X + graphIndex;
  tft.drawFastVLine(curX, GRAPH_Y, GRAPH_H, COL_BLACK);
  
  // Volver a dibujar la línea del grid si correspondía
  if (graphIndex % 60 == 0) {
    tft.drawFastVLine(curX, GRAPH_Y, GRAPH_H, COL_DKGRAY);
  }

  // Trazar línea de señal
  if (lastGraphY > 0 && graphIndex > 0) {
    tft.drawLine(curX - 1, lastGraphY, curX, curY, COL_GREEN);
  }

  lastGraphY = curY;
  graphIndex++;

  if (graphIndex >= GRAPH_W) {
    graphIndex = 0;
    lastGraphY = -1;
  }
}