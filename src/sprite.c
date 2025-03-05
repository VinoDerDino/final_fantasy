#include "sprite.h"

void drawSprite(Sprite sprite, PlaydateAPI *pd, int offset_x, int offset_y, LCDBitmapFlip flip) {
    if(!sprite.bm) {
        pd->system->logToConsole("Error: no bm");
        return;
    }
    pd->graphics->drawBitmap(sprite.bm, sprite.x + offset_x, sprite.y + offset_y, flip);
}

void drawAnimatedSprite(AnimatedSprite* sprite, PlaydateAPI* pd, int offset_x, int offset_y, LCDBitmapFlip flip, float dt) {
    if (sprite == NULL) {
        pd->system->logToConsole("Error: Invalid AnimatedSprite");
        return;
    }
    if(sprite->table == NULL) {
        pd->system->logToConsole("Error: Invalid BitmapTable (animated sprite)");
        return;
    }

    sprite->frameTimer += dt;
    // pd->system->logToConsole("DEBUG ANIMATED SPRITE: dt = %f, curr_frame: %d", sprite->frameTimer, sprite->currentFrame);
    if(sprite->frameTimer >= sprite->frameDelay) {
        sprite->currentFrame = (sprite->currentFrame + 1) % sprite->frameCount;
        sprite->frameTimer -= sprite->frameDelay;
    }

    LCDBitmap* bm = pd->graphics->getTableBitmap(sprite->table, sprite->currentFrame);
    if(bm == NULL) {
        pd->system->logToConsole("Error: Could not get bitmap for frame: %d", sprite->currentFrame);
        return;
    }

    pd->graphics->drawBitmap(bm, sprite->x + offset_x, sprite->y + offset_y, flip);
}

Sprite newSprite(const char *path, PlaydateAPI *pd, int x, int y) {
    LCDBitmap *bm = newBitmap(path, pd);
    Sprite s = {
        x,
        y,
        bm
    };
    return s;
}

Sprite newSpriteFromTable(LCDBitmapTable* table, int idx, PlaydateAPI *pd, int x, int y) {
    LCDBitmap* bm = pd->graphics->getTableBitmap(table, idx);
    Sprite s = {
        x,
        y,
        bm
    };
    return s;
}

AnimatedSprite newAnimatedSprite(const char *path, PlaydateAPI *pd, int x, int y, int frameCount, float frameDelay) {
    char buff[100];
    int width = 0, height = 0;
    
    if(sscanf(path, "%99[^-]-table-%d-%d", buff, &width, &height) != 3) {
        pd->system->logToConsole("Error parsing path: %s", path);
        return (AnimatedSprite){0};
    }

    LCDBitmapTable *bmt = newBitmapTable(buff, pd);
    if (bmt == NULL) {
        pd->system->logToConsole("Error: could not create BitmapTable for path: %s", buff);
        return (AnimatedSprite){0};
    }
    
    AnimatedSprite s;
    s.x = x;
    s.y = y;
    s.table = bmt;
    s.frameCount = frameCount;
    s.frameDelay = frameDelay;
    s.frameTimer = 0.0;
    s.currentFrame = 0;

    return s;
}

LCDBitmap* newBitmap(const char *path, PlaydateAPI *pd) {
    if(path == NULL) {
        pd->system->logToConsole("Error: the path for a new Bitmap is NULL");
        return NULL;
    }

    const char *err = NULL;
    LCDBitmap* bm = pd->graphics->loadBitmap(path, &err);
    if(err != NULL) {
        pd->system->logToConsole("Error while loading Bitmap: %s, %s", path, err);
        return NULL;
    }

    return bm;
}

LCDBitmapTable* newBitmapTable(const char* path, PlaydateAPI *pd){
    if(path == NULL) {
        pd->system->logToConsole("Error: the path for a new BitmapTable is NULL");
        return NULL;
    }

    const char *err;
    LCDBitmapTable* bTable = pd->graphics->loadBitmapTable(path, &err);

    if(bTable == NULL) {
        pd->system->logToConsole("Error while loading BitmapTable: %s, %s", path, err);
    }

    return bTable;
}