#include "fight.h"
#include <string.h>
#include <stdlib.h>

int selectPositions[GRID_SIZE][GRID_SIZE][2] = {
    { {  64,112 }, {  76, 70 }, {  88, 28 } },
    { { 304,112 }, { 292, 70 }, { 280, 28 } },
    { { 352,112 }, { 340, 70 }, { 328, 28 } }
};

const char* menuOptions[4] = {"Attack", "Show Infos", "Escape", "Move Player"};
const int menuPositions[4][2] = {
    {20, 160},
    {255, 160},
    {20, 200},
    {255, 200}
};

void battleOnEnter(void *params) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;
    
    pd->graphics->clear(kColorWhite);
    battleParams->currentState = PLAYER_TURN_INIT;
    battleParams->activePlayerIndex = 0;
    battleParams->elapsedTime = 0.0f;
    
    for (int i = 0; i < NUM_PLAYERS; i++) {
        Player* player = battleParams->players[i];
        player->sprite.frameTimer = 0.0;
        player->sprite.currentFrame = 0;
    }
    
    const int count = 3 + battleParams->enemyCount;
    for (int i = 0; i < count; i++) {
        battleParams->sequence[i] = i;
    }
    shuffleSequence(battleParams->sequence, count); 
    for(int i = 0; i < count; i++) {
        if(battleParams->sequence[i] < 3) {
            battleParams->activePlayerIndex = battleParams->sequence[i];
            break;
        }
    }
    
    battleParams->currSequencePos = 0;
    while(battleParams->sequence[battleParams->currSequencePos] >= 3) {
        battleParams->currSequencePos = (battleParams->currSequencePos + 1) % count;
    }
    
    battleDraw(params);
}

void battleOnExit(void *params) {
    free(params);
}

void battleUpdate(void* params, float dt) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;

    battleParams->elapsedTime += dt;

    switch (battleParams->currentState) {
        case PLAYER_TURN_INIT:
            battleParams->menuIndex = 0;
            battleParams->currentState = PLAYER_MENU;
            break;

        case PLAYER_MENU:
            handlePlayerMenu(battleParams, dt);
            break;

        case PLAYER_SELECT_INFO:
            handlePlayerSelectInfo(battleParams, dt);
            break;

        case PLAYER_ATTACK_SELECTION:
            handlePlayerAttackSelection(battleParams, dt);
            break;

        case PLAYER_MOVE:
            handlePlayerMove(battleParams);
            break;

        case PLAYER_SELECT_PLAYER: {
            Player* p = selectPlayer(battleParams, pd, false);
            if (p != NULL) {
                drawAttackOptions(battleParams, pd);
                battleParams->currentState = FIGHT_LOOP;
            }
            break;
        } 
        case FIGHT_LOOP:
            if(battleParams->triggerEnemyRedraw) {
                battleParams->triggerEnemyRedraw = false;
                battleParams->currentState = REDRAW_ENEMIES;
                battleParams->nextState = FIGHT_LOOP;
                break;
            }
            handleFightLoop(battleParams);
            drawAttackSequence(battleParams, pd);
            break;

        case REDRAW_ENEMIES:
            drawGridRight(battleParams, pd);
            battleParams->currentState = battleParams->nextState;
            break;

        case PLAYER_SELECT_TARGET_ENEMY:
            handlePlayerSelectTargetEnemy(battleParams);
            break;

        case PLAYER_SELECT_TARGET_ALLY:
            handlePlayerSelectTargetAlly(battleParams);
            break;

        case HANDLE_ACTION:
            handleAction(battleParams, pd, battleParams->players[battleParams->activePlayerIndex]->attacks[battleParams->menuIndex]);
            break;

        case ASSERT_ATTACK:
            assertAction(battleParams);
            break;

        case PLAYER_SELECT_ENEMY:
            drawGridRight(battleParams, pd);
            break;

        case ENEMIES_HIT:
            if(drawEnemiesHit(battleParams, dt)) {
                battleParams->currentState = FIGHT_LOOP;
            }
            break;

        case ENEMY_TURN:
            handleEnemyTurn(battleParams);
            battleParams->elapsedTimeText = 0.0f;
            break;
        
        case SHOW_ENEMY_ATTACK:
            if(drawTextArea(battleParams, dt, true)) {
                battleParams->currentState = FIGHT_LOOP;
            }
            break;
        
        default:
            break;
    } 

    const int offset = (GRID_CELL_SIZE - 32) / 2;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (battleParams->players[i]) {
            int selectX = GRID_LEFT_START_X + 1 * GRID_HORIZONTAL_SPACING +
                          battleParams->players[i]->fight_y * GRID_DIAGONAL_OFFSET + offset;
            int selectY = GRID_LEFT_START_Y - battleParams->players[i]->fight_y * GRID_VERTICAL_SPACING + offset;
            pd->graphics->fillRect(selectX - 1, selectY - 1, 34, 34, kColorWhite);
            drawAnimatedSprite(&battleParams->players[i]->sprite, pd, selectX, selectY, true, kBitmapUnflipped, dt);
        }
    }
}

void battleDraw(void* params) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;

    pd->graphics->clear(kColorWhite);
    drawGridLeft(pd);
    drawGridRight(battleParams, pd);
    drawAttackSequence(battleParams, pd);
    drawPlayerMenu(battleParams, pd);
    pd->graphics->drawBitmap(battleParams->textAreaBitmap, 0, 155, kBitmapUnflipped);
}