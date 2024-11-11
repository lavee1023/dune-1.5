/*
* display.h:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// 표시할 색상 정의. 대충 맞춰 뒀는데, 취향껏 추가하거나 변경하기
#define COLOR_DEFAULT         15
#define COLOR_CURSOR          7
#define COLOR_RESOURCE        112
#define COLOR_SYSTEM_MESSAGE  15
#define COLOR_OBJECT_INFO     15
#define COLOR_COMMAND         15
#define COLOR_BLUE 1
#define COLOR_RED 4
#define COLOR_WORM 6
#define COLOR_PLATE 112
#define COLOR_ROCK 8
#define COLOR_DEFAULT 15
#define COLOR_SPICE 12

// 앞으로 화면에 표시할 내용들 여기에 추가하기
void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor,
    bool show_1
);

#endif