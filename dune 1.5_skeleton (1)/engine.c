#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"
#include "engine.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
POSITION sample_obj_next_position(UNIT* obj);
void set_building_position(BUILDING* building, int row, int column, int size);
void set_unit_position(UNIT* unit, int row, int column);
void set_color_map(int color, unsigned short back);
void game_tick(void);
void create_spice(SPICE* spice, int row, int column);
int decrease_spice_if_contacted(SPICE* spice, UNIT* unit);
POSITION closest_target(POSITION current, char target_repr);

/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

/* ================= 아트레이디스(플레이어) 건물 및 유닛 목록 =================== */

// 아트레이디스의 건물
BUILDING atreides_Barracks = { "Barracks", 4, 20, 'S', {-1, -1}, 'B', COLOR_BLUE, 'A' };  // Barracks
BUILDING atreides_Shelter = { "Shelter", 5, 30, 'F', {-1, -1}, 'S', COLOR_BLUE, 'A' };  // Shelter
BUILDING atreides_Base = { "Base", -1, 50, 'H', {-1, -1}, 'b', COLOR_BLUE, 'A' };  // Base
BUILDING atreides_Plant = { "Plant", 1, -1, '\0', {-1, -1}, 'P', COLOR_BLUE, 'A' };  // Plant
BUILDING atreides_Dormitory = { "Dormitory", 2, 10, '\0', {-1, -1}, 'D', COLOR_BLUE, 'A' };  // Dormitory
BUILDING atreides_Garage = { "Garage", 4, 10, '\0',{-1, -1}, 'G', COLOR_BLUE, 'A' };  // Garage

// 아트레이디스의 유닛
UNIT atreides_Fremen = { "Fremen", 5, 2, 400, 15, 200, 25, 8, {'M', 'P'}, {-1, -1}, {-1, -1}, 400, 'F', COLOR_BLUE, 'A', false};  // Fremen
UNIT atreides_Soldier = { "Soldier", 1, 1, 1000, 5, 800, 15, 1, {'M', 'P'}, {-1, -1}, {-1, -1}, 1000, 'S', COLOR_BLUE, 'A', false};  // Soldier
UNIT atreides_Harvester = { "Harvester", 5, 5, 2000, 0, 0, 70, 0, {'H', 'M'}, {MAP_HEIGHT - 4, 1}, {MAP_HEIGHT - 2, MAP_WIDTH - 2}, 2000, 'H', COLOR_BLUE, 'A', false};

//#define NUM_ATREIDES_BUILDINGS 6
//#define NUM_ATREIDES_UNITS 3

/* ================= 하코넨(AI) 건물 및 유닛 목록 =================== */

// 하코넨의 건물
BUILDING harkonnen_Arena = { "Arena", 3, 15, 'F', {-1, -1}, 'a', COLOR_RED, 'H' };  // Arena
BUILDING harkonnen_Factory = { "Factory", 5, 30, 'T', {-1, -1}, 'f', COLOR_RED, 'H' };  // Factory
BUILDING harkonnen_Base = { "Base", -1, 50, 'H', {-1, -1}, 'v', COLOR_RED, 'H' };  // Base
BUILDING harkonnen_Plant = { "Plant", 1, -1, '\0', {-1, -1}, 'p', COLOR_RED, 'H' };  // Plant
BUILDING harkonnen_Dormitory = { "Dormitory", 2, 10, '\0', {-1, -1}, 'd', COLOR_RED, 'H' };  // Dormitory
BUILDING harkonnen_Garage = { "Garage", 4, 10, '\0', {-1, -1}, 'g', COLOR_RED, 'H' };  // Garage

// 하코넨의 유닛
UNIT harkonnen_Fighter = { "Fighter", 1, 1, 1200, 6, 600, 10, 1, {'M', 'P'}, {-1, -1}, {-1, -1}, 1200, 'f', COLOR_RED, 'H', false};  // Fighter
UNIT harkonnen_HeavyTank = { "Heavy Tank", 12, 5, 3000, 40, 4000, 60, 4, {'M', 'P'}, {-1, -1}, {-1, -1}, 3000, 't', COLOR_RED, 'H', false};  // Heavy Tank
UNIT harkonnen_Harvester = { "Harvester", 5, 5, 2000, 0, 0, 70, 0, {'H', 'M'}, {3, MAP_WIDTH - 2}, {MAP_HEIGHT - 2, MAP_WIDTH - 2}, 2000, 'h', COLOR_RED, 'H', false};

