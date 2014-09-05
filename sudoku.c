#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

/*
 * Basic Types
 * ===========
 * 
 * In C, the char is always an octet (a byte), but I prefer to call them
 * 'byte' when I mean a non-negative number in the range [0..255].
 */

typedef unsigned char byte;

/*
 * Digits
 * ======
 *
 * A solved sudoku consists of a 9x9 grid of digits. Each such digit
 * is an integer between 1 and 9, so we define digit as an alias for byte.
 */

typedef byte digit;

#define MIN_DIGIT        ((digit)1)
#define MAX_DIGIT        ((digit)9)
#define NUMBER_OF_DIGITS (MAX_DIGIT - MIN_DIGIT + 1)

/*
 * Digits to Text
 * --------------
 *
 * Internally, we represent digits as bytes between 1 and 9, but
 * for input and output we'll be dealing with characters '1'
 * though '9'. These macros provide this conversion.
 */

#define DIGIT_TO_CHAR(d) ((char)(d + '0'))
#define CHAR_TO_DIGIT(c) ((digit)(c - '0'))

/*
 * Popcount (Hamming Distance)
 * ===========================
 *
 * Implementing digit sets (below) requires an implementation of
 * popcount to count the number of 1 bits in the digit sets.
 * GCC provides __builtin_popcount(n), but the performance of
 * same on ARM is underwhelming.
 */

unsigned char popcount_lut[512];

void popcount_init(void)
{
  for (int i = 0; i < 512; i++)
    popcount_lut[i] = __builtin_popcount(i);
}

int popcount(unsigned short n)
{
  return popcount_lut[n >> 1];
}

/*
 * Digit Sets
 * ==========
 *
 * For each of the 81 positions on the Sudoku we'll want to keep track
 * of which of the 9 digits are (still) possible for that position.
 * (So, a set of the digits [1..9].
 * 
 * We represent this set using a short int (16 bit) where the bits 1
 * through 9 are set to 1 to indicate the presence of the corresponding
 * digit in the set. The other bits [0, 10..15] are always zero.
 */

typedef unsigned short int digit_set;

#define NO_DIGITS          ((digit_set)0)
#define ALL_DIGITS         ((digit_set)0x03FE)
#define SET_OF(digit)      ((digit_set)(1 << (digit)))
#define IN_SET(set, digit) ((set & SET_OF(digit)) != 0)
#define SET_SIZE(set)      (popcount(set))

/*
 * Representing the Sudoku Board
 * =============================
 * 
 * The sudoku board has 81 positions arranged in a square with 9 rows
 * and 9 columns.  We follow the convention of numbering the positions
 * from 0 to 80 beginning in the upper left corner and working our way
 * across each row from left to right before dropping down to the next
 * row below.
 *
 *  0  1  2 |  3  4  5 |  6  7  8 
 *  9 10 11 | 12 13 14 | 15 16 17 
 * 18 19 20 | 21 22 23 | 24 25 26 
 * ---------+----------+---------
 * 27 28 29 | 30 31 32 | 33 34 35
 * 36 37 38 | 39 40 41 | 42 43 44 
 * 45 46 47 | 48 49 50 | 51 52 53
 * ---------+----------+--------- 
 * 54 55 56 | 57 58 59 | 60 61 62 
 * 63 64 65 | 66 67 68 | 69 70 71 
 * 72 73 74 | 75 76 77 | 78 79 80
 *
 */

#define SUDOKU_SIZE 81

/*
 * A byte suffices to name any position on the Sudoku board.
 * (Positions are numbered [0 .. 80].)
 */
typedef byte pos; 

/*
 * Determining Neighbors
 * ---------------------
 *
 * Each of the 81 positions of the sudoku board is influenced by
 * exactly 20 other positions, which we refer to as neighbors.  The
 * neighbors of position P include all the positions that share P's
 * row, column or quadrant without duplicates and without P itself.
 *
 * A position may only contain a digit not contained by any of its
 * neighbors.
 */

#define NUM_NEIGHBORS 20

