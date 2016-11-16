#include "utilities.h"
#include "playerFunctionality.h"

#define ATTACK_IN 0
#define ATTACK_OUT 1
#define USAGE 1
#define COUNT 2
#define BAD_ID 3
#define LOST 4
#define BAD_MESSAGE 5
#define MAX_HEALTH 10

void check_invocation(int, char**);
void check_player_count(char*);
void check_player_type(char*, char*);
void check_invocation_argument_count(int);
void exit_with_code(int);
void start_play(int, char**);
char* get_input(int);
char** clean_input(char*);
void check_word_count(int);
void run_turn(char*);
void take_commands(Player*);
void check_valid_turn(char**);
bool check_dice_order(char*);
void check_dice(char*);
Player* create_player(char**);
void process_reroll(char* commands, Player* player);
void work_with_dice(char* commands, Player* player);
void check_rolled(char* commands, Player* player);
void process_rolled(char* commands, Player* player);
void check_attack(char* commands, Player* player);
void process_attack(char* commands, Player* player);
void reroll(char*, Player*);
bool decide_if_keeping(char*, Player*);
void process_elimination(char* commands, Player* player);

int main(int argc, char** argv) {
    check_invocation(argc, argv);

    // Signals program has started successfully
    fprintf(stdout, "!");
    fflush(stdout);

    start_play(argc, argv);
}

/**
 * @brief Creates a player and starts the game with the player. If the game
 * finishes or an error occurs, this program will terminate
 *
 * @param argc
 *      The number of invocation arguments provided
 *
 * @param argv
 *      The invocation arguments provided
 */
void start_play(int argc, char** argv) {
    Player* player = create_player(argv);
    while(1) {
        take_commands(player);
    }
}

/**
 * @brief Initializes the state of this player
 *
 * @param argv
 *      The arguments provided at invocation
 *
 * @return A player with the appropriate state given the invocation parameters
 */
Player* create_player(char** argv) {
    Player* player = (Player*) malloc(sizeof(Player));

    // Initialize information for this player
    player->health = MAX_HEALTH;
    player->label = argv[2][0];
    player->totalPlayers = strtol(argv[1], NULL, 10);
    player->remainingPlayers = player->totalPlayers;
    player->points = 0;
    player->players = (Players*) malloc(sizeof(Player) * player->totalPlayers);
    player->stLuciaOccupant = '1';

    // Initialize information for all players in the game
    for(int i = 0; i < player->totalPlayers; i++) {
        player->players[i].label = 'A' + i;
        player->players[i].health = MAX_HEALTH;
        player->players[i].points = 0;
    }

    return player;
}

/**
 * @brief Checks that a label represents a player that is still in the game
 *
 * @param label
 *      The label to check
 *
 * @param player
 *      This player
 *
 * @return True if and only if the label belongs to a player in the game
 */
bool check_label_in_play(char label, Player* player) {
    bool isValid = false;
    for(int i = 0; i < player->remainingPlayers; i++) {
        if(player->players[i].label == label) {
            isValid = true;
        }
    }
    return isValid;
}

/**
 * @brief Ensures a points command from stLucia is valid. If not, program exits
 *
 * @param wholeMessage
 *      A command from stLucia to award points to a player
 *
 * @param player
 *      This player
 */
void check_points(char* wholeMessage, Player* player) {
    // Ensure there are the correct number of words in the command
    if(count_the_words(wholeMessage) != 3) {
        exit_with_code(BAD_MESSAGE);
    }

    // Get the word corresponding to the label position for a correct command
    char* playerLabel = get_target_word(wholeMessage, 2);

    // Get the word corresponding to the point position for a correct command
    char* points = get_target_word(wholeMessage, 3);

    if(strlen(playerLabel) != 1) {
        exit_with_code(BAD_MESSAGE);
    }

    // Ensures label belongs to a player in the game
    if(!check_label_in_play(playerLabel[0], player)) {
        exit_with_code(BAD_MESSAGE);
    }

    // Make sure points only has integers
    if(!check_only_integers(points)) {
        exit_with_code(BAD_MESSAGE);
    }

    // Ensure the points awarded are in a reasonable range
    errno = 0;
    int value = strtol(points, NULL, 10);
    if(value < 1 || errno != 0) {
        exit_with_code(BAD_MESSAGE);
    }

    free(playerLabel);
    free(points);
}

