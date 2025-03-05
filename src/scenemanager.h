#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <stdbool.h>
#include <stddef.h>

#include "inventory.h"
#include "overworld.h"
#include "fight.h"

typedef enum {
    MENU,
    OVERWORLD,
    INVENTORY,
    BATTLE
} Scenetypes;

typedef struct {
    Scenetypes type;
    void* data;
} SceneParams;

typedef struct {
    void (*OnSceneEnter)(void* params);
    void (*OnSceneExit)(void* params);
    void (*UpdateScene)(void* params, float dt);
    void (*DrawScene)(void* params);
    SceneParams params;
} Scenemanager;

void changeScene(Scenemanager *manager, Scenetypes type, void* sceneParams);
void updateScene(Scenemanager manager, float dt, PlaydateAPI* pd);
void drawScene(Scenemanager manager, PlaydateAPI* pd);

#endif