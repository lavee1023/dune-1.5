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


/* ================= 위치와 방향 =================== */
// 맵에서 위치를 나타내는 구조체
typedef struct {
	int row, column;
} POSITION;

// 커서 위치
typedef struct {
	POSITION previous;  // 직전 위치
	POSITION current;   // 현재 위치
} CURSOR;

// 입력 가능한 키 종류.
// 수업에서 enum은 생략했는데, 크게 어렵지 않으니 예제 검색
typedef enum {
	// k_none: 입력된 키가 없음. d_stay(안 움직이는 경우)에 대응
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_undef, // 정의되지 않은 키 입력	
} KEY;


// DIRECTION은 KEY의 부분집합이지만, 의미를 명확하게 하기 위해서 다른 타입으로 정의
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;


/* ================= 위치와 방향(2) =================== */
// 편의성을 위한 함수들. KEY, POSITION, DIRECTION 구조체들을 유기적으로 변환

// 편의성 함수
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.row + p2.row, p1.column + p2.column };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

// 방향키인지 확인하는 함수
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// 화살표 '키'(KEY)를 '방향'(DIRECTION)으로 변환. 정수 값은 똑같으니 타입만 바꿔주면 됨
#define ktod(k)		(DIRECTION)(k)

// DIRECTION을 POSITION 벡터로 변환하는 함수
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p를 d 방향으로 이동시킨 POSITION
#define pmove(p, d)		(padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct {
	int spice;		// 현재 보유한 스파이스
	int spice_max;  // 스파이스 최대 저장량
	int population; // 현재 인구 수
	int population_max;  // 수용 가능한 인구 수
} RESOURCE;


// 대강 만들어 봤음. 기능 추가하면서 각자 수정할 것
typedef struct {
	POSITION pos;		// 현재 위치(position)
	POSITION dest;		// 목적지(destination)
	char repr;			// 화면에 표시할 문자(representation)
	int move_period;
	int next_move_time;
	int speed;
} OBJECT_SAMPLE;

//* ================= 건물 및 유닛 구조체 정의 =================== */

// 건물의 정보를 담는 구조체 (일반 건물 및 특수 건물 통합)
typedef struct {
    char name[20];       // 이름
    int build_cost;      // 건설 비용  (없음 또는 무한대는 -1로 표시)
    int durability;      // 내구도  (없음 또는 무한대는 -1로 표시)
    char command;        // 명령어 단축키
} BUILDING;

// 유닛의 정보를 담는 구조체
typedef struct {
    char name[20];       // 이름
    int production_cost; // 생산 비용  (없음 또는 무한대는 -1로 표시)
    int population_cost; // 인구 수  (없음 또는 무한대는 -1로 표시)
    int move_period;     // 이동 주기
    int attack_power;    // 공격력  (없음 또는 무한대는 -1로 표시)
    int attack_period;   // 공격 주기
    int health;          // 체력  (없음 또는 무한대는 -1로 표시)
    int vision;          // 시야  (없음 또는 무한대는 -1로 표시)
    char commands[2];    // 명령어 단축키
} UNIT;

/* ================= 공통 건물 및 유닛 목록 =================== */

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

/* ================= 아트레이디스(플레이어) 건물 및 유닛 목록 =================== */

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

/* ================= 하코넨(AI) 건물 및 유닛 목록 =================== */

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

/* ================= 중립 유닛 목록 =================== */

static const UNIT neutral_units[] = {
    {"Sandworm", -1, -1, 2500, -1, 10000, -1, -1, {'\0', '\0'}}  // 명령어 없음
};

#define NUM_NEUTRAL_UNITS (sizeof(neutral_units) / sizeof(neutral_units[0]))

#endif