/**
 * @brief Checks that a winner message is valid, exits the program if it is not
 *
 * @param wholeMessage
 *      The winner message from stLucia
 *
 * @param player
 *      This player
 */
void check_winner(char* wholeMessage, Player* player) {
    // Ensure the winner command has the correct number of words
    if(count_the_words(wholeMessage) != 2) {
        exit_with_code(BAD_MESSAGE);
    }

    char* winnerLabel = get_target_word(wholeMessage, 2);

    // Ensure winner label is a valid character
    if(strlen(winnerLabel) != 2 || winnerLabel[1] != '\n') {
        exit_with_code(BAD_MESSAGE);
    }

    // Ensure the winner label belongs to someone in the game
    if(!check_label_in_play(winnerLabel[0], player)) {
        exit_with_code(BAD_MESSAGE);
    }

    free(winnerLabel);
}

/**
 * @brief Checks that an elimination command is valid, exists the program if
 * it is not. If the elimination message refers to this player, this player
 * will also terminate.
 *
 * @param wholeMessage
 *      The elimination command from stLucia
 *
 * @param player
 *      This player
 */
void check_eliminated(char* wholeMessage, Player* player) {
    // Ensures the elimination message has the correct number of words
    if(count_the_words(wholeMessage) != 2) {
        exit_with_code(BAD_MESSAGE);
    }

    char* winnerLabel = get_target_word(wholeMessage, 2);

    // Ensures the label of the winner represents a valid character and
    // terminates in a newline
    if(strlen(winnerLabel) != 2 || winnerLabel[1] != '\n') {
        exit_with_code(BAD_MESSAGE);
    }

    // Ensures the label of the winner belongs to someone in the game
    if(!check_label_in_play(winnerLabel[0], player)) {
        exit_with_code(BAD_MESSAGE);
    }
}

/**
 * @brief Checks that a turn is valid. If not, this program terminates
 *
 * @param commands
 *      A turn command.
 */
void check_turn(char* commands) {
    // Ensure we only have rolls in the roll part of the command
    if(!check_only_rolls(get_target_word(commands, 2))) {
        exit_with_code(BAD_MESSAGE);
    }

    // Ensure the dice are ordered
    if(!check_roll_order(get_target_word(commands, 2))) {
        exit_with_code(BAD_MESSAGE);
    }
}

/**
 * @brief Checks a turn command for validity and then executes it. If it is
 * not valid, this program terminates
 *
 * @param commands
 *      The command to provide a turn from StLucia.
 *
 * @param player
 *      This player
 *
 * @param wordCount
 *      The number of words in the command
 */
void handle_turn(char* commands, Player* player, int wordCount) {
    if(wordCount != 2) {
        exit_with_code(BAD_MESSAGE);
    }
    check_turn(commands);
    process_reroll(commands, player);
}

/**
 * @brief Checks a reroll command for validity and then executes it. If it
 * is not valid, this program terminates.
 *
 * @param commands
 *      The command to reroll from StLucia
 *
 * @param player
 *      This player
 *
 * @param wordCount
 *      The number of words in the command
 */
void handle_reroll(char* commands, Player* player, int wordCount) {
    if(wordCount != 2) {
        exit_with_code(BAD_MESSAGE);
    }
    check_turn(commands);
    process_reroll(commands, player);
}

/**
 * @brief Checks a rolled command for validity and then executes it. If it is
 * not valid, this program terminates
 *
 * @param commands
 *      The rolled command to notify players of what other players rolled
 *
 * @param player
 *      This player
 *
 * @param wordCount
 *      The number of words in the command
 */
void handle_rolled(char* commands, Player* player, int wordCount) {
    if(wordCount != 3) {
        exit_with_code(BAD_MESSAGE);
    }
    check_rolled(commands, player);
    process_rolled(commands, player);
}

/**
 * @brief Checks a command to notify players of points scored for validity.
 * If it is not valid, this program terminates
 *
 * @param commands
 *      The points notification
 *
 * @param player
 *      This player
 *
 * @param wordCount
 *      The number of words in the command
 */
void handle_points(char* commands, Player* player, int wordCount) {
    if(wordCount != 3) {
        exit_with_code(BAD_MESSAGE);
    }
    check_points(commands, player);
}

/**
 * @brief Checks an attack notification from StLucia and then executes it. If
 * If the attack is not valid, this program will terminate
 *
 * @param commands
 *      The attack command
 *
 * @param player
 *      This player
 *
 * @param wordCount
 *      The number of words in the command
 */
