#ifndef PLAYER_FUNCTIONALITY_H
#define PLAYER_FUNCTIONALITY_H
#include "utilities.h"
#define KEEP 0

/* Other player's essential information for this player */
typedef struct {
    char label;
    int health;
    int points;
} Players;

/* This player's essential information */
typedef struct {
    int health;
    int rerolls;
    int points;
    char label;
    int totalPlayers;
    int remainingPlayers;
    char stLuciaOccupant;
    Players* players;
} Player;

void process_stay(char* command, Player* player);
void keep_dice(DiceContent* dice, Player* player);

#endif
