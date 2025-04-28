#ifndef FIGHT_H_
#define FIGHT_H_

#define TARGET_EXTENSION 1

#include "pd_api.h"
#include "player.h"
#include "enemy.h"
#include "scenemanager.h"

#define GRID_CELL_SIZE 36
#define GRID_SIZE 3
#define ATTACK_BTN_WIDTH  125
#define ATTACK_BTN_HEIGHT 30
#define GRID_LEFT_START_X 14
#define GRID_LEFT_START_Y 110
#define GRID_RIGHT_START_X 350
#define GRID_RIGHT_START_Y 110

#define BUTTON_WIDTH         125
#define BUTTON_HEIGHT        30
#define BUTTON_OUTLINE_OFFSET 2
#define BUTTON_TEXT_OFFSET_X 5
#define BUTTON_TEXT_OFFSET_Y 7

#define GRID_HORIZONTAL_SPACING 48
#define GRID_VERTICAL_SPACING   42
#define GRID_DIAGONAL_OFFSET    12

#define ANIMATION_SPEED         200.0f

#define NUM_PLAYERS 3

extern int selectPositions[GRID_SIZE][GRID_SIZE][2];
extern const char* menuOptions[4];
extern const int menuPositions[4][2];


// drawing functions
void drawButton(PlaydateAPI* pd, int x, int y, int width, int height, const char* text, bool highlighted);
void drawGridLeft(PlaydateAPI* pd);
void drawGridRight(BattleParams* battleParams, PlaydateAPI* pd);
void drawAttackOptions(BattleParams* battleParams, PlaydateAPI* pd);
void drawAttackSequence(BattleParams* battleParams, PlaydateAPI* pd);
void drawFillHealthbar(PlaydateAPI* pd, int curr_health, int total_health);
void drawPlayerMenu(BattleParams* battleParams, PlaydateAPI* pd);
void drawActionAndTargetTooltips(BattleParams* battleParams, PlaydateAPI* pd);
bool drawEnemiesHit(BattleParams* battleParams, float dt);
bool drawTextArea(BattleParams* battleParams, float dt, bool flush);
void drawSelector(BattleParams* battleParams);

//logic functions
void handlePlayerMenu(BattleParams* battleParams, float dt);
void handlePlayerSelectInfo(BattleParams* battleParams, float dt);
void handleAction(BattleParams* battleParams, PlaydateAPI* pd, Attack attack);
void handlePlayerAttackSelection(BattleParams* battleParams, float dt);
void handleFightLoop(BattleParams* battleParams);
void handleBattleEscape(BattleParams* battleParams);
void handlePlayerMove(BattleParams* battleParams);
void handlePlayerSelectTargetAlly(BattleParams *battleParams);
void handlePlayerSelectTargetEnemy(BattleParams *battleParams);
void assertAction(BattleParams *battleParams);
void handleEnemyTurn(BattleParams* battleParams);

//helper functions
int attackEnemy(Enemy *e, Attack attack);
void getDirection(PDButtons btn_pressed, int* dx, int* dy);
Player* selectPlayer(BattleParams* battleParams, PlaydateAPI* pd, bool onSelect);
Enemy* getEnemyAtPos(BattleParams* battleParams, int selectX, int selectY);
Enemy* selectEnemy(BattleParams* battleParams, PlaydateAPI* pd, bool onSelect);
void shuffleSequence(int* sequence, int count);
char *substring(const char* str, size_t n);
void clearInfoArea(PlaydateAPI* pd);
void getGridPosition(int fight_x, int fight_y, int* xPos, int* yPos);
void playAButtonSound(BattleParams *battleParams);
void playBButtonSound(BattleParams *battleParams);

#endif
