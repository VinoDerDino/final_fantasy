#include "fight.h"
#include <string.h>
#include <stdlib.h>

int selectPositions[GRID_SIZE][GRID_SIZE][2];
const char* menuOptions[4] = {"Show Infos", "Attack", "Escape", "Move Player"};
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

    battleParams->state = PLAYER_TURN_INIT;
    battleParams->activeP = 0;
    battleParams->passed_time = 0.0f;
    battleParams->menu_offset = 180.0f;

    for (int i = 0; i < NUM_PLAYERS; i++) {
        Player* player = battleParams->chars[i];
        player->sprite.frameTimer = 0.0;
        player->sprite.currentFrame = 0;
        for (int j = 0; j < player->attack_count; j++) {
            int dest_x = (j % 2) ? 255 : 20;
            int dest_y = (j / 2) ? 200 : 160;
            if (i == battleParams->activeP) {
                player->attacks[j].rect_y = (float)(dest_y + 80);
                player->attacks[j].dest_y = dest_y;
            } else {
                player->attacks[j].rect_y = (float)(dest_y);
            }
            player->attacks[j].rect_x = dest_x;
        }
    }

    const int count = 3 + battleParams->countMonsters;
    for (int i = 0; i < count; i++) {
        battleParams->sequence[i] = i;
    }
    shuffleSequence(battleParams->sequence, count); 

    battleParams->currSequencePos = 0;
    while(battleParams->sequence[battleParams->currSequencePos] >= 3) {
        battleParams->currSequencePos = (battleParams->currSequencePos + 1) % count;
    }

    initializeSelectPositions();
    battleDraw(params);
}

void battleOnExit(void *params) {
    free(params);
}


void battleUpdate(void* params, float dt) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;

    battleParams->passed_time += dt;

    switch (battleParams->state) {
        case PLAYER_TURN_INIT:
            battleParams->menuIndex = 0;
            battleParams->exit_menu = false;
            battleParams->enter_menu = true;
            battleParams->state = PLAYER_MENU;
            break;

        case PLAYER_MENU:
            handlePlayerMenu(battleParams, dt);
            break;

        case PLAYER_SELECT_INFO:
            handlePlayerSelectInfo(battleParams, dt);
            break;

        case PLAYER_ATTACK_SELECTION_ANIMATION:
            if (drawAttackButtonAnimation(battleParams, dt)) {
                battleParams->state = PLAYER_ATTACK_SELECTION;
                drawAttackOptions(battleParams, pd);
            }
            break;

        case PLAYER_ATTACK_SELECTION_ANIMATION_REVERSE:
            if(drawAttackButtonAnimationReverse(battleParams, dt)) {
                battleParams->state = PLAYER_TURN_INIT;
            }
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
                battleParams->state = FIGHT_LOOP;
            }
            break;
        }
        case FIGHT_LOOP:
            handleFightLoop(battleParams);
            break;

        case PLAYER_SELECT_ENEMY:
            break;
        default:
            break;
    } 

    const int offset = (GRID_CELL_SIZE - 32) / 2;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (battleParams->chars[i]) {
            int selectX = GRID_LEFT_START_X + battleParams->chars[i]->fight_x * GRID_HORIZONTAL_SPACING +
                          battleParams->chars[i]->fight_y * GRID_DIAGONAL_OFFSET + offset;
            int selectY = GRID_LEFT_START_Y - battleParams->chars[i]->fight_y * GRID_VERTICAL_SPACING + offset;
            pd->graphics->fillRect(selectX - 1, selectY - 1, 34, 34, kColorWhite);
            drawAnimatedSprite(&battleParams->chars[i]->sprite, pd, selectX, selectY, true, kBitmapUnflipped, dt);
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
}