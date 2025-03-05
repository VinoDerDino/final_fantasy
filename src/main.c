#define TARGET_EXTENSION 1

#include "pd_api.h"

#include "menu.h"
#include "jsonparser.h"
#include "inventory.h"
#include "game.h"
#include "overworld.h"
#include "fight.h"

static int update(void* userdata);

PlaydateAPI *pd = NULL;
Game game;
World world;
LCDFont* font = NULL;

AnimatedSprite tree1;
AnimatedSprite tree2;
AnimatedSprite tree3;
AnimatedSprite tree4;

InventoryParams params;
OverworldParams o_params;
BattleParams b_params;

AnimatedSprite player_idle;

AudioSample* sample;

#ifdef _WINDLL
__declspec(dllexport)
#endif

void menuChangeScene(void *userdata) {
	int type = (int)userdata;
	switch (type)
	{
	case 1:
		changeScene(&game.scenemanager, OVERWORLD, &o_params);
		break;
	
	case 2:
		changeScene(&game.scenemanager, INVENTORY, &params);
		break;

	case 3:
		changeScene(&game.scenemanager, BATTLE, &b_params);
		break;

	default:
		break;
	}
}

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
    (void)arg;
    
	if (event == kEventInit) {
		pd = playdate;
		pd->display->setRefreshRate(0);
		game.itemtable = newBitmapTable("images/monsters", pd); 
		game.tiletable = newBitmapTable("images/grassybig", pd);
		decodeItems("jsons/test.json", &game.itemlist);

		game.chars[0].sprite = newAnimatedSprite("images/playeridle-table-32-32", pd, 0, 0, 4, 0.25);

		int count, cellswide;
		pd->graphics->getBitmapTableInfo(game.chars[0].sprite.table, &count, &cellswide);
		pd->system->logToConsole("INFO count: %d, cellswide: %d", count, cellswide);

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
		game.chars[0].movement = (PlayerMovement){0};

		game.lastFrameTime = 0;

		params.inv = &game.chars[0].inventory;
		params.pd = pd;
		params.selectedItem = 1;

		world.width = 20;
		world.height = 20;
		for(int i = 0; i < world.width; i++) {
			for(int j = 0; j < world.height; j++) {
				world.tiles[i + j * world.width] = i * j % 5;
			}
		}
		world.tile_sprites = newBitmapTable("images/grassybig", pd);
		
		o_params.pd = pd;
		o_params.world = &world;
		o_params.player = &game.chars[0];
		o_params.camera.x = o_params.player->sprite.x;
		o_params.camera.y = o_params.player->sprite.y;
		o_params.camera.maxX = 20 * 32;
		o_params.camera.maxY = 20 * 32;
		o_params.select = newBitmap("images/select", pd);

		b_params.pd = pd;
		b_params.selectX = 0;
		b_params.selectY = 0;
		b_params.select = newBitmapTable("images/select", pd);
		b_params.monsters = game.itemtable;

		changeScene(&game.scenemanager, OVERWORLD, &o_params);

		tree1 = newAnimatedSprite("images/tree-table-32-32", pd, 0, 0, 4, 0.25);
		tree2 = newAnimatedSprite("images/tree-table-32-32", pd, 32, 0, 4, 0.25);
		tree3 = newAnimatedSprite("images/tree-table-32-32", pd, 0, 32, 4, 0.25);
		tree4 = newAnimatedSprite("images/tree-table-32-32", pd, 32, 32, 4, 0.25);

		pd->system->addMenuItem("Overworld", menuChangeScene, (void*)1);
		pd->system->addMenuItem("Inventory", menuChangeScene, (void*)2);
		pd->system->addMenuItem("Fight", menuChangeScene, (void*)3);

        pd->system->setUpdateCallback(update, pd);
    } 
     
    return 0;
}

static int update(void* userdata) {

	float currTime = pd->system->getCurrentTimeMilliseconds() / 1000.0f;
	float dt = currTime - game.lastFrameTime;
	game.lastFrameTime = currTime;

	updateScene(game.scenemanager, dt, pd);
	pd->system->drawFPS(0, 230);

    return 1;
} 