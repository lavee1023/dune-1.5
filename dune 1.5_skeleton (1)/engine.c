#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
POSITION sample_obj_next_position(UNIT* obj);
void set_building_position(BUILDING* building, int row, int column, int size);
void set_unit_position(UNIT* unit, int row, int column);
void set_unit_position2(UNIT* unit, int row, int column);
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
	.spice = 5,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

#define SANDWORM_BIRTH_INTERVAL 10040
#define MAX_ATREIDES_UNITS 100
#define MAX_HARKONNEN_UNITS 100

UNIT atreides_units[MAX_ATREIDES_UNITS];
UNIT harkonnen_units[MAX_HARKONNEN_UNITS];

int current_atreides_units = 3;
int current_harkonnen_units = 3;
int current_units[2] = { 3, 3 };

// =================== 아트레이디스(플레이어) 건물 및 유닛 배열 ===================

// 아트레이디스 건물 배열
#define NUM_ATREIDES_BUILDINGS 6
BUILDING atreides_buildings[NUM_ATREIDES_BUILDINGS] = {
	{ "Barracks", 4, 20, 'S', {-1, -1}, 'B', COLOR_BLUE, 'A' },
	{ "Shelter", 5, 30, 'F', {-1, -1}, 'S', COLOR_BLUE, 'A' },
	{ "Base", -1, 50, 'H', {-1, -1}, 'b', COLOR_BLUE, 'A' },
	{ "Plant", 1, -1, '\0', {-1, -1}, 'P', COLOR_BLUE, 'A' },
	{ "Dormitory", 2, 10, '\0', {-1, -1}, 'D', COLOR_BLUE, 'A' },
	{ "Garage", 4, 10, '\0', {-1, -1}, 'G', COLOR_BLUE, 'A' }
};

// 아트레이디스 유닛 배열
#define NUM_ATREIDES_UNITS 3
UNIT atreides_units[MAX_ATREIDES_UNITS] = {
	{ "Fremen", 5, 2, 400, 15, 200, 25, 8, {'M', 'P'}, {-1, -1}, {-1, -1}, 400, 'F', COLOR_BLUE, 'A', false, true },
	{ "Soldier", 1, 1, 1000, 5, 800, 15, 1, {'M', 'P'}, {-1, -1}, {-1, -1}, 1000, 'S', COLOR_BLUE, 'A', false, true },
	{ "Harvester", 5, 5, 2000, 0, 0, 70, 0, {'H', 'M'}, {MAP_HEIGHT - 4, 1}, {MAP_HEIGHT - 2, MAP_WIDTH - 2}, 2000, 'H', COLOR_BLUE, 'A', false, true }
};

// =================== 하코넨(AI) 건물 및 유닛 배열 ===================

// 하코넨 건물 배열
#define NUM_HARKONNEN_BUILDINGS 6
BUILDING harkonnen_buildings[NUM_HARKONNEN_BUILDINGS] = {
	{ "Arena", 3, 15, 'F', {-1, -1}, 'a', COLOR_RED, 'H' },
	{ "Factory", 5, 30, 'T', {-1, -1}, 'f', COLOR_RED, 'H' },
	{ "Base", -1, 50, 'H', {-1, -1}, 'v', COLOR_RED, 'H' },
	{ "Plant", 1, -1, '\0', {-1, -1}, 'p', COLOR_RED, 'H' },
	{ "Dormitory", 2, 10, '\0', {-1, -1}, 'd', COLOR_RED, 'H' },
	{ "Garage", 4, 10, '\0', {-1, -1}, 'g', COLOR_RED, 'H' }
};

// 하코넨 유닛 배열
#define NUM_HARKONNEN_UNITS 3
UNIT harkonnen_units[MAX_HARKONNEN_UNITS] = {
	{ "Fighter", 1, 1, 1200, 6, 600, 10, 1, {'M', 'P'}, {-1, -1}, {-1, -1}, 1200, 'f', COLOR_RED, 'H', false, true },
	{ "Heavy Tank", 12, 5, 3000, 40, 4000, 60, 4, {'M', 'P'}, {-1, -1}, {-1, -1}, 3000, 't', COLOR_RED, 'H', false, true },
	{ "Harvester", 5, 5, 2000, 0, 0, 70, 0, {'H', 'M'}, {3, MAP_WIDTH - 2}, {MAP_HEIGHT - 2, MAP_WIDTH - 2}, 2000, 'h', COLOR_RED, 'H', false, true }
};

