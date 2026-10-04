/*
 * config.h
 * Pines, colores, dimensiones, fuentes y objetos compartidos por todas
 * las pestañas.
 * Placa: Arduino MEGA 2560
 */
#ifndef CONFIG_H
// --- Calibración Táctil (Ajusta los valores si las coordenadas quedan invertidas) ---
#ifndef TS_MINX
  #define TS_MINX 120
  #define TS_MAXX 900
  #define TS_MINY 100
  #define TS_MAXY 940
#endif
#define CONFIG_H
#include <Wire.h>
#include <RTClib.h>


#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

// Tipografia moderna (vectorial)
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>

#define FONT_SMALL  (&FreeSans9pt7b)         // texto secundario
#define FONT_LABEL  (&FreeSansBold9pt7b)     // botones, etiquetas
#define FONT_SUB    (&FreeSansBold12pt7b)    // subtitulos, HUD
#define FONT_TITLE  (&FreeSansBold18pt7b)    // titulos, display grande

// ------------------------- CONFIGURACION -------------------------
//#define DEBUG_TOUCH            // imprime valores crudos del touch por Serial

#define LCD_ROTATION 1            // 1 o 3 para modo horizontal / apaisado
//#define TOUCH_FLIP_X            // descomentar si el touch sale espejado en X
//#define TOUCH_FLIP_Y            // descomentar si el touch sale espejado en Y

// Pines del touch TFT
#define YP A3
#define XM A2
#define YM 9
#define XP 8
#define TS_RESIST 300

// Calibración y presión
#define TS_MIN 150
#define TS_MAX 900
#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define SCR_W 480
#define SCR_H 320

// ------------------------- COLORES (RGB565) -------------------------
#define COL_BLACK   0x0000
#define COL_WHITE   0xFFFF
#define COL_RED     0xF800
#define COL_GREEN   0x07E0
#define COL_BLUE    0x001F
#define COL_CYAN    0x07FF
#define COL_YELLOW  0xFFE0
#define COL_ORANGE  0xFD20
#define COL_MAGENTA 0xF81F
#define COL_GRAY    0x8410
#define COL_DKGRAY  0x4208
#define COL_DISPLAY 0x2104
#define COL_NAVY    0x0014

// ------------------------- OBJETOS GLOBALES -------------------------
extern MCUFRIEND_kbv tft;
extern TouchScreen ts;
extern RTC_DS3231 rtc; // O RTC_DS1307 rtc; según el módulo que tengas
// ------------------------- FUNCIONES DE HARDWARE -------------------------
bool readTouch(int16_t &x, int16_t &y);
bool touchPressed(int16_t &x, int16_t &y);

// ------------------------- FUNCIONES DE UI -------------------------------
bool inRect(int16_t px, int16_t py, int16_t x, int16_t y, int16_t w, int16_t h);
void printCenteredF(const char *s, int16_t cx, int16_t cy, const GFXfont *font, uint16_t color);
void drawButtonF(int16_t x, int16_t y, int16_t w, int16_t h,
                  const char *label, const GFXfont *font, uint16_t fill, uint16_t txt);

#endif