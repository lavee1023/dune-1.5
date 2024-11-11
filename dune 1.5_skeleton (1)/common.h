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
#define MAX_UNITS 7 


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
    k_quit, k_spacebar, k_esc,
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

//* ================= 건물 및 유닛 구조체 정의 =================== */

// 건물의 정보를 담는 구조체 (일반 건물 및 특수 건물 통합)
typedef struct {
    char name[20];       // 이름
    int build_cost;      // 건설 비용  (없음 또는 무한대는 -1로 표시)
    int durability;      // 내구도  (없음 또는 무한대는 -1로 표시)
    char command;        // 명령어 단축키
    POSITION pos;        // 위치
    char repr;           // 맵에 표시되는 문자
    int background_color;
    char player;            // 소속
} BUILDING;

// 유닛의 정보를 담는 구조체
typedef struct {
    char name[20];       // 이름
    int production_cost; // 생산 비용  (없음 또는 무한대는 -1로 표시)
    int population_cost; // 인구 수  (없음 또는 무한대는 -1로 표시)
    int next_move_time;     // 이동 주기
    int attack_power;    // 공격력  (없음 또는 무한대는 -1로 표시)
    int attack_period;   // 공격 주기
    int health;          // 체력  (없음 또는 무한대는 -1로 표시)
    int vision;          // 시야  (없음 또는 무한대는 -1로 표시)
    char commands[2];    // 명령어 단축키
    POSITION pos;         // 현재 위치
    POSITION dest;        // 목적지 (목표 위치)
    int speed;   // 다음 이동 시간
    char repr;            // 유닛을 맵에 표시하는 문자
    int background_color;
    char player;             // 소속
    bool has_contacted_spice;  // 스파이스와 접촉했는지 여부
} UNIT;

typedef struct {
    int quantity;  // 매장량, 9에서 1까지 감소
    POSITION pos;  // 스파이스 위치
} SPICE;

#endif