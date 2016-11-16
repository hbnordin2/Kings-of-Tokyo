#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include "utilities.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h> 
#include <fcntl.h>

#define READ_SIDE 0
#define WRITE_SIDE 1
#define USAGE 1
#define SCORE 2
#define ACCESS 3
#define ROLLFILE 4
#define SUB 5
#define QUIT 6
#define MESSAGE 7
#define REQUEST 8
#define SIG 9
#define MAXDICE 6
#define ROLL_FILE_INDEX 1
#define END_SCORE_INDEX 2
#define MAX_SIZE 50
#define MAX_PLAYERS 26
#define MAX_HEALTH 10
#define MAX_REROLLS 2

/* A player from the perspective of StLucia */
typedef struct {
    FILE* input;
    FILE* output;
    char label;
    int tokens;
    int currentScore;
    int totalScore;
    int health;
    pid_t processId;
} Player;

/* A node indicating the next player in the sequence */
typedef struct Node {
    Player* player;
    struct Node* nextNode;
} PlayerNode;

/* The game, contains all meta-information about it's state and the players */
typedef struct {
    char* rollFile;
    int nextRoll;
    pid_t** allPids;
    FILE** allPlayerOutputs;
    int totalPlayers;
    PlayerNode* currentNode;
    PlayerNode* king;
    char kingLabel;
    int endScore;
} Game;

/* A set of pipes, one for output and one for input into StLucia */
typedef struct {
    int input[2];
    int output[2];
} Pipe;

void check_invocation(int, char**);
void check_number_of_args(int);
void check_winscore_valid(char*);
void check_rollfile_openable(char*);
void check_rollfile_valid_content(char*);
char* get_file_content(char*);
void exit_with_code(int);
void start_game(int, char**);
void setup_child(Pipe*, int, char**, int);
void setup_st_lucia(Pipe*, Player*);
void setup_processes(int, char**);
void read_in_rollfile(char*);
void signal_exit_wrapper(int);
void prepare_signal_responses(void);
void play_game(void);
char get_dice_roll(void);
void sort(char*);
char clean_dice_roll(char outcome);
char* get_dice_set(void);
void kill_all_children(void);
char* int_to_string(int, int);
char* char_to_string(char);
char* get_message_from_player(FILE*, int);

/* Required to shut down processes in the event of StLucia shutdown.
 * This struct contains all the information for StLucia's state */
Game* game;

int main(int argc, char** argv) {
    prepare_signal_responses();
    check_invocation(argc, argv);
    start_game(argc, argv);
}

/**
 * @brief Sets up the game
 *
 * @param argv
 *      The arguments passed in at invocation
 */
void setup_game(char** argv) {
    game->endScore = strtol(argv[END_SCORE_INDEX], NULL, 10);
    game->currentNode = NULL;
    game->king = NULL;
    game->kingLabel = '1';
    game->nextRoll = 0;
    game->allPids = (pid_t**) calloc(MAX_PLAYERS, sizeof(pid_t*));
    game->totalPlayers = 0;
    game->allPlayerOutputs = (FILE**) calloc(MAX_PLAYERS, sizeof(FILE*));
}

/**
 * @brief Sets up the players and starts running the game
 *
 * @param argc
 *      The number of invocation arguments
 *
 * @param argv
 *      The invocation arguments
 */
void start_game(int argc, char** argv) {
    game = (Game*) malloc(sizeof(Game));

    read_in_rollfile(argv[ROLL_FILE_INDEX]);

    setup_game(argv);

    setup_processes(argc, argv);
    play_game();
}

/**
 * @brief Generates a turn command with 6 sorted dice rolls
 *
 * @return turnCommand
 *      The turn command with 6 sorted dice rolls
 */
char* generate_turn(void) {
    char* turnCommand = (char*) malloc(sizeof(char) * MAX_SIZE);
    char* diceRolls = get_dice_set();
    sort(diceRolls);
    sprintf(turnCommand, "turn %s\n", diceRolls);
    free(diceRolls);
    return turnCommand;
}

/**
 * @brief Checks that the reply to a turn message is valid. If it is not, this
 * program will terminate. It is valid if and only if it's command is 
 * keepall or reroll, and it only attempts to reroll 6 dice at the most.
 *
 * @param reply
 *      The reply to a previous turn message
 */
void check_turn_reply(char* reply) {
    // Check that the command keyword is valid
    char* commandWord = get_target_word(reply, 1);
    if(!strcmp(reply, "keepall\n")) {
        return;
    }
    if(strcmp(commandWord, "reroll")) {
        exit_with_code(MESSAGE);
    }

    // Check that the dice is valid
    char* commandDice = get_target_word(reply, 2);
    if(strlen(commandDice) > MAXDICE + 1) {
        exit_with_code(MESSAGE);
    }
    if(!check_roll_order(commandDice) || !check_only_rolls(commandDice)) {
        exit_with_code(MESSAGE);
    }
    free(commandWord);
    free(commandDice);
}

/**
 * @brief Returns true if and only if the reply to a turn or reroll command is 
 * to reroll
 *
 * @param reply
 *      The reply to a turn or reroll command
 *
 * @return True if and only if reroll is the command
 */
