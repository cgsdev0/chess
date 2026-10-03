#include "chess.h"
#include "moves.h"
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

char board[8][8] = {};


void init_board() {
    board[0][0] = board[0][7] = -ROOK;
    board[0][1] = board[0][6] = -KNIGHT;
    board[0][2] = board[0][5] = -BISHOP;
    board[0][3] = -QUEEN;
    board[0][4] = -KING;
    for (int c = 0; c < 8; c++) {
        board[1][c] = -PAWN;
    }

    for (int c = 0; c < 8; c++) {
        board[6][c] = PAWN;
    }
    board[7][0] = board[7][7] = ROOK;
    board[7][1] = board[7][6] = KNIGHT;
    board[7][2] = board[7][5] = BISHOP;
    board[7][3] = QUEEN;
    board[7][4] = KING;
}

int to_index(char *str, size_t size) {
    if (size != 3) return -1;
    char first = tolower(str[0]);
    if (first >= 'a' && first <= 'h') {
        if (str[1] >= '1' && str[1] <= '8') {
            int col = first - 'a';
            int row = 8 - (str[1] - '0');
            return row * 8 + col;
        }
    } 
    return -1;
}

int get_position(char *prompt) {
        while(1) {
            char *buf = NULL;
            size_t size = 0;
            printf("%s", prompt);
            size_t n = getline(&buf, &size, stdin);
            int src = to_index(buf, n);
            free(buf);
            if (src == -1) { 
                printf("What the fak\n");
                continue; 
            }
            return src;
        }

}
int can_move_to(int src, int dest) {
    int src_row = src / 8;
    int src_col = src % 8;
    int dest_row = dest / 8;
    int dest_col = dest % 8;
    int dx = dest_col - src_col;
    int dy = dest_row - src_row;
    if (dx) dx /= abs(dx);
    if (dy) dy /= abs(dy);
    int c_row = src_row;
    int c_col = src_col;
    while (c_row != dest_row - dy || c_col != dest_col - dx) {
        c_row += dy;
        c_col += dx;
        if (board[c_row][c_col]) {
            return 0;
        }
    }
    return 1;
}

int is_in_check(Player p) {
    int king_row = 0;
    int king_col = 0;
    for(int r = 0; r < 8; ++r) {
        for(int c = 0; c < 8; ++c) {
            if (board[r][c] * p == KING) {
                king_row = r;
                king_col = c;
            }
        }
    }
    for(int r = 0; r < 8; ++r) {
        for(int c = 0; c < 8; ++c) {
            if (board[r][c] * p < 0) {
                if (can_do_move(r * 8 + c, king_row * 8 + king_col, -p)) {
                    // we are in check oh no
                    return 1;
                }
            }
        }
    }
    return 0;
}

char * errstring = "";

int can_do_move(int src, int dest, int turn) {
        int src_row = src / 8;
        int src_col = src % 8;
        int dest_row = dest / 8;
        int dest_col = dest % 8;
        if (board[dest_row][dest_col] * turn > 0) {
            errstring = ("You can't capture your own piece\n");
            return 0;
        }

        if (src == dest) {
            errstring = ("You can't not move\n");
            return 0;
        }

        switch (abs(board[src_row][src_col])) {
            case KNIGHT:
                if (!knight(src_row, src_col, dest_row, dest_col)) {
                    errstring = ("Horse don't move like that\n");
                    return 0;
                }
                break;
            case PAWN:
                {
                    int to = board[dest_row][dest_col];
                    int dx = abs(src_col - dest_col);
                    int dy = src_row - dest_row;
                    int start_row = turn == BLACK ? 1 : 6;
                    if (dx == 0 && to == 0 && dy == turn) {
                        // this is us moving forward by 1
                    } else if (dx == 0 && to == 0 && dy == turn * 2 && start_row == src_row) {
                        // this is us moving forward by 2 on the first move
                        // check if the middle space is clear
                        if (board[(dest_row + src_row) / 2][dest_col]) {
                            errstring = ("noclip is not enabled\n");
                            return 0;
                        }
                    } 
                    else if (dx == 1 && dy == turn && to) {
                        // this is a capture, so it's legal
                    }
                    else {
                        errstring = ("Illegal pawn move\n");
                        return 0;
                    }
                    break;
                }
            case QUEEN: 
                {
                    int dx = dest_col - src_col;
                    int dy = dest_row - src_row;
                    if (abs(dx) == abs(dy) || dx == 0 || dy == 0) {
                        if (!can_move_to(src, dest)) {
                            errstring = ("there was a car crash\n");
                            return 0;
                        }
                    }
                    else {
                        errstring = ("bro has never played chess before lol\n");
                        return 0;
                    }
                    break;
                }
            case ROOK: 
                {
                    int dx = dest_col - src_col;
                    int dy = dest_row - src_row;
                    if (dx == 0 || dy == 0) {
                        if (!can_move_to(src, dest)) {
                            errstring = ("there was a car crash\n");
                            return 0;
                        }
                    }
                    else {
                        errstring = ("bro has never played chess before lol\n");
                        return 0;
                    }
                    break;
                }
            case BISHOP: 
                {
                    int dx = dest_col - src_col;
                    int dy = dest_row - src_row;
                    if (abs(dx) == abs(dy)) {
                        if (!can_move_to(src, dest)) {
                            errstring = ("there was a car crash\n");
                            return 0;
                        }
                    }
                    else {
                        errstring = ("bro has never played chess before lol\n");
                        return 0;
                    }
                    break;
                }
            case KING: 
                {
                    int dx = dest_col - src_col;
                    int dy = dest_row - src_row;
                    if (abs(dx) <= 1 && abs(dy) <= 1) {
                    }
                    // TODO: castling
                    else {
                        errstring = ("sorry king\n");
                        return 0;
                    }
                    break;
                }
        }
        return 1;
}

int main() {
    setlocale(LC_ALL, "");
    init_board();
    size_t game_good = 1;
    int error = 0;
    printf("\033[H\033[2J");
    render();
    while(game_good) {
        int src = get_position("Which piece? ");
        int src_row = src / 8;
        int src_col = src % 8;
        if (board[src_row][src_col] == 0) {
            printf("There's no piece dumbass\n");
            continue;
        }
        if (board[src_row][src_col] * turn <= 0) {
            printf("Lol that's not your piece bucko\n");
            continue;
        }
        int dest = get_position("Where to? ");
        int dest_row = dest / 8;
        int dest_col = dest % 8;

        if (!can_do_move(src, dest, turn)) {
            printf("%s", errstring);
            continue;
        }

        // TODO: verify we didn't enter check
        int temp = board[dest_row][dest_col];
        board[dest_row][dest_col] = board[src_row][src_col];
        board[src_row][src_col] = 0;

        // are we in check now?
        if (is_in_check(turn)) {
            // shit fuck, we need to undo
            board[src_row][src_col] = board[dest_row][dest_col];
            board[dest_row][dest_col] = temp;
            printf("that would leave you in check\n");
            continue;
        }


        turn = -turn;
        // clear
        printf("\033[H\033[2J");
        render();
    }
    return 0;
}

