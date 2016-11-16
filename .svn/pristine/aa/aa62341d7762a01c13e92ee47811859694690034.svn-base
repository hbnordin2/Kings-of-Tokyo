#include "playerFunctionality.h"

/**
 * @brief Keeps all dice that have been rolled twice and all health dice
 * if this player's health is below 6.
 *
 * @param dice
 *      The dice last passed to this player
 *
 * @param player
 *      This player
 */
void keep_dice(DiceContent* dice, Player* player) {
    if(dice->one > 2) {
        dice->one = KEEP;
    }
    if(dice->two > 2) {
        dice->two = KEEP;
    }
    if(dice->three > 2) {
        dice->three = KEEP;
    }
    if(player->health < 6) {
        dice->health = KEEP;
    }
}

/**
 * @brief Decides to leave if health is below 5, otherwise decides to stay
 *
 * @param command
 *      The last command sent to this player
 *
 * @param player
 *      This player
 */
void process_stay(char* command, Player* player) {
    if(player->health < 5) {
        printf("go\n");
        fflush(stdout);
    } else {
        printf("stay\n");
        fflush(stdout);
    }
}
