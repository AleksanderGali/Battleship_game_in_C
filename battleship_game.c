/* Title: Battleship Game
 * Description: Battleship game. User versus computer.
 * Author: Aleksander Gali */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "battleship_game.h"

/* Function check if user won or lost the game */
int check_win_condition(struct map_pattern *map_set) {

    int flag_user = 0;
    int flag_enemy = 0;

    for(int x = 0; x < 10; x++) {
        for(int y = 0; y < 10; y++) {

            if ((map_set->map_user[x][y] == 'C') || (map_set->map_user[x][y] == 'B') ||
                (map_set->map_user[x][y] == 'D') || (map_set->map_user[x][y] == 'S') ||
                (map_set->map_user[x][y] == 'P')) {
                    flag_user++;
            } else if
               ((map_set->map_enemy[x][y] == 'C') || (map_set->map_enemy[x][y] == 'B') ||
                (map_set->map_enemy[x][y] == 'D') || (map_set->map_enemy[x][y] == 'S') ||
                (map_set->map_enemy[x][y] == 'P')) {
                    flag_enemy++;
            }
        }
    }

    if (flag_enemy == 0) {
        printf("Congratulations! You won the game!\n");
        exit(0);
    } else if (flag_user == 0) {
        printf("You lost the game! Try again!\n");
        exit(0);
    }
    return 0;
}

/* Choosing axis on which user boat will be placed */
int choose_boat_axis(int *boat_axis){

    char letter;
    printf("You want to put boat vertical | (type 'V') or horizontally - (type 'H')\n");

    while ((scanf("%c", &letter) != 1) || (letter != 'V') && (letter != 'H')) {
        printf("Wrong letter, try again! (V or H)\n");
        while (getchar() != '\n');
    }
    while (getchar() != '\n');

    if (letter == 'V') {
        *boat_axis = 2;
    } else if (letter == 'H') {
        *boat_axis = 3;
    } else {
        printf("choose_boat_error: letter value error\n");
        return 1;
    }
    return 0;
}

/* Function prepares data for the boat creation */
int generate_boat_management(struct map_pattern *map_set, struct boat_struct *boat_spec, const int *human) {

    int gen_rv;
    time_t t;
    srand((unsigned)time(&t));
    int ret = 0;

    while (boat_spec->flag < boat_spec->boat_quantity) {

        if(*human) {
            ret = map_print(map_set);
            if (ret) {
                printf("generate_boat_management: map_print return value");
                return 1;
            }
            printf("%s\n", boat_spec->boat_info);
            ret = choose_boat_axis(&boat_spec->boat_axis);
            if (ret) {
                printf("generate_boat_management: choose_boat_axis return value");
                return 1;
            }
            ret = user_input_coordinates(&boat_spec->boat_x, &boat_spec->boat_y);
            if (ret) {
                printf("generate_boat_management: user_input_coordinates return value");
                return 1;
            }
        } else {
            boat_spec->boat_axis = rand() % 2 + 2;
            boat_spec->boat_x = rand() % 10;
            boat_spec->boat_y = rand() % 10;
        }

        /* Boat have to be on the map */
        if ((boat_spec->boat_axis == 3) && ((boat_spec->boat_x + boat_spec->boat_length) > 10)) {
            if (*human) { /* Print warnings only to humans */
                printf("Boat won't fit on the map! Try again!");
            }
            continue;
        } else if ((boat_spec->boat_axis == 2) && ((boat_spec->boat_y + boat_spec->boat_length) > 10)) {
            if (*human) { /* Print warnings only to humans */
                printf("Boat won't fit on the map! Try again!");
            }
            continue;
        }

        if (*human) { /* place ships on users map */
            gen_rv = generate_ship(map_set->map_user, boat_spec->boat_axis, boat_spec->boat_length, boat_spec->boat_x,
                                   boat_spec->boat_y, boat_spec->sub);
        } else {
            gen_rv = generate_ship(map_set->map_enemy, boat_spec->boat_axis, boat_spec->boat_length, boat_spec->boat_x,
                                   boat_spec->boat_y, boat_spec->sub);
        }
        if (gen_rv == 2) {
            if (*human) { /* Print warnings only to humans */
                printf("Ships can't cross yourself! Repeat\n");
            }
            continue;
        } else if (gen_rv == 1) {
            if (*human) { /* Print warnings only to humans */
                printf("generate_users_boats: generate_ship return value\n");
            }
        } else {
            boat_spec->flag += 1;
        }
    }
    boat_spec->flag = 0; /* reset flag */
    return 0;
}

