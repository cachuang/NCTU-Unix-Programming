#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "othello.h"

#define MAXLINE 1024
#define COMPUTER_DELAY 1

static int width;
static int height;
static int cx = 3;
static int cy = 3;

int sockfd;
int player;
int opponent;
bool myturn;
bool computer_mode = false;

int client_mode(char *ip, int port)
{
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *hptr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if( (hptr = gethostbyname(ip)) == NULL ) {
        perror("gethostbyname error");
        exit(errno);
    }
    server_addr.sin_addr = *(struct in_addr *)(hptr->h_addr);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(errno);
    }

    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
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

    memset(&server_addr, 0, sizeof(server_addr));
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

void draw_title()
{
    char message[MAXLINE];

    if (myturn) {
        snprintf(message, sizeof(message), "Player #%d: It's your turn", player == PLAYER1 ? 1 : 2);
        draw_message(message, 0);
    }
    else {
        snprintf(message, sizeof(message), "Player #%d: Waiting for peer ...", player == PLAYER1 ? 1 : 2);
        draw_message(message, 1);
    }
}

// return 1 for yes, 0 for no
int draw_yes_no_box(WINDOW *win, char *message)
{
    int select = 1;
    int height, width;

    getmaxyx(win, height, width);

    box(win, 0, 0);
    mvwprintw(win, height/3, width/2 - strlen(message)/2, "%s", message);
    wattron(win, A_REVERSE);
    mvwprintw(win, height-2, width/2-6, "%s", "Yes");
    wattroff(win, A_REVERSE);
    mvwprintw(win, height-2, width/2+2, "%s", "No");
    wrefresh(win);

    while (true)
    {
        int ch = getch();

        switch (ch)
        {
            case 'h':
            case KEY_LEFT:
                if(select == 0) {
                    wattron(win, A_REVERSE);
                    mvwprintw(win, height-2, width/2-6, "%s", "Yes");
                    wattroff(win, A_REVERSE);
                    mvwprintw(win, height-2, width/2+2, "%s", "   ");
                    mvwprintw(win, height-2, width/2+2, "%s", "No");
                    select = 1;
                }
                break;
            case 'l':
            case KEY_RIGHT:
                if(select != 0) {
                    wattron(win, A_REVERSE);
                    mvwprintw(win, height-2, width/2+2, "%s", "No");
                    wattroff(win, A_REVERSE);
                    mvwprintw(win, height-2, width/2-6, "%s", "Yes");
                    select = 0;
                }
                break;
            case ' ':
            case 0x0d:
            case 0x0a:
            case KEY_ENTER:
                wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
                wrefresh(win);
                delwin(win);
                draw_board();
                refresh();

                return select;
        }
        wrefresh(win);
    }
}

void change_turn()
{
    myturn = !myturn;
    draw_title();
}

bool isValidMove(int _x, int _y, int replace)
{
    int x, y;

    // check top
    x = _x; y = _y;
    while (--y >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (y = y+1; y < _y; y++) {
                return true;
            }
            break;
        }
    }
    // check bottom
    x = _x; y = _y;
    while (++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (y = y-1; y > _y; y--) {
                return true;
            }
            break;
        }
    }
    // check left
    x = _x; y = _y;
    while (--x >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x+1; x < _x; x++) {
                return true;
            }
            break;
        }
    }
    // check right
    x = _x; y = _y;
    while (++x < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1; x > _x; x--) {
                return true;
            }
            break;
        }
    }
    // check top left
    x = _x; y = _y;
    while (--x >= 0 && --y >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x+1, y = y+1; (x < _x && y < _y); x++, y++) {
                return true;
            }
            break;
        }
    }
    // check top right
    x = _x; y = _y;
    while (++x < BOARDSZ && --y >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1, y = y+1; (x > _x && y < _y); x--, y++) {
                return true;
            }
            break;
        }
    }
    // check bottom left
    x = _x; y = _y;
    while (--x >= 0 && ++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x + 1, y = y - 1; (x < _x && y > _y); x++, y--) {
                return true;
            }
            break;
        }
    }
    // check bottom right
    x = _x; y = _y;
    while (++x < BOARDSZ && ++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1, y = y-1; (x > _x && y > _y); x--, y--) {
                return true;
            }
            break;
        }
    }

    return false;
}