bool want_to_reroll(char* reply) {
    if(!strcmp(get_target_word(reply, 1), "reroll")) {
        return true;
    }
    return false;
}

/**
 * @brief Creates a reroll command to send to the player that requested the
 * reroll
 *
 * @param newDice
 *      The new dice that resulted from a reroll requested by a player
 *
 * @return A command to reroll, terminating in a new line character
 */
char* build_reroll_command(char* newDice) {
    char* rerollCommand = (char*) malloc(sizeof(char) * MAX_SIZE);
    sort(newDice);
    sprintf(rerollCommand, "rerolled %s", newDice);
    return rerollCommand;
}

/**
 * @brief Returns a set of 6 dice, with every dice the player requested 
 * to be rerolled, rerolled.
 *
 * @param reply
 *      The reply to the last command sent to the current player
 *
 * @param command
 *      The last command sent to the current player
 *
 * @return
 *      Sorted dice, where all the requested rerolls have been rerolled
 */
char* get_new_dice(char* reply, char* command) {
    char* rerollTarget = get_target_word(reply, 2);
    char* newDice = get_target_word(command, 2);
    int countRerolls = count_dice(rerollTarget);
    int replaced = 0;
    for(int i = 0; i < countRerolls; i++) {
        for(int j = replaced; j < strlen(command) - 1; j++) {
            if(newDice[j] == rerollTarget[i]) {
                newDice[j] = get_dice_roll();
                replaced = j + 1;
                break;
            }
        }
    }
    free(rerollTarget);
    return newDice;
}

/**
 * @brief This function will reroll every dice that a player wishes to reroll
 * and build the command to send the rerolled dice to the player
 *
 * @require
 *      Every dice to reroll must be in the set of dice sent out
 *
 * @param reply
 *      The reply to the last turn or reroll command passed to the current
 *      player
 *
 * @param command
 *      The command last sent to the current player
 *
 * @return A reroll command to be sent to the player requesting it, with the
 * dice the player wanted to reroll, rerolled.
 */
char* get_reroll(char* reply, char* command) {
    char* newDice = get_new_dice(reply, command);
    char* rerollCommand = build_reroll_command(newDice);
    return rerollCommand;
}

/**
 * @brief Sends a message to every player in the game except for the current
 * player
 *
 * @param message
 *      The message to send to all players but the current one
 */
void send_to_all_other_players(char* message) {
    char label = game->currentNode->player->label;
    PlayerNode* currentNode = game->currentNode->nextNode;
    while(currentNode->player->label != label) {
        fprintf(currentNode->player->output, "%s", message);
        fflush(currentNode->player->output);
        currentNode = currentNode->nextNode;
    }
}

/**
 * @brief Builds a message to announce which dice the current player has
 * settled on
 *
 * @param command
 *      The last command sent to the player, containing the dice which the
 *      player has now accepted as it's rolls
 *
 * @return A message to notify all other players of the current player's final
 * dice roll
 */
char* build_turn_message_for_players(char* command) {
    char label = game->currentNode->player->label;
    char* dice = get_target_word(command, 2);
    char* message = (char*) malloc(sizeof(char) * MAX_SIZE);
    sprintf(message, "rolled %c %s", label, dice);
    return message;
}

/**
 * @brief Sends the current players final turn to all players
 *
 * @param finalTurn
 *      The command sent to the player, that the player accepts as containing
 *      it's rolls
 */
void send_turn_to_players(char* finalTurn) {
    char* message = build_turn_message_for_players(finalTurn);
    send_to_all_other_players(message);
}

/**
 * @brief For every H that the player rolls, if the player is not in stLucia,
 * the player will be given 1 health point, until it is out of H rolls or
 * it has full health, whichever happens first.
 *
 * @param command
 *      The command containing the current player's rolls
 *
 * @return The number of heal points gained by the player
 */
int count_heals(char* command) {
    int numberOfHeals = 0;
    char* dice = get_target_word(command, 2);
    for(int i = 0; i < count_dice(dice); i++) {
        if(game->currentNode->player->health < MAX_HEALTH && dice[i] == 'H' 
                && game->currentNode->player->label 
                != game->kingLabel) {
            numberOfHeals += 1;
            game->currentNode->player->health += 1;
        }
    }
    return numberOfHeals;
}

/**
 * @brief Sends a message to every player in the game
 *
 * @param message
 *      The message to send every player in the game
 */
void send_message_to_all_players(char* message) {
    char label = game->currentNode->player->label;
    char currentLabel = '1';
    while(currentLabel != label) {
        fprintf(game->currentNode->player->output, "%s", message);
        fflush(game->currentNode->player->output);
        game->currentNode = game->currentNode->nextNode;
        currentLabel = game->currentNode->player->label;
    }
}

/**
 * @brief Creates a message to send to players to indicate that an attack has
 * happened
 *
 * @param label
 *      The label of the attacking player
 *
 * @param damage
 *      The amount of damage the player is unleashing
 *
 * @return A message to be sent to players to indicate that an attack has
 * happened
 */
char* build_player_attack_message(char* label, char* damage) {
    char* messageToPlayers = (char*) malloc(sizeof(char) * MAX_SIZE);
    if(game->kingLabel == label[0]) {
        sprintf(messageToPlayers, "attacks %s %s out\n", label, damage);
    } else {
        sprintf(messageToPlayers, "attacks %s %s in\n", label, damage);
    }
    return messageToPlayers;
}

