#ifndef OVERWORLD_H_
#define OVERWORLD_H_

#define TARGET_EXTENSION 1

#include "pd_api.h"
#include "sprite.h"
#include "player.h"

#define MAX_OVERWORLD_TILES 400
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240
#define SCREEN_HALF_WIDTH 200
#define SCREEN_HALF_HEIGHT 120

typedef struct {
    int x, y;
    int maxX, maxY;
} Camera;

typedef struct {
    int width, height;
    int tiles[MAX_OVERWORLD_TILES];
    LCDBitmapTable* tile_sprites;
} World;

typedef struct {
    PlaydateAPI* pd;
    World* world;
    Player* player;
    Camera camera;
    LCDFont* font;
    LCDBitmap* select;
    bool check_info;
    int selectX, selectY;
} OverworldParams;

void updateCamera(Camera* camera, Player* player, World* world);
void overworldEnter(void* params);
void overworldExit(void* params);
void overworldUpdate(void* params, float dt);
void overworldDraw(void* params);

#endif