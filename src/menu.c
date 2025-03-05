#include "menu.h"

void printHello(PlaydateAPI* pd) {
	pd->system->logToConsole("HELLO");
}

void menu_init(Menu* menu, Menutype type, ItemList list) {
    menu->buttonCount = 0;
    menu->spriteCount = 0;
    menu->animatedSpriteCount = 0;
    menu->selectedButtonIndex = 0;

    switch (type) {
    case START:
        menu_add_button(menu, "Test1", 50, 150, 100, 30, printHello);
        menu_add_button(menu, "Test2", 50, 200, 100, 30, printHello);
        menu_add_button(menu, "Test3", 250, 150, 100, 30, printHello);
        menu_add_button(menu, "Test4", 250, 200, 100, 30, printHello);
        break;
    
    case FIGHT:
        menu_add_button(menu, "Ability1", 50, 150, 100, 30, printHello);
        menu_add_button(menu, "Ability2", 50, 200, 100, 30, printHello);
        menu_add_button(menu, "Ability3", 250, 150, 100, 30, printHello);
        menu_add_button(menu, "Ability4", 250, 200, 100, 30, printHello);
        break;

    case ITEMS:
        menu_add_button(menu, list.items[0].name, 50, 150, 100, 30, printHello);
        menu_add_button(menu, list.items[1].name, 50, 200, 100, 30, printHello);
        menu_add_button(menu, list.items[2].name, 250, 150, 100, 30, printHello);
        menu_add_button(menu, list.items[3].name, 250, 200, 100, 30, printHello);

    default:
        break;
    }
}

void menu_add_button(Menu* menu, const char *txt, int x, int y, int width, int height, ButtonCallback action) {
    if (menu->buttonCount >= MAX_BUTTONS) return;

    MenuButton* btn = &menu->buttons[menu->buttonCount++];
    strncpy(btn->text, txt, sizeof(btn->text) - 1);
    btn->text[sizeof(btn->text) - 1] = '\0';
    btn->x = x;
    btn->y = y;
    btn->width = width;
    btn->height = height;
    btn->action = action;
    btn->selected = menu->buttonCount == 1;
}

void menu_add_sprite(Menu* menu, Sprite s) {
    if(menu->spriteCount < MAX_SPRITES) {
        menu->sprites[menu->spriteCount++] = s;
    }
}

void menu_add_animated_sprite(Menu* menu, AnimatedSprite s) {
    if(menu->animatedSpriteCount < MAX_SPRITES) {
        menu->animatedSprites[menu->animatedSpriteCount++] = s;
    }
}

void menu_draw(Menu* menu, PlaydateAPI *pd) {
    for (int i = 0; i < menu->spriteCount; i++) {
        pd->graphics->drawBitmap(menu->sprites[i].bm, menu->sprites[i].x, menu->sprites[i].y, kBitmapUnflipped);
    }

    for (int i = 0; i < menu->animatedSpriteCount; i++) {
        AnimatedSprite* anim = &menu->animatedSprites[i];
        pd->graphics->drawBitmap(pd->graphics->getTableBitmap(anim->table, anim->currentFrame),
                                 anim->x, anim->y, kBitmapUnflipped);

        anim->frameTimer++;
        if (anim->frameTimer >= anim->frameDelay) {
            anim->frameTimer = 0;
            anim->currentFrame = (anim->currentFrame + 1) % anim->frameCount;
        }
    }

    for (int i = 0; i < menu->buttonCount; i++) {
        MenuButton* btn = &menu->buttons[i];

        pd->graphics->fillRect(btn->x - 2, btn->y - 2, btn->width + 4, btn->height + 4, kColorWhite);
        pd->graphics->drawRect(btn->x, btn->y, btn->width, btn->height, kColorBlack);
        pd->graphics->drawText(btn->text, strlen(btn->text), kASCIIEncoding, btn->x + 5, btn->y + 5);

        if (btn->selected) {
            pd->graphics->fillRect(btn->x - 2, btn->y - 2, btn->width + 4, btn->height + 4, kColorXOR);
        }
    }
}

void menu_handle_input(Menu* menu, PDButtons buttons, PlaydateAPI* pd) {
    if (buttons & kButtonUp) {
        menu->selectedButtonIndex = (menu->selectedButtonIndex - 1 + menu->buttonCount) % menu->buttonCount;
    }
    if (buttons & kButtonDown) {
        menu->selectedButtonIndex = (menu->selectedButtonIndex + 1) % menu->buttonCount;
    }

    for (int i = 0; i < menu->buttonCount; i++) {
        menu->buttons[i].selected = (i == menu->selectedButtonIndex);
    }

    if (buttons & kButtonA) {
        if (menu->buttons[menu->selectedButtonIndex].action) {
            menu->buttons[menu->selectedButtonIndex].action(pd);
        }
    }
}