/**
 * @brief Creates a message to send to the screen to indicate that an attack
 * has happened to a player
 *
 * @param label
 *      The label of the attacked player
 *
 * @param attack
 *      The amount of damage the attacked player has absorbed
 *
 * @param health
 *      The amount of health the player has left
 *
 * @return A message to send to the screen to indicate a player has been
 * attacked
 */
char* build_screen_attack_message(char* label, char* attack, char* health) {
    char* message = (char*) malloc(sizeof(char) * MAX_SIZE);
    sprintf(message, "Player %s took %s damage, health is now %s\n", 
            label, attack, health);
    return message;
}

/**
 * @brief Returns the player with the lowest label who is alive
 *
 * @return The player with the lowest label who is alive
 */
PlayerNode* get_first_player(void) {
    PlayerNode* currentPlayer = game->currentNode;
    PlayerNode* lowestPlayer = game->currentNode->nextNode;

    // Get the smallest labelled player that is still alive
    for(int i = 0; i < MAX_PLAYERS + 1; i++) {
        if(lowestPlayer->player->label > 
                currentPlayer->player->label) {
            lowestPlayer = currentPlayer;
        }
        currentPlayer = currentPlayer->nextNode;
    }
    return lowestPlayer;
}

/**
 * @brief Deals damage to all players that are not in StLucia when a player in
 * StLucia attacks outwards
 *
 * @param dice
 *      The dice currently being used
 */
void hub_attack(DiceContent* dice) {
    char* label = char_to_string(game->currentNode->player->label);
    char* attackCount;
    PlayerNode* currentPlayer = game->currentNode;
    PlayerNode* stopHere = get_first_player();

    // Start announcing attacks to the earliest player that is still alive
    PlayerNode* startHere = get_first_player();

    do {
        // Do not deal damage to the player in StLucia
        if(startHere == currentPlayer) {
            startHere = startHere->nextNode;
            continue;
        }

        // Deal only enough damage to take players to 0 at worst
        if(dice->attack > startHere->player->health) {
            int maxAttack = startHere->player->health;
            startHere->player->health = 0;
            attackCount = int_to_string(maxAttack, 3);
        } else {
            attackCount = int_to_string(dice->attack, 3);
            startHere->player->health -= dice->attack;
        }

        // Notify screen of attack
        char* health = int_to_string(startHere->player->health, 2);
        char* displayLabel = 
                char_to_string(startHere->player->label);
        char* screenMessage = 
                build_screen_attack_message(displayLabel, attackCount, health);
        fprintf(stderr, "%s", screenMessage);
        
        // Clean up
        free(health);
        free(displayLabel);
        free(screenMessage);

        startHere = startHere->nextNode;
    } while(stopHere != startHere);

    // Notify all players of attack that just happened
    char* messageToPlayers = build_player_attack_message(label, attackCount);
    send_message_to_all_players(messageToPlayers);

    // Clean up
    free(messageToPlayers);
    free(label);
    free(attackCount);
}

/**
 * @brief Grants StLucia to an attacking player. Grants it a point, and tells
 * the screen and other players about the capture.
 */
void capture_st_lucia_by_attack(void) {
    // Award points for capture
    game->currentNode->player->currentScore += 1;

    // Announce capture to screen
    fprintf(stderr, "Player %c claimed StLucia\n", 
            game->currentNode->player->label);

    //Announce capture to players
    char* playerMessage = (char*) calloc(30, sizeof(char));
    sprintf(playerMessage, "claim %c\n", game->currentNode->player->label);
    send_message_to_all_players(playerMessage);
    free(playerMessage);

    //Update stLucia state to reflect change
    game->king = game->currentNode;
    game->kingLabel = game->currentNode->player->label;
}

/**
 * @brief Delivers an attack into stLucia from a monster on the outside
 *
 * @param dice
 *      The dice the current player agreed on
 */
void outside_attack_hub(DiceContent* dice) {
    char* currentPlayerLabel = 
            char_to_string(game->currentNode->player->label);
    char* attackCount;
    char* kingLabel = char_to_string(game->kingLabel);
    bool death = false;

    // Ensure maximum damage does not exceed the attacked player's life
    if(dice->attack >= game->king->player->health) {
        int maxAttack = game->king->player->health;
        game->king->player->health = 0;
        attackCount = int_to_string(maxAttack, 3);
        death = true;
    } else {
        attackCount = int_to_string(dice->attack, 3);
        game->king->player->health -= dice->attack;
    }

    // Notify players of attack
    char* messageToPlayers = 
            build_player_attack_message(currentPlayerLabel, attackCount);
    send_message_to_all_players(messageToPlayers);
    free(messageToPlayers);

    // Notify screen of attack
    char* health = int_to_string(game->king->player->health, 2);
    char* screenMessage = 
            build_screen_attack_message(kingLabel, attackCount, health);
    fprintf(stderr, "%s", screenMessage);
    free(screenMessage);

    // Ask attacked player if they wish to stay
    fprintf(game->king->player->output, "stay?\n");
    fflush(game->king->player->output);

    char* staying = get_message_from_player(game->king->player->input, 
            MAX_SIZE);

    // If the player chooses to leave or dies, give StLucia to the attacker
    if(!strcmp(staying, "go\n") || death) {
        capture_st_lucia_by_attack();
    }
    free(staying);
}

