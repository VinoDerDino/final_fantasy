#include "fight.h"

void shuffleSequence(int* sequence, int count) {
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = sequence[i];
        sequence[i] = sequence[j];
        sequence[j] = temp;
    }
}

void initializeSelectPositions(void) {
    const int offset = (GRID_CELL_SIZE - 32) / 2;
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            if (y > 2) {
                selectPositions[x][y][0] = (x < 3) ? 20 : 255;
                selectPositions[x][y][1] = (y == 3) ? 200 : 160;
            } else {
                if (x < 3) {
                    selectPositions[x][y][0] = 14 + x * GRID_HORIZONTAL_SPACING + y * GRID_DIAGONAL_OFFSET + offset;
                    selectPositions[x][y][1] = 110 - y * GRID_VERTICAL_SPACING + offset;
                } else {
                    int rightCol = x - 3;
                    selectPositions[x][y][0] = 302 + rightCol * GRID_HORIZONTAL_SPACING - y * GRID_DIAGONAL_OFFSET + offset;
                    selectPositions[x][y][1] = 110 - y * GRID_VERTICAL_SPACING + offset;
                }
            }
        }
    }
}

void getDirection(PDButtons btn_pressed, int* dx, int* dy) {
    *dx = 0;
    *dy = 0;
    if (btn_pressed & kButtonLeft)  *dx = -1;
    else if (btn_pressed & kButtonRight) *dx = 1;
    if (btn_pressed & kButtonUp)    *dy = 1;
    else if (btn_pressed & kButtonDown)  *dy = -1;
}

Player* selectPlayer(BattleParams* battleParams, PlaydateAPI* pd, bool onSelect) {
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    int dx = 0, dy = 0;
    getDirection(btn_pressed, &dx, &dy);

    if (dx || dy) {
        int oldSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int oldSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        pd->graphics->fillRect(oldSelectX - 12, oldSelectY + 12, 8, 8, kColorWhite);
        battleParams->selectX = (battleParams->selectX + dx + 3) % 3;
        battleParams->selectY = (battleParams->selectY + dy + 3) % 3;
    
        int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->select, 2);
        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
    }

    if ((btn_pressed & kButtonA) || onSelect) {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            Player* p = battleParams->chars[i];
            if (p && p->fight_x == battleParams->selectX && p->fight_y == battleParams->selectY) {
                return p;
            }
        }
    }
    return NULL;
}

Enemy* selectEnemy(BattleParams* battleParams, PlaydateAPI* pd) {
    return NULL;
}