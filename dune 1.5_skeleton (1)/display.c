/*
* display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ������� �»�� ��ǥ ����
const POSITION resource_pos = { 0, 0 };         // �ڿ� ����
const POSITION map_pos = { 1, 0 };              // �� ��ġ
const POSITION system_message_pos = { 19, 0 };  // �ý��� �޽��� ��ġ
const POSITION object_info_pos = { 1, 62 - 2 }; // ����â ��ġ
const POSITION command_pos = { 19, 62 - 2 };    // ���â ��ġ

// �� ���ۿ� ����Ʈ ���� ����
char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

// �Լ� ����
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_system_message();
void display_object_info();
void display_commands();

// display �Լ�: ������ ��� ȭ�� ��Ҹ� ���
void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor)
{
    display_resource(resource);   // �ڿ� ���� ǥ��
    display_map(map);             // �� ǥ��
    display_cursor(cursor);       // Ŀ�� ǥ��
    display_system_message();     // �ý��� �޽��� ǥ��
    display_object_info();        // ������Ʈ ���� ǥ��
    display_commands();           // ��ɾ� ǥ��
}

// �ڿ� ���¸� ǥ���ϴ� �Լ�
void display_resource(RESOURCE resource) {
    set_color(COLOR_RESOURCE);
    gotoxy(resource_pos);
    printf("spice = %d/%d, population = %d/%d\n",
        resource.spice, resource.spice_max,
        resource.population, resource.population_max
    );
}

// draw_map()�� ���� �Լ�: ���� ���̾ �ϳ��� ������ �����Ͽ� ��� �غ�
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            for (int k = 0; k < N_LAYER; k++) {
                if (src[k][i][j] >= 0) {
                    dest[i][j] = src[k][i][j];
                }
            }
        }
    }
}

// ���� ȭ�鿡 ����ϴ� �Լ�
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    project(map, backbuf);  // �� ���ۿ� �� ������ ����

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (frontbuf[i][j] != backbuf[i][j]) {  // ����� ��ġ�� ������Ʈ
                POSITION pos = { i, j };
                set_color(frontbuf[i][j]);
                printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
            }
            frontbuf[i][j] = backbuf[i][j];  // ����Ʈ ���۸� �� ���۷� ������Ʈ
        }
    }

    // ���ϴܿ� �÷��̾� ����
    set_color_map(COLOR_DEFAULT, COLOR_BLUE);
    printc_map((POSITION) { MAP_HEIGHT - 2, 1 }, 'B', COLOR_DEFAULT, COLOR_BLUE);
    printc_map((POSITION) { MAP_HEIGHT - 1, 1 }, 'B', COLOR_DEFAULT, COLOR_BLUE);
    printc_map((POSITION) { MAP_HEIGHT - 2, 2 }, 'B', COLOR_DEFAULT, COLOR_BLUE);
    printc_map((POSITION) { MAP_HEIGHT - 1, 2 }, 'B', COLOR_DEFAULT, COLOR_BLUE);

    set_color_map(COLOR_DEFAULT, COLOR_BLUE);
    printc_map((POSITION) { MAP_HEIGHT - 3, 1 }, 'H', COLOR_DEFAULT, COLOR_BLUE);
    set_color_map(COLOR_DEFAULT, COLOR_SPICE);
    printc_map((POSITION) { MAP_HEIGHT - 5, 1 }, '5', COLOR_DEFAULT, COLOR_SPICE);
    set_color_map(COLOR_PLATE, COLOR_PLATE);
    printc_map((POSITION) { MAP_HEIGHT - 1, 3 }, 'P', COLOR_PLATE, COLOR_PLATE);
    printc_map((POSITION) { MAP_HEIGHT - 2, 4 }, 'P', COLOR_PLATE, COLOR_PLATE);
    printc_map((POSITION) { MAP_HEIGHT - 2, 3 }, 'P', COLOR_PLATE, COLOR_PLATE);
    printc_map((POSITION) { MAP_HEIGHT - 1, 4 }, 'P', COLOR_PLATE, COLOR_PLATE);

    // ���ܿ� AI ����
    set_color_map(COLOR_DEFAULT, COLOR_RED);
    printc_map((POSITION) { 2, MAP_WIDTH - 2 }, 'B', COLOR_DEFAULT, COLOR_RED);
    printc_map((POSITION) { 2, MAP_WIDTH - 3 }, 'B', COLOR_DEFAULT, COLOR_RED);
    printc_map((POSITION) { 3, MAP_WIDTH - 2 }, 'B', COLOR_DEFAULT, COLOR_RED);
    printc_map((POSITION) { 3, MAP_WIDTH - 3 }, 'B', COLOR_DEFAULT, COLOR_RED);
    
    set_color_map(COLOR_DEFAULT, COLOR_RED);
    printc_map((POSITION) { 4, MAP_WIDTH - 2 }, 'H', COLOR_DEFAULT, COLOR_RED);
    set_color_map(COLOR_DEFAULT, COLOR_SPICE);
    printc_map((POSITION) { 6, MAP_WIDTH - 2 }, '5', COLOR_DEFAULT, COLOR_SPICE);
    set_color_map(COLOR_PLATE, COLOR_PLATE);
    printc_map((POSITION) { 2, MAP_WIDTH - 4 }, 'P', COLOR_PLATE, COLOR_PLATE);
    printc_map((POSITION) { 3, MAP_WIDTH - 4 }, 'P', COLOR_PLATE, COLOR_PLATE);
    printc_map((POSITION) { 2, MAP_WIDTH - 5 }, 'P', COLOR_PLATE, COLOR_PLATE);
    printc_map((POSITION) { 3, MAP_WIDTH - 5 }, 'P', COLOR_PLATE, COLOR_PLATE);

    // ����� ��ġ
    set_color_map(COLOR_DEFAULT, COLOR_WORM);
    printc_map((POSITION) { 3, 7 }, 'W', COLOR_DEFAULT, COLOR_WORM);
    printc_map((POSITION) { 13, MAP_WIDTH - 13 }, 'W', COLOR_DEFAULT, COLOR_WORM);

    // ���� ��ġ
    set_color_map(COLOR_ROCK, COLOR_ROCK);
    printc_map((POSITION) { 14, 8 }, 'R', COLOR_ROCK, COLOR_ROCK);
    printc_map((POSITION) { 5, MAP_WIDTH - 8 }, 'R', COLOR_ROCK, COLOR_ROCK);
    printc_map((POSITION) { 6, 21 }, 'R', COLOR_ROCK, COLOR_ROCK);
    printc_map((POSITION) { 5, 21 }, 'R', COLOR_ROCK, COLOR_ROCK);
    printc_map((POSITION) { 6, 22 }, 'R', COLOR_ROCK, COLOR_ROCK);
    printc_map((POSITION) { 5, 22 }, 'R', COLOR_ROCK, COLOR_ROCK);
    printc_map((POSITION) { 15, MAP_WIDTH - 5 }, 'R', COLOR_ROCK, COLOR_ROCK);
    printc_map((POSITION) { 13, MAP_WIDTH - 21 }, 'R', COLOR_ROCK, COLOR_ROCK);
    printc_map((POSITION) { 14, MAP_WIDTH - 21 }, 'R', COLOR_ROCK, COLOR_ROCK);
    printc_map((POSITION) { 13, MAP_WIDTH - 22 }, 'R', COLOR_ROCK, COLOR_ROCK);
    printc_map((POSITION) { 14, MAP_WIDTH - 22 }, 'R', COLOR_ROCK, COLOR_ROCK);
}

// Ŀ���� �ʿ� ǥ���ϴ� �Լ�
void display_cursor(CURSOR cursor) {
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;

    char ch = frontbuf[prev.row][prev.column];
    printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

    ch = frontbuf[curr.row][curr.column];
    printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

// �ý��� �޽��� ��� �Լ�
void display_system_message() {
    set_color(COLOR_SYSTEM_MESSAGE);
    gotoxy(system_message_pos);
    for (int i = 0; i < 60; i++) {
        printf("#");
    }
    printf("\n");

    for (int i = 0; i < 5; i++) {
        printf("#                                                          #\n");
    }

    for (int i = 0; i < 60; i++) {
        printf("#");
    }
    printf("\n");
}

// ����â ��� �Լ�
void display_object_info() {
    set_color(COLOR_OBJECT_INFO);
    gotoxy(object_info_pos);

    for (int i = 0; i < 56; i++) {
        printf("#");
    }
    printf("\n");

    for (int i = 0; i < MAP_HEIGHT - 2; i++) {
        gotoxy((POSITION) { object_info_pos.row + 1 + i, object_info_pos.column });
        printf("#                                                      #\n");
    }

    gotoxy((POSITION) { object_info_pos.row + MAP_HEIGHT - 1, object_info_pos.column });
    for (int i = 0; i < 56; i++) {
        printf("#");
    }
    printf("\n");
}

// ���â ��� �Լ�
void display_commands() {
    set_color(COLOR_COMMAND);
    gotoxy(command_pos);

    for (int i = 0; i < 56; i++) {
        printf("#");
    }
    printf("\n");

    for (int i = 0; i < 5; i++) {
        gotoxy((POSITION) { command_pos.row + 1 + i, command_pos.column });
        printf("#                                                      #\n");
    }

    gotoxy((POSITION) { command_pos.row + 6, command_pos.column });
    for (int i = 0; i < 56; i++) {
        printf("#");
    }
    printf("\n");
}