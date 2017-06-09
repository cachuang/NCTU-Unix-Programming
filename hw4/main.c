#include "othello.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <stdbool.h>

#define MAXLINE 1024
#define COMPUTER_DELAY 1

extern int suggest_x;
extern int suggest_y;
extern int direction;

static int width;
static int height;
static int cx = 3;
static int cy = 3;
static int sockfd;
int player;
int opponent;
bool myturn;
bool computer_mode = false;
bool skipped = false;

int client_mode(char *ip, int port)
{
    int sockfd;
    struct sockaddr_in server_addr;

    memset(&server_addr, sizeof(server_addr), 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(errno);
    }

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(errno);
    }

    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr) ) < 0) {
        perror("connect error");
        exit(errno);
    }

    player = PLAYER2;
    opponent = PLAYER1;
    myturn = false;

    return sockfd;
}

int server_mode(int port)
{
    int listenfd, connfd;
    struct sockaddr_in server_addr, clientaddr;
    socklen_t len;

    memset(&server_addr, sizeof(server_addr), 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() Error");
        exit(errno);
    }

    if (bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind() Error");
        exit(errno);
    }

    if (listen(listenfd, 20) < 0) {
        perror("listen() Error");
        exit(errno);
    }

    connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &len);

    player = PLAYER1;
    opponent = PLAYER2;
    myturn = true;

    return connfd;
}

void change_turn()
{
    char message[MAXLINE];

    myturn = !myturn;

    if (myturn) {
        snprintf(message, sizeof(message), "Player #%d %s: It's your turn      ", player == PLAYER1 ? 1 : 2,
                 player == PLAYER1 ? "(O)" : "(X)");
        draw_message(message, 0);
    }
    else {
        snprintf(message, sizeof(message), "Player #%d %s: Waiting for peer ...      ", player == PLAYER1 ? 1 : 2,
                 player == PLAYER1 ? "(O)" : "(X)");
        draw_message(message, 1);
    }
}

bool isValidMove(int _x, int _y, int replace)
{
    int x, y;

    // check top
    x = _x; y = _y;
    while(--y >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (y = y+1; y < _y; y++) {
                direction = 1;
                return true;
            }
            break;
        }
    }
    // check bottom
    x = _x; y = _y;
    while(++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (y = y-1; y > _y; y--) {
                direction = 2;
                return true;
            }
            break;
        }
    }
    // check left
    x = _x; y = _y;
    while(--x >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x+1; x < _x; x++) {
                direction = 3;
                return true;
            }
            break;
        }
    }
    // check right
    x = _x; y = _y;
    while(++x < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1; x > _x; x--) {
                direction = 4;
                return true;
            }
            break;
        }
    }
    // check top left
    x = _x; y = _y;
    while(--x >= 0 && --y >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x+1, y = y+1; (x < _x && y < _y); x++, y++) {
                direction = 5;
                return true;
            }
            break;
        }
    }
    // check top right
    x = _x; y = _y;
    while(++x < BOARDSZ && --y >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1, y = y+1; (x > _x && y < _y); x--, y++) {
                direction = 6;
                return true;
            }
            break;
        }
    }
    // check bottom left
    x = _x; y = _y;
    while(--x >= 0 && ++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x + 1, y = y - 1; (x < _x && y > _y); x++, y--) {
                direction = 7;
                return true;
            }
            break;
        }
    }
    // check bottom right
    x = _x; y = _y;
    while(++x < BOARDSZ && ++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1, y = y-1; (x > _x && y > _y); x--, y--) {
                direction = 8;
                return true;
            }
            break;
        }
    }

    return false;
}

bool hasValidMove(int replace)
{
    for (int i = 0; i < BOARDSZ; i++)
        for (int j = 0; j < BOARDSZ; j++) {
            if (board[j][i] == 0 && isValidMove(i, j, replace)) {
                suggest_x = j;
                suggest_y = i;
                return true;
            }
        }

    return false;
}

