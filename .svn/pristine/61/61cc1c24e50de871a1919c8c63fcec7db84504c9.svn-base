#include "playerFunctionality.h"

/**
 * @brief Returns the player who is occupying StLucia currently according to
 * this player, if no player is occupying StLucia then it returns a player
 * with label 1
 *
 * @param player
 *      This player
 *
 * @return The player in StLucia or a player with label '1' if that player
 * does not exist
 */
Players get_st_lucia_occupant(Player* player) {
    Players playerToReturn;
    playerToReturn.label = '1';
    for(int i = 0; i < player->remainingPlayers; i++) {
        if(player->stLuciaOccupant == player->players[i].label) {
            playerToReturn = player->players[i];
        }
    }
    return playerToReturn;
}

/**
 * @brief Always returns stay, since this player will always want to leave
 * StLucia
 *
 * @param command
 *      The command to stay
 *
 * @param player
 *      This player
 */
void process_stay(char* command, Player* player) {
    printf("stay\n");
    fflush(stdout);
}

/**
 * @brief Keeps all the point dice if this player is in StLucia, otherwise
 * rerolls everything unless it has enough attack dice to kill the player in
 * StLucia.
 *
 * @param dice
 *      The dice from this player's final roll
 *
 * @param player
 *      This player
 */
void keep_dice(DiceContent* dice, Player* player) {
    if(player->stLuciaOccupant == player->label) {
        dice->points = KEEP;
    } else {
        Players stLuciaOccupant = get_st_lucia_occupant(player);
        if(stLuciaOccupant.label == '1') {
            return;
        }
        if(stLuciaOccupant.health - dice->attack < 1) {
            dice->attack = KEEP;
        }
    }
}