//#define NUM_HARKONNEN_BUILDINGS 6  
//#define NUM_HARKONNEN_UNITS 3  

/* ================= 중립 건물 및 유닛 목록 =================== */

// 중립 건물
BUILDING Rock = { "Rock", -1, -1, -1, {-1, -1}, 'R', COLOR_ROCK, 'R' };
BUILDING Rock_1 = { "Rock", -1, -1, -1, {-1, -1}, 'R', COLOR_ROCK, 'R' };

// 중립 유닛
UNIT Sandworm_1 = { "Sandworm", -1, -1, 2500, -1, 10000, -1, -1, {'\0', '\0'}, {-1, -1}, {-1, -1}, 2500, 'W', COLOR_WORM, 'W' };  // Sandworm
UNIT Sandworm_2 = { "Sandworm", -1, -1, 2500, -1, 10000, -1, -1, {'\0', '\0'}, {-1, -1}, {-1, -1}, 2500, 'W', COLOR_WORM, 'W' };  // Sandworm

//#define NUM_NEUTRAL_BUILDINGS 2
//#define NUM_NEUTRAL_UNITS 2

/* ================= 스파이스 =================== */

SPICE spice = { 9, {-1, -1}};

#define MAX_SPICES 10  // 스파이스 최대 개수
SPICE spices[MAX_SPICES];

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));  // 난수 초기화

	init();  // 초기화
	intro(); // 인트로 화면 표시
	display(resource, map, cursor, false);  // 초기 화면 표시 (show_1은 false)

	bool show_1 = false;  // 스페이스바가 눌렸는지 여부를 저장하는 변수

	while (1) {
		KEY key = get_key();  // 키 입력 받기

		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			switch (key) {
			case k_quit:
				outro();
				return 0;
			case k_spacebar:
				show_1 = true;
				break;
			case k_esc:  // ESC 키가 눌리면 명령창과 상태창을 초기화
				show_1 = false;
				show_1 = false;
				display_object_info(show_1, cursor, map);  // 상태창 비우기
				display_commands(cursor, map, show_1);  // 명령창 비우기
				break;
			case k_none:
			case k_undef:
			default:
				break;
			}
		}


		game_tick();

		display(resource, map, cursor, show_1);

		Sleep(TICK);
		sys_clock += 10;
	}

	return 0;  // 정상 종료
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// 유닛 초기 위치 설정
	set_unit_position(&atreides_Harvester, MAP_HEIGHT - 4, 1);
	set_unit_position(&harkonnen_Harvester, 3, MAP_WIDTH - 2);
	set_unit_position(&Sandworm_1, 3, 7);
	set_unit_position(&Sandworm_2, 13, MAP_WIDTH - 13);

	// 2x2 크기로 건물 배치
	set_building_position(&atreides_Base, MAP_HEIGHT - 3, 1, 2);
	set_building_position(&atreides_Plant, MAP_HEIGHT - 3, 3, 2);

	set_building_position(&harkonnen_Base, 1, MAP_WIDTH - 3, 2);
	set_building_position(&harkonnen_Plant, 1, MAP_WIDTH - 5, 2);

	// Rock 출력
	set_building_position(&Rock, 5, 21, 2);
	set_building_position(&Rock, 13, MAP_WIDTH - 21, 2);
	set_building_position(&Rock_1, 14, 8, 1);
	set_building_position(&Rock_1, 5, MAP_WIDTH - 8, 1);
	set_building_position(&Rock_1, 15, MAP_WIDTH - 5, 1);
	
	create_spice(&spices[0], MAP_HEIGHT - 6, 1);
	create_spice(&spices[1], 5, MAP_WIDTH - 2);
}

