/*
 * app_notes.ino
 * Bloc de Notas con soporte para tarjeta Micro SD y Teclado Táctil
 */
#include "config.h"
#include "app.h"
#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN 10 // Pin CS estándar para shields TFT (cambia a 4 si tu shield lo requiere)

enum NotesState { STATE_LIST, STATE_EDIT, STATE_VIEW };
static NotesState currentState = STATE_LIST;

static char currentNoteText[120] = "";
static uint8_t textLen = 0;
static bool sdAvailable = false;

// Teclado en pantalla
static const char keys[3][10] = {
  {'Q','W','E','R','T','Y','U','I','O','P'},
  {'A','S','D','F','G','H','J','K','L',' '},
  {'Z','X','C','V','B','N','M','<','=','S'} // '<'=Borrar, '='=Espacio, 'S'=Guardar
};

static void drawKeyboard() {
  tft.fillRect(0, 160, SCR_W, 160, COL_DKGRAY);
  for (uint8_t r = 0; r < 3; r++) {
    for (uint8_t c = 0; c < 10; c++) {
      int16_t x = 5 + c * 31;
      int16_t y = 165 + r * 38;
      tft.fillRect(x, y, 28, 34, COL_BLUE);
      tft.drawRect(x, y, 28, 34, COL_WHITE);

      tft.setTextColor(COL_WHITE);
      tft.setTextSize(2);
      tft.setCursor(x + 8, y + 8);
      tft.print(keys[r][c]);
    }
  }
}

static void drawEditor() {
  tft.fillScreen(COL_BLACK);
  
  // Cabecera
  tft.fillRect(0, 0, SCR_W, 30, COL_DKGRAY);
  printCenteredF("NUEVA NOTA", SCR_W / 2, 20, FONT_TITLE, COL_WHITE);
  
  // Botón Volver
  tft.fillRect(5, 3, 50, 24, COL_RED);
  printCenteredF("X", 30, 18, FONT_SMALL, COL_WHITE);

  // Área de texto
  tft.drawRect(10, 40, SCR_W - 20, 110, COL_WHITE);
  tft.fillRect(11, 41, SCR_W - 22, 108, COL_BLACK);
  
  tft.setTextColor(COL_YELLOW);
  tft.setTextSize(2);
  tft.setCursor(18, 50);
  tft.print(currentNoteText);

  drawKeyboard();
}

static void saveNoteToSD() {
  if (!sdAvailable) return;

  // Genera nombre automático NOTA_1.TXT, NOTA_2.TXT...
  char filename[15];
  uint8_t noteNum = 1;
  do {
    snprintf(filename, sizeof(filename), "NOTA_%d.TXT", noteNum++);
  } while (SD.exists(filename) && noteNum < 99);

  File noteFile = SD.open(filename, FILE_WRITE);
  if (noteFile) {
    noteFile.println(currentNoteText);
    noteFile.close();
  }
}

static void drawNotesList() {
  tft.fillScreen(COL_BLACK);

  // Cabecera
  tft.fillRect(0, 0, SCR_W, 35, COL_DKGRAY);
  printCenteredF("BLOC DE NOTAS (SD)", SCR_W / 2 - 20, 22, FONT_TITLE, COL_WHITE);

  // Botón Salir al menú principal
  tft.fillRect(SCR_W - 55, 5, 50, 25, COL_RED);
  printCenteredF("MENU", SCR_W - 30, 20, FONT_SMALL, COL_WHITE);

  // Botón Crear Nueva Nota
  tft.fillRect(15, 45, SCR_W - 30, 40, COL_GREEN);
  printCenteredF("+ NUEVA NOTA", SCR_W / 2, 68, FONT_TITLE, COL_BLACK);

  // Estado de la Micro SD
  if (!sdAvailable) {
    printCenteredF("Tarxeta SD no detectada", SCR_W / 2, 150, FONT_SMALL, COL_RED);
    printCenteredF("Inserta una tarjeta FAT16/32", SCR_W / 2, 170, FONT_SMALL, COL_YELLOW);
  } else {
    printCenteredF("Ranura SD Lista", SCR_W / 2, 130, FONT_SMALL, COL_GREEN);
  }
}

void notesInit() {
  sdAvailable = SD.begin(SD_CS_PIN);
  currentState = STATE_LIST;
  textLen = 0;
  currentNoteText[0] = '\0';
  drawNotesList();
}

void notesUpdate() {
  int16_t tx, ty;
  if (!touchPressed(tx, ty)) return;

  if (currentState == STATE_LIST) {
    // Salir al menú principal
    if (inRect(tx, ty, SCR_W - 55, 5, 50, 25)) {
      switchApp(APP_MENU);
      return;
    }

    // Crear Nueva Nota
    if (inRect(tx, ty, 15, 45, SCR_W - 30, 40)) {
      currentState = STATE_EDIT;
      textLen = 0;
      currentNoteText[0] = '\0';
      drawEditor();
      delay(150);
      return;
    }
  } 
  else if (currentState == STATE_EDIT) {
    // Volver / Cancelar
    if (inRect(tx, ty, 5, 3, 50, 24)) {
      currentState = STATE_LIST;
      drawNotesList();
      delay(150);
      return;
    }

    // Detección de pulsaciones en el teclado virtual
    if (ty >= 160) {
      for (uint8_t r = 0; r < 3; r++) {
        for (uint8_t c = 0; c < 10; c++) {
          int16_t x = 5 + c * 31;
          int16_t y = 165 + r * 38;

          if (inRect(tx, ty, x, y, 28, 34)) {
            char key = keys[r][c];

            if (key == '<') { // Borrar
              if (textLen > 0) {
                textLen--;
                currentNoteText[textLen] = '\0';
              }
            } else if (key == '=') { // Espacio
              if (textLen < 115) {
                currentNoteText[textLen++] = ' ';
                currentNoteText[textLen] = '\0';
              }
            } else if (key == 'S') { // Guardar Nota en SD
              saveNoteToSD();
              currentState = STATE_LIST;
              drawNotesList();
              delay(200);
              return;
            } else { // Caracter normal
              if (textLen < 115) {
                currentNoteText[textLen++] = key;
                currentNoteText[textLen] = '\0';
              }
            }

            drawEditor();
            delay(120);
            return;
          }
        }
      }
    }
  }
}