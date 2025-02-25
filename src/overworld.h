#ifndef OVERWORLD_H_
#define OVERWORLD_H_

#include "sprite.h"

#define MAX_OVERWORLD_SPRITES 400

typedef struct {
    int width, height;
    Sprite tiles[MAX_OVERWORLD_SPRITES];
} World;

void overworldEnter(void* params);
void overworldExit(void* params);
void overworldUpdate(void* params);
void overworldDraw(void* params);

#endif