// 커서 이동 함수
void cursor_move(DIRECTION dir) {
	static DIRECTION last_dir = 0;    // 마지막 입력된 방향
	static clock_t last_time = 0;     // 마지막 입력 시간
	clock_t current_time = clock();   // 현재 시간

	POSITION curr = cursor.current;

	if (dir == last_dir && (double)(current_time - last_time) / CLOCKS_PER_SEC < 0.2) {
		POSITION new_pos = pmove(curr, dir);
		new_pos = pmove(new_pos, dir);
		new_pos = pmove(new_pos, dir);

		if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
			cursor.previous = cursor.current;
			cursor.current = new_pos;
		}
	}
	else {
		// 한 번만 입력된 경우 1칸 이동
		POSITION new_pos = pmove(curr, dir);

		if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
			cursor.previous = cursor.current;
			cursor.current = new_pos;
		}
	}

	last_dir = dir;
	last_time = current_time;
}

/* ================= sample object movement =================== */
void set_building_position(BUILDING* building, int row, int column, int size) {
	if (row >= 1 && row + size - 1 < MAP_HEIGHT - 1 && column >= 1 && column + size - 1 < MAP_WIDTH - 1) {
		building->pos.row = row;
		building->pos.column = column;

		// 크기가 2x2일 경우
		if (size == 2) {
			map[0][row][column] = building->repr;        // 좌측 상단
			map[0][row][column + 1] = building->repr;    // 우측 상단
			map[0][row + 1][column] = building->repr;    // 좌측 하단
			map[0][row + 1][column + 1] = building->repr; // 우측 하단

			// 건물 색상 반영
			set_color_map(building->background_color, 0);  // 건물의 배경색을 설정
		}
		// 크기가 1x1일 경우
		else if (size == 1) {
			map[0][row][column] = building->repr;
			// 색상값 설정
			set_color_map(building->background_color, 0); // 건물의 배경색을 설정
		}
	}
}


void set_unit_position(UNIT* unit, int row, int column) {
	if (row >= 1 && row < MAP_HEIGHT - 1 && column >= 1 && column < MAP_WIDTH - 1) {

		unit->pos.row = row;
		unit->pos.column = column;

		map[1][row][column] = unit->repr;

		set_color_map(unit->background_color, 0);
	}
}

// 스파이스를 생성하는 함수
void create_spice(SPICE* spice, int row, int column) {
	if (row >= 1 && row < MAP_HEIGHT - 1 && column >= 1 && column < MAP_WIDTH - 1) {
		spice->pos.row = row;
		spice->pos.column = column;
		spice->quantity = 9;  // 초기 매장량은 9

		map[0][row][column] = '0' + spice->quantity;

		set_color_map(COLOR_SPICE, 0);
	}
}

POSITION sample_obj_next_position(UNIT* obj) {   
	POSITION diff = psub(obj->dest, obj->pos);
	DIRECTION dir;

	if (diff.row == 0 && diff.column == 0) {
		if (obj->dest.row == 1 && obj->dest.column == 1) {
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj->dest = new_dest;
		}
		else {
			POSITION new_dest = { 1, 1 };
			obj->dest = new_dest;
		}
		return obj->pos;
	}

	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj->pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj->pos;  // 제자리
	}
}

// 거리 계산 함수
int manhattan_distance(POSITION pos1, POSITION pos2) {
	return abs(pos1.row - pos2.row) + abs(pos1.column - pos2.column);
}

// 스파이스매장량을 감소시키는 함수
int decrease_spice_if_contacted(SPICE* spice, UNIT* unit) {
	if (unit->pos.row == spice->pos.row && unit->pos.column == spice->pos.column) {
		if (spice->quantity > 0) {
			spice->quantity--;  // 스파이스 양 감소

			map[0][spice->pos.row][spice->pos.column] = '0' + spice->quantity;

			return 1;
		}
	}
	return 0;
}

POSITION closest_target(POSITION current, char target_repr) {
	int min_distance = MAP_HEIGHT + MAP_WIDTH;
	POSITION closest = { -1, -1 };

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[0][i][j] == target_repr) {
				int distance = manhattan_distance(current, (POSITION) { i, j });
				if (distance < min_distance) {
					min_distance = distance;
					closest = (POSITION){ i, j };
				}
			}
		}
	}
	return closest;
}

// 스파이스 방향으로 이동하는 함수
void move_towards(POSITION* current, POSITION target) {
	if (current->row < target.row) {
		current->row += 1;
	}
	else if (current->row > target.row) {
		current->row -= 1;
	}

	if (current->column < target.column) {
		current->column += 1;
	}
	else if (current->column > target.column) {
		current->column -= 1;
	}
}

