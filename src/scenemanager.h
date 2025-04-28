#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <stdbool.h>

#include "pd_api.h"
#include "player.h"
#include "enemy.h"
#include "camera.h"

void overworldEnter(void* params);
void overworldExit(void* params);
void overworldUpdate(void* params, float dt);
void overworldDraw(void* params);

void invOnEnter(void* params);
void invOnExit(void* params);
void invUpdate(void* params, float dt);
void invDraw(void* params);

void battleOnEnter(void* params);
void battleOnExit(void* params);
void battleUpdate(void* params, float dt);
void battleDraw(void* params);

typedef enum
{
    PLAYER_TURN_INIT,
    PLAYER_MENU,
    PLAYER_MENU_EXIT,
    PLAYER_SELECT_INFO,
    PLAYER_ATTACK_SELECTION,
    PLAYER_MOVE,
    PLAYER_SELECT_PLAYER,
    PLAYER_SELECT_ENEMY,
    PLAYER_SELECT_TARGET_ALLY,
    PLAYER_SELECT_TARGET_ENEMY,
    HANDLE_ACTION,
    ASSERT_ATTACK,
    FIGHT_LOOP,
    ENEMY_TURN,
    SHOW_ENEMY_ATTACK,
    ENEMIES_HIT,
    REDRAW_ENEMIES,
    BATTLE_ESCAPE
} BattleStates;

typedef enum {
    PENDING = -1,
    MENU,
    OVERWORLD,
    INVENTORY,
    BATTLE
} Scenetypes;

typedef struct {
    void (*OnSceneEnter)(void* params);
    void (*OnSceneExit)(void* params);
    void (*UpdateScene)(void* params, float dt);
    void (*DrawScene)(void* params);
    Scenetypes type;
    Scenetypes pendingSceneChange;
    void* pendingData;
    void* data;
} Scenemanager;

struct World;

typedef struct {
    PlaydateAPI* pd;
    Scenemanager* manager;
    struct World* world;
    Player* player;
    Camera camera;
    LCDFont* font;
    LCDBitmap* select;
    bool check_info;
    int selectX, selectY;
} OverworldParams;

typedef struct {
    PlaydateAPI* pd;
    float selectedItem;
    Inventory* inv;
} InventoryParams;

typedef struct {
    // Core
    PlaydateAPI* pd;

    // Player & Enemies
    Player* players[3];
    Enemy enemies[3];
    int enemyCount;

    // Battle Flow Control
    BattleStates currentState;
    BattleStates nextState;
    int sequence[6];            // Turn order sequence
    int currSequencePos;

    // Selection & Input
    int selectX, selectY;       // Grid selection
    int selectPlayerIndex;      // Selected player
    int activePlayerIndex;      // Whose turn it is
    int menuIndex;              // Current menu position

    // Flags
    float elapsedEnemiesHitTime, enemyHitLastOffsetChangeTime;
    int enemyHitOffsetX, enemyHitOffsetY;

    // Visuals
    LCDBitmapTable* selectorIcons;
    LCDBitmapTable* enemySprites;
    int characterInfoOffset;
    LCDBitmap* characterInfoBitmap;
    char *areaText;
    float elapsedTimeText;
    LCDBitmap* textAreaBitmap;

    //Audio
    PDSynth *synth;
    SoundChannel *sound_effect;
    int lastTextLength;

    // Player Action Info
    Attack currentAttack;
    Player* infoTargetPlayer;

    // State Timing
    float elapsedTime;

    // Redraw Flags
    bool triggerEnemyRedraw;
} BattleParams;

void changeScene(Scenemanager *manager, Scenetypes type, void* sceneParams);
void updateScene(Scenemanager* manager, float dt, PlaydateAPI* pd);
void drawScene(Scenemanager* manager, PlaydateAPI* pd);

#endif