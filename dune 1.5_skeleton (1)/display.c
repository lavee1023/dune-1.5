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
void display_object_info(bool show_info, CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_commands(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool show_1);
void set_background_color(int color, unsigned short back);
void set_color_map(int color, unsigned short back);
void printc_map(POSITION pos, char ch, int color, unsigned short back);

// display �Լ�: ������ ��� ȭ�� ��Ҹ� ���
void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor,
    bool show_1)
{
    display_resource(resource);   // �ڿ� ���� ǥ��
    display_map(map);             // �� ǥ��
    display_cursor(cursor);       // Ŀ�� ǥ��
    display_system_message();     // �ý��� �޽��� ǥ��
    display_object_info(show_1, cursor, map);        // ������Ʈ ���� ǥ��
    display_commands(cursor, map, show_1);           // ��ɾ� ǥ��
}

void set_background_color(int color, unsigned short back) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color | (back << 4));
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

// ���� ���� ���� �Լ�
int get_unit_color(char player) {
    if (player == 'F') {
        return COLOR_BLUE;
    }
    else if (player == 'S') {
        return COLOR_BLUE;
    }
    else if (player == 'H') {
        return COLOR_BLUE;
    }
    else if (player == 'H') {
        return COLOR_RED;
    }
    else if (player == 'f') {
        return COLOR_RED;
    }
    else if (player == 't') {
        return COLOR_RED;
    }
    else if (player == 'h') {
        return COLOR_RED;
    }
    else if (player == 'W') {
        return COLOR_WORM;
    }
    return COLOR_DEFAULT;
}

// �ǹ� ���� ���� �Լ�
int get_building_color(char player) {
    if (player == 'B') {
        return COLOR_BLUE;
    }
    else if (player == 'S') {
        return COLOR_BLUE;
    }
    else if (player == 'b') {
        return COLOR_BLUE;
    }
    else if (player == 'D') {
        return COLOR_BLUE;
    }
    else if (player == 'G') {
        return COLOR_BLUE;
    }
    else if (player == 'a') {
        return COLOR_RED;
    }
    else if (player == 'f') {
        return COLOR_RED;
    }
    else if (player == 'v') {
        return COLOR_RED;
    }
    else if (player == 'd') {
        return COLOR_RED;
    }
    else if (player == 'g') {
        return COLOR_RED;
    }
    else if (player == 'P') {
        return COLOR_PLATE;
    }
    else if (player == 'p') {
        return COLOR_PLATE;
    }
    else if (player == 'R') {
        return COLOR_ROCK;
    }
    else if (player == '9') {
        return COLOR_SPICE;
    }
    else if (player == '8') {
        return COLOR_SPICE;
    }
    else if (player == '7') {
        return COLOR_SPICE;
    }
    else if (player == '6') {
        return COLOR_SPICE;
    }
    else if (player == '5') {
        return COLOR_SPICE;
    }
    else if (player == '4') {
        return COLOR_SPICE;
    }
    else if (player == '3') {
        return COLOR_SPICE;
    }
    else if (player == '2') {
        return COLOR_SPICE;
    }
    else if (player == '1') {
        return COLOR_SPICE;
    }

    return COLOR_DEFAULT;
}

// ���� ȭ�鿡 ����ϴ� �Լ�
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    project(map, backbuf);  // �� ���ۿ� �� ������ ����

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            POSITION pos = { i, j };
            unsigned short back = COLOR_DEFAULT;
            int color = COLOR_DEFAULT;

            // ����� ��ġ�� ������Ʈ
            if (frontbuf[i][j] != backbuf[i][j]) {
                if (backbuf[i][j] == ' ' || backbuf[i][j] == '#') {
                    printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
                }
                else {
                    // BUILDING�� ���� ����� ���� ����
                    if (map[0][i][j] != ' ') {
                        color = get_building_color(map[0][i][j]);
                        back = color;
                    }
                    // UNIT�� ���� ����� ���� ����
                    else if (map[1][i][j] != ' ') {
                        color = get_unit_color(map[1][i][j]);
                        back = color;
                    }

                    set_color_map(COLOR_DEFAULT, back);

                    // ������ �ؽ�Ʈ ������ �и��Ͽ� ���
                    printc_map(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT, back);
                }
            }

            frontbuf[i][j] = backbuf[i][j];  // ����Ʈ ���۸� �� ���۷� ������Ʈ
        }
    }
}

