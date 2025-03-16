#include "fight.h"

inline void drawButton(PlaydateAPI* pd, int x, int y, int width, int height, const char* text, bool highlighted) {
    pd->graphics->fillRect(x - BUTTON_OUTLINE_OFFSET, y - BUTTON_OUTLINE_OFFSET,
    width + 4, height + 4, kColorWhite);
    pd->graphics->drawRect(x, y, width, height, kColorBlack);
    pd->graphics->drawText(text, strlen(text), kASCIIEncoding, x + BUTTON_TEXT_OFFSET_X, y + BUTTON_TEXT_OFFSET_Y);
    if (highlighted) {
        pd->graphics->fillRect(x - 1, y - 1,
            width + 2, height + 2, kColorXOR);
    }
}

inline void getDirection(PDButtons btn_pressed, int* dx, int* dy) {
    *dx = 0;
    *dy = 0;
    if (btn_pressed & kButtonLeft)  *dx = -1;
    else if (btn_pressed & kButtonRight) *dx = 1;
    if (btn_pressed & kButtonUp)    *dy = 1;
    else if (btn_pressed & kButtonDown)  *dy = -1;
}

bool drawAttackButtonAnimation(void* params, float dt) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;
    Player* player = battleParams->chars[battleParams->activeP];

    bool finished = true;

    for (int i = 0; i < 4; i++) {
        pd->graphics->fillRect(player->attacks[i].rect_x - BUTTON_OUTLINE_OFFSET, (int)player->attacks[i].rect_y - BUTTON_OUTLINE_OFFSET,
                               ATTACK_BTN_WIDTH + BUTTON_OUTLINE_OFFSET * 2, ATTACK_BTN_HEIGHT + BUTTON_OUTLINE_OFFSET * 2, kColorWhite);

        if ((int)player->attacks[i].rect_y > player->attacks[i].dest_y) {
            finished = false;
            float newY = player->attacks[i].rect_y - (ANIMATION_SPEED * dt);
            player->attacks[i].rect_y = (newY < player->attacks[i].dest_y) ?
                                        (float)player->attacks[i].dest_y : newY;
        }
        
        drawButton(pd, player->attacks[i].rect_x, (int)player->attacks[i].rect_y,
                   ATTACK_BTN_WIDTH, ATTACK_BTN_HEIGHT, player->attacks[i].name, false);
    }
    return finished;
}

bool drawAttackButtonAnimationReverse(void* params, float dt) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;
    Player* player = battleParams->chars[battleParams->activeP];

    bool finished = true;

    for (int i = 0; i < 4; i++) {
        pd->graphics->fillRect(player->attacks[i].rect_x - BUTTON_OUTLINE_OFFSET, (int)player->attacks[i].rect_y - BUTTON_OUTLINE_OFFSET,
                               ATTACK_BTN_WIDTH + BUTTON_OUTLINE_OFFSET * 2, ATTACK_BTN_HEIGHT + BUTTON_OUTLINE_OFFSET * 2, kColorWhite);

        if ((int)player->attacks[i].rect_y < player->attacks[i].dest_y) {  
            finished = false;
            float newY = player->attacks[i].rect_y + (ANIMATION_SPEED * dt);

            player->attacks[i].rect_y = (newY > player->attacks[i].dest_y) ?
                                        (float)player->attacks[i].dest_y : newY;
        }

        drawButton(pd, player->attacks[i].rect_x, (int)player->attacks[i].rect_y,
                   ATTACK_BTN_WIDTH, ATTACK_BTN_HEIGHT, player->attacks[i].name, false);
    }
    return finished;
}


void drawGridLeft(PlaydateAPI* pd) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int xPos = GRID_LEFT_START_X + i * GRID_HORIZONTAL_SPACING + j * GRID_DIAGONAL_OFFSET;
            int yPos = GRID_LEFT_START_Y - j * GRID_VERTICAL_SPACING;
            pd->graphics->drawRect(xPos, yPos, GRID_CELL_SIZE, GRID_CELL_SIZE, kColorBlack);
        }
    }
}

