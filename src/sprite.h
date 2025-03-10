#ifndef SPRITE_H_
#define SPRITE_H_

#define TARGET_EXTENSION 1

#include "pd_api.h"
#include <stdbool.h>

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
    float frameDelay;
    float frameTimer;
} AnimatedSprite;

void drawSprite(Sprite sprite, PlaydateAPI *pd, int offset_x, int offset_y, LCDBitmapFlip flip);
void drawAnimatedSprite(AnimatedSprite* sprite, PlaydateAPI* pd, int offset_x, int offset_y, bool offsetAsPos, LCDBitmapFlip flip, float dt);

LCDBitmap* newBitmap(const char *path, PlaydateAPI *pd);
LCDBitmapTable* newBitmapTable(const char* path, PlaydateAPI *pd); 

Sprite newSprite(const char *path, PlaydateAPI *pd, int x, int y);
Sprite newSpriteFromTable(LCDBitmapTable* table, int idx, PlaydateAPI *pd, int x, int y);
AnimatedSprite newAnimatedSprite(const char *path, PlaydateAPI *pd, int x, int y, int width, int height, int frameCount, float frameDelay);

#endif