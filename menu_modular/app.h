/*
 * app.h
 */
/* app.h */
#ifndef APP_H
#define APP_H

#include <Arduino.h>

#define INACTIVITY_TIMEOUT_MS 30000UL // 30 segundos sin actividad

// Declaración global de la variable (extern)
extern unsigned long lastActivityTime;

// Prototipos de funciones del temporizador
void resetInactivityTimer();
void checkInactivity();
void switchApp(uint8_t index);

// ... resto de tu app.h ...
typedef void (*AppFn)();

struct App {
  const char *name;
  AppFn init;
  AppFn update;
};

typedef struct {
  uint8_t appIndex;
  const char *title;
  uint16_t accentColor;
  void (*drawIcon)(int16_t x, int16_t y);
} MenuCardItem;

void switchApp(uint8_t index);
void resetInactivityTimer();
void checkInactivity();

extern uint8_t currentApp;
extern const uint8_t APP_COUNT;
extern App apps[];
extern unsigned long lastActivityTime;

enum {
  APP_MENU   = 0,
  APP_CALC   = 1,
  APP_GAME   = 2,
  APP_FLAPPY = 3,
  APP_TATETI = 4,
  APP_PAINT  = 5,
  APP_SNAKE  = 6,
  APP_SIMON  = 7,
  APP_SCOPE  = 8,
  APP_NOTES  = 9,
  APP_SAVER  = 10
};

#endif