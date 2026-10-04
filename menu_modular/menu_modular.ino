/*
 * menu_modular.ino
 * Archivo principal.
 */
#include "config.h"
#include "app.h"

// INSTANCIACIÓN REAL DE LA VARIABLE (Obligatorio para evitar undefined reference)
unsigned long lastActivityTime = 0;

// Prototipos de Apps
void menuInit();   void menuUpdate();
void calcInit();   void calcUpdate();
void gameInit();   void gameUpdate();
void flappyInit(); void flappyUpdate();
void tatetiInit(); void tatetiUpdate();
void paintInit();  void paintUpdate();
void snakeInit();  void snakeUpdate();
void simonInit();  void simonUpdate();
void scopeInit();  void scopeUpdate();
void notesInit();  void notesUpdate();
void saverInit();  void saverUpdate();

App apps[] = {
  {"Menu Principal",  menuInit,   menuUpdate},
  {"Calculadora",     calcInit,   calcUpdate},
  {"Rompe-ladrillos", gameInit,   gameUpdate},
  {"Flappy Bird",     flappyInit, flappyUpdate},
  {"Ta Te Ti",        tatetiInit, tatetiUpdate},
  {"Paint Tactil",    paintInit,  paintUpdate},
  {"Snake",           snakeInit,  snakeUpdate},
  {"Simon Says",      simonInit,  simonUpdate},
  {"Osciloscopio",    scopeInit,  scopeUpdate},
  {"Bloc Notas",      notesInit,  notesUpdate},
  {"Salvapantallas",  saverInit,  saverUpdate}
};

const uint8_t APP_COUNT = sizeof(apps) / sizeof(apps[0]);
uint8_t currentApp = APP_MENU;

void resetInactivityTimer() {
  lastActivityTime = millis();
}

void checkInactivity() {
  if (currentApp == APP_SAVER) return;

  if (millis() - lastActivityTime >= INACTIVITY_TIMEOUT_MS) {
    switchApp(APP_SAVER);
  }
}

void switchApp(uint8_t index) {
  if (index < APP_COUNT) {
    currentApp = index;
    apps[currentApp].init();
    Serial.print(F("-> App activa: "));
    Serial.println(apps[currentApp].name);
  }
}

void handleSerialMenu() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '0' || c == 'm' || c == 'M') switchApp(APP_MENU);
    else if (c == '1' || c == 'c' || c == 'C') switchApp(APP_CALC);
    else if (c == '2' || c == 'r' || c == 'R') switchApp(APP_GAME);
    else if (c == '3' || c == 'f' || c == 'F') switchApp(APP_FLAPPY);
    else if (c == '4' || c == 't' || c == 'T') switchApp(APP_TATETI);
    else if (c == '5' || c == 'p' || c == 'P') switchApp(APP_PAINT);
    else if (c == '6' || c == 's' || c == 'S') switchApp(APP_SNAKE);
    else if (c == '7') switchApp(APP_SIMON);
    else if (c == '8') switchApp(APP_SCOPE);
    else if (c == '9' || c == 'n' || c == 'N') switchApp(APP_NOTES);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("Iniciando sistema TFT ILI9486 (Arduino MEGA)..."));
  
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486;
  tft.begin(ID);
  tft.setRotation(LCD_ROTATION);
  randomSeed(analogRead(A5));

  resetInactivityTimer();
  switchApp(APP_MENU);
}

void loop() {
  handleSerialMenu();
  checkInactivity();
  
  if (currentApp < APP_COUNT && apps[currentApp].update) {
    apps[currentApp].update();
  }
}