/**
 * @brief Processes an attack command, depending on the type of attack. An
 * attack can be from stLucia, from outside stLucia, or into an empty
 * stLucia.
 *
 * @param dice
 *      The dice rolled by the current player
 */
void attack(DiceContent* dice) {
    if(game->king == game->currentNode) {
        // King is attacking outwards
        hub_attack(dice);
        return;
    } else if(game->king == NULL) {
        // Attacker is attacking empty StLucia, should claim
        capture_st_lucia_by_attack();
        return;
    }
    // Monster is attacking king of StLucia
    outside_attack_hub(dice);
}

/**
 * @brief Builds a message to announce the healing of a player to the terminal
 *
 * @param label
 *      The label of the healed player
 *
 * @param healPoints
 *      The amount of points the healed player will received
 *
 * @param health
 *      The amount of health the player now has
 *
 * @return A message to announce the healing of a player to the terminal
 */
char* build_heal_screen_message(char* label, char* healPoints, char* health) {
    char* healMessage = (char*) malloc(sizeof(char) * MAX_SIZE);
    sprintf(healMessage, "Player %s healed %s, health is now %s\n", 
            label, healPoints, health);
    return healMessage;
}

/**
 * @brief Builds a message to announce the outcome of a roll to the terminal
 *
 * @param label
 *      The name of the player who just accepted the roll
 *
 * @param dice
 *      The set of dice the player rolled, terminating with a newline character
 *
 * @return A message to announce the outcome of an accepted roll to the
 * terminal
 */
char* build_roll_screen_message(char* label, char* dice) {
    char* turnAnnouncement = (char*) malloc(sizeof(char) * 30);
    // Note that the set of dice terminates with a newline character
    sprintf(turnAnnouncement, "Player %s rolled %s", label, dice);
    return turnAnnouncement;
}

/**
 * @brief Sends a message to a player
 *
 * @param destination
 *      The standard input of the player
 *
 * @param command
 *      The message to send to the player
 */
void send_message_to_player(FILE* destination, char* command) {
    fprintf(destination, "%s", command);
    fflush(destination);
}

/**
 * @brief Gets the message from a player
 *
 * @param destination
 *      The origin of the message from the player
 *
 * @param size
 *      The size of the message to gather from the player
 *
 * @return The message from the player
 */
char* get_message_from_player(FILE* destination, int size) {
    char* reply = (char*) calloc(size + 1, sizeof(char));
    int index = 0;
    while(1) {
        if(index > size) {
            exit_with_code(MESSAGE);
        }
        char current = fgetc(destination);
        if(current == EOF) {
            exit_with_code(QUIT);
        }
        if(current == '\n') {
            reply[index] = current;
            index++;
            break;
        }
        reply[index] = current;
        index++;
    }
    reply[index] = '\0';
    return reply;
}

/**
 * @brief On the event of a reroll request, generates the reroll, sends it out,
 * until the player accepts the reroll or runs out of rerolls.
 *
 * @param command
 *      The last roll the player wants to reroll
 *
 * @param reply
 *      The reroll request from the player
 */
void handle_rerolls(char** command, char** reply) {
    int rerolls = 0;
    while(rerolls < MAX_REROLLS) {
        //Send out reroll dice set
        *command = get_reroll(*reply, *command);
        fprintf(game->currentNode->player->output, "%s", *command);
        fflush(game->currentNode->player->output);

        //Get reroll reply
        *reply = get_message_from_player(game->currentNode->player->input, 
                MAX_SIZE);

        //Check if reply was valid
        check_turn_reply(*reply);
        if(!strcmp(*reply, "keepall\n")) {
            break;
        }
        rerolls += 1;
    }
}

/**
 * @brief Converts an integer to a string
 *
 * @param target
 *      The integer to convert
 *
 * @param size
 *      The number of characters to assign to the string
 *
 * @return A string representation of the integer
 */
char* int_to_string(int target, int size) {
    char* correctFormat = (char*) calloc(size + 1, sizeof(char));
    sprintf(correctFormat, "%d", target);
    return correctFormat;
}

/**
 * @brief Converts a char into a string
 *
 * @param target
 *      The char to convert into a string
 *
 * @return The string representation of the character
 */
char* char_to_string(char target) {
    char* correctFormat = (char*) calloc(2, sizeof(char));
    sprintf(correctFormat, "%c", target);
    return correctFormat;
}

/**
 * @brief Award points at the end of the turn based on the dice rolled
 *
 * @param dice
 *      The contents of the dice roll
 */
