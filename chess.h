#include <stdio.h>
#include <locale.h>
#include <stddef.h> 

typedef enum {
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

int turn = WHITE;

char board[8][8];

void piece(int p) {
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
            piece(board[r][c]);
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
