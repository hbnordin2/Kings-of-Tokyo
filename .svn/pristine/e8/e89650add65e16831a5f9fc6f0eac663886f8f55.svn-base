#include "playerFunctionality.h"

/**
 * @brief Tells StLucia this player wants to stay if there are 2 players left,
 * or that it wants to stay if it has 4 or more health and there are more than
 * 2 players in the game
 *
 * @param command
 *      The command asking this player if it wants to stay
 *
 * @param player
 *      This player
 */
void process_stay(char* command, Player* player) {
    if(player->remainingPlayers == 2) {
        printf("stay\n");
        fflush(stdout);
        return;
    }
    if(player->health < 4) {
        printf("go\n");
        fflush(stdout);
    } else {
        printf("stay\n");
        fflush(stdout);
    }
}

/**
 * @brief Keeps all dice except for attack dice if this player's health is less
 * than 5 health
 *
 * @param dice
 *      The final dice for this player
 *
 * @param player
 *      This player
 */
void keep_dice(DiceContent* dice, Player* player) {
    dice->one = KEEP;
    dice->two = KEEP;
    dice->three = KEEP;
    dice->health = KEEP;
    dice->points = KEEP;
    if(player->health >= 5) {
        dice->attack = KEEP;
        return;
    }
}
