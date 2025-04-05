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

void drawGridLeft(PlaydateAPI* pd) {
    for (int j = 0; j < 3; j++) {
        int xPos = selectPositions[0][j][0];
        int yPos = selectPositions[0][j][1];
        pd->graphics->drawRect(xPos - 2, yPos - 2, GRID_CELL_SIZE, GRID_CELL_SIZE, kColorBlack);
    }
}

void drawGridRight(BattleParams* battleParams, PlaydateAPI* pd) {
    int xPos, yPos;
    for(int i = 0; i < battleParams->enemyCount; i++) {
        Enemy e = battleParams->enemies[i];
        getGridPosition(e.fight_x, e.fight_y, &xPos, &yPos);
        if(!e.isAlive) {
            pd->graphics->fillRect(xPos + 1, yPos + 1, 34, 34, kColorWhite);
            continue;
        }
        LCDBitmap *m = pd->graphics->getTableBitmap(battleParams->enemySprites, e.id);
        pd->graphics->drawBitmap(m, xPos + 2, yPos + 2, kBitmapUnflipped);
    }
    for (int i = 1; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            getGridPosition(i, j, &xPos, &yPos);
            pd->graphics->drawRect(xPos, yPos, GRID_CELL_SIZE, GRID_CELL_SIZE, kColorBlack);
        }
    }
}

void drawAttackOptions(BattleParams* battleParams, PlaydateAPI* pd) {
    Player* player = battleParams->players[battleParams->activePlayerIndex];
    for (int i = 0; i < player->attack_count; i++) {
        drawButton(pd, menuPositions[i][0], menuPositions[i][1],
                   ATTACK_BTN_WIDTH, ATTACK_BTN_HEIGHT, player->attacks[i].name, i == battleParams->menuIndex);
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
        if (curr < 3) 
            m = pd->graphics->getTableBitmap(battleParams->players[curr]->sprite.table, 0);
        else  
            m = pd->graphics->getTableBitmap(battleParams->enemySprites, battleParams->enemies[curr - 3].id);

        pd->graphics->drawScaledBitmap(m, (200 - offset) + i * 18, 1, 0.5, 0.5);
        
        if (i == battleParams->currSequencePos) 
            pd->graphics->fillRect((199 - offset) + i * 18, 0, 18, 18, kColorXOR);
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

    clearInfoArea(pd);
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

bool drawTextArea(BattleParams* battleParams, float dt, bool flush) {
    PlaydateAPI* pd = battleParams->pd;
    battleParams->elapsedTimeText += dt;

    if(battleParams->elapsedTimeText < (strlen(battleParams->areaText) * 0.05f) + 1.0) {
        int len = (int)(battleParams->elapsedTimeText / 0.05f);
        char *c = substring(battleParams->areaText, len);
        if(c == NULL) return false;
        
        pd->graphics->drawTextInRect(c, strlen(c), kASCIIEncoding, 10, 170, 380, 60, kWrapWord, kAlignTextCenter);
        free(c);
        if(flush) clearInfoArea(pd);
        return false;
    }
    if(flush) clearInfoArea(pd);
    return true;
}

bool drawEnemiesHit(BattleParams* battleParams, float dt) {
    battleParams->elapsedEnemiesHitTime += dt;
    float elapsedTime = battleParams->elapsedEnemiesHitTime;
    PlaydateAPI* pd = battleParams->pd;

    bool isTextDrawn = drawTextArea(battleParams, dt, false);
    if (elapsedTime >= 1.7f && isTextDrawn) {
        clearInfoArea(pd);
        for(int i = 0; i < battleParams->enemyCount; i++) {
            Enemy* e = &battleParams->enemies[i];
            e->hit = false;
            if(!e->isAlive) continue;

            int xPos, yPos;
            getGridPosition(e->fight_x, e->fight_y, &xPos, &yPos);
            LCDBitmap* m = pd->graphics->getTableBitmap(battleParams->enemySprites, e->id);
    
            pd->graphics->fillRect(xPos + 1, yPos + 1, 34, 34, kColorWhite);
            pd->graphics->drawBitmap(m, xPos + 2, yPos + 2, kBitmapUnflipped);
        }
        return true;
    }


    if (elapsedTime - battleParams->enemyHitLastOffsetChangeTime >= 0.15f) {
        battleParams->enemyHitOffsetX = 1 + (rand() % 3);
        battleParams->enemyHitOffsetY = 1 + (rand() % 3);
        battleParams->enemyHitLastOffsetChangeTime = elapsedTime;
    }

    int xPos, yPos;
    for (int i = 0; i < 3; i++) {
        if (!battleParams->enemies[i].hit) continue;
        Enemy e = battleParams->enemies[i];
        getGridPosition(e.fight_x, e.fight_y, &xPos, &yPos);
        LCDBitmap* m = pd->graphics->getTableBitmap(battleParams->enemySprites, e.id);

        pd->graphics->fillRect(xPos + 1, yPos + 1, 34, 34, kColorWhite);
        pd->graphics->drawBitmap(m, xPos + battleParams->enemyHitOffsetX, yPos + battleParams->enemyHitOffsetY, kBitmapUnflipped);
    }

    return false; 
}

void drawSelector(BattleParams* battleParams) {
    int newSelectX, newSelectY;
    getGridPosition(battleParams->selectX, battleParams->selectY, &newSelectX, &newSelectY);
    LCDBitmap* s = battleParams->pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
    battleParams->pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
}