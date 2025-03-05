#include "scenemanager.h"

void changeScene(Scenemanager *manager, Scenetypes type, void* sceneParams) {
    
    if(manager->OnSceneExit) {
        manager->OnSceneExit(manager->params.data);
    }

    manager->params.type = type;
    manager->params.data = sceneParams;
    
    switch (type) {
        
        case OVERWORLD:
            manager->OnSceneEnter = overworldEnter;
            manager->OnSceneExit = overworldExit;
            manager->UpdateScene = overworldUpdate;
            manager->DrawScene = overworldDraw;
            break;

        case INVENTORY:
            manager->OnSceneEnter = invOnEnter;
            manager->OnSceneExit = invOnExit;
            manager->UpdateScene = invUpdate;
            manager->DrawScene = invDraw;
            break;

        case BATTLE:
            manager->OnSceneEnter = battleOnEnter;
            manager->OnSceneExit = battleOnExit;
            manager->UpdateScene = battleUpdate;
            manager->DrawScene = battleDraw;
            break;

        case MENU:
            manager->OnSceneEnter = NULL;
            manager->OnSceneExit = NULL;
            manager->UpdateScene = NULL;
            manager->DrawScene = NULL;

        default:
            break;
    }

    if(manager->OnSceneEnter) {
        manager->OnSceneEnter(sceneParams);
    }
}

void updateScene(Scenemanager manager, float dt, PlaydateAPI* pd) {
    if(manager.UpdateScene) {
        manager.UpdateScene(manager.params.data, dt);
    } 
}

void drawScene(Scenemanager manager, PlaydateAPI* pd) {
    if(manager.DrawScene) {
        manager.DrawScene(manager.params.data);
    }
}