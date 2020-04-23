#ifndef UNTITLED_BATTLESHIP_GAME_H
#define UNTITLED_BATTLESHIP_GAME_H

/* collection of maps for the game */
struct map_pattern {
    char map_user[10][10];
    char map_target[10][10];
    char map_enemy[10][10];
};

/* parameters for boat construction */
struct boat_struct {
    int boat_axis;
    int boat_x;
    int boat_y;
    int boat_length;
    int boat_quantity;
    int flag;
    int sub; /* true if submarine */
    char *boat_info; /* string with boat name */
};

int check_win_condition(struct map_pattern *map_set);
int choose_boat_axis(int *boat_axis);
int generate_boat_management(struct map_pattern *map_set, struct boat_struct *boat_spec, const int *human);
int generate_enemy_boats(struct map_pattern *map_set);
int generate_users_boats(struct map_pattern *map_set);
int generate_ship(char selected_map[10][10], int axis, int boat_length, int boat_x, int boat_y, int sub);
int map_generate(struct map_pattern *map_set);
int map_print(struct map_pattern *map_set);
int enemy_shot(struct map_pattern *map_set);
int user_input_coordinates(int *user_x, int *ser_y);
int user_shot(struct map_pattern *map_set);

#endif