// Ŀ���� �ʿ� ǥ���ϴ� �Լ�
void display_cursor(CURSOR cursor) {
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;

    char prev_ch = frontbuf[prev.row][prev.column];
    unsigned short prev_back = COLOR_DEFAULT;

    if (prev_ch != ' ' && prev_ch != '#') {
        prev_back = get_building_color(prev_ch);
        if (prev_back == COLOR_DEFAULT) {
            prev_back = get_unit_color(prev_ch);
        }
    }

    if (prev_ch != ' ' && prev_ch != '#') {
        printc_map(padd(map_pos, prev), prev_ch, COLOR_DEFAULT, prev_back);
    }
    else {
        printc(padd(map_pos, prev), prev_ch, COLOR_DEFAULT);
    }

    char curr_ch = frontbuf[curr.row][curr.column];
    unsigned short curr_back = COLOR_CURSOR;

    printc_map(padd(map_pos, curr), curr_ch, COLOR_DEFAULT, curr_back);
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
        for (int j = 0; j < 60; j++) {
            if (j == 0) printf("#");
            else if (j == 59) printf("#\n");
            else printf(" ");
        }
    }

    for (int i = 0; i < 60; i++) {
        printf("#");
    }
    printf("\n");
}

// ����â ��� �Լ�
void display_object_info(bool show_info, CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    set_color(COLOR_OBJECT_INFO);
    gotoxy(object_info_pos);

    for (int i = 0; i < 56; i++) {
        printf("#");
    }
    printf("\n");
    static POSITION last_cursor_pos = { -1, -1 };

    if (cursor.current.row != last_cursor_pos.row || cursor.current.column != last_cursor_pos.column) {
        for (int i = 0; i < MAP_HEIGHT - 2; i++) {
            gotoxy((POSITION) { object_info_pos.row + 2 + i, object_info_pos.column + 2 });
            printf("                                                \n");
        }
    }

    if (show_info) {
        int row = cursor.current.row;
        int col = cursor.current.column;

        gotoxy((POSITION) { object_info_pos.row + 2, object_info_pos.column + 2 });

        char ch = frontbuf[row][col];

        if (ch != ' ') {
            if (map[0][row][col] != ' ') {
                char building_type = map[0][row][col];
                switch (building_type) {
                case 'B':
                    printf("(%d, %d)�� ��ġ�� atreides_Barracks\n", row, col);
                    break;
                case 'S':
                    printf("(%d, %d)�� ��ġ�� atreides_Shelter\n", row, col);
                    break;
                case 'b':
                    printf("(%d, %d)�� ��ġ�� atreides_Base\n", row, col);
                    break;
                case 'P':
                    printf("(%d, %d)�� ��ġ�� atreides_Plant\n", row, col);
                    break;
                case 'D':
                    printf("(%d, %d)�� ��ġ�� atreides_Dormitory\n", row, col);
                    break;
                case 'G':
                    printf("(%d, %d)�� ��ġ�� atreides_Garage\n", row, col);
                    break;
                case 'a':
                    printf("(%d, %d)�� ��ġ�� harkonnen_Arena\n", row, col);
                    break;
                case 'f':
                    printf("(%d, %d)�� ��ġ�� harkonnen_Factory\n", row, col);
                    break;
                case 'v':
                    printf("(%d, %d)�� ��ġ�� harkonnen_Base\n", row, col);
                    break;
                case 'p':
                    printf("(%d, %d)�� ��ġ�� harkonnen_Plant\n", row, col);
                    break;
                case 'd':
                    printf("(%d, %d)�� ��ġ�� harkonnen_Dormitory\n", row, col);
                    break;;
                case 'g':
                    printf("(%d, %d)�� ��ġ�� harkonnen_Garage\n", row, col);
                    break;
                case 'R':
                    printf("����ó�� �ܴ��ϰ�\n");
                    break;
                case '9':
                    printf("(%d, %d)�� ��ġ�� spice ���差 : %c\n", row, col, map[0][row][col]);
                    break;
                case '8':
                    printf("(%d, %d)�� ��ġ�� spice ���差 : %c\n", row, col, map[0][row][col]);
                    break;
                case '7':
                    printf("(%d, %d)�� ��ġ�� spice ���差 : %c\n", row, col, map[0][row][col]);
                    break;
                case '6':
                    printf("(%d, %d)�� ��ġ�� spice ���差 : %c\n", row, col, map[0][row][col]);
                    break;
                case '5':
                    printf("(%d, %d)�� ��ġ�� spice ���差 : %c\n", row, col, map[0][row][col]);
                    break;
                case '4':
                    printf("(%d, %d)�� ��ġ�� spice ���差 : %c\n", row, col, map[0][row][col]);
                    break;
                case '3':
                    printf("(%d, %d)�� ��ġ�� spice ���差 : %c\n", row, col, map[0][row][col]);
                    break;
                case '2':
                    printf("(%d, %d)�� ��ġ�� spice ���差 : %c\n", row, col, map[0][row][col]);
                    break;
                case '1':
                    printf("(%d, %d)�� ��ġ�� spice ���差 : %c\n", row, col, map[0][row][col]);
                    break;
                case '0':
                    printf("(%d, %d)�� ��ġ�� spice ���差 : %c\n", row, col, map[0][row][col]);
                    break;
                default:
                    break;
                }
            }
            else if (map[1][row][col] != ' ') {
                char unit_type = map[1][row][col];
                switch (unit_type) {
                case 'S':
                    printf("(%d, %d)�� ��ġ�� atreides_Soldier\n", row, col);
                    break;
                case 'F':
                    printf("(%d, %d)�� ��ġ�� atreides_Fremen\n", row, col);
                    break;
                case 'H':
                    printf("(%d, %d)�� ��ġ�� atreides_Harvester\n", row, col);
                    break;
                case 'f':
                    printf("(%d, %d)�� ��ġ�� harkonnen_Fighter\n", row, col);
                    break;
                case 't':
                    printf("(%d, %d)�� ��ġ�� harkonnen_HeavyTank\n", row, col);
                    break;
                case 'h':
                    printf("(%d, %d)�� ��ġ�� harkonnen_Harvester\n", row, col);
                    break;
                case 'W':
                    printf("(%d, %d)�� ��ġ�� Sandworm\n", row, col);
                    break;
                default:
                    break;
                }
            }
        }
        else {
            printf("�縷 ���� : �ǹ��� ���� �� �����ϴ�.\n");
        }

        last_cursor_pos = cursor.current;
    }
    else {
        gotoxy((POSITION) { object_info_pos.row + 2, object_info_pos.column + 2 });
        printf("                                                    ");
    }

    gotoxy((POSITION) { object_info_pos.row + MAP_HEIGHT - 1, object_info_pos.column });
    for (int i = 0; i < 56; i++) {
        printf("#");
    }
    printf("\n");

    for (int i = 1; i < MAP_HEIGHT - 1; i++) {
        gotoxy((POSITION) { object_info_pos.row + i, object_info_pos.column });
        printf("#");
        gotoxy((POSITION) { object_info_pos.row + i, object_info_pos.column + 55 });
        printf("#");
    }
}

