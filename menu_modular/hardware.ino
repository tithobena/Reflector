/*
 * hardware.ino
 * Manejo directo de la pantalla TFT y el touch resistivo.
 */
#include "config.h"
#include "app.h"
#define TOUCH_FLIP_X            // Invierte izquierda / derecha
#define TOUCH_FLIP_Y            // Invierte arriba / abajo
MCUFRIEND_kbv tft;
TouchScreen   ts(XP, YP, XM, YM, TS_RESIST);

bool readTouch(int16_t &x, int16_t &y) {
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);   // restaurar pines tras lectura tactil
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
#ifdef DEBUG_TOUCH
    Serial.print(F("raw x=")); Serial.print(p.x);
    Serial.print(F(" y="));    Serial.print(p.y);
    Serial.print(F(" z="));    Serial.println(p.z);
#endif

    // --- CAMBIO PARA ROTAR EL TÁCTIL 90 GRADOS ---
    // Invertimos las lecturas: p.y pasa a mapear la X, y p.x a la Y
#ifdef TOUCH_FLIP_X
    x = map(p.y, TS_MIN, TS_MAX, SCR_W, 0);
#else
    x = map(p.y, TS_MIN, TS_MAX, 0, SCR_W);
#endif

#ifdef TOUCH_FLIP_Y
    y = map(p.x, TS_MIN, TS_MAX, SCR_H, 0);
#else
    y = map(p.x, TS_MIN, TS_MAX, 0, SCR_H);
#endif

    x = constrain(x, 0, SCR_W - 1);
    y = constrain(y, 0, SCR_H - 1);
    return true;
  }
  return false;
}

bool touchPressed(int16_t &x, int16_t &y) {
  TSPoint p = ts.getPoint();
  
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    x = map(p.x, TS_MINX, TS_MAXX, 0, SCR_W);
    y = map(p.y, TS_MINY, TS_MAXY, 0, SCR_H);

    resetInactivityTimer(); // Reinicia el temporizador de inactividad
    return true;
  }
  return false;
}