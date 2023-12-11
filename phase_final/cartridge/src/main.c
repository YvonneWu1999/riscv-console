#include <stdbool.h>
#include <stdint.h>

#include "api.h"

volatile int global = 42;
volatile int cmd = 0;
volatile int video = 0;
volatile uint32_t controller_status = 0;
volatile int count = 0;
volatile uint32_t mcause = 0;
volatile int currentPlayer = 0;
volatile int scoreboard[3][3] = {0};
volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xF4800);
// coloring
volatile uint32_t blue = 0xFF0000FF;
volatile uint32_t red = 0xFFFF0000;
volatile uint32_t green = 0xFF00FF00;
volatile uint32_t pink = 0xFFFF00FF;
volatile uint32_t yellow = 0xFFFFEA00;
volatile uint32_t orange = 0xFFFF7F50;

const char *gameResult[] = {
    "player two wins",
    "",
    "player one wins",
    "it's a tie",
};

int movePlayer(uint32_t controller_status, int position, int x, int y);
int changeX(uint32_t controller_status, int x);
int changeY(uint32_t controller_status, int y);
bool boardIsFull(volatile int scoreboard[3][3]);
bool boxIsValid(int x, int y);
void updateBox(int x, int y);
void switchPlayer(int player);
void createPawn(int player, int position);
int checkWinner(volatile int scoreboard[3][3]);
int checkEnd(volatile int scoreboard[3][3]);
void printString(const char *str);
void busyWaiting(int wait);
int main() {
    int a = 4;
    int b = 12;
    int last_global = 42;
    int last_cmd = 0;
    int x_pos = 12;
    int x_pos_player_1 = 0; // read last position from save (from before
                            // power-off), if none exists, set to 12
    int x_pos_player_2 = 56;
    int countdown = 1;
    int large_sprite_count;

    int x1 = -1;
    int x2 = 3;
    int y1 = 0;
    int y2 = 0;

    last_global = getTicks();
    // EnableTextMode();
    enableGraphicsMode();
    // Create Background Game Board
    createBackground(POUND);
    // Create large sprite data
    setBackgroundPalette(0, blue);
    setBackgroundControl(0, 0, 0, 0, 0);
    createLargeSprite(CIRCLE);
    setLargePalette(0, red);
    setLargeControl(0, 2, 0, 0, 0);
    createLargeSprite(SQUARE);
    setLargePalette(1, pink);
    setLargeControl(1, 3, 448, 0, 0);
    setLargePalette(2, red);
    setLargePalette(3, pink);

    while (1) {
        int c = a + b + global;
        if (a == 11) {
            b++;
        }
        global = getTicks();
        if (global != last_global) {
            controller_status = getController();
            cmd = getCMDInterruptCount();
            video = getVideoInterruptCount();
            if (video % 100 == 30) {
                setLargePalette(currentPlayer, green);
            } else if (video % 100 == 60) {
                setLargePalette(currentPlayer, yellow);
            } else if (video % 100 == 90) {
                setLargePalette(currentPlayer, orange);
            }
            if (last_cmd != cmd) {
                // check current player position valid or not
                if (currentPlayer == 0) {
                    if (boxIsValid(x1, y1)) {
                        updateBox(x1, y1);
                        if (checkEnd(scoreboard) != 0) {
                            enableTextMode();
                            printString("GAME OVER!!! ");
                            printString(gameResult[checkEnd(scoreboard) + 1]);
                        }
                        createPawn(0, x_pos_player_1);
                        x1 = -1;
                        y1 = 0;
                        x_pos_player_1 = 0;
                        switchPlayer(currentPlayer);
                    }
                }
                if (currentPlayer == 1) {
                    if (boxIsValid(x2, y2)) {
                        updateBox(x2, y2);
                        if (checkEnd(scoreboard) != 0) {
                            enableTextMode();
                            printString("GAME OVER!!! ");
                            printString(gameResult[checkEnd(scoreboard) + 1]);
                        }
                        createPawn(1, x_pos_player_2);
                        x2 = 3;
                        y2 = 0;
                        x_pos_player_2 = 56;
                        switchPlayer(currentPlayer);
                    }
                }
            }
            last_cmd = cmd;
            large_sprite_count = getLargeSpriteCount();
            if (controller_status & 0x1 || controller_status & 0x2 ||
                controller_status & 0x4 || controller_status & 0x8) {
                busyWaiting(999999);
                int isEnterBox = 0;
                if (currentPlayer == 0) {
                    x1 = changeX(controller_status, x1);
                    if (x1 != -1) {
                        y1 = changeY(controller_status, y1);
                        isEnterBox = 1;
                    }
                    x_pos_player_1 =
                        movePlayer(controller_status, x_pos_player_1, x1, y1);
                    setLargeControl(0, (isEnterBox == 1) ? 0 : 2,
                                    (x_pos_player_1 & 0x3F) << 3,
                                    (x_pos_player_1 >> 6) << 3, 0);
                }
                if (currentPlayer == 1) {
                    x2 = changeX(controller_status, x2);
                    if (x2 != 3) {
                        y2 = changeY(controller_status, y2);
                        isEnterBox = 1;
                    }
                    x_pos_player_2 =
                        movePlayer(controller_status, x_pos_player_2, x2, y2);
                    setLargeControl(1, (isEnterBox == 1) ? 1 : 3,
                                    (x_pos_player_2 & 0x3F) << 3,
                                    (x_pos_player_2 >> 6) << 3, 0);
                }
            }
        }
        last_global = global;
    }
}

