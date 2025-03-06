#ifndef OVERWORLD_H_
#define OVERWORLD_H_

#define TARGET_EXTENSION 1

#include "pd_api.h"

#include "sprite.h"
#include "scenemanager.h"

#define MAX_OVERWORLD_TILES 400
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240
#define SCREEN_HALF_WIDTH 200
#define SCREEN_HALF_HEIGHT 120

typedef struct World {
    int width, height;
    int tiles[MAX_OVERWORLD_TILES];
    LCDBitmapTable* tile_sprites;
} World;

#endif