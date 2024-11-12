/*
* raw(?) I/O
*/
#include "io.h"
#include "display.h"

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // ��, �� �ݴ�� ����
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printc(POSITION pos, char ch, int color) {
	if (color >= 0) {
		set_color(color);  // ���ڻ� ����
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
		set_color_map(color, back);  // ���ڻ��� ���� ����
	}
	else if (color >= 0) {
		set_color(color);  // ���ڻ��� ����
	}
	else if (back >= 0) {
		set_background_color(COLOR_DEFAULT, COLOR_BACKGROUND);  // ������ ����
	}
	gotoxy(pos);  // Ŀ�� ��ġ �̵�
	printf("%c", ch);  // ���� ���
}


// Ű �Է� �Լ�
KEY get_key(void) {
	if (!_kbhit()) {  // �Էµ� Ű�� �ִ��� Ȯ��
		return k_none;
	}

	int byte = _getch();  // �Էµ� Ű�� ���� �ޱ�
	switch (byte) {
	case 'q': return k_quit;  // 'q'�� ������ ����
	case ' ': return k_spacebar;  // �����̽��� ó��
	case 27: return k_esc;  // ESC Ű ó��
	case 224:
		byte = _getch();  // MSB 224�� �ԷµǸ� 1����Ʈ �� ���� �ޱ�
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