// =================== 중립 건물 및 유닛 배열 ===================

// 중립 건물 배열
#define NUM_NEUTRAL_BUILDINGS 2
BUILDING neutral_buildings[NUM_NEUTRAL_BUILDINGS] = {
	{ "Rock", -1, -1, -1, {-1, -1}, 'R', COLOR_ROCK, 'R' },
	{ "Rock", -1, -1, -1, {-1, -1}, 'R', COLOR_ROCK, 'R' }
};

// 중립 유닛 배열 (샌드웜)
#define MAX_SANDWORMS 2
UNIT sandworms[MAX_SANDWORMS] = {
	{ "Sandworm", -1, -1, 2500, -1, 10000, -1, -1, {'\0', '\0'}, {-1, -1}, {-1, -1}, 2500, 'W', COLOR_WORM, 'W', false, true },
	{ "Sandworm", -1, -1, 2500, -1, 10000, -1, -1, {'\0', '\0'}, {-1, -1}, {-1, -1}, 2500, 'W', COLOR_WORM, 'W', false, true }
};

#define MAX_EAGLES 2
UNIT eagle[MAX_EAGLES] = {
	{ "Eagle", -1, -1, 500, -1, 0, -1, -1, {'\0', '\0'}, {-1, -1}, {-1, -1}, 500, 'E', COLOR_EAGLE, 'W', false, true }
};

// =================== 스파이스 배열 ===================
#define MAX_SPICES 10
SPICE spices[MAX_SPICES] = {
	{ 9, {-1, -1} },
	{ 8, {-1, -1} },
	{ 7, {-1, -1} },
	{ 6, {-1, -1} },
	{ 5, {-1, -1} },
	{ 4, {-1, -1} },
	{ 3, {-1, -1} },
	{ 2, {-1, -1} },
	{ 1, {-1, -1} },
	{ 0, {-1, -1} }
};


// =================== 메시지 로그 배열 ===================
MESSAGE_LOG message_log = { .message_count = 0 };

// =================== 스파이스 배열 ===================
#define MAX_SPICES 10
SPICE spices[MAX_SPICES];


/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));  // 난수 초기화

	intro();  // 인트로 화면 먼저 표시
	init();   // 초기화

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
				display_object_info(show_1, cursor, map);  // 상태창 비우기
				display_commands(cursor, map, show_1);     // 명령창 비우기
				break;
			case k_none:
			case k_undef:
			default:
				break;
			}
		}

		game_tick();  // 게임 상태 업데이트
		display(resource, map, cursor, show_1);  // 업데이트된 상태로 화면 다시 표시

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
	// 맵 경계 초기화
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

	// 상위 레이어 초기화
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// 유닛 초기 위치 설정
	set_unit_position(&atreides_units[2], MAP_HEIGHT - 4, 1);  // atreides_Harvester
	set_unit_position(&harkonnen_units[2], 3, MAP_WIDTH - 2);  // harkonnen_Harvester
	set_unit_position(&sandworms[0], 5, 20);                   // 첫 번째 샌드웜
	set_unit_position(&sandworms[1], 13, MAP_WIDTH - 13);      // 두 번째 샌드웜
	set_unit_position(&harkonnen_units[0], 5, 24);             // harkonnen_Fighter
	set_unit_position(&harkonnen_units[0], 10, 24);             // harkonnen_Fighter
	set_unit_position2(&eagle[0], 10, 10);

	// 건물 및 스파이스 초기 위치 설정
	set_building_position(&atreides_buildings[2], MAP_HEIGHT - 3, 1, 2);  // atreides_Base
	set_building_position(&atreides_buildings[3], MAP_HEIGHT - 3, 3, 2);  // atreides_Plant
	set_building_position(&harkonnen_buildings[2], 1, MAP_WIDTH - 3, 2);  // harkonnen_Base
	set_building_position(&harkonnen_buildings[3], 1, MAP_WIDTH - 5, 2);  // harkonnen_Plant

	// Rock (중립 건물) 및 스파이스 위치 설정
	set_building_position(&neutral_buildings[0], 5, 21, 2);
	set_building_position(&neutral_buildings[0], 13, MAP_WIDTH - 21, 2);
	set_building_position(&neutral_buildings[1], 14, 8, 1);
	set_building_position(&neutral_buildings[1], 5, MAP_WIDTH - 8, 1);
	set_building_position(&neutral_buildings[1], 15, MAP_WIDTH - 5, 1);

	// 스파이스 생성
	create_spice(&spices[0], MAP_HEIGHT - 6, 1);
	create_spice(&spices[1], 5, MAP_WIDTH - 2);

	// 초기 화면 표시
	display(resource, map, cursor, false);
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