/*
 * For each position, we record the positions of its 20 neighbors.
 *
 * This array was computed once with a separate program and then
 * simply include here, which is more efficient than computing it
 * every time the program starts up.
 */
static const pos neighbors[SUDOKU_SIZE][NUM_NEIGHBORS] = {
    { 1, 9, 2,18, 3,27, 4,36,10, 5,45,11, 6,54, 7,63,19, 8,72,20},
    { 0,10, 2,19, 3,28, 9, 4,37, 5,46,11, 6,55,18, 7,64, 8,73,20},
    { 0, 1,11,20, 3,29, 9, 4,38,10, 5,47, 6,56,18, 7,65,19, 8,74},
    { 0, 1,12, 4, 2,21, 5,30,39,13,48,14, 6,57, 7,66,22, 8,75,23},
    { 0, 3, 1,13, 2,22, 5,31,12,40,49,14, 6,58,21, 7,67, 8,76,23},
    { 0, 3, 1,14, 4, 2,23,32,12,41,13,50, 6,59,21, 7,68,22, 8,77},
    { 0, 1,15, 7, 2,24, 8, 3,33, 4,42,16, 5,51,17,60,69,25,78,26},
    { 0, 6, 1,16, 2,25, 8, 3,34,15, 4,43, 5,52,17,61,24,70,79,26},
    { 0, 6, 1,17, 7, 2,26, 3,35,15, 4,44,16, 5,53,62,24,71,25,80},
    { 0,10, 1,11,18, 2,12,27,13,36,14,45,15,54,16,63,19,17,72,20},
    { 9, 1, 0,11,19, 2,12,28,13,37,14,46,15,55,18,16,64,17,73,20},
    { 9, 2, 0,10, 1,20,12,29,13,38,14,47,15,56,18,16,65,19,17,74},
    { 9, 3,10, 4,11,21, 5,30,13,39,14,48,15,57,16,66,22,17,75,23},
    { 9, 4, 3,10,11,22, 5,12,31,40,14,49,15,58,21,16,67,17,76,23},
    { 9, 5, 3,10, 4,11,23,12,32,13,41,50,15,59,21,16,68,22,17,77},
    { 9, 6,10, 7,11,24, 8,12,33,13,42,16,14,51,17,60,69,25,78,26},
    { 9, 7, 6,10,11,25, 8,12,34,15,13,43,14,52,17,61,24,70,79,26},
    { 9, 8, 6,10, 7,11,26,12,35,15,13,44,16,14,53,62,24,71,25,80},
    { 0,19, 9, 1,20, 2,21,27,22,36,10,23,45,11,24,54,25,63,26,72},
    {18, 1, 0,10,20, 2,21,28, 9,22,37,23,46,11,24,55,25,64,26,73},
    {18, 2, 0,19,11, 1,21,29, 9,22,38,10,23,47,24,56,25,65,26,74},
    {18, 3,19,12, 4,20, 5,30,22,39,13,23,48,14,24,57,25,66,26,75},
    {18, 4, 3,19,13,20, 5,21,31,12,40,23,49,14,24,58,25,67,26,76},
    {18, 5, 3,19,14, 4,20,21,32,12,22,41,13,50,24,59,25,68,26,77},
    {18, 6,19,15, 7,20, 8,21,33,22,42,16,23,51,17,60,25,69,26,78},
    {18, 7, 6,19,16,20, 8,21,34,15,22,43,23,52,17,24,61,70,26,79},
    {18, 8, 6,19,17, 7,20,21,35,15,22,44,16,23,53,24,62,25,71,80},
    { 0,28, 9,29,18,30,36,31,37,32,45,38,33,54,34,63,46,35,72,47},
    {27, 1,10,29,19,30,36,31,37,32,46,38,33,55,45,34,64,35,73,47},
    {27, 2,28,11,20,30,36,31,38,37,32,47,33,56,45,34,65,46,35,74},
    {27, 3,28,12,31,29,21,32,39,40,48,41,33,57,34,66,49,35,75,50},
    {27, 4,30,28,13,29,22,32,39,40,49,41,33,58,48,34,67,35,76,50},
    {27, 5,30,28,14,31,29,23,39,41,40,50,33,59,48,34,68,49,35,77},
    {27, 6,28,15,34,29,24,35,30,42,31,43,32,51,44,60,69,52,78,53},
    {27, 7,33,28,16,29,25,35,30,42,31,43,32,52,44,61,51,70,79,53},
    {27, 8,33,28,17,34,29,26,30,42,31,44,43,32,53,62,51,71,52,80},
    { 0,27,37, 9,28,38,18,29,39,40,41,45,42,54,43,63,46,44,72,47},
    {36, 1,27,10,28,38,19,29,39,40,41,46,42,55,45,43,64,44,73,47},
    {36, 2,27,37,11,28,20,29,39,40,41,47,42,56,45,43,65,46,44,74},
    {36, 3,30,37,12,31,38,21,32,40,41,48,42,57,43,66,49,44,75,50},
    {36, 4,30,37,13,31,38,22,32,39,41,49,42,58,48,43,67,44,76,50},
    {36, 5,30,37,14,31,38,23,32,39,40,50,42,59,48,43,68,49,44,77},
    {36, 6,33,37,15,34,38,24,35,39,40,43,41,51,44,60,69,52,78,53},
    {36, 7,33,37,16,34,38,25,35,39,42,40,41,52,44,61,51,70,79,53},
    {36, 8,33,37,17,34,38,26,35,39,42,40,43,41,53,62,51,71,52,80},
    { 0,27,46, 9,28,47,18,29,48,36,49,37,50,38,51,54,52,63,53,72},
    {45, 1,27,10,28,47,19,29,48,36,49,37,50,38,51,55,52,64,53,73},
    {45, 2,27,46,11,28,20,29,48,36,49,38,37,50,51,56,52,65,53,74},
    {45, 3,30,46,12,31,47,21,32,39,49,40,50,41,51,57,52,66,53,75},
    {45, 4,30,46,13,31,47,22,32,48,39,40,50,41,51,58,52,67,53,76},
    {45, 5,30,46,14,31,47,23,32,48,39,49,41,40,51,59,52,68,53,77},
    {45, 6,33,46,15,34,47,24,35,48,42,49,43,50,44,60,52,69,53,78},
    {45, 7,33,46,16,34,47,25,35,48,42,49,43,50,44,51,61,70,53,79},
    {45, 8,33,46,17,34,47,26,35,48,42,49,44,43,50,51,62,52,71,80},
    { 0,55, 9,56,18,57,27,63,58,36,64,59,45,65,60,72,61,73,62,74},
    {54, 1,10,56,19,57,28,63,58,37,64,59,46,65,60,72,61,73,62,74},
    {54, 2,55,11,20,57,29,63,58,38,64,59,47,65,60,72,61,73,62,74},
    {54, 3,55,12,58,56,21,59,30,66,39,67,48,68,60,75,61,76,62,77},
    {54, 4,57,55,13,56,22,59,31,66,40,67,49,68,60,75,61,76,62,77},
    {54, 5,57,55,14,58,56,23,32,66,41,67,50,68,60,75,61,76,62,77},
    {54, 6,55,15,61,56,24,62,57,33,69,58,42,70,59,51,71,78,79,80},
    {54, 7,60,55,16,56,25,62,57,34,69,58,43,70,59,52,71,78,79,80},
    {54, 8,60,55,17,61,56,26,57,35,69,58,44,70,59,53,71,78,79,80},
    { 0,54,64, 9,55,65,18,56,66,27,67,36,68,45,69,72,70,73,71,74},
    {63, 1,54,10,55,65,19,56,66,28,67,37,68,46,69,72,70,73,71,74},
    {63, 2,54,64,11,55,20,56,66,29,67,38,68,47,69,72,70,73,71,74},
    {63, 3,57,64,12,58,65,21,59,30,67,39,68,48,69,75,70,76,71,77},
    {63, 4,57,64,13,58,65,22,59,66,31,40,68,49,69,75,70,76,71,77},
    {63, 5,57,64,14,58,65,23,59,66,32,67,41,50,69,75,70,76,71,77},
    {63, 6,60,64,15,61,65,24,62,66,33,67,42,70,68,51,71,78,79,80},
    {63, 7,60,64,16,61,65,25,62,66,34,69,67,43,68,52,71,78,79,80},
    {63, 8,60,64,17,61,65,26,62,66,35,69,67,44,70,68,53,78,79,80},
    { 0,54,73, 9,55,74,18,56,75,27,63,76,36,64,77,45,65,78,79,80},
    {72, 1,54,10,55,74,19,56,75,28,63,76,37,64,77,46,65,78,79,80},
    {72, 2,54,73,11,55,20,56,75,29,63,76,38,64,77,47,65,78,79,80},
    {72, 3,57,73,12,58,74,21,59,30,66,76,39,67,77,48,68,78,79,80},
    {72, 4,57,73,13,58,74,22,59,75,31,66,40,67,77,49,68,78,79,80},
    {72, 5,57,73,14,58,74,23,59,75,32,66,76,41,67,50,68,78,79,80},
    {72, 6,60,73,15,61,74,24,62,75,33,69,76,42,70,77,51,71,79,80},
    {72, 7,60,73,16,61,74,25,62,75,34,69,76,43,70,77,52,71,78,80},
    {72, 8,60,73,17,61,74,26,62,75,35,69,76,44,70,77,53,71,78,79}
  };

