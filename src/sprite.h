#ifndef SPRITE_H_
#define SPRITE_H_

#include "pd_api.h"
#include <stdio.h>

typedef struct Sprite_S {
    int x;
    int y;
    LCDBitmap* bm;
} Sprite;

typedef struct AnimatedSprite_S {
    int x;
    int y;
    LCDBitmapTable* table;
    int frameCount;
    int currentFrame;
    int frameDelay;
    int frameTimer;
} AnimatedSprite;

void drawSprite(Sprite sprite, PlaydateAPI *pd, int offset_x, int offset_y, float scale);

LCDBitmap* newBitmap(const char *path, PlaydateAPI *pd);
LCDBitmapTable* newBitmapTable(const char* path, PlaydateAPI *pd, int count, int width, int height); 

Sprite newSprite(const char *path, PlaydateAPI *pd, int x, int y);
Sprite newSpriteFromTable(LCDBitmapTable* table, int idx, PlaydateAPI *pd, int x, int y);
AnimatedSprite newAnimatedSprite(const char *path, PlaydateAPI *pd, int x, int y, int frameCount, int frameDelay);

#endif