// ================= sample object movement (revised) ===================

void set_building_position(BUILDING* building, int row, int column, int size) {
	// 건물이 맵 범위 내에 위치할 수 있는지 확인
	if (row >= 1 && row + size - 1 < MAP_HEIGHT - 1 && column >= 1 && column + size - 1 < MAP_WIDTH - 1) {
		building->pos.row = row;
		building->pos.column = column;

		// 건물 크기가 2x2일 경우
		if (size == 2) {
			map[0][row][column] = building->repr;            // 좌측 상단
			map[0][row][column + 1] = building->repr;        // 우측 상단
			map[0][row + 1][column] = building->repr;        // 좌측 하단
			map[0][row + 1][column + 1] = building->repr;    // 우측 하단

			// 건물 색상 설정
			set_color_map(building->background_color, 0);
		}
		// 건물 크기가 1x1일 경우
		else if (size == 1) {
			map[0][row][column] = building->repr;
			// 색상 설정
			set_color_map(building->background_color, 0);
		}
	}
}

void set_unit_position(UNIT* unit, int row, int column) {
	// 유닛이 맵 범위 내에 위치할 수 있는지 확인
	if (row >= 1 && row < MAP_HEIGHT - 1 && column >= 1 && column < MAP_WIDTH - 1) {
		unit->pos.row = row;
		unit->pos.column = column;
		unit->active = true;  // 유닛 활성화

		map[1][row][column] = unit->repr;

		// 유닛의 배경색 설정
		set_color_map(unit->background_color, 1);
	}
}

void set_unit_position2(UNIT* unit, int row, int column) {
	// 유닛이 맵 범위 내에 위치할 수 있는지 확인
	if (row >= 1 && row < MAP_HEIGHT - 1 && column >= 1 && column < MAP_WIDTH - 1) {
		unit->pos.row = row;
		unit->pos.column = column;
		unit->active = true;  // 유닛 활성화

		map[1][row][column] = unit->repr;

		// 유닛의 배경색 설정
		set_color_map(unit->background_color, 1);
	}
}

