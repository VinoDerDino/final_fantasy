#ifndef PLAYER_H_
#define PLAYER_H_

#include "inventory.h"

#define MAX_NAME_LENGTH 25
#define MAX_PLAYER_COUNT 3
#define TOTAL_PLAYER_COUNT 10

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    int health;
    int maxHealth;
    int power;
    Inventory inventory;
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