#ifndef __OTHELLO_H__
#define	__OTHELLO_H__

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#define	BOARDSZ		8
#define	PLAYER1		+1
#define	PLAYER2		-1

extern int board[BOARDSZ][BOARDSZ];

void init_board();
void init_colors();

void draw_message(const char *msg, int highlight);
void draw_cursor(int x, int y, int hide);
void draw_board();
void draw_score();

#endif	/* __OTHELLO_H__ */