/* function that set boats parameters and call management functions for the computer */
int generate_enemy_boats(struct map_pattern *map_set) {

    struct boat_struct enemy_boat_spec = {0};
    int human = 0; /* not a human */

    /* Carrier */
    enemy_boat_spec.boat_length = 5;
    enemy_boat_spec.boat_quantity = 1;
    generate_boat_management(map_set, &enemy_boat_spec, &human);

    /* Battleships */
    enemy_boat_spec.boat_length = 4;
    enemy_boat_spec.boat_quantity = 1;
    generate_boat_management(map_set, &enemy_boat_spec, &human);

    /* Destroyer */
    enemy_boat_spec.boat_length = 3;
    enemy_boat_spec.boat_quantity = 1;
    generate_boat_management(map_set, &enemy_boat_spec, &human);

    /* Submarine */
    enemy_boat_spec.boat_length = 3;
    enemy_boat_spec.boat_quantity = 2;
    enemy_boat_spec.sub = 1; /* true for submarine */
    generate_boat_management(map_set, &enemy_boat_spec, &human);

    /* Patrol boat */
    enemy_boat_spec.boat_length = 2;
    enemy_boat_spec.boat_quantity = 3;
    enemy_boat_spec.sub = 0;
    generate_boat_management(map_set, &enemy_boat_spec, &human);
    return 0;
}

/* function that set boats parameters and call management functions for the user */
int generate_users_boats(struct map_pattern *map_set) {

    struct boat_struct boat_spec = {0};
    char carrier_info[39] = "Time to place your Carrier (5 squares)";
    char battleship_info[42] = "Time to place your Battleship (4 squares)";
    char destroyer_info[41] = "Time to place your Destroyer (3 squares)";
    char submarine_info[41] = "Time to place your Submarine (3 squares)";
    char patrol_boat_info[43] = "Time to place your Patrol boat (2 squares)";
    int human = 1;

    /* Carrier */
    boat_spec.boat_length = 5;
    boat_spec.boat_quantity = 1;
    boat_spec.boat_info = carrier_info;
    generate_boat_management(map_set, &boat_spec, &human);

    /* Battleships */
    boat_spec.boat_length = 4;
    boat_spec.boat_quantity = 1;
    boat_spec.boat_info = battleship_info;
    generate_boat_management(map_set, &boat_spec, &human);

    /* Destroyer */
    boat_spec.boat_length = 3;
    boat_spec.boat_quantity = 1;
    boat_spec.boat_info = destroyer_info;
    generate_boat_management(map_set, &boat_spec, &human);

    /* Submarine */
    boat_spec.boat_length = 3;
    boat_spec.boat_quantity = 2;
    boat_spec.sub = 1;
    boat_spec.boat_info = submarine_info;
    generate_boat_management(map_set, &boat_spec, &human);

    /* Patrol boat */
    boat_spec.boat_length = 2;
    boat_spec.boat_quantity = 3;
    boat_spec.sub = 0; /* change submarine flag back to zero*/
    boat_spec.boat_info = patrol_boat_info;
    generate_boat_management(map_set, &boat_spec, &human);

    return 0;
}

/* placing ships on the map and checking if ships don't cross*/
int generate_ship(char selected_map[10][10], int axis, int boat_length, int boat_x, int boat_y, int sub) {

    int boat_offset_x = 0;
    int boat_offset_y = 0;
    char symbol;

    /* Set boat symbol */
    switch (boat_length) {
        case 5:
            symbol = 'C';
            break;
        case 4:
            symbol = 'B';
            break;
        case 3:
            if (sub) {
                symbol = 'S';
            } else {
                symbol = 'D';
            }
            break;
        case 2:
            symbol = 'P';
            break;
        default:
            printf("generate_ship: switch boat symbol");
            return 1;
    }

    /* create boat */
    for (int i = 0; i < boat_length; i++) {

        /* Check if ships are not crossing */
        if ((selected_map[boat_y + boat_offset_y][boat_x + boat_offset_x] == 'C') ||
            (selected_map[boat_y + boat_offset_y][boat_x + boat_offset_x] == 'B') ||
            (selected_map[boat_y + boat_offset_y][boat_x + boat_offset_x] == 'D') ||
            (selected_map[boat_y + boat_offset_y][boat_x + boat_offset_x] == 'S') ||
            (selected_map[boat_y + boat_offset_y][boat_x + boat_offset_x] == 'P')) {
            return 2; /* ships are crossing */
        }

        if (axis == 3) {
            boat_offset_x += 1;
        } else if (axis == 2) {
            boat_offset_y += 1;
        } else {
            printf("generate_ship: wrong axis");
            return 1;
        }
    }

    boat_offset_x = 0;
    boat_offset_y = 0;

    /* place ship */
    for (int j = 0; j < boat_length; j++) {

        selected_map[boat_y + boat_offset_y][boat_x + boat_offset_x] = symbol;

        if (axis == 3) {
            boat_offset_x += 1;
        } else if (axis == 2) {
            boat_offset_y += 1;
        } else {
            printf("generate_ship: wrong axis");
            return 1;
        }
    }
    return 0;
}

/* filling maps with the dots */
int map_generate(struct map_pattern *map_set) {

    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            map_set->map_user[y][x] = '.';
            map_set->map_target[y][x] = '.';
            map_set->map_enemy[y][x] = '.';
        }
    }
    return 0;
}

