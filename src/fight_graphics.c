#include "fight.h"

void drawButton(PlaydateAPI* pd, int x, int y, int width, int height, const char* text, bool highlighted) {
    pd->graphics->fillRect(x - BUTTON_OUTLINE_OFFSET, y - BUTTON_OUTLINE_OFFSET,
    width + 4, height + 4, kColorWhite);
    pd->graphics->drawRect(x, y, width, height, kColorBlack);
    pd->graphics->drawText(text, strlen(text), kASCIIEncoding, x + BUTTON_TEXT_OFFSET_X, y + BUTTON_TEXT_OFFSET_Y);
    if (highlighted) {
        pd->graphics->fillRect(x - 1, y - 1,
            width + 2, height + 2, kColorXOR);
    }
}

bool drawAttackButtonAnimation(void* params, float dt) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;
    Player* player = battleParams->players[battleParams->activePlayerIndex];

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
    Player* player = battleParams->players[battleParams->activePlayerIndex];

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
    for (int j = 0; j < 3; j++) {
        int xPos = selectPositions[0][j][0];
        int yPos = selectPositions[0][j][1];
        pd->graphics->drawRect(xPos - 2, yPos - 2, GRID_CELL_SIZE, GRID_CELL_SIZE, kColorBlack);
    }
}

void drawGridRight(BattleParams* battleParams, PlaydateAPI* pd) {
    for(int i = 0; i < battleParams->enemyCount; i++) {
        Enemy e = battleParams->enemies[i];
        int xPos = selectPositions[e.fight_x][e.fight_y][0];
        int yPos = selectPositions[e.fight_x][e.fight_y][1];
        if(!e.isAlive) {
            pd->graphics->fillRect(xPos + 2, yPos + 2, 32, 32, kColorWhite);
            continue;
        }
        LCDBitmap *m = pd->graphics->getTableBitmap(battleParams->enemySprites, e.id);
        pd->graphics->drawBitmap(m, xPos + 2, yPos + 2, kBitmapUnflipped);
        pd->system->logToConsole("Enemy %s:: x: %d, y: %d", e.name, xPos + 2, yPos + 2);
    }
    for (int i = 1; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int xPos = selectPositions[i][j][0];
            int yPos = selectPositions[i][j][1];
            pd->graphics->drawRect(xPos, yPos, GRID_CELL_SIZE, GRID_CELL_SIZE, kColorBlack);
            pd->system->logToConsole("Grid:: x: %d, y: %d", xPos, yPos);
        }
    }
}

void drawAttackOptions(BattleParams* battleParams, PlaydateAPI* pd) {
    Player* player = battleParams->players[battleParams->activePlayerIndex];
    for (int i = 0; i < player->attack_count; i++) {
        bool highlighted = (i == battleParams->menuIndex);
        drawButton(pd, player->attacks[i].rect_x, (int)player->attacks[i].rect_y,
                   ATTACK_BTN_WIDTH, ATTACK_BTN_HEIGHT, player->attacks[i].name, highlighted);
    }
}

void drawAttackSequence(BattleParams* battleParams, PlaydateAPI* pd) {
    if (!battleParams->sequence) return;

    const int sequenceCount = 3 + battleParams->enemyCount;
    const int offset = ((3 + battleParams->enemyCount) / 2) * 18;
    const int rectX = 196 - offset;
    const int rectWidth = 18 * (3 + battleParams->enemyCount) + 4;

    pd->graphics->fillRect(rectX, -1, rectWidth, 22, kColorWhite);
    pd->graphics->drawRect(rectX, -1, rectWidth, 22, kColorBlack);

    for (int i = 0; i < sequenceCount; i++) {
        if (i >= (int)(sizeof(battleParams->sequence) / sizeof(battleParams->sequence[0])))
            return;

        int curr = battleParams->sequence[i];
        LCDBitmap *m = NULL;
        if (curr < 3) {
            m = pd->graphics->getTableBitmap(battleParams->players[curr]->sprite.table, 0);
        } else {
            m = pd->graphics->getTableBitmap(battleParams->enemySprites, battleParams->enemies[curr - 3].id);
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
    pd->system->realloc(healthText, 0);
}

void drawPlayerMenu(BattleParams* battleParams, PlaydateAPI* pd) {
    for (int i = 0; i < 4; i++) {
        bool highlighted = (i == battleParams->menuIndex);
        drawButton(pd, menuPositions[i][0], menuPositions[i][1],
                   BUTTON_WIDTH, BUTTON_HEIGHT, menuOptions[i], highlighted);
    }
}

void drawActionAndTargetTooltips(BattleParams* battleParams, PlaydateAPI* pd) {
    Attack attack = battleParams->currentAttack;

    pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
    pd->graphics->drawText(attack.name, strlen(attack.name), kASCIIEncoding, 10, 175);
    const char *s;
    pd->system->formatString(&s, "DMG: %d", attack.dmg);
    pd->graphics->drawText(s, strlen(s), kASCIIEncoding, 10, 195);
    Enemy *e = getEnemyAtPos(battleParams, battleParams->selectX, battleParams->selectY);
    pd->system->realloc(s, 0);

    if(e == NULL) {
        pd->graphics->drawText("No Enemy selected", strlen("No Enemy selected"), kASCIIEncoding, 210, 175);
    } else {
        pd->graphics->drawText(e->name, strlen(e->name), kASCIIEncoding, 210, 175);
        const char *s2;
        pd->system->formatString(&s2, "HP: %d", e->hp);
        pd->graphics->drawText(s2, strlen(s2), kASCIIEncoding, 210, 195);
        pd->system->realloc(s2, 0); 
    }
}