void handle_attacks(char* commands, Player* player, int wordCount) {
    if(wordCount != 4) {
        exit_with_code(BAD_MESSAGE);
    }
    check_attack(commands, player);
    process_attack(commands, player);
}

/**
 * @brief Checks an elimination notification for validity. If it is not valid
 * or it refers to this player, this program will end with the appropriate
 * exit code. Otherwise, it will execute the elimination command
 *
 * @param commands
 *      The command to eliminate
 *
 * @param player
 *      This player
 *
 * @param wordCount
 *      The number of words in the command
 */
void handle_eliminated(char* commands, Player* player, int wordCount) {
    if(wordCount != 2) {
        exit_with_code(BAD_MESSAGE);
    }

    check_eliminated(commands, player);

    // If this player was the player eliminated, it should exit.
    char* label = get_target_word(commands, 2);
    if(label[0] == player->label) {
        exit(0);
    }

    process_elimination(commands, player);
}

/**
 * @brief Checks that a claim command is valid, and terminates the program
 * if it is not.
 *
 * @param commands
 *      The claim command
 */
void check_claim(char* commands, Player* player) {
    char* label = get_target_word(commands, 2);

    if(strlen(label) != 2) {
        exit_with_code(BAD_MESSAGE);
    }

    if(!check_label_in_play(label[0], player)) {
        exit_with_code(BAD_MESSAGE);
    }
}

/**
 * @brief Checks if a claim command is valid and then executes it. If it is
 * not valid, this program will terminate
 *
 * @param commands
 *      The claim command
 *
 * @param player
 *      This player
 *
 * @param wordCount
 *      The number of words in the command
 */
void handle_claim(char* commands, Player* player, int wordCount) {
    if(wordCount != 2) {
        exit_with_code(BAD_MESSAGE);
    }
    check_claim(commands, player);
    player->stLuciaOccupant = get_target_word(commands, 2)[0];
}

/**
 * @brief Checks a winner command for validity and then closes the program
 *
 * @param commands
 *      The winner command
 *
 * @param player
 *      This player
 *
 * @param wordCount
 *      The number of words in commands
 */
void handle_winner(char* commands, Player* player, int wordCount) {
    check_winner(commands, player);
    exit(0);
}

/**
 * @brief Processes the commands sent from StLucia. Exits if the game ends or
 * a command is invalid
 *
 * @param player
 *      This player
 */
void take_commands(Player* player) {
    int maximumMessageSize = 70;
    char* commands = get_input(maximumMessageSize);

    if(!strcmp(commands, "shutdown\n")) {
        exit(0);
    }

    fprintf(stderr, "From StLucia:%s", commands);

    int wordCount = count_the_words(commands);
    char* commandWord = get_target_word(commands, 1);

    if(!strcmp(commandWord, "turn")) {
        handle_turn(commands, player, wordCount);
    } else if(!strcmp(commandWord, "rerolled")) {
        handle_reroll(commands, player, wordCount);
    } else if(!strcmp(commandWord, "rolled")) {
        handle_rolled(commands, player, wordCount);
    } else if(!strcmp(commandWord, "points")) {
        handle_points(commands, player, wordCount);
    } else if(!strcmp(commandWord, "attacks")) {
        handle_attacks(commands, player, wordCount);
    } else if(!strcmp(commandWord, "eliminated")) {
        handle_eliminated(commands, player, wordCount);
    } else if(!strcmp(commandWord, "claim")) {
        handle_claim(commands, player, wordCount);
    } else if(!strcmp(commandWord, "stay?\n")) {
        process_stay(commands, player);
    } else if(!strcmp(commandWord, "winner")) {
        handle_winner(commands, player, wordCount);
    } else {
        exit_with_code(BAD_MESSAGE);
    }
}

/**
 * @brief In the event of an elimination of a player that is not this player,
 * removes that player from the state of this player
 *
 * @param commands
 *      The elimination message
 *
 * @param player
 *      This player
 */
void process_elimination(char* commands, Player* player) {
    char* label = get_target_word(commands, 2);
    int i;
    bool death = false;

    // Looks for the player in this player's list of players
    for(i = 0; i < player->remainingPlayers; i++) {
        if(player->players[i].label == label[0]) {
            death = true;
            break;
        }
    }

    // Writes over the top of that player if a player has died
    if(death) {
        for(int j = i; j < player->remainingPlayers - 1; j++) {
            player->players[j] = player->players[j + 1];
        }
        player->remainingPlayers -= 1;
    }
}

