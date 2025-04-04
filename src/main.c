#define TARGET_EXTENSION 1

#include "pd_api.h"

#include <stdint.h>

#include "jsonparser.h"
#include "inventory.h"
#include "game.h"
#include "overworld.h"
#include "fight.h"
#include "attack.h"

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

		for(int i = 0; i < 16; i++) {
			game.chars[0].inventory.items[i] = game.itemlist.items[i];
		}

		game.chars[0].sprite = newAnimatedSprite("images/playeridle", pd, 0, 0, 32, 32, 4, 0.25);
		game.chars[0].inventory.count = 16;
		game.chars[0].name = "Knight";
		game.chars[0].health = 100;
		game.chars[0].maxHealth = 100;
		game.chars[0].inventory.curr_pos = 0;
		game.chars[0].movement = (PlayerMovement){0};
		game.chars[0].dir = 0;
		game.chars[0].fight_y = 1;
		game.chars[0].attacks[0] = knight_attacks[0];
		game.chars[0].attacks[1] = knight_attacks[1];
		game.chars[0].attacks[2] = knight_attacks[2];
		game.chars[0].attacks[3] = knight_attacks[3];
		game.chars[0].attack_count = 4;

		game.chars[1].sprite = newAnimatedSprite("images/mage", pd, 0, 0, 32, 32, 4, 0.25);
		game.chars[1].inventory.count = 16;
		game.chars[1].name = "Mage";
		game.chars[1].health = 50;
		game.chars[1].maxHealth = 100;
		game.chars[1].inventory.curr_pos = 0;
		game.chars[1].movement = (PlayerMovement){0};
		game.chars[1].dir = 0;
		game.chars[1].fight_y = 0;
		game.chars[1].attacks[0] = mage_attacks[0];
		game.chars[1].attacks[1] = mage_attacks[1];
		game.chars[1].attacks[2] = mage_attacks[2];
		game.chars[1].attacks[3] = mage_attacks[3];
		game.chars[1].attack_count = 4;

		game.chars[2].sprite = newAnimatedSprite("images/hunter", pd, 0, 0, 32, 32, 4, 0.25);
		game.chars[2].inventory.count = 16;
		game.chars[2].name = "Hunter";		
		game.chars[2].health = 10;
		game.chars[2].maxHealth = 100;
		game.chars[2].inventory.curr_pos = 0;
		game.chars[2].movement = (PlayerMovement){0};
		game.chars[2].dir = 0;
		game.chars[2].fight_y = 2;
		game.chars[2].attacks[0] = hunter_attacks[0];
		game.chars[2].attacks[1] = hunter_attacks[1];
		game.chars[2].attacks[2] = hunter_attacks[2];
		game.chars[2].attacks[3] = hunter_attacks[3];
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

		b_params.pd = pd;
		b_params.players[0] = &game.chars[0];
		b_params.players[1] = &game.chars[1];
		b_params.players[2] = &game.chars[2];
		b_params.enemies[0] = enemies[0];
		b_params.enemies[1] = enemies[1];
		b_params.enemies[2] = enemies[2];
		b_params.enemyCount = 3;
		b_params.currentState = PLAYER_MENU;
		b_params.nextState = PLAYER_MENU;
		b_params.currSequencePos = 0;
		b_params.selectX = 0;
		b_params.selectY = 0;
		b_params.selectPlayerIndex = -1;
		b_params.activePlayerIndex = 0;
		b_params.menuIndex = 0;
		b_params.exitMenu = false;
		b_params.enterMenu = false;
		b_params.menuOffset = 0.0;
		b_params.characterInfoBitmap = newBitmap("images/characterinfo", pd);
		b_params.characterInfoOffset = 84;
		b_params.selectorIcons = newBitmapTable("images/select", pd);
		b_params.enemySprites = newBitmapTable("images/monsters", pd);
		b_params.infoTargetPlayer = NULL;

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