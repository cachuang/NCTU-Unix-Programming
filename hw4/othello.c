#include "othello.h"

#define	PLAYER1SYM	('O')
#define	PLAYER2SYM	('X')

int board[BOARDSZ][BOARDSZ];

static int const box_top = 1;
static int const box_left = 2;
static int const boxwidth = 3;

static int use_color = 0;
static int colorborder;
static int colorplayer1;
static int colorplayer2;
static int colorcursor;
static int colormsgwarn;
static int colormsgok;

void
init_board() {
	bzero(board, sizeof(board));
	board[3][3] = board[4][4] = PLAYER1;
	board[3][4] = board[4][3] = PLAYER2;
}

void
init_colors() {
	int coloridx = 0;	// color idx 0 is default color
	if(has_colors() == FALSE)
		return;
	start_color();
	//
	colorborder = ++coloridx;
	init_pair(colorborder, COLOR_WHITE, COLOR_BLACK);

	colorplayer1 = ++coloridx;
	init_pair(colorplayer1, COLOR_BLACK, COLOR_GREEN);

	colorplayer2 = ++coloridx;
	init_pair(colorplayer2, COLOR_BLACK, COLOR_MAGENTA);

	colorcursor = ++coloridx;
	init_pair(colorcursor, COLOR_YELLOW, COLOR_BLACK);

	colormsgwarn = ++coloridx;
	init_pair(colormsgwarn, COLOR_RED, COLOR_BLACK);

	colormsgok = ++coloridx;
	init_pair(colormsgok, COLOR_GREEN, COLOR_BLACK);
	//
	use_color = 1;
	return;
}

static chtype
BCH(int x, int y) {
	if(board[y][x] == PLAYER1) return PLAYER1SYM|COLOR_PAIR(colorplayer1);
	if(board[y][x] == PLAYER2) return PLAYER2SYM|COLOR_PAIR(colorplayer2);
	return ' ';
}

static void
draw_box(int x, int y, int ch, int color, int highlight) {
	int i;
	attron(highlight ? A_BOLD : A_NORMAL);
	attron(COLOR_PAIR(color));
	//
	move(box_top + y*2 + 0, box_left + x*(boxwidth+1));
	if(y == 0) addch(x == 0 ? ACS_ULCORNER : ACS_TTEE);
	else       addch(x == 0 ? ACS_LTEE : ACS_PLUS);
	for(i = 0; i < boxwidth; i++) addch(ACS_HLINE);
	if(y == 0) addch(x+1 == BOARDSZ ? ACS_URCORNER : ACS_TTEE);
	else       addch(x+1 == BOARDSZ ? ACS_RTEE : ACS_PLUS);
	//
	move(box_top + y*2 + 1, box_left + x*(boxwidth+1));
	addch(ACS_VLINE);
	for(i = 0; i < boxwidth/2; i++) addch(' ');
	addch(ch);
	for(i = 0; i < boxwidth/2; i++) addch(' ');
	addch(ACS_VLINE);
	//
	move(box_top + y*2 + 2, box_left + x*(boxwidth+1));
	if(y+1 == BOARDSZ) addch(x == 0 ? ACS_LLCORNER : ACS_BTEE);
	else               addch(x == 0 ? ACS_LTEE : ACS_PLUS);
	for(i = 0; i < boxwidth; i++) addch(ACS_HLINE);
	if(y+1 == BOARDSZ) addch(x+1 == BOARDSZ ? ACS_LRCORNER : ACS_BTEE);
	else               addch(x+1 == BOARDSZ ? ACS_RTEE : ACS_PLUS);
	//
	attroff(COLOR_PAIR(color));
	attroff(highlight ? A_BOLD : A_NORMAL);
}

void
draw_message(const char *msg, int highlight) {
	move(0, 0);
	attron(highlight ? A_BLINK : A_NORMAL);
	attron(COLOR_PAIR(highlight ? colormsgwarn : colormsgok));
	printw(msg);
	attroff(COLOR_PAIR(highlight ? colormsgwarn : colormsgok));
	attroff(highlight ? A_BLINK : A_NORMAL);
	return;
}

void
draw_cursor(int x, int y, int show) {
	draw_box(x, y, BCH(x, y), show ? colorcursor : colorborder, show);
	return;
}

void
draw_board() {
	int i, j;
	for(i = 0; i < BOARDSZ; i++) {
		for(j = 0; j < BOARDSZ; j++) {
			draw_box(i, j, BCH(i, j), colorborder, 0);
		}
	}
	return;
}

void
draw_score() {
	int i, j;
	int black = 0, white = 0;
	for(i = 0; i < BOARDSZ; i++) {
		for(j = 0; j < BOARDSZ; j++) {
			if(board[i][j] == PLAYER1) white++;
			if(board[i][j] == PLAYER2) black++;
		}
	}
	attron(A_BOLD);
	move(box_top+3, box_left + 4*BOARDSZ + 10);
	printw("Player #1 ");
	addch(PLAYER1SYM|COLOR_PAIR(colorplayer1));
	printw(" : %d", white);
	move(box_top+5, box_left + 4*BOARDSZ + 10);
	printw("Player #2 ");
	addch(PLAYER2SYM|COLOR_PAIR(colorplayer2));
	printw(" : %d", black);
	attroff(A_BOLD);
	return;
}