/**
 * @brief Processes an attack announcement from StLucia, deducting health
 * from this player and other players from the perspective of this player as
 * appropriate
 *
 * @param commands
 *      The command to announce an attack
 *
 * @param player
 *      This player
 */
void process_attack(char* commands, Player* player) {
    char* attackWord = get_target_word(commands, 3);
    char* directionWord = get_target_word(commands, 4);

    int attackPoints = strtol(attackWord, NULL, 10);
    int direction;

    // Work out if player was attacking in or out
    if(!strcmp(directionWord, "out\n")) {
        direction = ATTACK_OUT;
    } else if(!strcmp(directionWord, "in\n")) {
        direction = ATTACK_IN;
    }

    // Deducts the health of the StLucia occupant if an attack is into StLucia
    if(direction == ATTACK_IN) {
        for(int i = 0; i < player->remainingPlayers; i++) {
            if(player->stLuciaOccupant == player->players[i].label) {
                player->players[i].health -= attackPoints;
            }
        }
    }

    // Deducts the health of all other players if an attack is from StLucia
    if(direction == ATTACK_OUT) {
        for(int i = 0; i < player->remainingPlayers; i++) {
            if(player->stLuciaOccupant != player->players[i].label) {
                player->players[i].health -= attackPoints;
            }
        }
    }

    // Update this players health to reflect it's health in it's list
    for(int i = 0; i < player->remainingPlayers; i++) {
        if(player->label == player->players[i].label) {
            player->health = player->players[i].health;
            break;
        }
    }
}

/**
 * @brief Checks that an announcement of a player's attack coming from StLucia
 * is a valid one. If it is not, this program will terminate
 *
 * @param commands
 *      The command to announce the attack
 *
 * @param player
 *      This player
 */
void check_attack(char* commands, Player* player) {
    char* label = get_target_word(commands, 2);
    char* attack = get_target_word(commands, 3);
    char* direction = get_target_word(commands, 4);

    if(!strcmp(direction, "out\n") && !strcmp(direction, "in\n")) {
        exit_with_code(BAD_MESSAGE);
    }

    errno = 0;
    int attackPoints = strtol(attack, NULL, 10);
    if(errno != 0 || attackPoints < 1 || attackPoints > 6) {
        exit_with_code(BAD_MESSAGE);
    }

    if(strlen(label) != 1) {
        exit_with_code(BAD_MESSAGE);
    }

    if(!check_label_in_play(label[0], player)) {
        exit_with_code(BAD_MESSAGE);
    }

    if(!check_only_integers(attack)) {
        exit_with_code(BAD_MESSAGE);
    }
}

/**
 * @brief Updates the state of this player in response to the final roll of
 * a different player
 *
 * @param commands
 *      The final roll of a different player
 *
 * @param player
 *      This player
 */
void process_rolled(char* commands, Player* player) {
    char* rollerLabel = get_target_word(commands, 2);
    DiceContent* dice = get_dice_content(get_target_word(commands, 3));
    for(int i = 0; i < player->remainingPlayers; i++) {
        // If the player is in StLucia, it cannot heal
        if(rollerLabel[0] == player->stLuciaOccupant) {
            break;
        }
        if(rollerLabel[0] == player->players[i].label) {
            player->players[i].health += dice->health;
            // Ensure no player's health exceeds 10
            if(player->players[i].health > 10) {
                player->players[i].health = 10;
            }
            break;
        }
    }
}

/**
 * @brief Checks if the command to announce a different player's final dice
 * roll is a valid one. If not, this program will terminate
 *
 * @param commands
 *      The command to notify this player of another player's dice roll
 *
 * @param player
 *      This player
 */
void check_rolled(char* commands, Player* player) {
    char* label = get_target_word(commands, 2);
    char* dice = get_target_word(commands, 3);

    // Makes sure the third word only contains dice
    if(!check_only_rolls(dice)) {
        exit_with_code(BAD_MESSAGE);
    }

    // Makes sure the dice are ordered
    if(!check_roll_order(dice)) {
        exit_with_code(BAD_MESSAGE);
    }

    // Makes sure the label for the player is valid
    if(strlen(label) != 1) {
        exit_with_code(BAD_MESSAGE);
    }

    // Makes sure the label for the player is an actual label for a player in
    // this game
    if(!check_label_in_play(label[0], player)) {
        exit_with_code(BAD_MESSAGE);
    }

    check_dice(get_target_word(commands, 3));
}