void award_points(DiceContent* dice) {
    // Award points based on rolls
    if(dice->one > 2) {
        game->currentNode->player->currentScore += dice->one - 2;
    }
    if(dice->two > 2) {
        game->currentNode->player->currentScore += dice->two - 1;
    }
    if(dice->three > 2) {
        game->currentNode->player->currentScore += dice->three;
    }

    // Award points based on tokens
    game->currentNode->player->tokens += dice->points;
    if(game->currentNode->player->tokens > 9) {
        game->currentNode->player->currentScore += 1;
        game->currentNode->player->tokens = 
                game->currentNode->player->tokens % 10;
    }

    if(game->currentNode->player->currentScore > 0) {

        // Alert all players as to points awarded
        char* pointMessage = (char*) calloc(MAX_SIZE, sizeof(char));
        sprintf(pointMessage, "points %c %d\n", 
                game->currentNode->player->label, 
                game->currentNode->player->currentScore);
        send_message_to_all_players(pointMessage);
        game->currentNode->player->totalScore += 
                game->currentNode->player->currentScore;
        free(pointMessage);

        // Alert screen as to points awarded
        char* visiblePointMessage = (char*) calloc(MAX_SIZE, sizeof(char));
        sprintf(visiblePointMessage, "Player %c scored %d for a total of %d\n",
                game->currentNode->player->label,
                game->currentNode->player->currentScore,
                game->currentNode->player->totalScore);
        fprintf(stderr, "%s", visiblePointMessage);
        game->currentNode->player->currentScore = 0;
        free(visiblePointMessage);
    }
}

/**
 * @brief Eliminates all players who have lost all their health
 */
void eliminate_losers(void) {
    PlayerNode* previousNode = game->currentNode;
    PlayerNode* currentNode = previousNode->nextNode;
    for(int i = 0; i < MAX_PLAYERS; i++) {
        if(currentNode->player->health == 0) {

            // If eliminated player is in StLucia, clear StLucia
            if(game->king->player->label == currentNode->player->label) {
                game->king = NULL;
                game->kingLabel = EOF;
            }

            // Announce the elimination to the players
            char killMessage[MAX_SIZE];
            sprintf(killMessage, "eliminated %c\n", 
                    currentNode->player->label);
            send_message_to_all_players(killMessage);
            previousNode->nextNode = currentNode->nextNode;
            currentNode = previousNode;
        }
        previousNode = currentNode;
        currentNode = currentNode->nextNode;
    }
}

/**
 * @brief Checks every player in the game for a score that is higher than or
 * equal to the score required to win, and checks if there is only 1 player
 * left. 
 *
 * If either is true, a winner will be announced and this program
 * will exit cleanly
 */
void check_game_over(void) {
    PlayerNode* currentPlayer = game->currentNode;
    for(int i = 0; i < MAX_PLAYERS; i++) {
        if(currentPlayer->player->totalScore >= game->endScore 
                || currentPlayer->nextNode == currentPlayer) {
            fprintf(stderr, "Player %c wins\n", currentPlayer->player->label);
            
            // Announce game over to all children
            char messageForChildren[MAX_SIZE];
            sprintf(messageForChildren, "winner %c\n", 
                    currentPlayer->player->label);
            send_message_to_all_players(messageForChildren);

            // Exit cleanly
            kill_all_children();
            exit(0);
        }
        currentPlayer = currentPlayer->nextNode;
    }
}

/**
 * @brief Handles the heals for a roll, healing the player and alerting the
 * screen as to the heal status
 *
 * @param command
 *      The command with the dice rolls which contain the heals
 *
 * @param label
 *      The label of the player who is healing
 */
void heal(char* command, char* label) {
    int totalHeals = count_heals(command);
    char* healPoints = int_to_string(totalHeals, 2);
    char* health = int_to_string(game->currentNode->player->health, 2);
    char* healMessage = build_heal_screen_message(label, healPoints, health); 
    fprintf(stderr, "%s", healMessage);
    free(healMessage);
}

/**
 * @brief Gets the final roll of a player. If a player tries to reroll too
 * many times or sends an incorrect reply, this program will terminate.
 */
char* get_final_roll(void) {
    //Generate turn and send it off
    char* command = generate_turn();
    send_message_to_player(game->currentNode->player->output, command);

    //Get reply from player
    char* reply = get_message_from_player(game->currentNode->player->input, 
            MAX_SIZE);
    check_turn_reply(reply);

    if(want_to_reroll(reply)) {

        // Make sure the reroll request has ordered dice
        if(!check_roll_order(get_target_word(reply, 2))) {
            exit_with_code(MESSAGE);
        }
        handle_rerolls(&command, &reply);

        // Player has tried to reroll too many times
        if(strcmp(reply, "keepall\n")) {
            exit_with_code(REQUEST);
        }
    }
    free(reply);
    return command;
}

/**
 * @brief Gets commands from players, processes them, and sends replies. This
 * function will terminate this program if a player wins or sends a bad message
 */
void play_game(void) {
    while(1) {
        if(game->king != NULL 
                && game->currentNode->player->label == game->kingLabel) {
            game->currentNode->player->currentScore += 2;
        }

        char* finalRoll = get_final_roll();

        // Make sure dice rolls are in order
        char* diceRolls = get_target_word(finalRoll, 2);
        if(!check_roll_order(diceRolls)) {
            exit_with_code(MESSAGE);
        }

        char* label = char_to_string(game->currentNode->player->label);

        // Send out final roll messages to players and screen
        send_turn_to_players(finalRoll);
        char* turnAnnouncement = build_roll_screen_message(label, diceRolls);
        fprintf(stderr, "%s", turnAnnouncement);

        DiceContent* dice = get_dice_content(get_target_word(finalRoll, 2));

        // Deal with healing if there are heal dice
        if(dice->health > 0 && (game->king == NULL 
                || (game->kingLabel != game->currentNode->player->label))) {
            heal(finalRoll, label);
        }

        // Deal with attacks if there are attack dice
        if(dice->attack > 0) {
            attack(dice);
        }

        award_points(dice);
        eliminate_losers();
        check_game_over();
        game->currentNode = game->currentNode->nextNode;

        free(finalRoll);
        free(label);
        free(diceRolls);
        free(turnAnnouncement);
    }
}

