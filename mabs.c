#include "playerFunctionality.h"

/**
 * @brief Tells StLucia this player wants to leave always
 *
 * @param command
 *      The last command sent to this player
 *
 * @param player
 *      This player
 */
void process_stay(char* command, Player* player) {
    printf("go\n");
    fflush(stdout);
}

/**
 * @brief Keeps all the attack dice if this player is in StLucia, otherwise
 * keeps all the health dice. Also always keeps three.
 *
 * @param dice
 *      The dice for this player
 *
 * @param player
 *      This player
 */
void keep_dice(DiceContent* dice, Player* player) {
    dice->three = KEEP;
    if(player->stLuciaOccupant == player->label) {
        dice->attack = KEEP;
    } else {
        dice->health = KEEP;
    }
}
