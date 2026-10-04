/*
 * app_tateti.ino
 * App "Ta Te Ti". Soporta modo 2 Jugadores o vs IA Inteligente.
 */
#include "config.h"
#include "app.h"

#define GRID_X   120
#define GRID_Y   60
#define CELL_S   80
#define HUD_H    40

static char board[3][3];
static char turn = 'X';
static bool vsAI = true;
static bool matchOver = false;
static char winner = ' ';

static void drawBoardLines() {
  for (uint8_t i = 1; i < 3; i++) {
    tft.fillRect(GRID_X + i * CELL_S - 2, GRID_Y, 4, CELL_S * 3, COL_WHITE);
    tft.fillRect(GRID_X, GRID_Y + i * CELL_S - 2, CELL_S * 3, 4, COL_WHITE);
  }
}

static void drawMark(uint8_t r, uint8_t c, char mark) {
  int16_t cx = GRID_X + c * CELL_S + CELL_S / 2;
  int16_t cy = GRID_Y + r * CELL_S + CELL_S / 2;

  if (mark == 'X') {
    for (int8_t i = -2; i <= 2; i++) {
      tft.drawLine(cx - 22 + i, cy - 22, cx + 22 + i, cy + 22, COL_CYAN);
      tft.drawLine(cx - 22 + i, cy + 22, cx + 22 + i, cy - 22, COL_CYAN);
    }
  } else if (mark == 'O') {
    tft.fillCircle(cx, cy, 24, COL_YELLOW);
    tft.fillCircle(cx, cy, 18, COL_BLACK);
  }
}

static char checkWinner() {
  for (uint8_t i = 0; i < 3; i++) {
    if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) return board[i][0];
    if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) return board[0][i];
  }
  if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) return board[0][0];
  if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) return board[0][2];

  bool full = true;
  for (uint8_t r = 0; r < 3; r++)
    for (uint8_t c = 0; c < 3; c++)
      if (board[r][c] == ' ') full = false;

  return full ? 'D' : ' ';
}

static void drawStatus() {
  tft.fillRect(0, 0, 360, HUD_H, COL_BLACK);
  tft.setFont(FONT_SMALL);
  tft.setTextColor(COL_WHITE);
  tft.setCursor(10, 26);

  if (matchOver) {
    if (winner == 'D') tft.print(F("¡Empate!"));
    else {
      tft.print(F("¡Ganador: "));
      tft.print(winner);
      tft.print(F("!"));
    }
  } else {
    tft.print(F("Turno: "));
    tft.print(turn);
    tft.print(vsAI ? F(" (vs IA)") : F(" (2P)"));
  }
  tft.setFont(NULL);
}

static void resetBoard() {
  for (uint8_t r = 0; r < 3; r++)
    for (uint8_t c = 0; c < 3; c++) board[r][c] = ' ';
  turn = 'X';
  matchOver = false;
  winner = ' ';
  tft.fillScreen(COL_BLACK);

  drawButtonF(380, 4, 95, 32, "MENU", FONT_SMALL, COL_NAVY, COL_WHITE);
  drawButtonF(10, 305 - 32, 140, 32, vsAI ? "Modo: vs IA" : "Modo: 2 Jug.", FONT_SMALL, COL_DKGRAY, COL_WHITE);
  drawButtonF(330, 305 - 32, 140, 32, "Reiniciar", FONT_SMALL, COL_GREEN, COL_BLACK);

  drawBoardLines();
  drawStatus();
}

// Función auxiliar para buscar si un jugador está a 1 movimiento de ganar
static bool findWinningMove(char player, uint8_t &outR, uint8_t &outC) {
  for (uint8_t r = 0; r < 3; r++) {
    for (uint8_t c = 0; c < 3; c++) {
      if (board[r][c] == ' ') {
        board[r][c] = player;
        char w = checkWinner();
        board[r][c] = ' '; // deshacer simulación
        if (w == player) {
          outR = r;
          outC = c;
          return true;
        }
      }
    }
  }
  return false;
}

static void aiMove() {
  if (matchOver) return;

  uint8_t targetR = 255, targetC = 255;

  // 1. ¿La IA ('O') puede ganar en este turno?
  if (findWinningMove('O', targetR, targetC)) {
    // Hace la jugada ganadora
  }
  // 2. ¿El Jugador ('X') está a punto de ganar? ¡BLOQUEAR!
  else if (findWinningMove('X', targetR, targetC)) {
    // Bloquea al jugador
  }
  // 3. Tomar el centro si está libre
  else if (board[1][1] == ' ') {
    targetR = 1;
    targetC = 1;
  }
  // 4. Tomar una esquina libre al azar
  else {
    uint8_t corners[4][2] = {{0,0}, {0,2}, {2,0}, {2,2}};
    uint8_t freeCorners[4];
    uint8_t fcCount = 0;
    for (uint8_t i = 0; i < 4; i++) {
      if (board[corners[i][0]][corners[i][1]] == ' ') {
        freeCorners[fcCount++] = i;
      }
    }
    if (fcCount > 0) {
      uint8_t pick = freeCorners[random(0, fcCount)];
      targetR = corners[pick][0];
      targetC = corners[pick][1];
    } else {
      // 5. Cualquier casilla libre restante
      uint8_t emptyCount = 0;
      uint8_t er[9], ec[9];
      for (uint8_t r = 0; r < 3; r++) {
        for (uint8_t c = 0; c < 3; c++) {
          if (board[r][c] == ' ') {
            er[emptyCount] = r;
            ec[emptyCount] = c;
            emptyCount++;
          }
        }
      }
      if (emptyCount > 0) {
        uint8_t idx = random(0, emptyCount);
        targetR = er[idx];
        targetC = ec[idx];
      }
    }
  }

  if (targetR != 255 && targetC != 255) {
    board[targetR][targetC] = 'O';
    drawMark(targetR, targetC, 'O');
    winner = checkWinner();
    if (winner != ' ') matchOver = true;
    else turn = 'X';
    drawStatus();
  }
}

void tatetiInit() {
  resetBoard();
}

void tatetiUpdate() {
  int16_t x, y;
  if (touchPressed(x, y)) {
    if (inRect(x, y, 380, 4, 95, 32)) { switchApp(APP_MENU); return; }

    if (inRect(x, y, 10, 305 - 32, 140, 32)) {
      vsAI = !vsAI;
      resetBoard();
      return;
    }

    if (inRect(x, y, 330, 305 - 32, 140, 32)) {
      resetBoard();
      return;
    }

    if (!matchOver && inRect(x, y, GRID_X, GRID_Y, CELL_S * 3, CELL_S * 3)) {
      uint8_t c = (x - GRID_X) / CELL_S;
      uint8_t r = (y - GRID_Y) / CELL_S;

      if (board[r][c] == ' ') {
        board[r][c] = turn;
        drawMark(r, c, turn);

        winner = checkWinner();
        if (winner != ' ') {
          matchOver = true;
          drawStatus();
        } else {
          turn = (turn == 'X') ? 'O' : 'X';
          drawStatus();
          if (vsAI && turn == 'O') {
            delay(200);
            aiMove();
          }
        }
      }
    }
  }
}