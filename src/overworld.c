#include "overworld.h"
#include <string.h>

#define TILE_SIZE 32

static inline int clamp(int value, int min, int max) {
    return (value < min) ? min : (value > max ? max : value);
}

void overworldEnter(void* params) {
    OverworldParams* worldParams = (OverworldParams*)params;
    PlaydateAPI* pd = worldParams->pd;
    Camera* camera = &worldParams->camera;

    pd->graphics->setDrawOffset(-camera->x, -camera->y);
    pd->graphics->clear(kColorWhite);

    overworldDraw(params);
}

void overworldExit(void* params) {
    OverworldParams* worldParams = (OverworldParams*)params;
    PlaydateAPI* pd = worldParams->pd;

    pd->graphics->setDrawOffset(0, 0);
}

void updateCamera(Camera* camera, Player* player, World* world) {
    camera->x = player->sprite.x - SCREEN_HALF_WIDTH;
    camera->y = player->sprite.y - 112;
    camera->maxX = TILE_SIZE * world->width - SCREEN_WIDTH;
    camera->maxY = TILE_SIZE * world->height - 224;
    
    camera->x = clamp(camera->x, 0, camera->maxX);
    camera->y = clamp(camera->y, 0, camera->maxY);
}

void drawTileInd(const World* world, int tileIndex, PlaydateAPI* pd, bool clear) {
    LCDBitmap* spriteBitmap = pd->graphics->getTableBitmap(world->tile_sprites, world->tiles[tileIndex]);
    int x = (tileIndex % world->width) * TILE_SIZE;
    int y = (tileIndex / world->width) * TILE_SIZE;
    
    if (clear) {
        pd->graphics->fillRect(x, y, TILE_SIZE, TILE_SIZE, kColorWhite);
    }
    pd->graphics->drawBitmap(spriteBitmap, x, y, kBitmapUnflipped);
}

void drawPlayer(Player* player, PlaydateAPI* pd, float dt) {
    drawAnimatedSprite(&player->sprite, pd, 0, 0, false, player->dir, dt);
}

void drawMap(Camera* camera, const World* world, PlaydateAPI* pd) {
    int startCol = camera->x / TILE_SIZE;
    int startRow = camera->y / TILE_SIZE;
    int endCol = (camera->x + SCREEN_WIDTH) / TILE_SIZE;
    int endRow = (camera->y + 224) / TILE_SIZE;
    
    if (startCol < 0) startCol = 0;
    if (startRow < 0) startRow = 0;
    if (endCol >= world->width)  endCol = world->width - 1;
    if (endRow >= world->height) endRow = world->height - 1;
    
    pd->graphics->clear(kColorWhite);
    for (int row = startRow; row <= endRow; row++) {
        for (int col = startCol; col <= endCol; col++) {
            int tileIndex = col + row * world->width;
            drawTileInd(world, tileIndex, pd, false);
        }
    }
}

bool checkForFight(Player* player, World* world) {
    int tileIndex = ((int)(player->sprite.x + 16) / TILE_SIZE) + ((int)(player->sprite.y + 16) / TILE_SIZE) * world->width;
    return world->tiles[tileIndex] == 4 && (rand() % 4) == 0;
}

void processPlayerInput(Player* player, const World* world, PlaydateAPI* pd, PDButtons btn_const, PDButtons btn_pressed) {  
    int dx = 0, dy = 0;
    
    if (btn_const & kButtonLeft) {
        dx = -TILE_SIZE;
        player->dir = 1;
    } else if (btn_const & kButtonRight) {
        dx = TILE_SIZE;
        player->dir = 0;
    } else if (btn_const & kButtonUp) {
        dy = -TILE_SIZE;
    } else if (btn_const & kButtonDown) {
        dy = TILE_SIZE;
    }

    int new_x = player->sprite.x + dx;
    int new_y = player->sprite.y + dy;
    if (new_x < 0 || new_y < 0 || new_x > (world->width - 1) * TILE_SIZE || new_y > (world->height - 1) * TILE_SIZE) {
        return;
    }

    if (dx || dy) {
        player->movement.startX = player->sprite.x;
        player->movement.startY = player->sprite.y;
        player->movement.targetX = new_x;
        player->movement.targetY = new_y;
        player->movement.isMoving = 1;
        player->movement.elapsedTime = 0;

    }
}

void drawHub(Player* player, Camera* camera, const World* world, int tileIndex, PlaydateAPI* pd) {
    pd->graphics->fillRect(camera->x, 224 + camera->y, 400, 16, kColorBlack);

    int type = world->tiles[tileIndex];
    const char* typeNames[] = {"Trees", "City", "Village", "Grass", "Plane"};

    LCDBitmapDrawMode oldMode = pd->graphics->setDrawMode(kColorXOR);
    pd->graphics->drawText(typeNames[type], strlen(typeNames[type]), kASCIIEncoding, 20 + camera->x, 224 + camera->y);
    pd->graphics->setDrawMode(oldMode);
}

