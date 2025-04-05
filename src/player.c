#include "player.h"

int takeDamage(Player* player, AttackEnemy attack) {
    player->health -= attack.damage;
    return attack.damage;
}