bool update_board(int _x, int _y, int replace)
{
    int x, y;
    bool updated = false;

    for (int i = 0; i < BOARDSZ; i++)
        for (int j = 0; j < BOARDSZ; j++)
            prev_board[i][j] = board[i][j];
    prev_board[_y][_x] = 0;

    // check top
    x = _x; y = _y;
    while(--y >= 0) {
        if (board[y][x] == 0)
            break;
         if (board[y][x] == replace) {
             for (y = y+1; y < _y; y++) {
                 board[y][x] = replace;
                 updated = true;
             }
             break;
         }
    }
    // check bottom
    x = _x; y = _y;
    while(++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (y = y-1; y > _y; y--) {
                board[y][x] = replace;
                updated = true;
            }
            break;
        }
    }
    // check left
    x = _x; y = _y;
    while(--x >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x+1; x < _x; x++) {
                board[y][x] = replace;
                updated = true;
            }
            break;
        }
    }
    // check right
    x = _x; y = _y;
    while(++x < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1; x > _x; x--) {
                board[y][x] = replace;
                updated = true;
            }
            break;
        }
    }
    // check top left
    x = _x; y = _y;
    while(--x >= 0 && --y >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x+1, y = y+1; (x < _x && y < _y); x++, y++) {
                board[y][x] = replace;
                updated = true;
            }
            break;
        }
    }
    // check top right
    x = _x; y = _y;
    while(++x < BOARDSZ && --y >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1, y = y+1; (x > _x && y < _y); x--, y++) {
                board[y][x] = replace;
                updated = true;
            }
            break;
        }
    }
    // check bottom left
    x = _x; y = _y;
    while(--x >= 0 && ++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x + 1, y = y - 1; (x < _x && y > _y); x++, y--) {
                board[y][x] = replace;
                updated = true;
            }
            break;
        }
    }
    // check bottom right
    x = _x; y = _y;
    while(++x < BOARDSZ && ++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1, y = y-1; (x > _x && y > _y); x--, y--) {
                board[y][x] = replace;
                updated = true;
            }
            break;
        }
    }

    if (updated)
        draw_board();

    return updated;
}

void game_over()
{
    draw_message("Game Over                     ", 0);
    refresh();
}

