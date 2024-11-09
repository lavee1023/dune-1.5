#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>

/* ================= system parameters =================== */
#define TICK 10		// time unit(ms)

#define N_LAYER 2
#define MAP_WIDTH	60
#define MAP_HEIGHT	18


/* ================= ��ġ�� ���� =================== */
// �ʿ��� ��ġ�� ��Ÿ���� ����ü
typedef struct {
	int row, column;
} POSITION;

// Ŀ�� ��ġ
typedef struct {
	POSITION previous;  // ���� ��ġ
	POSITION current;   // ���� ��ġ
} CURSOR;

// �Է� ������ Ű ����.
// �������� enum�� �����ߴµ�, ũ�� ����� ������ ���� �˻�
typedef enum {
	// k_none: �Էµ� Ű�� ����. d_stay(�� �����̴� ���)�� ����
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_undef, // ���ǵ��� ���� Ű �Է�	
} KEY;


// DIRECTION�� KEY�� �κ�����������, �ǹ̸� ��Ȯ�ϰ� �ϱ� ���ؼ� �ٸ� Ÿ������ ����
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;


/* ================= ��ġ�� ����(2) =================== */
// ���Ǽ��� ���� �Լ���. KEY, POSITION, DIRECTION ����ü���� ���������� ��ȯ

// ���Ǽ� �Լ�
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.row + p2.row, p1.column + p2.column };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

// ����Ű���� Ȯ���ϴ� �Լ�
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// ȭ��ǥ 'Ű'(KEY)�� '����'(DIRECTION)���� ��ȯ. ���� ���� �Ȱ����� Ÿ�Ը� �ٲ��ָ� ��
#define ktod(k)		(DIRECTION)(k)

// DIRECTION�� POSITION ���ͷ� ��ȯ�ϴ� �Լ�
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p�� d �������� �̵���Ų POSITION
#define pmove(p, d)		(padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct {
	int spice;		// ���� ������ �����̽�
	int spice_max;  // �����̽� �ִ� ���差
	int population; // ���� �α� ��
	int population_max;  // ���� ������ �α� ��
} RESOURCE;


// �밭 ����� ����. ��� �߰��ϸ鼭 ���� ������ ��
typedef struct {
	POSITION pos;		// ���� ��ġ(position)
	POSITION dest;		// ������(destination)
	char repr;			// ȭ�鿡 ǥ���� ����(representation)
	int move_period;
	int next_move_time;
	int speed;
} OBJECT_SAMPLE;

//* ================= �ǹ� �� ���� ����ü ���� =================== */

// �ǹ��� ������ ��� ����ü (�Ϲ� �ǹ� �� Ư�� �ǹ� ����)
typedef struct {
    char name[20];       // �̸�
    int build_cost;      // �Ǽ� ���  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    int durability;      // ������  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    char command;        // ��ɾ� ����Ű
} BUILDING;

// ������ ������ ��� ����ü
typedef struct {
    char name[20];       // �̸�
    int production_cost; // ���� ���  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    int population_cost; // �α� ��  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    int move_period;     // �̵� �ֱ�
    int attack_power;    // ���ݷ�  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    int attack_period;   // ���� �ֱ�
    int health;          // ü��  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    int vision;          // �þ�  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    char commands[2];    // ��ɾ� ����Ű
} UNIT;

/* ================= ���� �ǹ� �� ���� ��� =================== */

static const BUILDING common_buildings[] = {
    {"Base", -1, 50, 'H'},  // Base
    {"Plant", 1, -1, 'P'},  // Plant
    { "Dormitory", 2, 10, '\0'},  // Dormitory
    { "Garage", 4, 10, '\0'}  // Garage
};

static const UNIT common_units[] = {
    {"Harvester", 5, 5, 2000, 0, 0, 70, 0, {'H', 'M'}}
};

#define NUM_COMMON_BUILDINGS (sizeof(common_buildings) / sizeof(common_buildings[0]))
#define NUM_COMMON_UNITS (sizeof(common_units) / sizeof(common_units[0]))

/* ================= ��Ʈ���̵�(�÷��̾�) �ǹ� �� ���� ��� =================== */

static const BUILDING atreides_buildings[] = {
    {"Barracks", 4, 20, 'S'},  // Barracks
    {"Shelter", 5, 30, 'F'}  // Shelter
};

static const UNIT atreides_units[] = {
    {"Fremen", 5, 2, 400, 15, 200, 25, 8, {'M', 'P'}},  // Fremen
    {"Soldier", 1, 1, 1000, 5, 800, 15, 1, {'M', 'P'}}  // Soldier
};

#define NUM_ATREIDES_BUILDINGS (sizeof(atreides_buildings) / sizeof(atreides_buildings[0]))
#define NUM_ATREIDES_UNITS (sizeof(atreides_units) / sizeof(atreides_units[0]))

/* ================= ���ڳ�(AI) �ǹ� �� ���� ��� =================== */

static const BUILDING harkonnen_buildings[] = {
    {"Arena", 3, 15, 'F'},  // Arena
    {"Factory", 5, 30, 'T'}  // Factory
};

static const UNIT harkonnen_units[] = {
    {"Figther", 1, 1, 1200, 6, 600, 10, 1, {'M', 'P'}},  // Figther
    {"Heavy Tank", 12, 5, 3000, 40, 4000, 60, 4, {'M', 'P'}}  // Heavy Tank
};

#define NUM_HARKONNEN_BUILDINGS (sizeof(harkonnen_buildings) / sizeof(harkonnen_buildings[0]))
#define NUM_HARKONNEN_UNITS (sizeof(harkonnen_units) / sizeof(harkonnen_units[0]))

/* ================= �߸� ���� ��� =================== */

static const UNIT neutral_units[] = {
    {"Sandworm", -1, -1, 2500, -1, 10000, -1, -1, {'\0', '\0'}}  // ��ɾ� ����
};

#define NUM_NEUTRAL_UNITS (sizeof(neutral_units) / sizeof(neutral_units[0]))

#endif