/*
 * Internal Sudoku Board
 * ---------------------
 * 
 * Internally, we represent the sudoku board with 81 digit sets.
 * For each position we record the digits that are possible at
 * that position given to cofiguration of the rest of the board.
 */

typedef struct {
  digit_set free[SUDOKU_SIZE];
} sudoku;


void claim(sudoku *s, pos p, digit_set ds)
{
  assert( SET_SIZE(ds) == 1 );
  s->free[p] = ds;
  for (int i = 0; i < NUM_NEIGHBORS; i++) {
    pos n = neighbors[p][i];
    digit_set old_set = s->free[n];
    digit_set new_set = old_set & ~ds;
    s->free[n] = new_set;
    if (SET_SIZE(old_set) > 1 && SET_SIZE(new_set) == 1) {
      claim(s, n, new_set);
    }
  }
}


/*
 * Sudoku Solver
 * =============
 * 
 * The solver is a data structure which holds the Sudoku to
 * be solved as well as some additional metadata.
 */

typedef struct {
  sudoku sudoku;
  struct {
    int backtrack;
    int choice;
  } count;
} solver;


/*
 * Find the position which we'll next try to solve for.
 *
 * If the position returned has 0 degrees of freedom, then the
 * puzzle's current configuration violates the rules of sudoku and
 * cann not be solved.
 *
 * If the position returned has 1 degree of freedom, then the puzzle
 * has been successfully solved.
 *
 * If the position returned has 2 or more degrees of freedom, then the
 * possible digits should be explored until a solution is found.
 */