// ���â ��� �Լ�
void display_commands(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool show_1) {
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
  
    if (show_1) {
        int row = cursor.current.row;
        int col = cursor.current.column;

        gotoxy((POSITION) { command_pos.row + 2, command_pos.column + 2 });

        char ch = frontbuf[row][col];

        if (ch != ' ') {
            if (map[0][row][col] != ' ') {
                char building_type = map[0][row][col];
                switch (building_type) {
                case 'B':
                    printf("S : ���� ����\n");
                    break;
                case 'S':
                    printf("F : ������ ����\n");
                    break;
                case 'b':
                    printf("H : �Ϻ����� ����\n");
                    break;
                case 'a':
                    printf("F : ���� ����\n");
                    break;
                case 'f':
                    printf("T : ������ ����\n");
                    break;
                case 'v':
                    printf("H : �Ϻ����� ����\n");
                    break;
                default:
                    break;
                }
            }
            else if (map[1][row][col] != ' ') {
                char unit_type = map[1][row][col];
                switch (unit_type) {
                case 'S':
                    printf("M : �̵� / P : ����\n");
                    break;
                case 'F':
                    printf("M : �̵� / P : ����\n");
                    break;
                case 'H':
                    printf("M : �̵� / H : �Ϻ�����\n");
                    break;
                case 'f':
                    printf("M : �̵� / P : ����\n");
                    break;
                case 't':
                    printf("M : �̵� / P : ����\n");
                    break;
                case 'h':
                    printf("M : �̵� / H : �Ϻ�����\n");
                    break;
                default:
                    break;
                }
            }
        }
    }
    else {
        gotoxy((POSITION) { command_pos.row + 2, command_pos.column + 2 });
        printf("INFO: �ƹ� ��ɵ� �����ϴ�.");
    }

    gotoxy((POSITION) { command_pos.row + 6, command_pos.column });
    for (int i = 0; i < 56; i++) {
        printf("#");
    }
    printf("\n");
}