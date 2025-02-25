#include "scenemanager.h"

void changeScene(Scenemanager *manager, Scenetypes type, void* sceneParams) {
    if(manager->OnSceneExit) {
        manager->OnSceneExit(manager->params.data);
    }
    
    manager->params.type = type;
    manager->params.data = sceneParams;
    
    switch (type) {
        case MENU:
            manager->OnSceneEnter = NULL;
            manager->OnSceneExit = NULL;
            manager->UpdateScene = NULL;
            manager->DrawScene = NULL;
            break;
        
        case OVERWORLD:
            manager->OnSceneEnter = NULL;
            manager->OnSceneExit = NULL;
            manager->UpdateScene = NULL;
            manager->DrawScene = NULL;
            break;

        case INVENTORY:
            manager->OnSceneEnter = invOnEnter;
            manager->OnSceneExit = invOnExit;
            manager->UpdateScene = invUpdate;
            manager->DrawScene = invDraw;
            break;

        default:
            manager->OnSceneEnter = NULL;
            manager->OnSceneExit = NULL;
            manager->UpdateScene = NULL;
            manager->DrawScene = NULL;
            break;
    }

    if(manager->OnSceneEnter) {
        manager->OnSceneEnter(sceneParams);
    }
}

void updateScene(Scenemanager manager) {
    if(manager.UpdateScene) {
        manager.UpdateScene(manager.params.data);
    }
}

void drawScene(Scenemanager manager) {
    if(manager.DrawScene) {
        manager.DrawScene(manager.params.data);
    }
}