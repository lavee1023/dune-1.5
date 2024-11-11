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
    k_quit, k_spacebar, k_esc,
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

//* ================= �ǹ� �� ���� ����ü ���� =================== */

// �ǹ��� ������ ��� ����ü (�Ϲ� �ǹ� �� Ư�� �ǹ� ����)
typedef struct {
    char name[20];       // �̸�
    int build_cost;      // �Ǽ� ���  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    int durability;      // ������  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    char command;        // ��ɾ� ����Ű
    POSITION pos;        // ��ġ
    char repr;           // �ʿ� ǥ�õǴ� ����
    int background_color;
    char player;            // �Ҽ�
} BUILDING;

// ������ ������ ��� ����ü
typedef struct {
    char name[20];       // �̸�
    int production_cost; // ���� ���  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    int population_cost; // �α� ��  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    int next_move_time;     // �̵� �ֱ�
    int attack_power;    // ���ݷ�  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    int attack_period;   // ���� �ֱ�
    int health;          // ü��  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    int vision;          // �þ�  (���� �Ǵ� ���Ѵ�� -1�� ǥ��)
    char commands[2];    // ��ɾ� ����Ű
    POSITION pos;         // ���� ��ġ
    POSITION dest;        // ������ (��ǥ ��ġ)
    int speed;   // ���� �̵� �ð�
    char repr;            // ������ �ʿ� ǥ���ϴ� ����
    int background_color;
    char player;             // �Ҽ�
    bool has_contacted_spice;  // �����̽��� �����ߴ��� ����
} UNIT;

typedef struct {
    int quantity;  // ���差, 9���� 1���� ����
    POSITION pos;  // �����̽� ��ġ
} SPICE;

#endif