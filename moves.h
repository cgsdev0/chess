#include <stdbool.h>
#include <stdlib.h>

#define valid(d) d >= 0 && d <= 7
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

bool knight(int row_s, int col_s, int row_d, int col_d) {
    int dx = abs(row_d - row_s);
    int dy = abs(col_d - col_s);
    return min(dx,dy) == 1 && max(dx,dy) == 2;
}
