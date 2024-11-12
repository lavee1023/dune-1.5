/*
* display.h:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
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
#define COLOR_EAGLE 7
#define COLOR_SANDSTORM 6

// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor,
    bool show_1
);

void display_object_info(bool show_info, CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_commands(CURSOR cursor, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool show_1);
void set_background_color(int color, unsigned short back);

#endif