bool hasValidMove(int replace)
{
    for (int i = 0; i < BOARDSZ; i++) {
        for (int j = 0; j < BOARDSZ; j++) {
            if (board[i][j] == 0 && isValidMove(j, i, replace)) {
                return true;
            }
        }
    }

    return false;
}

void update_board(int _x, int _y, int replace)
{
    int x, y;

    memcpy(old_board, board, sizeof(int) * BOARDSZ * BOARDSZ);
    old_board[_y][_x] = 0;

    // check top
    x = _x; y = _y;
    while (--y >= 0) {
        if (board[y][x] == 0)
            break;
         if (board[y][x] == replace) {
             for (y = y+1; y < _y; y++) {
                 board[y][x] = replace;
             }
             break;
         }
    }
    // check bottom
    x = _x; y = _y;
    while (++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (y = y-1; y > _y; y--) {
                board[y][x] = replace;
            }
            break;
        }
    }
    // check left
    x = _x; y = _y;
    while (--x >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x+1; x < _x; x++) {
                board[y][x] = replace;
            }
            break;
        }
    }
    // check right
    x = _x; y = _y;
    while (++x < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1; x > _x; x--) {
                board[y][x] = replace;
            }
            break;
        }
    }
    // check top left
    x = _x; y = _y;
    while (--x >= 0 && --y >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x+1, y = y+1; (x < _x && y < _y); x++, y++) {
                board[y][x] = replace;
            }
            break;
        }
    }
    // check top right
    x = _x; y = _y;
    while (++x < BOARDSZ && --y >= 0) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1, y = y+1; (x > _x && y < _y); x--, y++) {
                board[y][x] = replace;
            }
            break;
        }
    }
    // check bottom left
    x = _x; y = _y;
    while (--x >= 0 && ++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x + 1, y = y - 1; (x < _x && y > _y); x++, y--) {
                board[y][x] = replace;
            }
            break;
        }
    }
    // check bottom right
    x = _x; y = _y;
    while (++x < BOARDSZ && ++y < BOARDSZ) {
        if (board[y][x] == 0)
            break;
        if (board[y][x] == replace) {
            for (x = x-1, y = y-1; (x > _x && y > _y); x--, y--) {
                board[y][x] = replace;
            }
            break;
        }
    }
}

void computer_move()
{
    for (int i = 0; i < BOARDSZ; i++)
    {
        for (int j = 0; j < BOARDSZ; j++)
        {
            if (board[i][j] == 0 && isValidMove(j, i, player))
            {
                sleep(COMPUTER_DELAY);

                board[i][j] = player;
                update_board(j, i, player);
                draw_board();
                draw_score();
                change_turn();
                refresh();

                char message[MAXLINE];
                snprintf(message, sizeof(message), "put %d %d", j, i);
                write(sockfd, message, strlen(message));

                return;
            }
        }
    }
}

void quit()
{
    WINDOW *win = newwin(9, 37, height/2 - 4, 0);
    int answer = draw_yes_no_box(win, "Are you sure you want to quit?");

    if (answer == 1) {
        endwin();
        close(sockfd);
        exit(0);
    }
    else
        return;
}