/**
 * @brief Returns a set of the next 6 dice in the sequence of dice
 *
 * @return The next 6 dice in the sequence of dice loaded into the game
 */
char* get_dice_set(void) {
    char* diceRolls = (char*) malloc(sizeof(char) * 7);
    int i;
    for(i = 0; i < 6; i++) {
        diceRolls[i] = get_dice_roll();
    }
    diceRolls[i] = '\0';
    return diceRolls;
}

/**
 * @brief Returns the next dice roll in the sequence of rolls loaded in
 *
 * @return The next dice in the sequence of dice loaded into the game
 */
char get_dice_roll(void) {
    char outcome;

    // Skips whitespace
    do {
        // If all the dice have been rolled, return to the beginning
        if(game->nextRoll >= strlen(game->rollFile)) {
            game->nextRoll = 0;
        }    
        outcome = game->rollFile[game->nextRoll];
        game->nextRoll += 1;
    } while(outcome == '\n' || outcome == ' ');

    return outcome;
}

/**
 * @brief Goes through every newly opened child process, and ensures that they
 * are able to print an initial exclamation mark. If not, the program will
 * terminate
 */
void check_processes_open(void) {
    char identifier = game->currentNode->player->label;
    while(1) {
        char exclamation = fgetc(game->currentNode->player->input);
        game->currentNode = game->currentNode->nextNode;
        if(exclamation != '!') {
            exit_with_code(SUB);
        }
        if(game->currentNode->player->label == identifier) {
            break;
        }
    }
}

/**
 * @brief Creates an input and output pipe. Will exit if the pipes cannot be
 * made
 *
 * @return A bundle of input and output pipes
 */
Pipe* setup_pipes(void) {
    Pipe* pipePacket = (Pipe*) malloc(sizeof(Pipe));
    if(pipe(pipePacket->input) == -1 
            || pipe(pipePacket->output) == -1) {
        exit_with_code(SUB);
    }
    return pipePacket;
}

/**
 * @brief Sets up the player information from the perspective of StLucia
 *
 * @param player
 *      The player to add to StLucia
 *
 * @param idOffset
 *      Two more than the number of players added.
 */
void setup_player(Player* player, int idOffset) {
    player->health = 10;
    player->tokens = 0;
    player->label = 'A' + idOffset - 3;
    player->currentScore = 0;
    player->totalScore = 0;
}

/**
 * @brief Sets up a child process for every player if possible, exiting if that
 * cannot be done.
 *
 * @param argc
 *      The number of arguments at invocation
 *
 * @param argv
 *      The arguments at invocation
 */
void setup_processes(int argc, char** argv) {
    PlayerNode* rootNode = NULL;
    PlayerNode* lastNode = NULL;
    bool madeRoot = false;
    for(int i = 3; i < argc; i++) {
        Pipe* pipePacket = setup_pipes();
        Player* player = (Player*) malloc(sizeof(Player));
        pid_t processId = fork();
        if(processId == 0) {
            // In child
            setup_child(pipePacket, i, argv, argc);
        } else {
            player->processId = processId;

            // In parent
            setup_player(player, i);

            // Setup linked list to record player information
            PlayerNode* playerNode = (PlayerNode*) malloc(sizeof(PlayerNode));
            playerNode->player = player;
            if(!madeRoot) {
                rootNode = playerNode;
                madeRoot = true;
                game->currentNode = rootNode;
            } else {
                lastNode->nextNode = playerNode;
            }
            lastNode = playerNode;

            // Prepare for clean shutdown
            game->allPids[game->totalPlayers] = (pid_t*) malloc(sizeof(pid_t));
            *game->allPids[game->totalPlayers] = processId;
            setup_st_lucia(pipePacket, player);
            game->totalPlayers += 1;
        }
    }
    lastNode->nextNode = rootNode;
    check_processes_open();
}

/**
 * @brief Loads the contents of a roll file into this game
 *
 * @param rollFileName
 *      The name of the roll file to load into this game
 */
void read_in_rollfile(char* rollFileName) {
    char* file = get_file_content(rollFileName);
    game->rollFile = file;
}

/**
 * @brief Shuts down all children. If they do not exit with a 0, this will
 * print their exit status. If they do not exit at all, this will kill them
 * and print how. The program will terminate after this is called.
 */
