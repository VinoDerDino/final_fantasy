#include "sprite.h"

void drawSprite(Sprite sprite, PlaydateAPI *pd, int offset_x, int offset_y, float scale) {
    pd->graphics->drawScaledBitmap(sprite.bm, sprite.x + offset_x, sprite.y + offset_y, scale, scale);
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

AnimatedSprite newAnimatedSprite(const char *path, PlaydateAPI *pd, int x, int y, int frameCount, int frameDelay) {
    char buff[100];
    int width = 0, height = 0;
    
    if(sscanf(path, "%99[^-]-table-%d-%d", buff, &width, &height) != 3) {
        pd->system->logToConsole("Error parsing path: %s", path);
        return (AnimatedSprite){0};
    }

    LCDBitmapTable *bmt = newBitmapTable(buff, pd, frameCount, width, height);
    if (!bmt) return (AnimatedSprite){0};
    
    return (AnimatedSprite){ x, y, bmt, frameCount, 0, frameDelay, 0 };
}

LCDBitmap* newBitmap(const char *path, PlaydateAPI *pd) {
    if(path == NULL) {
        pd->system->logToConsole("Error: the path for a new Bitmap is NULL");
        return NULL;
    }

    const char *err;
    LCDBitmap* bm = pd->graphics->loadBitmap(path, &err);
    if(err) {
        pd->system->logToConsole("Error while loading Bitmap, %s", err);
        return NULL;
    }

    return bm;
}

LCDBitmapTable* newBitmapTable(const char* path, PlaydateAPI *pd, int count, int width, int height){
    LCDBitmapTable* bTable = pd->graphics->newBitmapTable(count, width, height);

    pd->graphics->loadIntoBitmapTable(path, bTable, NULL);
    if(!bTable) {
        pd->system->logToConsole("Error while loading BitmapTable: %s", path);
    }

    return bTable;
}