/*
 * app_simon.ino
 * App "Simon Says": Memoriza y repite la secuencia de colores.
 */
#include "config.h"
#include "app.h"

#define BUZZER_PIN 44 // Opcional: conecta un buzzer en D44

#define MAX_SEQ 50
static uint8_t simon_seq[MAX_SEQ];
static uint8_t simon_step = 0;
static uint8_t simon_userStep = 0;
static bool simon_waitingUser = false;
static bool simon_gameOver = false;
static uint16_t simon_score = 0;

// Posiciones de los 4 botones
#define BTN_SIZE 120
#define PAD_X    60
#define PAD_Y    70

static void drawPad(uint8_t btn, bool highlight) {
  uint16_t colors[4] = {COL_GREEN, COL_RED, COL_YELLOW, COL_BLUE};
  uint16_t hiColors[4] = {0x07E0, 0xF800, 0xFFE0, 0x001F}; // Colores intensos

  int16_t x = (btn % 2 == 0) ? PAD_X : PAD_X + BTN_SIZE + 20;
  int16_t y = (btn < 2) ? PAD_Y : PAD_Y + BTN_SIZE + 20;

  uint16_t c = highlight ? COL_WHITE : colors[btn];
  tft.fillRect(x, y, BTN_SIZE, BTN_SIZE, c);
  tft.drawRect(x, y, BTN_SIZE, BTN_SIZE, COL_WHITE);

  if (highlight) {
    uint16_t tones[4] = {262, 330, 392, 523}; // Do, Mi, Sol, Do alta
    #if defined(BUZZER_PIN)
      tone(BUZZER_PIN, tones[btn], 200);
    #endif
  }
}

static void drawSimonUI() {
  tft.fillScreen(COL_BLACK);
  drawButtonF(380, 4, 95, 32, "MENU", FONT_SMALL, COL_NAVY, COL_WHITE);

  char buf[20];
  sprintf(buf, "Nivel: %u", simon_score);
  printCenteredF(buf, 200, 25, FONT_SUB, COL_YELLOW);

  for (uint8_t i = 0; i < 4; i++) drawPad(i, false);
}

static void addSequenceStep() {
  if (simon_step < MAX_SEQ) {
    simon_seq[simon_step++] = random(0, 4);
  }
}

static void playSequence() {
  simon_waitingUser = false;
  delay(500);

  for (uint8_t i = 0; i < simon_step; i++) {
    drawPad(simon_seq[i], true);
    delay(400);
    drawPad(simon_seq[i], false);
    delay(150);
  }

  simon_userStep = 0;
  simon_waitingUser = true;
}

void simonInit() {
  simon_step = 0;
  simon_score = 0;
  simon_gameOver = false;
  drawSimonUI();
  addSequenceStep();
  playSequence();
}

void simonUpdate() {
  int16_t tx, ty;
  if (touchPressed(tx, ty)) {
    if (inRect(tx, ty, 380, 4, 95, 32)) { switchApp(APP_MENU); return; }

    if (simon_gameOver) {
      simonInit();
      return;
    }

    if (!simon_waitingUser) return;

    int8_t clickedPad = -1;
    if (inRect(tx, ty, PAD_X, PAD_Y, BTN_SIZE, BTN_SIZE)) clickedPad = 0;
    else if (inRect(tx, ty, PAD_X + BTN_SIZE + 20, PAD_Y, BTN_SIZE, BTN_SIZE)) clickedPad = 1;
    else if (inRect(tx, ty, PAD_X, PAD_Y + BTN_SIZE + 20, BTN_SIZE, BTN_SIZE)) clickedPad = 2;
    else if (inRect(tx, ty, PAD_X + BTN_SIZE + 20, PAD_Y + BTN_SIZE + 20, BTN_SIZE, BTN_SIZE)) clickedPad = 3;

    if (clickedPad != -1) {
      drawPad(clickedPad, true);
      delay(250);
      drawPad(clickedPad, false);

      if (clickedPad == simon_seq[simon_userStep]) {
        simon_userStep++;
        if (simon_userStep >= simon_step) {
          simon_score++;
          addSequenceStep();
          drawSimonUI();
          playSequence();
        }
      } else {
        simon_gameOver = true;
        printCenteredF("¡ERROR!", 200, 160, FONT_TITLE, COL_RED);
        printCenteredF("Toca para reintentar", 200, 200, FONT_SMALL, COL_WHITE);
      }
    }
  }
}