void kill_all_children(void) {
    // Send everyone a shut down message
    for(int i = 0; i < game->totalPlayers; i++) {
        fprintf(game->allPlayerOutputs[i], "shutdown\n");
        fflush(game->allPlayerOutputs[i]);
    }

    sleep(2);

    // Checks that everyone actually shut down, otherwise SIGKILL it
    for(int i = 0; i < game->totalPlayers; i++) {
        char current = 'A' + i;
        pid_t changedPid;
        int exitStatus;

        // Check if it shut down
        changedPid = waitpid(*game->allPids[i], &exitStatus, WNOHANG);

        // If it did not shut down, kill it
        if(changedPid == 0 || changedPid == -1) {
            kill(*game->allPids[i], SIGKILL); 
            changedPid = waitpid(*game->allPids[i], &exitStatus, WNOHANG);
        }

        // Send out appropriate message
        if(WIFEXITED(exitStatus) && WEXITSTATUS(exitStatus) != 0) {
            fprintf(stderr, "Player %c exited with status %d\n", current, 
                    WEXITSTATUS(exitStatus));
        }
        if(WIFSIGNALED(exitStatus)) {
            fprintf(stderr, "Player %c terminated due to signal %d\n", current,
                    WTERMSIG(exitStatus));
        }
    }
}

/**
 * @brief Given a path, returns the start of the index after the last backslash
 *
 * @param path
 *      The path which contains blackslashes or not
 *
 * @return The index of the first location after the last backslash
 */
int get_start_of_executable_name(char* path) {
    // Count the slashes
    int slashes = 0;
    for(int i = 0; i < strlen(path); i++) {
        if(path[i] == '/') {
            slashes += 1;
        }
    }

    // Find the index after the last backslash
    int lastPath = 0;
    int foundSlashes = 0;
    for(int i = 0; i < strlen(path); i++) {
        if(path[i] == '/') {
            foundSlashes += 1;
        }
        if(foundSlashes == slashes) {
            lastPath += 1;
            break;
        }
        lastPath += 1;
    }

    return lastPath;
}

/**
 * @brief Given a path and the start of where the executable name is in that
 * path, returns the name of a program in a path
 *
 * @param path
 *      The path with the executable name in it
 *
 * @param index
 *      The index of the start of the executable name in the path
 *
 * @return The name of the executable
 */
char* get_program_name(char* path, int index) {
    char* programName = (char*) malloc(sizeof(char) * strlen(path));
    char currentLetter = EOF;
    int programNameRead = 0;
    while(1) {
        currentLetter = path[index];
        programName[programNameRead] = path[index];
        if(currentLetter == '\0') {
            break;
        }
        programNameRead += 1;
        index += 1;
    }
    return programName;
}

/**
 * @brief Prepares a child process for communication with the parent process,
 * and then executes the program for the child process. If this process
 * cannot execute fully, this program will terminate.
 *
 * @param pipePacket
 *      A collection of pipes to be used for input and output communication
 *
 * @param position
 *      The position of player, with counting starting at 3
 *
 * @param argv
 *      The arguments passed in at invocation
 *
 * @param argc
 *      The number of arguments passed in at invocation
 */
void setup_child(Pipe* pipePacket, int position, char** argv, int argc) {
    // Close unused pipe ends
    if(close(pipePacket->input[READ_SIDE]) == -1
            || close(pipePacket->output[WRITE_SIDE]) == -1) {
        exit_with_code(SUB);
    }

    // Open nessacary streams
    if(fdopen(pipePacket->input[WRITE_SIDE], "w") == NULL 
            || fdopen(pipePacket->output[READ_SIDE], "r") == NULL) {
        exit_with_code(SUB);
    }

    int byteBin = open("/dev/null", O_WRONLY);
    if(byteBin == -1) {
        exit_with_code(SUB);
    }

    //Reorganize pipes
    if(dup2(pipePacket->input[WRITE_SIDE], STDOUT_FILENO) == -1
            || dup2(pipePacket->output[READ_SIDE], STDIN_FILENO) == -1
            || dup2(byteBin, STDERR_FILENO) == -1) {
        exit_with_code(SUB);
    }

    // Prepare parameters for execl
    int indexOfLastSlash = get_start_of_executable_name(argv[position]);
    char* programName = get_program_name(argv[position], indexOfLastSlash);
    char playerCount[10];
    sprintf(playerCount, "%d", argc - 3);
    char printableLabel[5];
    char label = 'A' - 3 + position;
    sprintf(printableLabel, "%c", label);

    execl(argv[position], programName, playerCount, printableLabel, NULL);
    exit_with_code(SUB);
}

/**
 * @brief Sets up piping between stLucia and player processes. If this piping
 * cannot be done, then this program with exit with the appropriate code
 *
 * @param pipePacket
 *      A collection of pipes needed to establish input and output
 *      communication between stLucia and it's child processes.
 *
 * @param player
 *      The player with whom this piping will be between
 */
void setup_st_lucia(Pipe* pipePacket, Player* player) {
    // Close unused ends of pipe
    if(close(pipePacket->input[WRITE_SIDE]) == -1 
            || close(pipePacket->output[READ_SIDE]) == -1) {
        exit_with_code(SUB);
    }

    // Set up input for StLucia
    player->input = fdopen(pipePacket->input[READ_SIDE], "r");

    // Set up output for StLucia
    player->output = fdopen(pipePacket->output[WRITE_SIDE], "w");
    
    // Set up for clean exit
    game->allPlayerOutputs[game->totalPlayers] = (FILE*) malloc(sizeof(FILE));
    game->allPlayerOutputs[game->totalPlayers] = player->output;

    // Ensure files opened up correctly
    if(player->input == NULL || player->output == NULL) {
        exit_with_code(SUB);
    }
}