void game_over()
{
    int player_count = 0;
    int opponent_count = 0;

    for (int i = 0; i < BOARDSZ; i++) {
        for (int j = 0; j < BOARDSZ; j++) {
            if (board[i][j] == player) player_count++;
            if (board[i][j] == opponent) opponent_count++;
        }
    }

    if (player_count > opponent_count)
        draw_message("YOU WIN !!!", 0);
    else if (player_count < opponent_count)
        draw_message("YOU LOSE ...", 0);
    else
        draw_message("Even", 0);

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

    draw_title();

    attron(A_BOLD);
    move(height-1, 0);
    printw("Arrow keys: move; Space/Enter: put; Q: quit");
    attroff(A_BOLD);

    refresh();

    FD_ZERO(&rset);
    FD_ZERO(&wset);

    while (true) // main loop
    {
        FD_SET(STDIN_FILENO, &rset);
        FD_SET(sockfd, &rset);
        FD_SET(sockfd, &wset);

        select(sockfd+1, &rset, &wset, NULL, NULL);

        if (FD_ISSET(sockfd, &wset) && computer_mode && myturn)
        {
            computer_move();
        }
        if (FD_ISSET(sockfd, &rset))
        {
            ssize_t n = read(sockfd, message, MAXLINE);
            message[n] = 0;

            if (n == 0) {
                endwin();
                printf("Peer has closed the connection.\n");
                exit(0);
            }

            if (!strncmp(message, "put", 3))
            {
                int x, y;

                sscanf(message, "%*s %d %d", &x, &y);

                board[y][x] = opponent;
                update_board(x, y, opponent);
                draw_board();
                draw_score();
                change_turn();

                if (!hasValidMove(player))
                {
                    if (!hasValidMove(opponent))
                        game_over();
                    else {
                        if (computer_mode) {
                            draw_message("No valid move ...", 0);
                            refresh();
                            sleep(COMPUTER_DELAY);
                        }
                        else {
                            draw_message("No valid move, press 's' to skip ...", 0);
                            while (true) {
                                int c = getch();
                                if (c == 's') break;
                            }
                        }

                        change_turn();
                    }

                    char message[] = "skip";
                    write(sockfd, message, sizeof(message));
                }

                refresh();
            }
            else if (!strcmp(message, "skip"))
            {
                if (hasValidMove(player))
                    change_turn();
                else
                {
                    game_over();

                    char message[] = "skip";
                    write(sockfd, message, sizeof(message));
                }

                refresh();
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
                        // prevent from overwriting existing piece and check if move is valid
                        if(board[cy][cx] != 0 || !isValidMove(cx, cy, player))
                            break;

                        // valid move
                        board[cy][cx] = player;
                        update_board(cx, cy, player);
                        draw_board();
                        draw_score();
                        change_turn();
                        refresh();

                        snprintf(message, sizeof(message), "put %d %d", cx, cy);
                        write(sockfd, message, sizeof(message));
                    }
                    break;
                case 'r':
                case 'R':
                    draw_old_board();
                    refresh();
                    break;
                case 'c':
                case 'C':
                    draw_board();
                    refresh();
                    break;
                case 'q':
                case 'Q':
                    quit();
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
}

int main(int argc, char **argv)
{
    if ((argc < 2) || (strcmp(argv[1], "-s") && strcmp(argv[1], "-c") && strcmp(argv[1], "-i"))) {
        printf("Run as server: ./othello -s [port]\n");
        printf("Run as client: ./othello -c [hostname | ip] [port]\n");
        printf("Run as computer: ./othello -i [port]\n");
        exit(0);
    }
    else if (!strcmp(argv[1], "-s")) {
        if (argc < 3) {
            printf("Usage: ./othello -s <port>\n");
            exit(0);
        }

        sockfd = server_mode(atoi(argv[2]));
    }
    else if (!strcmp(argv[1], "-c")) {
        if (argc < 4) {
            printf("Usage: ./othello -c [hostname | ip] [port]\n");
            exit(0);
        }

        sockfd = client_mode(argv[2], atoi(argv[3]));
    }
    else if (!strcmp(argv[1], "-i")) {
        if (argc < 3) {
            printf("Usage: ./othello -i <port>\n");
            exit(0);
        }

        computer_mode = true;
        sockfd = server_mode(atoi(argv[2]));
    }

    game_start();

	return 0;
}