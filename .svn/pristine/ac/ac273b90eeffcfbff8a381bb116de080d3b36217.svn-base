#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/* A summary of the dice rolls from a set of dice */
typedef struct {
    int one;
    int two;
    int three;
    int health;
    int attack;
    int points;
} DiceContent;

char* get_file_content(char*);
bool check_only_integers(char*);
char** split_string(char* firstLine, int target);
bool check_valid_range(int value, int min, int max);
char* get_file_content_by_file(FILE* file, int size);
int get_file_size(FILE* file);
char* read_in_file_content(FILE* file);
int count_the_words(char*);
char* get_target_word(char*, int);
bool check_roll_order(char*);
int count_dice(char*);
DiceContent* get_dice_content(char*);
void sort(char*);
bool check_only_rolls(char*);

#endif
