#include <stdio.h>
#include <locale.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

typedef enum {
    ILLEGAL = 0,
    LEGAL = 1,
    EN_PASSANT = 5,
    CASTLE_LONG = 6,
    CASTLE_SHORT = 8
} Result;

typedef enum {
    LOL = -1,
    KING = 1,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    PAWN
} Piece;

typedef enum {
    WHITE = 1,
    BLACK = -1
} Player;

// global state
Player turn = WHITE;
char board[8][8] = {};
int en_passant = -1;
int next_en_passant = -1;
// bit field
unsigned long long has_moved = 0;

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

void render_piece(int p) {
    int white = 0x2653;
    int black = 0x2659;
    if (!p) {
        printf(" ");
        return;
    }
    if (p < 0) {
        printf("%lc", black - p);
    } else {
        printf("%lc", white + p);
    }
}

void render() {
    printf("  ");
    for (char c = 'A'; c < 'I'; ++c) {
        printf("   %c", c);
    }
    printf("\n   %lc", 0x250C);
    for (int i = 0; i < 7; ++i)  {
        printf("%lc", 0x2500);
        printf("%lc", 0x2500);
        printf("%lc", 0x2500);
        printf("%lc", 0x252C);
    }
    printf("%lc", 0x2500);
    printf("%lc", 0x2500);
    printf("%lc", 0x2500);
    printf("%lc", 0x2510);
    printf("\n");
    for (int r = 0; r < 8; ++r) {
        printf(" %d ", 8-r);
        for (int c = 0; c < 8; ++c) {
            printf("%lc ", 0x2502);
            render_piece(board[r][c]);
            printf(" ");
        }
        printf("%lc\n", 0x2502);
        if (r < 7) {
        printf("   %lc", 0x251C);
            for (int i = 0; i < 7; ++i)  {
                printf("%lc", 0x2500);
                printf("%lc", 0x2500);
                printf("%lc", 0x2500);
                printf("%lc", 0x253C);
            }
            printf("%lc", 0x2500);
            printf("%lc", 0x2500);
            printf("%lc", 0x2500);
            printf("%lc", 0x2524);
            printf("\n");
        }
    }
    printf("   %lc", 0x2514);
    for (int i = 0; i < 7; ++i)  {
        printf("%lc", 0x2500);
        printf("%lc", 0x2500);
        printf("%lc", 0x2500);
        printf("%lc", 0x2534);
    }
    printf("%lc", 0x2500);
    printf("%lc", 0x2500);
    printf("%lc", 0x2500);
    printf("%lc", 0x2518);
    printf("\n");
    if (turn == WHITE)
    printf("White's Turn\n\n");
    else
    printf("Black's Turn\n\n");
}

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

int str_to_index(char *str, size_t size) {
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
            int src = str_to_index(buf, n);
            free(buf);
            if (src == -1) {
                printf("What the fak\n");
                continue;
            }
            return src;
        }
}

int is_path_clear(int src, int dest) {
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

Result can_do_move(int src, int dest, int turn, int castle);

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
                if (can_do_move(r * 8 + c, king_row * 8 + king_col, -p, 0)) {
                    // we are in check oh no
                    return 1;
                }
            }
        }
    }
    return 0;
}