// 스파이스를 생성하는 함수
void create_spice(SPICE* spice, int row, int column) {
	if (row >= 1 && row < MAP_HEIGHT - 1 && column >= 1 && column < MAP_WIDTH - 1) {
		if (map[0][row][column] != 'R' && map[0][row][column] != 'S') {
			spice->pos.row = row;
			spice->pos.column = column;
			spice->quantity = rand() % 9 + 1;

			map[0][row][column] = '0' + spice->quantity;

			set_color_map(COLOR_SPICE, 0);
		}
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

// 스파이스 매장량을 감소시키는 함수
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

void move_towards_sandworm(POSITION* current, POSITION target) {
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

int is_tile_blocked(POSITION current, int row_step, int col_step) {
	// 이동하려는 칸을 계산
	POSITION next_pos = { current.row + row_step, current.column + col_step };

	// 바위가 있는지 확인
	if (map[0][next_pos.row][next_pos.column] == 'R') {
		return 1; // 바위가 있으면 이동할 수 없음
	}
	return 0; // 바위가 없으면 이동 가능
}

void sandworm_move(UNIT* sandworm) {
	// 현재 시간이 샌드웜의 이동 시간을 초과했을 때만 이동
	if (sys_clock <= sandworm->next_move_time) {
		return;  // 아직 이동할 시간이 아님
	}

	// 샌드웜의 현재 위치를 맵에서 비움
	map[1][sandworm->pos.row][sandworm->pos.column] = -1;

	POSITION closest_target_pos = { -1, -1 };
	int min_distance = MAP_HEIGHT + MAP_WIDTH;
	UNIT* target_unit = NULL;

	// 가장 가까운 유닛을 찾음
	UNIT* all_units[] = { atreides_units, harkonnen_units };
	for (int team = 0; team < 2; team++) {
		for (int i = 0; i < current_units[team]; i++) {
			UNIT* unit = &all_units[team][i];
			if (unit->active && unit->repr != 'W') {  // 샌드웜이 아닌 유닛만 추적
				int distance = manhattan_distance(sandworm->pos, unit->pos);
				if (distance < min_distance) {
					min_distance = distance;
					closest_target_pos = unit->pos;
					target_unit = unit;
				}
			}
		}
	}

	// 가장 가까운 유닛을 향해 이동
	if (closest_target_pos.row != -1) {
		int row_step = (closest_target_pos.row > sandworm->pos.row) ? 1 : (closest_target_pos.row < sandworm->pos.row) ? -1 : 0;
		int col_step = (closest_target_pos.column > sandworm->pos.column) ? 1 : (closest_target_pos.column < sandworm->pos.column) ? -1 : 0;

		// 이동할 수 있는지 확인하고 이동
		if (!is_tile_blocked(sandworm->pos, row_step, col_step)) {
			sandworm->pos.row += row_step;
			sandworm->pos.column += col_step;
		}
		else {
			// 이동할 수 없으면 다른 방향으로 시도
			int directions[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
			for (int i = 0; i < 4; i++) {
				int new_row = sandworm->pos.row + directions[i][0];
				int new_col = sandworm->pos.column + directions[i][1];
				if (new_row >= 1 && new_row < MAP_HEIGHT - 1 && new_col >= 1 && new_col < MAP_WIDTH - 1) {
					if (map[0][new_row][new_col] != 'R') {
						sandworm->pos.row = new_row;
						sandworm->pos.column = new_col;
						break;
					}
				}
			}
		}

		if (sandworm->pos.row == target_unit->pos.row && sandworm->pos.column == target_unit->pos.column) {
			target_unit->active = false;
		}
	}

	// 스파이스 생성
	if (sys_clock % SANDWORM_BIRTH_INTERVAL == 0) {
		create_spice(&spices[rand() % MAX_SPICES], sandworm->pos.row, sandworm->pos.column);
	}

	// 샌드웜의 새로운 위치를 맵에 반영
	map[1][sandworm->pos.row][sandworm->pos.column] = sandworm->repr;

	// 샌드웜의 다음 이동 시간을 설정 (이동 주기)
	sandworm->next_move_time = sys_clock + sandworm->speed;

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

	// 이동 시 바위가 있는지 확인
	if (obj->pos.row >= 1 && obj->pos.row < MAP_HEIGHT - 1 &&
		obj->pos.column >= 1 && obj->pos.column < MAP_WIDTH - 1) {
		// 바위 'R'이 있는 위치로 이동하지 않도록 처리
		if (map[0][obj->pos.row][obj->pos.column] != 'R') {
			map[1][obj->pos.row][obj->pos.column] = obj->repr;
		}
	}

	// 이동 주기 업데이트
	obj->next_move_time = sys_clock + obj->speed;
}

void move_unit_randomly(UNIT* eagle) {
	if (sys_clock <= eagle->next_move_time) {
		return;
	}

	map[1][eagle->pos.row][eagle->pos.column] = -1;

	int direction = rand() % 4;
	int new_row = eagle->pos.row;
	int new_column = eagle->pos.column;

	switch (direction) {
	case 0: new_row--; break;
	case 1: new_row++; break;
	case 2: new_column--; break;
	case 3: new_column++; break;
	}

	if (new_row >= 1 && new_row < MAP_HEIGHT - 1 &&
		new_column >= 1 && new_column < MAP_WIDTH - 1) {
		// 기존 위치 초기화
		map[eagle->layer - 1][eagle->pos.row][eagle->pos.column] = ' ';
		// 새 위치 설정
		set_unit_position2(eagle, new_row, new_column);
	}

	eagle->next_move_time = sys_clock + eagle->speed;
}

// 유닛 초기화 함수
void initialize_units(void) {
	// Atreides 유닛 초기화
	for (int i = current_atreides_units; i < MAX_ATREIDES_UNITS; i++) {
		if (i >= MAX_ATREIDES_UNITS) break;
		atreides_units[i].active = false;           // 비활성화
		atreides_units[i].pos.row = -1;             // 유효하지 않은 위치
		atreides_units[i].pos.column = -1;          // 유효하지 않은 위치
	}

	// Harkonnen 유닛 초기화
	for (int i = current_harkonnen_units; i < MAX_HARKONNEN_UNITS; i++) {
		if (i >= MAX_HARKONNEN_UNITS) break;
		harkonnen_units[i].active = false;          // 비활성화
		harkonnen_units[i].pos.row = -1;            // 유효하지 않은 위치
		harkonnen_units[i].pos.column = -1;         // 유효하지 않은 위치
	}
}

void initialize_game(void) {
	current_atreides_units = 1;  // 초기 생성된 아트레이디스 유닛 수
	current_harkonnen_units = 3; // 초기 생성된 하코넨 유닛 수

	initialize_units();          // 모든 유닛 비활성화 및 초기화
	// 추가 초기화 코드
}

// 유닛 생산 관련 변수
int unit_production_time[MAX_ATREIDES_UNITS] = { 0 };
bool is_unit_being_produced[MAX_ATREIDES_UNITS] = { false };

void complete_unit_production(int unit_index) {
	UNIT* unit_to_produce = &atreides_units[unit_index];

	BUILDING* building = &atreides_buildings[unit_to_produce->layer];
	POSITION spawn_position = { building->pos.row, building->pos.column + 1 };
	set_unit_position(unit_to_produce, spawn_position.row, spawn_position.column);
}

void start_unit_production(char building_type) {

	UNIT* unit_to_produce = NULL;

	switch (building_type) {
	case 'b':
		unit_to_produce = &atreides_units[current_atreides_units];
		if (resource.spice >= unit_to_produce->production_cost) {
			resource.spice -= unit_to_produce->production_cost;
			is_unit_being_produced[current_atreides_units] = true;
			unit_production_time[current_atreides_units] = 1000;
			current_atreides_units++;
			set_unit_position(&atreides_units[2], MAP_HEIGHT - 4, 2);
			printf("A new %s is being produced!\n", unit_to_produce->name);
		}
		else {
			printf("Not enough spice.\n");
		}
		break;
	default:
		break;
	}
}

void game_tick(void) {
	static int atreides_moving_to_spice[MAX_ATREIDES_UNITS] = { 1 };
	for (int i = 0; i < current_atreides_units; i++) {
		if (atreides_units[i].active) {  // 활성화된 유닛만 이동
			unit_move(&atreides_units[i], &atreides_moving_to_spice[i]);
		}
	}

	for (int i = 0; i < MAX_ATREIDES_UNITS; i++) {
		if (is_unit_being_produced[i]) {
			unit_production_time[i]--;
			if (unit_production_time[i] <= 0) {
				is_unit_being_produced[i] = false;
				complete_unit_production(i);  // 생산 완료 후 유닛을 맵에 추가
			}
		}
	}

	static int harkonnen_moving_to_spice[MAX_HARKONNEN_UNITS] = { 1 };
	for (int i = 0; i < current_harkonnen_units; i++) {
		if (harkonnen_units[i].active) {  // 활성화된 유닛만 이동
			unit_move(&harkonnen_units[i], &harkonnen_moving_to_spice[i]);
		}
	}

	for (int i = 0; i < MAX_SANDWORMS; i++) {
		sandworm_move(&sandworms[i]);
	}

	for (int i = 0; i < MAX_EAGLES; i++) {
		move_unit_randomly(&eagle[i]);
	}
}