#include "utilities.h"

/**
 * @brief Checks that a string contains only integers
 *
 * @param content
 *      The string to check
 *
 * @return True if and only if the string contains only integers
 */
bool check_only_integers(char* content) {
    int index = 0;
    char inspectedValue = content[index];
    while(inspectedValue != '\0' && inspectedValue != '\n') {
        inspectedValue = content[index];
        if((inspectedValue > '9' || inspectedValue < '0')
                && inspectedValue != '\0' && inspectedValue != '\n') {
            return false;
        }
        index += 1;
    }
    return true;
}

/**
 * @brief Checks if a value is in a valid range
 *
 * @param value
 *      The value to check
 *
 * @param min
 *      The minimum value
 *
 * @param max
 *      The maximum value
 *
 * @return True if and only if the value is within a valid range
 */
bool check_valid_range(int value, int min, int max) {
    if(value < min || value > max) {
        return false;
    }
    return true;
}

/**
 * @brief Splits a string along it's white space
 *
 * @param firstLine
 *      The line to split
 *
 * @param target
 *      The number of words in firstLine
 *
 * @return The string as an array of words
 */
char** split_string(char* firstLine, int target) {
    char** splitLine = (char**) malloc(sizeof(char*) * target);
    char* breakPoint;
    for(int i = 0; i < target; i++) {
        splitLine[i] = (char*) malloc(sizeof(char*) * strlen(firstLine));
    }
    breakPoint = strtok(firstLine, " ");
    int i = 0;
    while (breakPoint != NULL) {
        splitLine[i] = breakPoint;
        i += 1;
        breakPoint = strtok(NULL, " ");
    }
    return splitLine;
}

/**
 * @brief Gets the content of a file by it's file pointer
 *
 * @param file
 *      The file
 *
 * @param size
 *      The number of characters in the file
 *
 * @return The content of the file
 */
char* get_file_content_by_file(FILE* file, int size) {
    int i;
    char* fileContent = (char*) malloc(sizeof(char) * size);
    for(i = 0; i < size - 1; i++) {
        fileContent[i] = fgetc(file);
    }
    fileContent[i] = '\0';
    return fileContent;
}

/**
 * @brief Gets the number of characters in a file
 *
 * @param file
 *      The file to get the number of characters of
 *
 * @return The number of characters in the file
 */
int get_file_size(FILE* file) {
    int count = 0;
    char fileContent = 'a';
    while(fileContent != EOF) {
        fileContent = fgetc(file);
        count += 1;
    }
    fseek(file, 0, SEEK_SET);
    return count;
}

/**
 * @brief Returns the content of a file
 *
 * @param file
 *      The file whose contents will be returned
 *
 * @return the content of the file
 */
char* read_in_file_content(FILE* file) {
    int size = get_file_size(file);
    char* fileContent = get_file_content_by_file(file, size);
    fclose(file);
    return fileContent;
}

/**
 * @brief Counts the words in a string
 *
 * @param line
 *      The string whose words will be counted
 *
 * @return The number of words in the string
 */
int count_the_words(char* line) {
    bool readyForNewWord = true;
    char valueAtIndex;
    int count = 0;
    for(int i = 0; i < strlen(line); i++) {
        valueAtIndex = line[i];
        if(readyForNewWord && valueAtIndex != ' ') {
            readyForNewWord = false;
            count += 1;
        }
        if(valueAtIndex == ' ') {
            readyForNewWord = true;
        }
    }
    return count;
}

/**
 * @brief Gets a certain word in a string
 *
 * @param theWord
 *      The string whose words will be taken
 *
 * @param targetWord
 *      The index of the word to take
 *
 * @return The word at the the targetWord position in the sentance
 */
char* get_target_word(char* theWord, int targetWord) {
    // 50 because it is larger than any valid word in this program
    int maxSize = 50;
    char* copiedWord = (char*) malloc(sizeof(char) * maxSize);
    char currentLetter = theWord[0];
    int counter = 0;
    int word = 1;

    // Locate that word
    while(word < targetWord) {
        currentLetter = theWord[counter];
        if(currentLetter == ' ') {
            counter++;
            word++;
            currentLetter = theWord[counter];
            continue;
        }
        counter += 1;
    }

    // Copy it
    int totalWordsCopied = 0;
    while(currentLetter != ' ' && currentLetter != '\0' 
            && totalWordsCopied < maxSize - 1) {
        copiedWord[totalWordsCopied] = currentLetter;
        totalWordsCopied++;
        counter++;
        currentLetter = theWord[counter];
    }

    // If a word ever gets as big as 50, it's bad anyway, so just cut it short
    copiedWord[maxSize - 1] = '\0';
    return copiedWord;
}

/**
 * @brief Counts the number of dice in a set of dice
 *
 * @param diceSet
 *      A set of dice
 *
 * @return The number of dice in the set
 */
int count_dice(char* diceSet) {
    int numberOfDice = 0;
    char currentDice = diceSet[0];
    while(currentDice != '\n' && currentDice != '\0') {
        numberOfDice += 1;
        currentDice = diceSet[numberOfDice];
    }
    return numberOfDice;
}