int movePlayer(uint32_t controller_status, int position, int x, int y) {
    // difference of x is 12
    int position_matrix[3][3] = {
        {0x090, 0x390, 0x690}, {0x09C, 0x39C, 0x69C}, {0x0A8, 0x3A8, 0x6A8}};

    if (x < 0 || x >= 3) {
        return position;
    }

    position = position_matrix[x][y];
    return position;
}

int changeX(uint32_t controller_status, int x) {
    if (controller_status & 0x1) { // left
        if (x > 0) {
            x--;
        }
    }
    if (controller_status & 0x8) { // right
        if (x < 2) {
            x++;
        }
    }
    return x;
}

int changeY(uint32_t controller_status, int y) {
    if (controller_status & 0x2) { // up
        if (y > 0) {
            y--;
        }
    }
    if (controller_status & 0x4) { // down
        if (y < 2) {
            y++;
        }
    }
    return y;
}

bool boardIsFull(volatile int scoreboard[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (scoreboard[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}

bool boxIsValid(int x, int y) {
    if (scoreboard[x][y] == 0) {
        return true;
    }
    return false;
}

int checkWinner(volatile int scoreboard[3][3]) {
    // check rows and columns
    for (int i = 0; i < 3; ++i) {
        if (scoreboard[i][0] + scoreboard[i][1] + scoreboard[i][2] == 3 ||
            scoreboard[0][i] + scoreboard[1][i] + scoreboard[2][i] == 3) {
            return 1; // player 1 wins
        } else if (scoreboard[i][0] + scoreboard[i][1] + scoreboard[i][2] ==
                       -3 ||
                   scoreboard[0][i] + scoreboard[1][i] + scoreboard[2][i] ==
                       -3) {
            return -1; // player 2 wins
        }
    }

    // check diagonals
    if (scoreboard[0][0] + scoreboard[1][1] + scoreboard[2][2] == 3 ||
        scoreboard[0][2] + scoreboard[1][1] + scoreboard[2][0] == 3) {
        return 1; // player 1 wins
    } else if (scoreboard[0][0] + scoreboard[1][1] + scoreboard[2][2] == -3 ||
               scoreboard[0][2] + scoreboard[1][1] + scoreboard[2][0] == -3) {
        return -1; // player 2 wins
    }

    // no winner yet
    return 0;
}

void updateBox(int x, int y) {
    scoreboard[x][y] = (currentPlayer == 1) ? -1 : 1;
}

void createPawn(int player, int position) {
    int sprite_count;
    // create new pawn for player
    if (player == 0) {
        sprite_count = createLargeSprite(NOUGHT);
        setLargeControl(0, 2, 0, 0, 0);
        setLargeControl(sprite_count - 1, 2, (position & 0x3F) << 3,
                        (position >> 6) << 3, 0);
    }

    if (player == 1) {
        sprite_count = createLargeSprite(CROSS);
        setLargeControl(1, 3, 448, 0, 0);
        setLargeControl(sprite_count - 1, 3, (position & 0x3F) << 3,
                        (position >> 6) << 3, 0);
    }
}
void switchPlayer(int player) { currentPlayer = (player == 1) ? 0 : 1; }

int checkEnd(volatile int scoreboard[3][3]) {
    if (checkWinner(scoreboard) != 0) {
        return checkWinner(scoreboard);
    }
    if (boardIsFull(scoreboard)) {
        return 2;
    }
    return 0;
}

// print a string to video memory
void printString(const char *str) {
    while (*str != '\0') {
        *VIDEO_MEMORY = *str;
        str++;
        VIDEO_MEMORY++;
    }
}

void busyWaiting(int wait) {
    if (wait <= 0) {
        return;
    }

    for (int i = 0; i < wait; i++) {
    }
}