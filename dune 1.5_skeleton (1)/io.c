/*
* raw(?) I/O
*/
#include "io.h"
#include "display.h"

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // 행, 열 반대로 전달
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printc(POSITION pos, char ch, int color) {
	if (color >= 0) {
		set_color(color);  // 글자색 설정
	}
	gotoxy(pos);
	printf("%c", ch);
}

void set_color_map(int color, unsigned short back) {
	if (color < 0) color = COLOR_DEFAULT;
	if (back < 0) back = COLOR_DEFAULT;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color | (back << 4));
}

void printc_map(POSITION pos, char ch, int color, unsigned short back) {
	if (color >= 0 && back >= 0) {
		set_color_map(color, back);  // 글자색과 배경색 설정
	}
	else if (color >= 0) {
		set_color(color);  // 글자색만 설정
	}
	else if (back >= 0) {
		set_background_color(COLOR_DEFAULT, COLOR_BACKGROUND);  // 배경색만 설정
	}
	gotoxy(pos);  // 커서 위치 이동
	printf("%c", ch);  // 문자 출력
}


// 키 입력 함수
KEY get_key(void) {
	if (!_kbhit()) {  // 입력된 키가 있는지 확인
		return k_none;
	}

	int byte = _getch();  // 입력된 키를 전달 받기
	switch (byte) {
	case 'q': return k_quit;  // 'q'를 누르면 종료
	case ' ': return k_spacebar;  // 스페이스바 처리
	case 27: return k_esc;  // ESC 키 처리
	case 224:
		byte = _getch();  // MSB 224가 입력되면 1바이트 더 전달 받기
		switch (byte) {
		case 72: return k_up;
		case 75: return k_left;
		case 77: return k_right;
		case 80: return k_down;
		default: return k_undef;
		}
	default: return k_undef;
	}
}