/**
 * @brief Ensures the dice in a set are in the correct order
 *
 * @param diceSet
 *      A set of dice
 *
 * @return True if and only if the set of dice is in the correct order
 */
bool check_roll_order(char* diceSet) {
    int numberOfDice = count_dice(diceSet);

    // Replace all the letter-based die with ordered integers
    for(int i = 0; i < numberOfDice; i++) {
        if(diceSet[i] == 'H') {
            diceSet[i] = '4';
        } else if(diceSet[i] == 'A') {
            diceSet[i] = '5';
        } else if(diceSet[i] == 'P') {
            diceSet[i] = '6';
        }
    }

    // Check order
    for(int i = 0; i < numberOfDice - 1; i++) {
        if(diceSet[i] > diceSet[i + 1]) {
            return false;
        }
    }

    // Put the lettered dice back
    for(int i = 0; i < numberOfDice; i++) {
        if(diceSet[i] == '4') {
            diceSet[i] = 'H';
        } else if(diceSet[i] == '5') {
            diceSet[i] = 'A';
        } else if(diceSet[i] == '6') {
            diceSet[i] = 'P';
        }
    }
    return true;
}

/**
 * @brief Counts each type of dice in a set of dice
 *
 * @param diceRolls
 *      A set of dice
 *
 * @return A count of all the dice in the set of dice
 */
DiceContent* get_dice_content(char* diceRolls) {
    DiceContent* content = (DiceContent*) calloc(1, sizeof(DiceContent));
    int currentIndex = 0;
    char currentDice = diceRolls[currentIndex];
    while(currentDice != '\0') {
        if(currentDice == '1') {
            content->one += 1;
        } else if(currentDice == '2') {
            content->two += 1;
        } else if(currentDice == '3') {
            content->three += 1;
        } else if(currentDice == 'H') {
            content->health += 1;
        } else if(currentDice == 'A') {
            content->attack += 1;
        } else if(currentDice == 'P') {
            content->points += 1;
        }
        currentIndex += 1;
        currentDice = diceRolls[currentIndex];
    }
    return content;
}

/**
 * @brief Sorts all the dice in a set of dice into bins corresponding with
 * their order
 *
 * @param numberOfDice
 *      The number of dice in the set of dice
 *
 * @param bins
 *      The bins where the dice will be placed
 *
 * @param diceRolls
 *      The set of dice to be placed into the bins
 */
void fill_in_bins(int numberOfDice, int* bins, char* diceRolls) {
    for(int i = 0; i < numberOfDice; i++) {
        if(diceRolls[i] == '1') {
            bins[0] += 1;
        } else if(diceRolls[i] == '2') {
            bins[1] += 1;
        } else if(diceRolls[i] == '3') {
            bins[2] += 1;
        } else if(diceRolls[i] == 'H') {
            bins[3] += 1;
        } else if(diceRolls[i] == 'A') {
            bins[4] += 1;
        } else if(diceRolls[i] == 'P') {
            bins[5] += 1;
        }
    }
}

/**
 * @brief Sorts a set of dice in place
 *
 * @param diceRolls
 *      A set of dice
 */
void sort(char* diceRolls) {
    int* bins = (int*) malloc(sizeof(int) * 6);
    int numberOfDice = 0;
    int position = 0;
    // Count the dice
    while(1) {
        char dice = diceRolls[position];
        if(dice == '\n' || dice == '\0') {
            break;
        }
        numberOfDice += 1;
        position += 1;
    }

    // Clear the array
    for(int i = 0; i < numberOfDice; i++) {
        bins[i] = 0;
    }

    fill_in_bins(numberOfDice, bins, diceRolls);

    // Take the dice back out of the bins in order
    int counter = 0;
    for(int i = 0; i < numberOfDice; i++) {
        while(bins[i] != 0) {
            if(i == 0) {
                diceRolls[counter] = '1';
            } else if(i == 1) {
                diceRolls[counter] = '2';
            } else if(i == 2) {
                diceRolls[counter] = '3';
            } else if(i == 3) {
                diceRolls[counter] = 'H';
            } else if(i == 4) {
                diceRolls[counter] = 'A';
            } else if(i == 5) {
                diceRolls[counter] = 'P';
            }
            counter += 1;
            bins[i] -= 1;
        }
    }
}

/**
 * @brief Goes through a set of dice and makes sure it only contains dice and
 * whitespace.
 *
 * @param rollFileContent
 *      A dice set
 *
 * @return True if and only if a set of dice only contains dice rolls
 */
bool check_only_rolls(char* rollFileContent) {
    for(int i = 0; i < strlen(rollFileContent); i++) {
        if(rollFileContent[i] != 'H'
                && rollFileContent[i] != 'A'
                && rollFileContent[i] != 'P'
                && !(rollFileContent[i] <= '3' && rollFileContent[i] >= '1')
                && rollFileContent[i] != ' '
                && rollFileContent[i] != '\n') {
            return false;
        }
    }
    return true;
}

