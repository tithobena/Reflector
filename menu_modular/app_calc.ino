/*
 * app_calc.ino
 * App "Calculadora". Usa tipografia moderna vectorial y botones tactiles.
 */
#include "config.h"
#include "app.h"

#define KEY_W   120
#define KEY_H   52
#define KEY_Y0  60

static const char keyMap[5][4] = {
  {'C', '<', 'M', '/'},
  {'7', '8', '9', '*'},
  {'4', '5', '6', '-'},
  {'1', '2', '3', '+'},
  {'N', '0', '.', '='}
};

static char  entry[14] = "0";
static float acc = 0;
static char  op = 0;
static bool  fresh = true;
static bool  err = false;

static void formatNumber(float v, char *out) {
  if (isnan(v) || isinf(v) || fabs(v) >= 1e9) { strcpy(out, "Error"); return; }
  if (v == (float)(long)v) { ltoa((long)v, out, 10); return; }
  long ip = labs((long)v);
  int8_t n = 1;
  while (ip >= 10) { ip /= 10; n++; }
  int8_t d = 7 - n;
  if (d < 0) d = 0;
  if (d > 6) d = 6;
  dtostrf(v, 1, d, out);
  if (strchr(out, '.')) {
    int8_t len = strlen(out);
    while (len > 0 && out[len - 1] == '0') out[--len] = 0;
    if (len > 0 && out[len - 1] == '.') out[--len] = 0;
  }
  if (strcmp(out, "-0") == 0) strcpy(out, "0");
}

static void setEntryFromNumber(float v) {
  char tmp[20];
  formatNumber(v, tmp);
  strncpy(entry, tmp, 12);
  entry[12] = 0;
}

static void applyOp() {
  float b = atof(entry);
  switch (op) {
    case '+': acc += b; break;
    case '-': acc -= b; break;
    case '*': acc *= b; break;
    case '/':
      if (b == 0) err = true; else acc /= b;
      break;
  }
  if (isnan(acc) || isinf(acc) || fabs(acc) >= 1e9) err = true;
}

static void calcClear() {
  strcpy(entry, "0");
  acc = 0;
  op = 0;
  fresh = true;
  err = false;
}

static void calcPress(char k) {
  if (err && k != 'C') return;

  if (k >= '0' && k <= '9') {
    if (fresh) { entry[0] = 0; fresh = false; }
    int8_t n = strlen(entry);
    if (n == 1 && entry[0] == '0') n = 0;
    else if (n == 2 && entry[0] == '-' && entry[1] == '0') n = 1;
    if (n < 11) { entry[n] = k; entry[n + 1] = 0; }
  }
  else if (k == '.') {
    if (fresh) { strcpy(entry, "0"); fresh = false; }
    if (!strchr(entry, '.') && strlen(entry) < 11) strcat(entry, ".");
  }
  else if (k == '<') {
    if (!fresh) {
      int8_t n = strlen(entry);
      if (n <= 1 || (n == 2 && entry[0] == '-')) strcpy(entry, "0");
      else entry[n - 1] = 0;
    }
  }
  else if (k == 'N') {
    if (fresh && op) {
      strcpy(entry, "-0");
      fresh = false;
    } else if (strcmp(entry, "0") != 0) {
      if (entry[0] == '-') memmove(entry, entry + 1, strlen(entry));
      else if (strlen(entry) < 11) { memmove(entry + 1, entry, strlen(entry) + 1); entry[0] = '-'; }
    }
  }
  else if (k == '+' || k == '-' || k == '*' || k == '/') {
    if (op && !fresh) applyOp();
    else if (!op) acc = atof(entry);
    if (err) { strcpy(entry, "Error"); op = 0; }
    else { setEntryFromNumber(acc); op = k; fresh = true; }
  }
  else if (k == '=') {
    if (op && !fresh) {
      applyOp();
      op = 0;
      if (err) strcpy(entry, "Error"); else setEntryFromNumber(acc);
      fresh = true;
    }
  }
  else if (k == 'C') {
    calcClear();
  }
}

static void drawCalcDisplay() {
  tft.fillRect(0, 0, SCR_W, KEY_Y0 - 4, COL_DISPLAY);
  if (op) {
    char t[24];
    formatNumber(acc, t);
    int8_t n = strlen(t);
    t[n] = ' ';
    t[n + 1] = (op == '*') ? 'x' : op;
    t[n + 2] = 0;
    
    tft.setFont(FONT_SMALL);
    tft.setTextColor(COL_GRAY);
    int16_t x1, y1; uint16_t w, h;
    tft.getTextBounds(t, 0, 0, &x1, &y1, &w, &h);
    // Subido de Y=18 a Y=12 para darle mas aire
    tft.setCursor(SCR_W - 12 - w - x1, 9 - y1);
    tft.print(t);
  }
  
  // Numero principal / entrada
  tft.setFont(FONT_TITLE);
  tft.setTextColor(err ? COL_RED : COL_WHITE);
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(entry, 0, 0, &x1, &y1, &w, &h);
  // Subido de Y=46 a Y=34 para centrarlo verticalmente en la franja
  tft.setCursor(SCR_W - 12 - w - x1, 25 - y1);
  tft.print(entry);
  
  tft.setFont(NULL); // Restaurar fuente por defecto
}

static void drawCalcKeys() {
  for (uint8_t r = 0; r < 5; r++) {
    for (uint8_t c = 0; c < 4; c++) {
      char k = keyMap[r][c];
      char lbl[5];
      uint16_t fill = COL_DKGRAY;
      uint16_t txt = COL_WHITE;
      switch (k) {
        case 'M': strcpy(lbl, "MENU"); fill = COL_NAVY; break;
        case 'N': strcpy(lbl, "+/-"); break;
        case '<': strcpy(lbl, "DEL"); fill = COL_MAGENTA; break;
        case 'C': strcpy(lbl, "C"); fill = COL_RED; break;
        case '=': strcpy(lbl, "="); fill = COL_GREEN; txt = COL_BLACK; break;
        case '*': strcpy(lbl, "x"); fill = COL_ORANGE; txt = COL_BLACK; break;
        case '/': case '-': case '+':
          lbl[0] = k; lbl[1] = 0; fill = COL_ORANGE; txt = COL_BLACK; break;
        default:  lbl[0] = k; lbl[1] = 0; break;
      }
      drawButtonF(c * KEY_W + 3, KEY_Y0 + r * KEY_H + 3, KEY_W - 6, KEY_H - 6,
                  lbl, FONT_LABEL, fill, txt);
    }
  }
}

void calcInit() {
  tft.fillScreen(COL_BLACK);
  calcClear();
  drawCalcKeys();
  drawCalcDisplay();
}

void calcUpdate() {
  int16_t x, y;
  if (touchPressed(x, y) && y >= KEY_Y0) {
    uint8_t c = x / KEY_W;
    uint8_t r = (y - KEY_Y0) / KEY_H;
    if (r < 5 && c < 4) {
      char k = keyMap[r][c];
      if (k == 'M') { switchApp(APP_MENU); return; }
      calcPress(k);
      drawCalcDisplay();
    }
  }
}