/**
 * @brief Once the final dice have been decided for this player, grants health
 * rolls to this player
 *
 * @param commands
 *      The last command this player received, containing the final dice roll
 *      for this player
 *
 * @param player
 *      This player
 */
void work_with_dice(char* commands, Player* player) {
    char* dicePart = get_target_word(commands, 2);
    DiceContent* dice = get_dice_content(dicePart);

    // Grant this player health
    int newHealth = 0;
    if(player->label != player->stLuciaOccupant) {
        // Make sure this player's health does not exceed 10
        if(player->health + dice->health <= 10) {
            player->health += dice->health;
            newHealth = player->health;
        } else {
            player->health = 10;
            newHealth = player->health;
        }
    } else {
        // If this player is in StLucia, it's health should not change
        newHealth = player->health;
    }

    // Grants health to this player in it's list of players
    for(int i = 0; i < player->remainingPlayers; i++) {
        if(player->players[i].label == player->label) {
            player->players[i].health = newHealth;
            player->health = newHealth;
        }
    }
}

/**
 * @brief Handles a reroll request if there are rerolls still left for this
 * player and it wants to reroll, otherwise keeps the dice. If the reroll
 * request is not valid, this program will terminate
 *
 * @param commands
 *      The last command sent to this player
 *
 * @param player
 *      This player
 */
void process_reroll(char* commands, Player* player) {
    check_dice(get_target_word(commands, 2));
    // If the player wants to hold it's dice or has no rerolls left, keep dice
    if(player->rerolls == 2 || decide_if_keeping(commands, player)) {
        printf("keepall\n");
        fflush(stdout);
        player->rerolls = 0;
        work_with_dice(commands, player);
        return;
    }
    reroll(commands, player);
}

/**
 * @brief Checks that every dice in a complete set of dice contains only dice.
 * if it does not, this program will termiante
 *
 * @param dicePacket
 *      The dice in player
 */
void check_dice(char* dicePacket) {
    if (strlen(dicePacket) != 7) {
        exit_with_code(BAD_MESSAGE);
    }
    if(!check_only_rolls(dicePacket)) {
        exit_with_code(BAD_MESSAGE);
    }
    if(!check_roll_order(dicePacket)) {
        exit_with_code(BAD_MESSAGE);
    }
}

/**
 * @brief Ensures the invocation parameters are valid, otherwise, will exit
 * with the appropriate exit code
 *
 * @param argc
 *      The number of arguments passed in at invocation
 *
 * @param argv
 *      The arguments passed in at invocation
 */
void check_invocation(int argc, char** argv) {
    check_invocation_argument_count(argc);
    check_player_count(argv[1]);
    check_player_type(argv[1], argv[2]);
}

/**
 * @brief Exits the program if there are not 3 arugments passed in at
 * invocation
 *
 * @param count
 *      The number of arguments at invocation
 */
void check_invocation_argument_count(int count) {
    if(count != 3) {
        exit_with_code(USAGE);
    }
}

/**
 * @brief Reads input into a buffer.
 *
 * @param size
 *      The size of the input
 *
 * @return The input from stdin
 */
char* get_input(int size) {
    char* buffer = (char*) malloc(sizeof(char) * size);
    int index = 0;
    int valueAtIndex = 'j';
    while(1) {
        if(index >= size) {
            exit_with_code(BAD_MESSAGE);
        }
        valueAtIndex = fgetc(stdin);
        if(valueAtIndex == '\n') {
            buffer[index] = (char) valueAtIndex;
            index++;
            break;
        }
        if(valueAtIndex == EOF) {
            exit_with_code(LOST);
        }
        buffer[index] = (char) valueAtIndex;
        index++;
    }
    buffer[index] = '\0';
    return buffer;
}

/**
 * @brief Checks if the number of players is valid. If not, the game will exit
 *
 * @param playerCount
 *      The number of players in the game
 */
