#ifndef MENU_H_
#define MENU_H_

#include "pd_api.h"
#include "sprite.h"
#include "item.h"

#include <string.h>
#include <stdbool.h>

#define MAX_BUTTONS 10
#define MAX_SPRITES 10

typedef void (*ButtonCallback)(PlaydateAPI* pd);

typedef enum {
    START,
    FIGHT,
    ITEMS,
} Menutype;

typedef struct {
    char text[32];
    int x, y, width, height;
    ButtonCallback action;
    bool selected;
} MenuButton;

typedef struct Menu_S {
    int buttonCount;
    MenuButton buttons[MAX_BUTTONS];

    int spriteCount;
    Sprite sprites[MAX_SPRITES];

    int animatedSpriteCount;
    AnimatedSprite animatedSprites[MAX_SPRITES];

    int selectedButtonIndex;
} Menu;

void menu_init(Menu* menu, Menutype type, ItemList list);
void menu_add_button(Menu* menu, const char *txt, int x, int y, int width, int height, ButtonCallback action);
void menu_add_sprite(Menu* menu, Sprite s);
void menu_add_animated_sprite(Menu* menu, AnimatedSprite s);
void menu_draw(Menu* menu, PlaydateAPI *pd);
void menu_handle_input(Menu* menu, PDButtons buttons, PlaydateAPI* pd);


#endif