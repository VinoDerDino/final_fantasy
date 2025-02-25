#include <stdio.h>
#include <stdlib.h>

#define TARGET_EXTENSION 1

#include "pd_api.h"

#include "menu.h"
#include "jsonparser.h"
#include "inventory.h"
#include "game.h"

static int update(void* userdata);

PlaydateAPI *pd = NULL;
Game game;

#ifdef _WINDLL
__declspec(dllexport)
#endif

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
    (void)arg;
	pd = playdate;
    
	if (event == kEventInit) {
		pd->display->setRefreshRate(0);

		game.itemtable = newBitmapTable("images/swag", pd, 50, 16, 16);
		decodeItems("jsons/test.json", &game.itemlist);

		game.chars[0].inventory.items[0] = game.itemlist.items[0];
		game.chars[0].inventory.items[1] = game.itemlist.items[1];
		game.chars[0].inventory.items[2] = game.itemlist.items[2];
		game.chars[0].inventory.items[3] = game.itemlist.items[3];
		game.chars[0].inventory.items[4] = game.itemlist.items[4];
		game.chars[0].inventory.items[5] = game.itemlist.items[5];
		game.chars[0].inventory.items[6] = game.itemlist.items[6];
		game.chars[0].inventory.items[7] = game.itemlist.items[7];
		game.chars[0].inventory.items[8] = game.itemlist.items[8];
		game.chars[0].inventory.items[9] = game.itemlist.items[9];
		game.chars[0].inventory.items[10] = game.itemlist.items[10];
		game.chars[0].inventory.items[11] = game.itemlist.items[11];
		game.chars[0].inventory.items[12] = game.itemlist.items[12];
		game.chars[0].inventory.items[13] = game.itemlist.items[13];
		game.chars[0].inventory.items[14] = game.itemlist.items[14];
		game.chars[0].inventory.items[15] = game.itemlist.items[15];

		game.chars[0].inventory.count = 16;
		game.chars[0].inventory.curr_pos = 0;

		InventoryParams params = {
			.inv = &game.chars[0].inventory,
			.pd = pd,
			.selectedItem = 1
		};
		
		changeScene(&game.scenemanager, INVENTORY, &params);

        pd->system->setUpdateCallback(update, pd);
    } 
     
    return 0;
}

static int update(void* userdata) {

	updateScene(game.scenemanager);
	drawScene(game.scenemanager);

    return 1;
}