int no_legal_moves(Player p) {
    for(int src_row = 0; src_row < 8; ++src_row) {
        for(int src_col = 0; src_col < 8; ++src_col) {
            if (board[src_row][src_col] * p > 0) {
                for(int dest_row = 0; dest_row < 8; ++dest_row) {
                    for(int dest_col = 0; dest_col < 8; ++dest_col) {
                        if (dest_row == src_row && dest_col == src_col) continue;
                        if (can_do_move(src_row * 8 + src_col, dest_row * 8 + dest_col, p, 0)) {
                            // ok this move is valid; see if it gets us out of check
                            int temp = board[dest_row][dest_col];
                            board[dest_row][dest_col] = board[src_row][src_col];
                            board[src_row][src_col] = 0;

                            // are we in check now?
                            int is_check = (is_in_check(turn));
                            // we need to undo
                            board[src_row][src_col] = board[dest_row][dest_col];
                            board[dest_row][dest_col] = temp;

                            // we can still win maybe!
                            if (!is_check) return 0;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

char * errstring = "";

int get_piece(int index) {
        int r = index / 8;
        int c = index % 8;
        return board[r][c];
}

void set_piece(int index, Piece piece) {
        int r = index / 8;
        int c = index % 8;
        board[r][c] = piece;
}

Result can_do_move(int src, int dest, int turn, int castle) {
        int src_row = src / 8;
        int src_col = src % 8;
        int dest_row = dest / 8;
        int dest_col = dest % 8;
        if (board[dest_row][dest_col] * turn > 0) {
            errstring = ("You can't capture your own piece\n");
            return ILLEGAL;
        }

        if (src == dest) {
            errstring = ("You can't not move\n");
            return ILLEGAL;
        }

        switch (abs(board[src_row][src_col])) {
            case KNIGHT:
                int dx = abs(dest_row - src_row);
                int dy = abs(dest_col - src_col);
                if (min(dx,dy) == 1 && max(dx,dy) == 2) {
                    return LEGAL;
                } else {
                    errstring = ("Horse don't move like that\n");
                    return ILLEGAL;
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
                        return LEGAL;
                    } else if (dx == 0 && to == 0 && dy == turn * 2 && start_row == src_row) {
                        // this is us moving forward by 2 on the first move
                        // check if the middle space is clear
                        if (board[(dest_row + src_row) / 2][dest_col]) {
                            errstring = ("noclip is not enabled\n");
                            return ILLEGAL;
                        }
                        next_en_passant = (dest_row + src_row) * 4 + dest_col;
                        return LEGAL;
                    } else if (dx == 1 && !to && dy == turn && en_passant != -1) {
                        // wtf en passant???
                        int ep_row = en_passant / 8;
                        int ep_col = en_passant % 8;
                        if (ep_row != dest_row || ep_col != dest_col) {
                            errstring = ("this en passant is incorrect\n");
                            return ILLEGAL;
                        }
                        return EN_PASSANT; // lmao
                    }
                    else if (dx == 1 && dy == turn && to) {
                        // this is a capture, so it's legal
                        return LEGAL;
                    }
                    else {
                        errstring = ("Illegal pawn move\n");
                        return ILLEGAL;
                    }
                    break;
                }
            case QUEEN:
                {
                    int dx = dest_col - src_col;
                    int dy = dest_row - src_row;
                    if (abs(dx) == abs(dy) || dx == 0 || dy == 0) {
                        if (!is_path_clear(src, dest)) {
                            errstring = ("there was a car crash\n");
                            return ILLEGAL;
                        }
                        return LEGAL;
                    }
                    else {
                        errstring = ("bro has never played chess before lol\n");
                        return ILLEGAL;
                    }
                    break;
                }
            case ROOK:
                {
                    int dx = dest_col - src_col;
                    int dy = dest_row - src_row;
                    if (dx == 0 || dy == 0) {
                        if (!is_path_clear(src, dest)) {
                            errstring = ("there was a car crash\n");
                            return ILLEGAL;
                        }
                        return LEGAL;
                    }
                    else {
                        errstring = ("bro has never played chess before lol\n");
                        return ILLEGAL;
                    }
                    break;
                }
            case BISHOP:
                {
                    int dx = dest_col - src_col;
                    int dy = dest_row - src_row;
                    if (abs(dx) == abs(dy)) {
                        if (!is_path_clear(src, dest)) {
                            errstring = ("there was a car crash\n");
                            return ILLEGAL;
                        }
                        return LEGAL;
                    }
                    else {
                        errstring = ("bro has never played chess before lol\n");
                        return ILLEGAL;
                    }
                    break;
                }
            case KING:
                {
                    int dx = dest_col - src_col;
                    int dy = dest_row - src_row;
                    int king_start = turn == BLACK ? 4 : 60;
                    int rook_row = turn == BLACK ? 0 : 7*8;
                    if (abs(dx) <= 1 && abs(dy) <= 1) {
                        return LEGAL;
                    }
                    else if (castle && !(has_moved & (1ULL << king_start))) {
                        if (is_in_check(turn)) {
                            errstring = ("you're in check buddy\n");
                            return ILLEGAL;
                        }
                        if (dest_col == 1) { // B
                            if (!(has_moved & (1ULL << rook_row))) {
                                int a = rook_row + 1;
                                int b = rook_row + 2;
                                int c = rook_row + 3;
                                if (get_piece(a) || get_piece(b) || get_piece(c)) {
                                    errstring = ("somebody in the way\n");
                                    return ILLEGAL;
                                }
                                // we're in business
                                int result = CASTLE_LONG;
                                errstring = ("couldn't castle sorry\n");

                                set_piece(rook_row + 4, 0);
                                set_piece(c, KING * turn);
                                if (is_in_check(turn)) result = ILLEGAL;

                                set_piece(c, 0);
                                set_piece(b, KING * turn);
                                if (is_in_check(turn)) result = ILLEGAL;

                                set_piece(b, 0);
                                set_piece(a, KING * turn);
                                if (is_in_check(turn)) result = ILLEGAL;

                                set_piece(a, 0);
                                set_piece(b, 0);
                                set_piece(c, 0);
                                set_piece(rook_row + 4, KING * turn);
                                return result;
                            }
                        } else if (dest_col == 6) { // G
                            if (!(has_moved & (1ULL << (rook_row+7)))) {
                                int a = rook_row + 6;
                                int b = rook_row + 5;
                                if (get_piece(a) || get_piece(b)) {
                                    errstring = ("somebody in the way\n");
                                    return ILLEGAL;
                                }
                                // we're in business
                                int result = CASTLE_SHORT;
                                errstring = ("couldn't castle sorry\n");

                                set_piece(rook_row + 4, 0);
                                set_piece(b, KING * turn);
                                if (is_in_check(turn)) result = ILLEGAL;

                                set_piece(b, 0);
                                set_piece(a, KING * turn);
                                if (is_in_check(turn)) result = ILLEGAL;

                                set_piece(a, 0);
                                set_piece(b, 0);
                                set_piece(rook_row + 4, KING * turn);
                                return result;
                            }
                        }
                        errstring = ("nahhhhh\n");
                        return ILLEGAL;
                    } else {
                        errstring = ("sorry king\n");
                        return ILLEGAL;
                    }
                    break;
                }
        }
        return LEGAL;
}

int main() {
    setlocale(LC_ALL, "");
    init_board();
    size_t game_good = 1;
    int error = 0;
    printf("\033[H\033[2J");
    render();
    while(game_good) {
        next_en_passant = -1;
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

        Result result = can_do_move(src, dest, turn, 1);
        if (result == ILLEGAL) {
            printf("%s", errstring);
            continue;
        }

        // apply the move to the board
        int temp = board[dest_row][dest_col];
        board[dest_row][dest_col] = board[src_row][src_col];
        board[src_row][src_col] = 0;

        // shhhh
        if (result == EN_PASSANT) {
            int ep_row = (en_passant / 8) + turn;
            int ep_col = en_passant % 8;
            board[ep_row][ep_col] = 0;
        }

        if (result == CASTLE_LONG) {
            int rook_row = turn == BLACK ? 0 : 7*8;
            board[rook_row][0] = 0;
            board[rook_row][2] = ROOK * turn;
        }

        if (result == CASTLE_SHORT) {
            int rook_row = turn == BLACK ? 0 : 7*8;
            board[rook_row][7] = 0;
            board[rook_row][5] = ROOK * turn;
        }

        // are we in check now?
        if (is_in_check(turn)) {
            // shit fuck, we need to undo
            board[src_row][src_col] = board[dest_row][dest_col];
            board[dest_row][dest_col] = temp;
            if (result == EN_PASSANT) {
                int ep_row = (en_passant / 8) - turn;
                int ep_col = en_passant % 8;
                board[ep_row][ep_col] = PAWN * -turn;
            }
            printf("that would leave you in check\n");
            continue;
        }
        // we have now confirmed our move!


        // basic auto-promotion to queen
        int promote_rank = turn == BLACK ? 7 : 0;
        if(abs(board[dest_row][dest_col]) == PAWN) {
            if (dest_row == promote_rank) {
                board[dest_row][dest_col] = QUEEN;
            }
        }

        has_moved |= 1ULL << src;
        en_passant = next_en_passant;

        turn = -turn;
        // clear
        printf("\033[H\033[2J");
        render();
        if(no_legal_moves(turn)) {
            game_good = 0;
            if (is_in_check(turn)) {
                printf("gg its mate m8\n");
            } else {
                printf("badgame its stalemate m8\n");
            }
        }
    }
    return 0;
}
