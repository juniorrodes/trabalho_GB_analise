#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define MAX(a, b) a < b ? b : a
#define MIN(a, b) a > b ? b : a

typedef struct {
    int top;
    int bottom;
} Plate;

typedef struct {
    Plate* plates;
    int number_of_plates;
} Game;

Game parseGame(FILE* file);
void destroyGame(Game* game);
void calculateMax(Game game, int* max_top, int* max_bottom, int swap_index);
int isPossibleCombination(Game game, int exclude_index, Plate** excluded_plate);

int main(int argc, char** argv) {
    FILE *input_file;

    if (argc < 2) {
        printf("Missing arguments\n");
        return -1;
    }
    char *file_name = (char*)malloc(100 * sizeof(char));
    sprintf(file_name, "resources/%s", argv[1]);
    printf("%s\n", file_name);

    input_file = fopen(file_name, "r");
    free(file_name);
    if (input_file == NULL) {
        perror("failed to open file");
        return -1;
    }
    
    Game game = parseGame(input_file);
    while (game.number_of_plates != 0) {
        printf("number of plates: %d\n", game.number_of_plates);
        for(int i = 0; i < game.number_of_plates; i++) {
            printf("plate %d: [%d %d]\n", i + 1, game.plates[i].top, game.plates[i].bottom);
        }
        int max_sum = INT_MIN;
        Plate* excluded_plate = NULL;
        for (int i = -1; i < game.number_of_plates; i++) {
            max_sum = MAX(isPossibleCombination(game, i, &excluded_plate), max_sum);
        }
        if (max_sum == INT_MIN) {
            printf("Impossível\n");
        } else {
            if (excluded_plate != NULL) {
                printf(
                    "%d descartada a placa %d, %d\n",
                    max_sum,
                    MIN(excluded_plate->top, excluded_plate->bottom),
                    MAX(excluded_plate->top, excluded_plate->bottom)
                );
            } else {
                printf("%d nenhuma placa descartada\n", max_sum);
            }
        }
        destroyGame(&game);

        game = parseGame(input_file);
    }
    destroyGame(&game);

    fclose(input_file);
}

char readLine(FILE* file, char* buffer) {
    size_t offset = 0;
    memset(buffer, '\0', 10 * sizeof(char));

    char byte = fgetc(file);
    while(byte != '\n' && byte != EOF) {
        buffer[offset++] = byte;
        byte = fgetc(file);
    }
    buffer[offset] = byte;

    return byte;
}

Plate parsePlate(char* line) {
    char number_string[5] = {0};
    Plate plate = {0};
    int i = 0;
    for(; line[i] != ' '; i++) {
        number_string[i] = line[i];
    }
    int space_index = i;
    i += 1;
    number_string[i] = '\0';
    plate.top = atoi(number_string);
    for(; line[i] != '\0'; i++) {
        number_string[i - space_index - 1] = line[i];
    }
    number_string[i - space_index - 1] = '\0';
    plate.bottom = atoi(number_string);
    return plate;
}

Game parseGame(FILE* file) {
    char* line = (char*)malloc(10 * sizeof(char));
    readLine(file, line);

    int number_of_plates = atoi(line);
    
    Game game = {
        .number_of_plates = number_of_plates,
        .plates = malloc(number_of_plates * sizeof(Plate)),
    };
    for (int i = 0; i < number_of_plates; i++) {
        readLine(file, line);
        game.plates[i] = parsePlate(line);
    }
    free(line);
    return game;
}

void destroyGame(Game* game) {
    free(game->plates);
}

void calculateMax(Game game, int* sum_top, int* sum_bottom, int swap_index) {
    *sum_top = *sum_bottom = 0;
    for (int i = 0; i < game.number_of_plates; i++) {
        if (swap_index != -1 && swap_index == i) {
            *sum_bottom += game.plates[i].top;
            *sum_top += game.plates[i].bottom;
        }
        *sum_bottom += game.plates[i].bottom;
        *sum_top += game.plates[i].top;
    }
}

int isPossibleCombination(Game game, int exclude_index, Plate** excluded_plate) {
    int max_sum = INT_MIN;
    for (int i = 1; i < (1 << game.number_of_plates); i = i << 1) {
        if (i & (1 << exclude_index)) continue;

        int current_top_sum = 0, current_bottom_sum = 0;
        for (int j = 0; j < game.number_of_plates; j++) {
            if (exclude_index == j) continue;
            if (i & (1 << j)) {
                current_top_sum += game.plates[j].bottom;
                current_bottom_sum += game.plates[j].top;
                continue;
            }
            current_top_sum += game.plates[j].top;
            current_bottom_sum += game.plates[j].bottom;
        }


        if (current_bottom_sum == current_top_sum && max_sum < (MAX(current_top_sum, max_sum))) {
            if (exclude_index > -1) {
                *excluded_plate = &game.plates[exclude_index];
            } 
            max_sum = MAX(current_top_sum, max_sum);
        }
    }
    int current_top_sum = 0, current_bottom_sum = 0;
    for (int j = 0; j < game.number_of_plates; j++) {
        current_top_sum += game.plates[j].top;
        current_bottom_sum += game.plates[j].bottom;
    }

    if (current_bottom_sum == current_top_sum) {
        max_sum = MAX(current_top_sum, max_sum);
        *excluded_plate = NULL;
    }

    return max_sum;
}