void handleInfoOverview(void* params, float dt) {
    OverworldParams* worldParams = (OverworldParams*)params;
    PlaydateAPI* pd = worldParams->pd;
    World* world = worldParams->world;
    Player* player = worldParams->player;
    LCDBitmap* selectBitmap = worldParams->select;
    
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if (btn_pressed & kButtonA) {
        worldParams->check_info = false;
        return;
    }

    int tileIndex = (int)(worldParams->selectX / TILE_SIZE) + (int)(worldParams->selectY / TILE_SIZE) * world->width;
    drawTileInd(world, tileIndex, pd, true);

    int dx = 0, dy = 0;
    if (btn_pressed & kButtonLeft) {
        dx = -TILE_SIZE;
    } else if (btn_pressed & kButtonRight) {
        dx = TILE_SIZE;
    } else if (btn_pressed & kButtonUp) {
        dy = -TILE_SIZE;
    } else if (btn_pressed & kButtonDown) {
        dy = TILE_SIZE;
    }
    
    drawPlayer(player, pd, dt);

    int newSelectX = worldParams->selectX + dx;
    int newSelectY = worldParams->selectY + dy;
    if (newSelectX < 0 || newSelectY < 0 || newSelectX > (world->width - 1) * TILE_SIZE || newSelectY > (world->height - 1) * TILE_SIZE) {
        return;
    }

    if (dx || dy) {
        worldParams->selectX = newSelectX;
        worldParams->selectY = newSelectY;
    }

    pd->graphics->drawBitmap(selectBitmap, worldParams->selectX, worldParams->selectY, kBitmapUnflipped);
}

void overworldUpdate(void* params, float dt) {
    OverworldParams* worldParams = (OverworldParams*)params;
    Scenemanager* manager = worldParams->manager;
    PlaydateAPI* pd = worldParams->pd;
    Camera* camera = &worldParams->camera; 
    World* world = worldParams->world;
    Player* player = worldParams->player;

    if (worldParams->check_info) {
        handleInfoOverview(params, dt);
        int tileIndex = ((int)(worldParams->selectX + 16) / TILE_SIZE) + ((int)(worldParams->selectY + 16) / TILE_SIZE) * world->width;
        drawHub(player, camera, world, tileIndex, pd);
        return;
    }

    if (player->movement.isMoving) {
        player->movement.elapsedTime += dt;
        float t = player->movement.elapsedTime / MOVE_DURATION;
        if (t > 1.0f) t = 1.0f;
        
        player->sprite.x = player->movement.startX + (int)((player->movement.targetX - player->movement.startX) * t);
        player->sprite.y = player->movement.startY + (int)((player->movement.targetY - player->movement.startY) * t);
        
        if (t >= 1.0f) {
            player->sprite.x = player->movement.targetX;
            player->sprite.y = player->movement.targetY;
            player->movement.justFinished = 1;
            if(checkForFight(player, world)) {
                BattleParams* battleParams = (BattleParams*)malloc(sizeof(BattleParams));
                if (battleParams != NULL) {
                    battleParams->chars[0] = player;
                    battleParams->chars[1] = player;
                    battleParams->chars[2] = player;
                    battleParams->pd = pd;
                    battleParams->selectX = 0;
                    battleParams->selectY = 0;
                    battleParams->select = newBitmapTable("images/select", pd);
                    battleParams->monsters = newBitmapTable("images/monsters", pd);
                    battleParams->countMonsters = 3;
                    battleParams->enemies[0] = enemies[0];
                    battleParams->enemies[1] = enemies[0];
                    battleParams->enemies[2] = enemies[0];
                    manager->pendingData = battleParams;
                    manager->pendingSceneChange = BATTLE;
                } else {
                    pd->system->logToConsole("Error: Could not allocate memory for BattleParams");
                }
            }
        }
    } else {
        PDButtons btn_const, btn_pressed;
        pd->system->getButtonState(&btn_const, &btn_pressed, NULL);
        if (btn_pressed & kButtonA) {
            worldParams->check_info = true;
            worldParams->selectX = player->sprite.x;
            worldParams->selectY = player->sprite.y;
            return;
        }
        processPlayerInput(player, world, pd, btn_const, btn_pressed);
    }

    if (player->movement.isMoving || player->movement.justFinished) {
        updateCamera(camera, player, world);
        pd->graphics->setDrawOffset(-camera->x, -camera->y);
        drawMap(camera, world, pd);
        
        if (player->movement.justFinished) {
            player->movement.isMoving = 0;
            player->movement.justFinished = 0;
        }
    }
    
    drawPlayer(player, pd, dt);
    int tileIndex = ((int)(player->sprite.x + 16) / TILE_SIZE) + ((int)(player->sprite.y + 16) / TILE_SIZE) * world->width;
    drawHub(player, camera, world, tileIndex, pd);
}

void overworldDraw(void* params) {
    OverworldParams* worldParams = (OverworldParams*)params;
    PlaydateAPI* pd = worldParams->pd;
    World* world = worldParams->world;

    int totalTiles = world->width * world->height;
    for (int i = 0; i < totalTiles; i++) {
        drawTileInd(world, i, pd, false);
    }
}
