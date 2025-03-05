#include "overworld.h"

void overworldEnter(void* params) {
    OverworldParams* worldParams = (OverworldParams*)params;
    PlaydateAPI* pd = worldParams->pd;
    pd->graphics->clear(kColorWhite);

    pd->system->logToConsole("New Scene Overworld");
    overworldDraw(params);
}

void overworldExit(void* params) {
}

void updateCamera(Camera* camera, Player* player, World* world) {
    camera->x = player->sprite.x - SCREEN_HALF_WIDTH;
    camera->y = player->sprite.y - 112;
    camera->maxX = 32 * world->width - SCREEN_WIDTH;
    camera->maxY = 32 * world->height - 224;
    
    if (camera->x < 0)         camera->x = 0;
    else if (camera->x > camera->maxX) camera->x = camera->maxX;
    
    if (camera->y < 0)         camera->y = 0;
    else if (camera->y > camera->maxY) camera->y = camera->maxY;
}

void drawTileInd(World* world, int tile_id, PlaydateAPI* pd, bool clear) {
    LCDBitmap* s = pd->graphics->getTableBitmap(world->tile_sprites, world->tiles[tile_id]);
    int x = (int)(tile_id % world->width) * 32;
    int y = (int)(tile_id / world->width) * 32;
    if(clear) pd->graphics->fillRect(x, y, 32, 32, kColorWhite);
    pd->graphics->drawBitmap(s, x, y, kBitmapUnflipped);
}

void drawPlayer(Player* player, PlaydateAPI* pd, float dt) {
    drawAnimatedSprite(&player->sprite, pd, 0, 0, 1.0, dt);
}

void drawMap(Camera* camera, World* world, PlaydateAPI* pd) {
    int tileSize = 32;
    int startCol = camera->x / tileSize;
    int startRow = camera->y / tileSize;
    int endCol = (camera->x + SCREEN_WIDTH) / tileSize;
    int endRow = (camera->y + 224) / tileSize;
    
    if (startCol < 0) startCol = 0;
    if (startRow < 0) startRow = 0;
    if (endCol >= world->width) endCol = world->width - 1;
    if (endRow >= world->height) endRow = world->height - 1;
    
    pd->graphics->clear(kColorWhite);

    for (int j = startRow; j <= endRow; j++) {
        for (int i = startCol; i <= endCol; i++) {
            int tile_id = i + j * world->width;
            drawTileInd(world, tile_id, pd, false);
        }
    }
}

void processPlayerInput(Player* player, World* world, PlaydateAPI* pd, PDButtons btn_const, PDButtons btn_pressed) {  
    int dx = 0, dy = 0;
    if (btn_const & kButtonLeft)      dx = -32;
    else if (btn_const & kButtonRight)  dx = 32;
    else if (btn_const & kButtonUp)     dy = -32;
    else if (btn_const & kButtonDown)   dy = 32;

    // Check if the player is trying to move outside the map
    int new_x = player->sprite.x + dx;
    int new_y = player->sprite.y + dy;
    if (new_x < 0 || new_y < 0 || new_x > (world->width - 1) * 32 || new_y > (world->height - 1) * 32) {
        return; // Do not move the player if the new position is outside the map
    }

    if (dx || dy) {
        player->movement.startX = player->sprite.x;
        player->movement.startY = player->sprite.y;
        player->movement.targetX = player->sprite.x + dx;
        player->movement.targetY = player->sprite.y + dy;
        player->movement.isMoving = 1;
        player->movement.elapsedTime = 0;
    }
}

void drawHub(Player* player, Camera* camera, World* world, int tile_id, PlaydateAPI* pd) {
    pd->graphics->fillRect(0 + camera->x, 224 + camera->y, 400, 16, kColorBlack);

    int type = world->tiles[tile_id];
    const char* s[] = {"Trees", "City", "Village", "Grass", "Plane"};

    LCDBitmapDrawMode old = pd->graphics->setDrawMode(kColorXOR);
    pd->graphics->drawText(s[type], strlen(s[type]), kASCIIEncoding, 20 + camera->x, 224 + camera->y);
    pd->graphics->setDrawMode(old);
}

void handle_info_overview(void* params, float dt) {
    OverworldParams* worldParams = (OverworldParams*)params;
    PlaydateAPI* pd = worldParams->pd;
    Camera* camera = &worldParams->camera;
    World* world = worldParams->world;
    Player* player = worldParams->player;
    LCDBitmap* select = worldParams->select;

    
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if(btn_pressed & kButtonA) {
        worldParams->check_info = false;
        pd->system->logToConsole("Switched back to normal view");
        return;
    }

    int tile_ind = (int)(worldParams->selectX / 32) + (int)(worldParams->selectY / 32) * world->width;
    drawTileInd(world, tile_ind, pd, true);

    int dx = 0, dy = 0;
    if (btn_pressed & kButtonLeft)      dx = -32;
    else if (btn_pressed & kButtonRight)  dx = 32;
    else if (btn_pressed & kButtonUp)     dy = -32;
    else if (btn_pressed & kButtonDown)   dy = 32;
    
    if (dx || dy) {
        worldParams->selectX += dx;
        worldParams->selectY += dy;
    }

    drawPlayer(player, pd, dt);
    pd->graphics->drawBitmap(select, worldParams->selectX, worldParams->selectY, kBitmapUnflipped);
}

void overworldUpdate(void* params, float dt) {
    OverworldParams* worldParams = (OverworldParams*)params;
    PlaydateAPI* pd = worldParams->pd;
    Camera* camera = &worldParams->camera; 
    World* world = worldParams->world;
    Player* player = worldParams->player;
    bool check_info = worldParams->check_info;

    if(check_info) {
        handle_info_overview(params, dt);
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
        }
    }
    else {
        PDButtons btn_const, btn_pressed;
        pd->system->getButtonState(&btn_const, &btn_pressed, NULL);
        if(btn_pressed & kButtonA) {
            worldParams->check_info = true;
            pd->system->logToConsole("Switched back to info view");
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
    int tile_id = (int)((player->sprite.x + 16) / 32) + (int)((player->sprite.y + 16) / 32) * world->width;
    drawHub(player, camera, world, tile_id, pd);
}

void overworldDraw(void* params) {
    OverworldParams* worldParams = (OverworldParams*) params;
    PlaydateAPI* pd = worldParams->pd;
    World* world = worldParams->world;

    for (int i = 0; i < world->width * world->height; i++) {
        drawTileInd(world, i, pd, false);
    }
}
