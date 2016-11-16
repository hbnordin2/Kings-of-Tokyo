#include "playerFunctionality.h"

/**
 * @brief Keeps health dice if health is below 5, keeps attack dice if health
 * is above 5 or 5, rerolls everything else
 *
 * @param dice
 *      The dice this player rolled
 *
 * @param player
 *      This player
 */
void keep_dice(DiceContent* dice, Player* player) {
    if(player->health < 5) {
        dice->health = KEEP;
    } else {
        dice->attack = KEEP;
    }
}

/**
 * @brief Always leaves StLucia if asked
 *
 * @param command
 *      The command asking to stay
 *
 * @param player
 *      This player
 */
void process_stay(char* command, Player* player) {
    printf("go\n");
    fflush(stdout);
    return;
}
