/*
* display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단 좌표 설정
const POSITION resource_pos = { 0, 0 };         // 자원 상태
const POSITION map_pos = { 1, 0 };              // 맵 위치
const POSITION system_message_pos = { 19, 0 };  // 시스템 메시지 위치
const POSITION object_info_pos = { 1, 62 - 2 }; // 상태창 위치
const POSITION command_pos = { 19, 62 - 2 };    // 명령창 위치

// 백 버퍼와 프론트 버퍼 선언
char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

// 함수 선언
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_system_message();
void display_object_info();
void display_commands();

// display 함수: 게임의 모든 화면 요소를 출력
void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor)
{
    display_resource(resource);   // 자원 상태 표시
    display_map(map);             // 맵 표시
    display_cursor(cursor);       // 커서 표시
    display_system_message();     // 시스템 메시지 표시
    display_object_info();        // 오브젝트 정보 표시
    display_commands();           // 명령어 표시
}

// 자원 상태를 표시하는 함수
void display_resource(RESOURCE resource) {
    set_color(COLOR_RESOURCE);
    gotoxy(resource_pos);
    printf("spice = %d/%d, population = %d/%d\n",
        resource.spice, resource.spice_max,
        resource.population, resource.population_max
    );
}

// draw_map()의 보조 함수: 맵의 레이어를 하나의 맵으로 결합하여 출력 준비
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

// 맵을 화면에 출력하는 함수
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    project(map, backbuf);  // 백 버퍼에 맵 정보를 결합

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (frontbuf[i][j] != backbuf[i][j]) {  // 변경된 위치만 업데이트
                POSITION pos = { i, j };
                set_color(frontbuf[i][j]);
                printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
            }
            frontbuf[i][j] = backbuf[i][j];  // 프론트 버퍼를 백 버퍼로 업데이트
        }
    }

    // 좌하단에 플레이어 본부
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

    // 우상단에 AI 본부
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

    // 샌드웜 배치
    set_color_map(COLOR_DEFAULT, COLOR_WORM);
    printc_map((POSITION) { 3, 7 }, 'W', COLOR_DEFAULT, COLOR_WORM);
    printc_map((POSITION) { 13, MAP_WIDTH - 13 }, 'W', COLOR_DEFAULT, COLOR_WORM);

    // 바위 배치
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

// 커서를 맵에 표시하는 함수
void display_cursor(CURSOR cursor) {
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;

    char ch = frontbuf[prev.row][prev.column];
    printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

    ch = frontbuf[curr.row][curr.column];
    printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

// 시스템 메시지 출력 함수
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

// 상태창 출력 함수
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

// 명령창 출력 함수
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