void game_start()
{
    fd_set rset, wset;
    char message[MAXLINE];

    initscr();			                // start curses mode
    getmaxyx(stdscr, height, width);    // get screen size

    cbreak();			                // disable buffering
                                        // - use raw() to disable Ctrl-Z and Ctrl-C as well,
    halfdelay(1);			            // non-blocking getch after n * 1/10 seconds
    noecho();			                // disable echo
    keypad(stdscr, TRUE);		        // enable function keys and arrow keys
    curs_set(0);			            // hide the cursor

    init_colors();

    clear();
    cx = cy = 3;
    init_board();
    draw_board();
    draw_cursor(cx, cy, 1);
    draw_score();
    refresh();

//    WINDOW *win = newwin(10, 37, height/2 - 5, 0);
//    box(win,'|','-');
//    wrefresh(win);

    attron(A_BOLD);
    move(height-1, 0);
    printw("Arrow keys: move; Space/Enter: put; Q: quit");
    attroff(A_BOLD);
    refresh();

    FD_ZERO(&rset);
    FD_ZERO(&wset);

    if (myturn) {
        snprintf(message, sizeof(message), "Player #%d %s: It's your turn      ", player == PLAYER1 ? 1 : 2,
                 player == PLAYER1 ? "(O)" : "(X)");
        draw_message(message, 0);
    }
    else {
        snprintf(message, sizeof(message), "Player #%d %s: Waiting for peer ...      ", player == PLAYER1 ? 1 : 2,
                 player == PLAYER1 ? "(O)" : "(X)");
        draw_message(message, 1);
    }
    refresh();

    while (true) // main loop
    {
        FD_SET(STDIN_FILENO, &rset);
        FD_SET(sockfd, &rset);
        FD_SET(sockfd, &wset);

        select(sockfd+1, &rset, &wset, NULL, NULL);

        if (FD_ISSET(sockfd, &wset) && computer_mode && myturn)
        {
            for (int i = 0; i < BOARDSZ; i++)
            {
                bool moved = false;
                for (int j = 0; j < BOARDSZ; j++)
                {
                    if (board[i][j] == 0 && isValidMove(j, i, player))
                    {
                        sleep(COMPUTER_DELAY);

                        board[i][j] = player;
                        draw_cursor(j, i, 1);
                        draw_score();
                        update_board(j, i, player);
                        change_turn();
                        refresh();

                        snprintf(message, sizeof(message), "put %d %d", j, i);
                        write(sockfd, message, strlen(message));
                        moved = true;
                        break;
                    }
                }
                if (moved)
                    break;
            }
        }
        if (FD_ISSET(sockfd, &rset))
        {
            int n = read(sockfd, message, MAXLINE);

            if (n == 0)
            {
                endwin();
                printf("Peer has closed the connection.\n");
                exit(0);
            }
            message[n] = 0;

            if (!strncmp(message, "put", 3)) {
                int x, y;

                sscanf(message, "%*s %d %d", &x, &y);
                board[y][x] = opponent;
                draw_cursor(x, y, 0);
                update_board(x, y, opponent);
                draw_score();

                if (hasValidMove(player)) {
                    skipped = false;
                    draw_score();
                }
                else {
                    draw_message("No more move ...                    ", 0);

                    char message[] = "skip";
                    write(sockfd, message, sizeof(message));
                }

                change_turn();
                refresh();
            }
            else if (!strcmp(message, "skip"))
            {
                if (!hasValidMove(player)) {
                    game_over();
                    char message[] = "skip";
                    write(sockfd, message, sizeof(message));
                }
            }
        }
        if (FD_ISSET(0, &rset))
        {
            int ch = getch();
            int moved = 0;

            switch (ch)
            {
                case ' ':
                case 0x0d:
                case 0x0a:
                case KEY_ENTER:
                    if (myturn)
                    {
                        // prevent from overwriting existing piece
                        if(board[cy][cx] != 0 || !isValidMove(cx, cy, player))
                            break;

                        board[cy][cx] = player;
                        draw_cursor(cx, cy, 1);
                        draw_score();
                        update_board(cx, cy, player);
                        change_turn();
                        refresh();

                        snprintf(message, sizeof(message), "put %d %d", cx, cy);
                        write(sockfd, message, sizeof(message));
                    }
                    break;
                case 'r':
                case 'R':
                    draw_prev_board();
                    refresh();
                    break;
                case 'c':
                case 'C':
                    draw_board();
                    refresh();
                    break;
                case 'q':
                case 'Q':
                    goto quit;
                    break;
                case 'k':
                case KEY_UP:
                    draw_cursor(cx, cy, 0);
                    cy = (cy-1+BOARDSZ) % BOARDSZ;
                    draw_cursor(cx, cy, 1);
                    moved++;
                    break;
                case 'j':
                case KEY_DOWN:
                    draw_cursor(cx, cy, 0);
                    cy = (cy+1) % BOARDSZ;
                    draw_cursor(cx, cy, 1);
                    moved++;
                    break;
                case 'h':
                case KEY_LEFT:
                    draw_cursor(cx, cy, 0);
                    cx = (cx-1+BOARDSZ) % BOARDSZ;
                    draw_cursor(cx, cy, 1);
                    moved++;
                    break;
                case 'l':
                case KEY_RIGHT:
                    draw_cursor(cx, cy, 0);
                    cx = (cx+1) % BOARDSZ;
                    draw_cursor(cx, cy, 1);
                    moved++;
                    break;
            }

            if (moved) {
                refresh();
                moved = 0;
            }

            napms(1);		// sleep for 1ms
        }
    }

quit:
    endwin();			    // end curses mode
    close(sockfd);
    exit(0);
}

int main(int argc, char **argv)
{
    if ((argc < 2) || (strcmp(argv[1], "-c") && strcmp(argv[1], "-s") && strcmp(argv[1], "-i")))
    {
        printf("Run as server: ./othello -s [port]\n");
        printf("Run as client: ./othello -c [hostname | ip_address] [port]\n");
        printf("Run as computer: ./othello -i [port]\n");
        exit(0);
    }
    else if (!strcmp(argv[1], "-s"))
    {
        if(argc < 3)
        {
            printf("Usage: ./othello -s <port>\n");
            exit(0);
        }

        sockfd = server_mode(atoi(argv[2]));
    }
    else if (!strcmp(argv[1], "-i"))
    {
        if(argc < 3)
        {
            printf("Usage: ./othello -i <port>\n");
            exit(0);
        }

        computer_mode = true;
        sockfd = server_mode(atoi(argv[2]));
    }
    else if (!strcmp(argv[1], "-c"))
    {
        if(argc < 4)
        {
            printf("Usage: ./othello -c [hostname | ip_address] [port]\n");
            exit(0);
        }
        sockfd = client_mode(argv[2], atoi(argv[3]));
    }

    game_start();

	return 0;
}