/* printing maps for the user */
int map_print(struct map_pattern *map_set) {

    int x, y;
    int letters = 'A';

    int num1 = 0; /* 0-9 index for first map */
    int num2 = 0; /* 0-9 index for second map */

    printf("\n             User Map                            Target Map\n");

    /* printing out ABC */
    for (x = 0; x < 2; x++) {
        for (y = 0; y < 11; y++) {

            if (y == 0) {
                printf("   ");
            } else {
                printf(" %c ", letters++);
            }
        }
        printf("   ");
        letters = 'A';
    }

    printf("\n");

    /* printing out arrays */
    for (x = 0; x < 10; x++) {
        for (y = 0; y < 10; y++) {

            if (y == 0) { /* printing out 0123 */
                printf(" %d ", num1++);
            }
            printf(" %c ", map_set->map_user[x][y]);
        }

        printf("   ");

        for (y = 0; y < 10; y++) {
            if (y == 0) { /* printing out 0123 */
                printf(" %d ", num2++);
            }
            printf(" %c ", map_set->map_target[x][y]);
        }
        printf("\n");
    }
    printf("\n"); /* extra space after the map */
    return 0;
}

/* shooting at the user map by the computer */
int enemy_shot(struct map_pattern *map_set) {

    time_t t;
    srand((unsigned)time(&t));
    int shot_y;
    int shot_x;

    /* Never shoot in the same place more than once */
    while (1) {
        shot_y = rand() % 10;
        shot_x = rand() % 10;

        if (map_set->map_user[shot_y][shot_x] == 'x') {
            continue;
        } else {
            if (map_set->map_user[shot_y][shot_x] != '.') {
                printf("Sir! Our ship got hit!");
            }
            map_set->map_user[shot_y][shot_x] = 'x';
            break;
        }
    }

    return 0;
}

/* choosing coordinates by the user */
int user_input_coordinates(int *user_x, int *user_y) {

    char letter;

    /* input x coordinate */
    printf("Please, insert x coordinate letter!\n");
    while ((scanf(" %c", &letter) != 1) || (letter < 'A') || (letter > 'J')) {
        printf("Wrong letter, try again!\n");
        while (getchar() != '\n');
    }
    while (getchar() != '\n');

    /* input y coordinate */
    printf("Please, insert y coordinate number!\n");
    while (scanf("%d", user_y) != 1 || *user_y < 0 || *user_y > 9) {
        printf("Wrong number, try again!\n");
        while (getchar() != '\n');
    }
    while (getchar() != '\n'); /* emptying buffer */

    /* change y coordinate from the letter to the number */
    switch (letter) {
        case 'A':
            *user_x = 0;
            break;
        case 'B':
            *user_x = 1;
            break;
        case 'C':
            *user_x = 2;
            break;
        case 'D':
            *user_x = 3;
            break;
        case 'E':
            *user_x = 4;
            break;
        case 'F':
            *user_x = 5;
            break;
        case 'G':
            *user_x = 6;
            break;
        case 'H':
            *user_x = 7;
            break;
        case 'I':
            *user_x = 8;
            break;
        case 'J':
            *user_x = 9;
            break;
        default:
            printf("user_input_coordinates: switch\n");
            return 1;
    }

    /* check if switch changed the letter properly */
    if(*user_x < 0 || *user_x > 9) {
        printf("user_input_coordinates: wrong input value\n");
        return 1;
    }
    return 0;
}

/* shooting at enemy map by the user. It will mark a shot on the target map */
int user_shot(struct map_pattern *map_set) {

    int coordinate_y = 100; /* If it won't be changed, it will cause error */
    int coordinate_x = 100;
    int rv;

    printf("Time to make a shot!\n");
    rv = user_input_coordinates(&coordinate_x, &coordinate_y);
    if (rv) {
        return 1;
    }

    if (map_set->map_enemy[coordinate_y][coordinate_x] != '.') {
        printf("You hit enemy ship!");
    }

    map_set->map_target[coordinate_y][coordinate_x] = 'x';
    map_set->map_enemy[coordinate_y][coordinate_x] = 'x';
    return 0;
}

int main() {

    struct map_pattern map_set;
    int return_value; /* Error checking in main game loop */

    printf("Welcome to the battleship game!\nPress Enter to start!");
    getchar();

    /* Generate maps and game units */
    return_value = map_generate(&map_set);
    if(return_value) {
        printf("main: map_generate return value\n");
        exit(1);
    }
    return_value = generate_enemy_boats(&map_set);
    if(return_value) {
        printf("main: generate_enemy_boats return value\n");
        exit(1);
    }
    return_value = generate_users_boats(&map_set);
    if(return_value) {
        printf("main: generate_users_boats return value\n");
        exit(1);
    }

    /* Main game loop */
    while (check_win_condition(&map_set) == 0) {

        return_value = map_print(&map_set);
        if(return_value) {
            printf("main: map_print return value\n");
            break;
        }
        return_value = user_shot(&map_set);
        if(return_value) {
            printf("main: user_shot return value\n");
            break;
        }
        return_value = enemy_shot(&map_set);
        if(return_value) {
            printf("main: enemy_shot return value\n");
            break;
        }
    }
    printf("main: quiting main loop\n");
    exit(1);
}