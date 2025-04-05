#include "sprite.h"

void drawSprite(Sprite sprite, PlaydateAPI *pd, int offset_x, int offset_y, LCDBitmapFlip flip) {
    if(!sprite.bm) {
        pd->system->logToConsole("Error: no bm");
        return;
    }
    pd->graphics->drawBitmap(sprite.bm, sprite.x + offset_x, sprite.y + offset_y, flip);
}

void drawAnimatedSprite(AnimatedSprite* sprite, PlaydateAPI* pd, int offset_x, int offset_y, bool offsetAsPos, LCDBitmapFlip flip, float dt) {
    if (sprite == NULL) {
        pd->system->logToConsole("Error: Invalid AnimatedSprite");
        return;
    }
    if(sprite->table == NULL) {
        pd->system->logToConsole("Error: Invalid BitmapTable (animated sprite)");
        return;
    }

    sprite->frameTimer += dt;
    if(sprite->frameTimer >= sprite->frameDelay) {
        sprite->currentFrame = (sprite->currentFrame + 1) % sprite->frameCount;
        sprite->frameTimer -= sprite->frameDelay;
    }

    LCDBitmap* bm = pd->graphics->getTableBitmap(sprite->table, sprite->currentFrame);
    if(bm == NULL) {
        pd->system->logToConsole("Error: Could not get bitmap for frame: %d", sprite->currentFrame);
        return;
    }

    if(offsetAsPos) {
        pd->graphics->drawBitmap(bm, offset_x, offset_y, flip);
    }else {
        pd->graphics->drawBitmap(bm, sprite->x + offset_x, sprite->y + offset_y, flip);
    }
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

AnimatedSprite newAnimatedSprite(const char *path, PlaydateAPI *pd, int x, int y, int width, int height, int frameCount, float frameDelay) {
    LCDBitmapTable *bmt = newBitmapTable(path, pd);
    if (bmt == NULL) {
        pd->system->logToConsole("Error: could not create BitmapTable for path: %s", path);
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