/**
 * @brief Performs all invocation checks, exiting the program if any of them
 * are not passed
 *
 * @param argc
 *      The number of arguments passed in at invocation
 *
 * @param argv
 *      The arguments passed in at invocation
 */
void check_invocation(int argc, char** argv) {
    check_number_of_args(argc);
    check_winscore_valid(argv[2]);
    check_rollfile_openable(argv[1]);
    check_rollfile_valid_content(argv[1]);
}

/**
 * @brief Checks that the number of arguments provided at invocation is enough
 * for this program to run. If there are not enough, this program will exit
 * with the appropriate code.
 *
 * @param argc
 *      The number of arguments provided at invocation
 */
void check_number_of_args(int argc) {
    if(argc < 5) {
        exit_with_code(USAGE);
    }
}

/**
 * @brief Checks that the winscore provided at invocation is a valid one. If
 * it is not, this program will exit with the appropriate code.
 *
 * @param winScore
 *      The winscore provided at invocation
 */
void check_winscore_valid(char* winScore) {
    // Checks that the winscore provided at invocation consists only of
    // integers
    bool isIntegers = check_only_integers(winScore);
    if(!isIntegers) {
        exit_with_code(SCORE);
    }

    // Checks that the winscore provided at invocation is a valid integer
    errno = 0;
    int winScoreVal = strtol(winScore, NULL, 10);
    if(errno != 0 || winScoreVal < 1) {
        exit_with_code(SCORE);
    }
}

/**
 * @brief Checks if the roll file name given at invocation represents a file
 * that exists and can be opened. If it cannot be opened, ths program will
 * exit with the appropriate code.
 *
 * @param rollFileName
 *      The roll file name given at invocation
 */
void check_rollfile_openable(char* rollFileName) {
    FILE* file;
    if((file = fopen(rollFileName, "r"))) {
        fclose(file);
    } else {
        exit_with_code(ACCESS);
    }
}

/**
 * @brief Opens a roll file and exits if it's contents are not valid
 *
 * @param rollFileName
 *      The name of the roll file
 */
void check_rollfile_valid_content(char* rollFileName) {
    char* rollFileContent = get_file_content(rollFileName);
    if(!check_only_rolls(rollFileContent)) {
        exit_with_code(ROLLFILE);
    }
    free(rollFileContent);
}

/**
 * @brief Returns the content of a file
 *
 * @param fileName
 *      The name of the file to open and extract the contents from
 *
 * @return The content of the file called fileName
 */
char* get_file_content(char* fileName) {
    FILE* rollFile = fopen(fileName, "r");
    int fileSize = get_file_size(rollFile);
    char* buffer = get_file_content_by_file(rollFile, fileSize);
    fclose(rollFile);
    return buffer;
}

/**
 * @brief Sets the appropriate response given various signals. This will
 * cause the program to continue running in the event of a broken pipe
 * expecting that it will be handled at the later point, and also will cause
 * the program to terminate in response to a SIGINT.
 */
void prepare_signal_responses(void) {
    struct sigaction* signalPacket;
    signalPacket = (struct sigaction*) calloc(1, sizeof(struct sigaction));

    // If a pipe breaks, do nothing and expect it to be handled later when
    // we read.
    signalPacket->sa_handler = SIG_IGN;
    sigaction(SIGPIPE, signalPacket, NULL);

    // If a SIGINT is received, exit gracefully
    signalPacket->sa_handler = signal_exit_wrapper;
    sigaction(SIGINT, signalPacket, NULL);
}

/**
 * @brief Called in the event of a SIGINT event. This will exit the program.
 *
 * @param dummy
 *      A throw-away value used to comply with sa_handler requirements
 */
void signal_exit_wrapper(int dummy) {
    exit_with_code(SIG);
}

/**
 * @brief Exits this program with a message and appropriate exit status
 *
 * @param exitCode
 *      The code of the event that triggered this exit
 */
void exit_with_code(int exitCode) {
    if(exitCode >= QUIT) {
        kill_all_children();
    }
    if(exitCode == USAGE) {
        fprintf(stderr, "Usage: stlucia rollfile winscore prog1 " 
                "prog2 [prog3 [prog4]]\n");
    } else if(exitCode == SCORE) {
        fprintf(stderr, "Invalid score\n");
    } else if(exitCode == ACCESS) {
        fprintf(stderr, "Unable to access rollfile\n");
    } else if(exitCode == ROLLFILE) {
        fprintf(stderr, "Error reading rolls\n");
    } else if(exitCode == SUB) {
        fprintf(stderr, "Unable to start subprocess\n");
    } else if(exitCode == QUIT) {
        fprintf(stderr, "Player quit\n");
    } else if(exitCode == MESSAGE) {
        fprintf(stderr, "Invalid message received from player\n");
    } else if(exitCode == REQUEST) {
        fprintf(stderr, "Invalid request by player\n");
    } else if(exitCode == SIG) {
        fprintf(stderr, "SIGINT caught\n");
    }
    exit(exitCode);
}
