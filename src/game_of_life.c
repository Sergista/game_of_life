#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define ROW 25
#define COL 80

void game();
void render(int **world);
void step(int **world);
int count_of_neighbors(int **world, int w, int h);
int first_set(int **world);
void key_handle(int key, int *speed);
void init();
int **create_matrix();
void free_matrix(int **matrix);
void copy_matrix(int **matrix1, int **matrix2);
void default_set(int **world);
int is_end(int **world);

int main() {
    game();
    return 0;
}

void game() {
    int key = ' ';
    int **world = create_matrix();
    int speed = 240;
    init();
    if (first_set(world) == -1) {
        printf("Stream redirect error\n");
        return;
    }
    while (key != 'e' && is_end(world) == 0) {
        key_handle(key, &speed);
        render(world);
        napms(speed);
        step(world);
        key = getch();
    }
    endwin();
    free_matrix(world);
}

void init() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, 1);
}

void key_handle(int key, int *speed) {
    if (key == '=' && *speed >= 60)
        *speed -= 20;
    else if (key == '-' && *speed <= 500)
        *speed += 20;
}

int first_set(int **world) {
    int ret = 0;
    if (isatty(STDIN_FILENO))
        default_set(world);
    else {
        for (int i = 0; i < ROW; i++)
            for (int j = 0; j < COL; j++) world[i][j] = (int)getchar() - 48;
    }
    if (freopen("/dev/tty", "r", stdin) == NULL) ret = -1;
    return ret;
}

void default_set(int **world) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if ((i == 12 && j == 12) || (i == 12 && j == 13) || (i == 12 && j == 14) ||
                (i == 11 && j == 14) || (i == 10 && j == 13)) {
                world[i][j] = 1;
            } else
                world[i][j] = 0;
        }
    }
}

void render(int **world) {
    clear();
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (world[i][j] == 1)
                printw("o");
            else
                printw(" ");
        }
        printw("\n");
    }
    refresh();
}

void step(int **world) {
    int **world_temp = create_matrix();
    copy_matrix(world, world_temp);
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            int neighbors = count_of_neighbors(world, i, j);
            if (world_temp[i][j] == 1) {
                if (neighbors <= 1 || neighbors >= 4)
                    world_temp[i][j] = 0;
                else
                    world_temp[i][j] = 1;
            } else {
                if (neighbors == 3)
                    world_temp[i][j] = 1;
                else
                    world_temp[i][j] = 0;
            }
        }
    }
    copy_matrix(world_temp, world);
    free_matrix(world_temp);
}

int count_of_neighbors(int **world, int w, int h) {
    int count = 0;
    for (int i = w - 1; i <= w + 1; i++) {
        for (int j = h - 1; j <= h + 1; j++) {
            int r = (i + ROW) % ROW;
            int c = (j + COL) % COL;
            if (!(i == w && j == h)) count += world[r][c];
        }
    }
    return count;
}

int **create_matrix() {
    int **matrix = malloc(sizeof(int *) * ROW);
    for (int i = 0; i < ROW; i++) matrix[i] = malloc(sizeof(int) * COL);
    return matrix;
}

void free_matrix(int **matrix) {
    for (int i = 0; i < ROW; i++) free(matrix[i]);
    free(matrix);
}

void copy_matrix(int **matrix1, int **matrix2) {
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++) matrix2[i][j] = matrix1[i][j];
}

int is_end(int **world) {
    int ret = -1;
    for (int i = 0; i < ROW && ret == -1; i++)
        for (int j = 0; j < COL && ret == -1; j++)
            if (world[i][j] == 1) ret = 0;
    return ret;
}