void unit_move(UNIT* obj) {
	if (sys_clock <= obj->next_move_time) {
		return;
	}

	map[1][obj->pos.row][obj->pos.column] = -1;

	static int H_moving_to_spice = 1;
	static int h_moving_to_spice = 1;

	if (obj->repr == 'H') {
		if (H_moving_to_spice) {
			// H가 스파이스로 이동 중인 경우
			int min_distance = MAP_HEIGHT + MAP_WIDTH;
			SPICE* closest_spice = NULL;

			// 가장 가까운 스파이스 찾기
			for (int i = 0; i < MAX_SPICES; i++) {
				if (spices[i].quantity > 0) {
					int distance = manhattan_distance(obj->pos, spices[i].pos);
					if (distance < min_distance) {
						min_distance = distance;
						closest_spice = &spices[i];
					}
				}
			}

			if (closest_spice != NULL) {
				move_towards(&obj->pos, closest_spice->pos);

				// 스파이스 매장량 감소
				if (decrease_spice_if_contacted(closest_spice, obj)) {
					H_moving_to_spice = 0;  // 스파이스 감소 후 b 위치로 전환
				}
			}
		}
		else {
			// H가 b로 이동 중인 경우
			POSITION closest_b = closest_target(obj->pos, 'b');
			if (closest_b.row != -1 && closest_b.column != -1) {
				move_towards(&obj->pos, closest_b);

				// b 위치에 도달하면 다시 스파이스로 전환
				if (obj->pos.row == closest_b.row && obj->pos.column == closest_b.column) {
					H_moving_to_spice = 1;
				}
			}
		}
	}
	else if (obj->repr == 'h') {
		if (h_moving_to_spice) {
			// h가 스파이스로 이동 중인 경우
			int min_distance = MAP_HEIGHT + MAP_WIDTH;
			SPICE* closest_spice = NULL;

			// 가장 가까운 스파이스 찾기
			for (int i = 0; i < MAX_SPICES; i++) {
				if (spices[i].quantity > 0) {
					int distance = manhattan_distance(obj->pos, spices[i].pos);
					if (distance < min_distance) {
						min_distance = distance;
						closest_spice = &spices[i];
					}
				}
			}

			if (closest_spice != NULL) {
				move_towards(&obj->pos, closest_spice->pos);

				// 스파이스 매장량 감소
				if (obj->pos.row == closest_spice->pos.row && obj->pos.column == closest_spice->pos.column) {
					decrease_spice_if_contacted(closest_spice, obj);
					h_moving_to_spice = 0;
				}
			}
		}
		else {
			// h가 v로 이동 중인 경우
			POSITION closest_v = closest_target(obj->pos, 'v');
			if (closest_v.row != -1 && closest_v.column != -1) {
				move_towards(&obj->pos, closest_v);

				// v 위치에 도달하면 다시 스파이스로 전환
				if (obj->pos.row == closest_v.row && obj->pos.column == closest_v.column) {
					h_moving_to_spice = 1;
				}
			}
		}
	}


	// 새로운 위치가 유효한지 확인
	if (obj->pos.row >= 1 && obj->pos.row < MAP_HEIGHT - 1 && obj->pos.column >= 1 && obj->pos.column < MAP_WIDTH - 1) {
		map[1][obj->pos.row][obj->pos.column] = obj->repr;  // 새로운 위치에 유닛 배치
	}

	// 이동 주기 업데이트
	obj->next_move_time = sys_clock + obj->speed;  // 현재 시각 + 속도로 다음 이동 시간 설정
}

void game_tick(void) {
	// 아트레이디스 유닛들 이동
	unit_move(&atreides_Fremen);
	unit_move(&atreides_Soldier);
	unit_move(&atreides_Harvester);

	// 하코넨 유닛들 이동
	unit_move(&harkonnen_Fighter);
	unit_move(&harkonnen_HeavyTank);
	unit_move(&harkonnen_Harvester);

	// 중립 유닛들 이동
	unit_move(&Sandworm_1);
	unit_move(&Sandworm_2);
}