pos next_move(solver const *s)
{
  int p = 0, m = 10, i;
  for (i = 0; i < SUDOKU_SIZE; i++) {
    int n = SET_SIZE(s->sudoku.free[i]);
    if (n == 0) {
      /*
       * n == 0 implies that the current puzzle configuration is
       * unsolveable. Our caller will detect this and backtrack.
       */
      p = i;
      break;
    }
    if (n > 1 && n < m) {
      /*
       * The primary goal of the surrounding loop is to find
       * the free position with the fewest degrees of freedom.
       */
      m = n;
      p = i;
    }
  }
  /*
   * If we never found (n > 1 && n < m) then we may reach this
   * point with p referring to a position that is fixed (1 degree
   * of freedom.)  This indicates that the puzzle has been solved.
   */
  return p;
}

bool solve(solver *s)
{
  pos p = next_move(s);
  s->count.choice++;

  digit_set dsp = s->sudoku.free[p];
  int n = SET_SIZE(dsp);
  if (n == 0)
    return false;
  if (n == 1)
    return true;

  sudoku r = s->sudoku;
  for (digit d = MIN_DIGIT; d <= MAX_DIGIT; d++)
    if (IN_SET(dsp, d)) {
      claim(&s->sudoku, p, SET_OF(d));
      if (solve(s)) {
        return true;
      }
      s->count.backtrack++;
      s->sudoku = r;
    }
  return false;
}