void check_player_count(char* playerCount) {
    if(!check_only_integers(playerCount)) {
        exit_with_code(COUNT);
    }
    errno = 0;
    int count = strtol(playerCount, NULL, 10);
    if(errno != 0) {
        exit_with_code(COUNT);
    }
    // 26 is the most players we can have, 2 is the least
    if(count > 26 || count < 2) {
        exit_with_code(COUNT);
    }
}

/**
 * @brief Checks if the player label on invocation is correct. If not, the game
 * will exit
 *
 * @param playerCount
 *      The number of players in the game
 *
 * @param playerLabel
 *      The label of the player
 */
void check_player_type(char* playerCount, char* playerLabel) {
    if(strlen(playerLabel) != 1) {
        exit_with_code(BAD_ID);
    }
    int maxPlayers = strtol(playerCount, NULL, 10);
    // Exit if the player is given a label outside of the reasonable range
    // given the number of players in the game
    if(playerLabel[0] < 'A' || playerLabel[0] > 'A' + maxPlayers - 1) {
        exit_with_code(BAD_ID);
    }
}

/**
 * @brief Prints a message to stderr and exits the program, depending on the
 * nature of what triggered this function call
 *
 * @param code
 *      The exit code
 */
void exit_with_code(int code) {
    if(code == USAGE) {
        fprintf(stderr, "Usage: player number_of_players my_id\n");
    } else if(code == COUNT) {
        fprintf(stderr, "Invalid player count\n");
    } else if(code == BAD_ID) {
        fprintf(stderr, "Invalid player ID\n");
    } else if(code == LOST) {
        fprintf(stderr, "Unexpectedly lost contact with StLucia\n");
    } else if(code == BAD_MESSAGE) {
        fprintf(stderr, "Bad message from StLucia\n");
    }
    exit(code);
}

/**
 * @brief Decides if a command contains a dice roll that the player wishes
 * to keep.
 *
 * @param commands
 *      The command containing the dice roll
 *
 * @param player
 *      This player
 *
 * @return True if and only if the player is keeping all the dice
 */
bool decide_if_keeping(char* commands, Player* player) {
    char* dicePart = get_target_word(commands, 2);
    DiceContent* dice = get_dice_content(dicePart);
    keep_dice(dice, player);
    // If we don't want to keep all the dice, return false
    if(dice->one
            + dice->two
            + dice->three
            + dice->health
            + dice->attack
            + dice->points
            != KEEP) {
        return false;
    }
    return true;
}

/**
 * @brief If this player wants to reroll, this function will build a set of
 * sorted dice consisting of the dice it wants to reroll and return it.
 *
 * @param commands
 *      The command that requested the reroll
 *
 * @param player
 *      This player
 *
 * @return A set of dice to reroll with length less than or equal to 6
 */
char* make_dice_request(char* commands, Player* player) {
    char* dicePart = get_target_word(commands, 2);
    DiceContent* diceToReroll = get_dice_content(dicePart);
    keep_dice(diceToReroll, player);
    char* diceRequest = (char*) malloc(sizeof(char) * 7);
    int diceAdded = 0;
    for(int i = 0; i < diceToReroll->one; i++) {
        diceRequest[diceAdded] = '1';
        diceAdded += 1;
    }
    for(int i = 0; i < diceToReroll->two; i++) {
        diceRequest[diceAdded] = '2';
        diceAdded += 1;
    }
    for(int i = 0; i < diceToReroll->three; i++) {
        diceRequest[diceAdded] = '3';
        diceAdded += 1;
    }
    for(int i = 0; i < diceToReroll->health; i++) {
        diceRequest[diceAdded] = 'H';
        diceAdded += 1;
    }
    for(int i = 0; i < diceToReroll->attack; i++) {
        diceRequest[diceAdded] = 'A';
        diceAdded += 1;
    }
    for(int i = 0; i < diceToReroll->points; i++) {
        diceRequest[diceAdded] = 'P';
        diceAdded += 1;
    }
    diceRequest[diceAdded] = '\0';
    return diceRequest;
}

/**
 * @brief Handles a reroll request
 *
 * @param command
 *      The command that is requesting the reroll
 *
 * @param player
 *      This player
 */
void reroll(char* command, Player* player) {
    char* diceToReroll = make_dice_request(command, player);
    char* rerollCommand = (char*) malloc(sizeof(char) * 50);
    sprintf(rerollCommand, "reroll %s\n", diceToReroll);
    printf("%s", rerollCommand);
    fflush(stdout);
    player->rerolls += 1;
}

