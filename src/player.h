#ifndef PLAYER_H_
#define PLAYER_H_

#include <stdbool.h>

#include "inventory.h"
#include "sprite.h"

#define MAX_PLAYER_COUNT 3
#define TOTAL_PLAYER_COUNT 10
#define MOVE_DURATION 0.2f

typedef struct {
    int startX, startY;
    int targetX, targetY;
    float elapsedTime;
    bool isMoving, justFinished;
} PlayerMovement;

typedef struct {
    int id;
    char* name;
    int health;
    int maxHealth;
    int power;
    Inventory inventory; 
    AnimatedSprite sprite;
    PlayerMovement movement;
} Player;

typedef struct {
    Player chars[MAX_PLAYER_COUNT];
    int current_index;
} PlayerList;


typedef struct {
    Player chars[TOTAL_PLAYER_COUNT];
    int current_index;
} PlayerPreviewList;


#endif