void init_solver(solver *s, sudoku const * sudoku)
{
  if (sudoku) {
    s->sudoku = *sudoku;
  }
  s->count.backtrack = 0;
  s->count.choice = 0;
}

/*
 * Input/Output
 * ============
 *
 * We need to be able to read puzzles from stdin and write
 * their solutions and other ancilliary information to stdout.
 */

/*
 * Textual Sudoku Board
 * --------------------
 *
 * The textual sudoku board is a string of 81 chars. Fixed
 * positions are marked by the characters '1' though '9'. 
 * Open positions (to be solved for) may be marked by
 * any character, though '.' is used by convention.
 */

/*
 * Copy a textual representation of the sudoku s to the string pointed
 * to be t, which must point to at least SUDOKU_SIZE+1 bytes of
 * storage.
 */
void sudoku_to_text(sudoku const *s, char *t) 
{
  for (int i = 0; i < SUDOKU_SIZE; i++) {
    switch(SET_SIZE(s->free[i])) {
    case 0:
      t[i] = '#';
      break;
    case 1:
      for (digit d = MIN_DIGIT; d <= MAX_DIGIT; d++)
        if (IN_SET(s->free[i], d)) {
          t[i] = DIGIT_TO_CHAR(d);
          break;
        }
      break;
    default:
      t[i] = '.';
    }
  }
  t[SUDOKU_SIZE] = '\0';
}

void sudoku_from_text(sudoku *s, char const *t)
{
  for (int i = 0; i < SUDOKU_SIZE; i++)
    s->free[i] = ALL_DIGITS;

  for (int i = 0; i < SUDOKU_SIZE && t[i]; i++) {
    char c = t[i];
    if ('1' <= c && c <= '9')
      claim(s, i, SET_OF(c - '0'));
  }
}

/*
 * Reader
 * ------
 *
 * The reader knows how to read sudokus from stdout.
 */

typedef struct { 
  FILE *stream;
  char *buf;
  size_t len;
} reader;

reader *new_reader()
{
  reader* r = malloc(sizeof(reader));
  r->stream = stdin;
  r->buf = NULL;
  r->len = 0;
  return r;
}

void free_reader(reader *r)
{
  fclose(r->stream);
  free(r->buf);
  free(r);
}
 
bool read_sudoku(reader *r, solver *s)
{
  ssize_t line_len;
  line_len = getline(&r->buf, &r->len, r->stream);
  if (line_len < 0) {
    /*
     * This is how getline signals end of input (or error), so return
     * false to let our caller know that there are no more puzzles to
     * be had.
     */
    r->buf[0] = '\0';
    return false;
  }
  if (line_len > SUDOKU_SIZE) {
    /* 
     * Terminate buf at SUDOKU_SIZE, cutting off any newline character
     * so that printing buf in verbose mode doesn't put an unwanted
     * newline into our output.
     */
    r->buf[SUDOKU_SIZE] = '\0';
  }
  /* Parse the text in buff into the solver's sudoku. */
  sudoku_from_text(&s->sudoku, r->buf);
  init_solver(s, NULL);
  return true;
}


/*
 * Main
 * ====
 *
 * Read one sudoku puzzle of 81 characters per line from stdin.
 *
 * Emit the first solution found to stderr.
 */

int main(int n, char **args) {
  bool verbose = true;
  solver s;
  char t[SUDOKU_SIZE+1];

  popcount_init();

  reader* r = new_reader();
  while (read_sudoku(r, &s)) {
    bool solved = solve(&s);
    sudoku_to_text(&s.sudoku, t);
    if (verbose) {
      printf("%8d %8d %1s %81s %81s\n",
             s.count.choice,
             s.count.backtrack,
             solved ? "o" : "x",
             r->buf,
             t);
    } else {
      printf("%s\n", t);
    }
  }
  free_reader(r);
  return 0;
}
