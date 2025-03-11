#define TARGET_EXTENSION 1

#include "pd_api.h"

#include <stdint.h>

#include "jsonparser.h"
#include "inventory.h"
#include "game.h"
#include "overworld.h"
#include "fight.h"

static int update(void* userdata);

PlaydateAPI *pd = NULL;
Game game;
World world;
Scenemanager manager;

InventoryParams params;
OverworldParams o_params;
BattleParams b_params;

#ifdef _WINDLL
__declspec(dllexport)
#endif

void menuChangeScene(void *userdata) {
	intptr_t type = (intptr_t)userdata;
	switch (type){
		case 1:
			changeScene(&manager, OVERWORLD, &o_params);
			break;
		
		case 2:
			changeScene(&manager, INVENTORY, &params);
			break;

		case 3:
			changeScene(&manager, BATTLE, &b_params);
			break;

		default:
			break;
	}
}

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
    (void)arg;
    
	if (event == kEventInit) {
		pd = playdate;
		srand(pd->system->getCurrentTimeMilliseconds());
		pd->display->setRefreshRate(0);
		game.itemtable = newBitmapTable("images/monsters", pd); 
		game.tiletable = newBitmapTable("images/grassybig", pd);
		decodeItems("jsons/test.json", &game.itemlist);

		game.chars[0].sprite = newAnimatedSprite("images/playeridle", pd, 0, 0, 32, 32, 4, 0.25);

		int count, cellswide;
		pd->graphics->getBitmapTableInfo(game.chars[0].sprite.table, &count, &cellswide);
		pd->system->logToConsole("INFO count: %d, cellswide: %d", count, cellswide);

		for(int i = 0; i < 16; i++) {
			game.chars[0].inventory.items[i] = game.itemlist.items[i];
		}

		game.chars[0].inventory.count = 16;
		game.chars[0].inventory.curr_pos = 0;
		game.chars[0].movement = (PlayerMovement){0};
		game.chars[0].dir = 0;
		game.chars[0].fight_x = 1;
		game.chars[0].fight_y = 1;
		game.chars[0].attacks[0].name = "player 0";
		game.chars[0].attacks[0].dmg = 5;
		game.chars[0].attacks[0].type = BASIC_ATTACK;
		game.chars[0].attacks[0].pattern = SINGLE_TARGET;
		game.chars[0].attacks[1].name = "2";
		game.chars[0].attacks[1].dmg = 5;
		game.chars[0].attacks[1].type = BASIC_ATTACK;
		game.chars[0].attacks[1].pattern = SINGLE_TARGET;
		game.chars[0].attacks[2].name = "3";
		game.chars[0].attacks[2].dmg = 5;
		game.chars[0].attacks[2].type = BASIC_ATTACK;
		game.chars[0].attacks[2].pattern = SINGLE_TARGET;
		game.chars[0].attacks[3].name = "4";
		game.chars[0].attacks[3].dmg = 5;
		game.chars[0].attacks[3].type = BASIC_ATTACK;
		game.chars[0].attacks[3].pattern = SINGLE_TARGET;
		game.chars[0].attack_count = 4;

		game.chars[1].sprite = newAnimatedSprite("images/playeridle", pd, 0, 0, 32, 32, 4, 0.25);
		game.chars[1].inventory.count = 16;
		game.chars[1].inventory.curr_pos = 0;
		game.chars[1].movement = (PlayerMovement){0};
		game.chars[1].dir = 0;
		game.chars[1].fight_x = 1;
		game.chars[1].fight_y = 0;
		game.chars[1].attacks[0].name = "player 1";
		game.chars[1].attacks[0].dmg = 5;
		game.chars[1].attacks[0].type = BASIC_ATTACK;
		game.chars[1].attacks[0].pattern = SINGLE_TARGET;
		game.chars[1].attacks[1].name = "2";
		game.chars[1].attacks[1].dmg = 5;
		game.chars[1].attacks[1].type = BASIC_ATTACK;
		game.chars[1].attacks[1].pattern = SINGLE_TARGET;
		game.chars[1].attacks[2].name = "3";
		game.chars[1].attacks[2].dmg = 5;
		game.chars[1].attacks[2].type = BASIC_ATTACK;
		game.chars[1].attacks[2].pattern = SINGLE_TARGET;
		game.chars[1].attacks[3].name = "4";
		game.chars[1].attacks[3].dmg = 5;
		game.chars[1].attacks[3].type = BASIC_ATTACK;
		game.chars[1].attacks[3].pattern = SINGLE_TARGET;
		game.chars[1].attack_count = 4;

		game.chars[2].sprite = newAnimatedSprite("images/mage", pd, 0, 0, 32, 32, 4, 0.25);
		game.chars[2].inventory.count = 16;
		game.chars[2].inventory.curr_pos = 0;
		game.chars[2].movement = (PlayerMovement){0};
		game.chars[2].dir = 0;
		game.chars[2].fight_x = 1;
		game.chars[2].fight_y = 2;
		game.chars[2].attacks[0].name = "Basic";
		game.chars[2].attacks[0].dmg = 5;
		game.chars[2].attacks[0].type = BASIC_ATTACK;
		game.chars[2].attacks[0].pattern = SINGLE_TARGET;
		game.chars[2].attacks[1].name = "Schwanz";
		game.chars[2].attacks[1].dmg = 5;
		game.chars[2].attacks[1].type = BASIC_ATTACK;
		game.chars[2].attacks[1].pattern = SINGLE_TARGET;
		game.chars[2].attacks[2].name = "Penis";
		game.chars[2].attacks[2].dmg = 5;
		game.chars[2].attacks[2].type = BASIC_ATTACK;
		game.chars[2].attacks[2].pattern = SINGLE_TARGET;
		game.chars[2].attacks[3].name = "Hoden";
		game.chars[2].attacks[3].dmg = 5;
		game.chars[2].attacks[3].type = BASIC_ATTACK;
		game.chars[2].attacks[3].pattern = SINGLE_TARGET;
		game.chars[2].attack_count = 4;

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
		o_params.manager = &manager;
		o_params.player = &game.chars[0];
		o_params.camera.x = o_params.player->sprite.x;
		o_params.camera.y = o_params.player->sprite.y;
		o_params.camera.maxX = 20 * 32;
		o_params.camera.maxY = 20 * 32;
		o_params.select = newBitmap("images/select_img", pd);

		b_params.chars[0] = &game.chars[0];
		pd->system->logToConsole("Game player 0: %s, Params player 0: %s", game.chars[0].attacks[0].name, b_params.chars[0]->attacks[0].name);
		b_params.chars[1] = &game.chars[1];
		pd->system->logToConsole("Game player 1: %s, Params player 1: %s", game.chars[1].attacks[0].name, b_params.chars[1]->attacks[0].name);
		b_params.chars[2] = &game.chars[2];
		pd->system->logToConsole("Game player 2: %s, Params player 2: %s", game.chars[2].attacks[0].name, b_params.chars[2]->attacks[0].name);
		b_params.pd = pd;
		b_params.selectX = 0;
		b_params.selectY = 0;
		b_params.select = newBitmapTable("images/select", pd);
		b_params.monsters = newBitmapTable("images/monsters", pd);
		b_params.state = PLAYER_TURN;
		b_params.countMonsters = 3;
		b_params.enemies[0] = enemies[0];
		b_params.enemies[1] = enemies[1];
		b_params.enemies[2] = enemies[2];

		manager.pendingSceneChange = PENDING;

		changeScene(&manager, OVERWORLD, &o_params);

		pd->system->addMenuItem("Overworld", menuChangeScene, (void*)1);
		pd->system->addMenuItem("Inventory", menuChangeScene, (void*)2);
		pd->system->addMenuItem("Battle", menuChangeScene, (void*)3);

        pd->system->setUpdateCallback(update, pd);
    } 
     
    return 0;
}

static int update(void* userdata) {

	float currTime = pd->system->getCurrentTimeMilliseconds() / 1000.0f;
	float dt = currTime - game.lastFrameTime;
	game.lastFrameTime = currTime;

	updateScene(&manager, dt, pd);
	pd->system->drawFPS(0, 230);

	return 1;
} 