void drawGridRight(BattleParams* battleParams, PlaydateAPI* pd) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            int xPos = GRID_RIGHT_START_X - i * GRID_HORIZONTAL_SPACING - j * GRID_DIAGONAL_OFFSET;
            int yPos = GRID_RIGHT_START_Y - j * GRID_VERTICAL_SPACING;
            pd->graphics->drawRect(xPos, yPos, GRID_CELL_SIZE, GRID_CELL_SIZE, kColorBlack);
            if (i == 1) {
                LCDBitmap *m = pd->graphics->getTableBitmap(battleParams->monsters, battleParams->enemies[j].id);
                pd->graphics->drawBitmap(m, xPos + 2, yPos + 2, kBitmapUnflipped);
            }
        }
    }
}

void drawAttackOptions(BattleParams* battleParams, PlaydateAPI* pd) {
    Player* player = battleParams->chars[battleParams->activeP];
    for (int i = 0; i < player->attack_count; i++) {
        bool highlighted = (i == battleParams->menuIndex);
        drawButton(pd, player->attacks[i].rect_x, (int)player->attacks[i].rect_y,
                   ATTACK_BTN_WIDTH, ATTACK_BTN_HEIGHT, player->attacks[i].name, highlighted);
    }
}

void drawAttackSequence(BattleParams* battleParams, PlaydateAPI* pd) {
    if (!battleParams->sequence) return;

    const int sequenceCount = 3 + battleParams->countMonsters;
    const int offset = ((3 + battleParams->countMonsters) / 2) * 18;
    const int rectX = 196 - offset;
    const int rectWidth = 18 * (3 + battleParams->countMonsters) + 4;

    pd->graphics->fillRect(rectX, -1, rectWidth, 22, kColorWhite);
    pd->graphics->drawRect(rectX, -1, rectWidth, 22, kColorBlack);

    for (int i = 0; i < sequenceCount; i++) {
        if (i >= (int)(sizeof(battleParams->sequence) / sizeof(battleParams->sequence[0])))
            return;

        int curr = battleParams->sequence[i];
        LCDBitmap *m = NULL;
        if (curr < 3) {
            m = pd->graphics->getTableBitmap(battleParams->chars[curr]->sprite.table, 0);
        } else {
            m = pd->graphics->getTableBitmap(battleParams->monsters, battleParams->enemies[curr - 3].id);
        }
        pd->graphics->drawScaledBitmap(m, (200 - offset) + i * 18, 1, 0.5, 0.5);
        
        if (i == battleParams->currSequencePos && curr < 3) {
            pd->graphics->fillRect((199 - offset) + i * 18, 0, 18, 18, kColorXOR);
        }
    }
}

void drawFillHealthbar(PlaydateAPI* pd, int curr_health, int total_health) {
    const int x = 101;
    const int y = 189;
    const int barWidth = 249;
    const int barHeight = 19;
    
    if (curr_health < 0)
        curr_health = 0;
    if (curr_health > total_health)
        curr_health = total_health;
    
    float healthRatio = (float)curr_health / (float)total_health;
    int fillWidth = (int)(healthRatio * barWidth);

    pd->graphics->fillRect(x, y, barWidth, barHeight, kColorWhite);
    pd->graphics->fillRect(x, y, fillWidth, barHeight, kColorBlack);

    int negX = x + fillWidth;
    int negWidth = barWidth - fillWidth;
    if (negWidth > 0) {
        for (int py = y; py < y + barHeight; py++) {
            for (int px = negX; px < negX + negWidth; px++) {
                if ((px + py) % 2 == 0)
                    pd->graphics->setPixel(px, py, kColorWhite);
                else
                    pd->graphics->setPixel(px, py, kColorBlack);
            }
        }
    }
    
    char *healthText;
    pd->system->formatString(&healthText, "%d/%d", curr_health, total_health);
    
    LCDBitmapDrawMode m = pd->graphics->setDrawMode(kColorXOR);
    pd->graphics->drawText(healthText, strlen(healthText), kASCIIEncoding, 103, 191);
    pd->graphics->setDrawMode(m);
}

void drawPlayerMenu(BattleParams* battleParams, PlaydateAPI* pd) {
    for (int i = 0; i < 4; i++) {
        bool highlighted = (i == battleParams->menuIndex);
        drawButton(pd, menuPositions[i][0], menuPositions[i][1],
                   BUTTON_WIDTH, BUTTON_HEIGHT, menuOptions[i], highlighted);
    }
}