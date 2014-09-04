/*
  originally from https://github.com/xkaycee/sudoku-generator/blob/master/sudoku-generator.c
  with modifications
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct 
{
  int sqNum;
  int filled;
} square;

int size = 9;
square box[9][9];
int sampleBox[9][9] = { 
  {1, 2, 3, 4, 5, 6, 7, 8, 9 },
  {4, 5, 6, 7, 8, 9, 1, 2, 3 },
  {7, 8, 9, 1, 2, 3, 4, 5, 6 },
  {2, 3, 4, 5, 6, 7, 8, 9, 1 },
  {5, 6, 7, 8, 9, 1, 2, 3, 4 },
  {8, 9, 1, 2, 3, 4, 5, 6, 7 },
  {3, 4, 5, 6, 7, 8, 9, 1, 2 },
  {6, 7, 8, 9, 1, 2, 3, 4, 5 },
  {9, 1, 2, 3, 4, 5, 6, 7, 8 } 
};

void initialize()
{
  int i, j;
        
  for(i = 0; i < size; i++)
    {
      for(j = 0; j < size; j++)
        {
          box[i][j].sqNum = sampleBox[i][j];
          box[i][j].filled = 0;
        }
    }   
}

void printSudoku(double blank_prob)
{
  double rmax = RAND_MAX*1.0;
  int i, j;
        
  for(i = 0; i < size; i++)
    {
      for(j = 0; j < size; j++)
        {
          double r = rand()/rmax;
          if (r < blank_prob) 
            {
              printf(".");
            }
          else 
            {
              printf("%i", box[i][j].sqNum);
            }
        }
    }
  printf("\n");
}

void swapLittleRow(int row1, int row2)
{
  int j, tempR;

  for(j = 0; j < size; j++)
    {
      tempR = box[row1][j].sqNum;
                
      box[row1][j].sqNum = box[row2][j].sqNum;
      box[row2][j].sqNum = tempR;
    }
}

void swapBigRow(int brow1, int brow2)
{       
  if(brow1 > 2 || brow2 > 2)
    printf("BIG ERROR");
  else
    {
      swapLittleRow(brow1 * 3, brow2 * 3);
      swapLittleRow(brow1 * 3 + 1, brow2 * 3 + 1);
      swapLittleRow(brow1 * 3 + 2, brow2 * 3 + 2);
    }
}

void swapLittleCol(int col1, int col2)
{
  int i, tempC; 
        
  for(i = 0; i < size; i++)
    {
      tempC = box[i][col1].sqNum;
                
      box[i][col1].sqNum = box[i][col2].sqNum;
      box[i][col2].sqNum = tempC;
    }
}

void swapBigCol(int bcol1, int bcol2)
{       
  if(bcol1 > 2 || bcol2 > 2)
    printf("BIG ERROR");
  else
    {
      swapLittleCol(bcol1 * 3, bcol2 * 3);
      swapLittleCol(bcol1 * 3 + 1, bcol2 * 3 + 1);
      swapLittleCol(bcol1 * 3 + 2, bcol2 * 3 + 2);
    }
}

void flipVert()
{
  int i, j, tempV;
        
  for(i = 0; i < size; i++)
    {
      for(j = 0; j < size/2; j++)
        {
          tempV = box[i][j].sqNum;
                        
          box[i][j].sqNum = box[i][8 - j].sqNum;
          box[i][8 - j].sqNum = tempV;
        }
    }
}

void flipHorz()
{
  int i, j, tempH;
        
  for(i = 0; i < size/2; i++)
    {
      for(j = 0; j < size; j++)
        {
          tempH = box[i][j].sqNum;
                        
          box[i][j].sqNum = box[8 - i][j].sqNum;
          box[8 - i][j].sqNum = tempH;
        }
    }
}

void flipDiag()
{
  int i, j, tempD;
        
  for(i = 0; i < size; i++)
    {
      for(j = i; j < size; j++)
        {
          tempD = box[i][j].sqNum;
                        
          box[i][j].sqNum = box[j][i].sqNum;
          box[j][i].sqNum = tempD;
        }
    }
}

void rotate90cw()
{
  int i, j;
  square tempbox[size][size];
        
  for(i = 0; i < size; i++)
    {
      for(j = 0; j < size; j++)
        {
          tempbox[i][j].sqNum = box[i][j].sqNum;
          tempbox[i][j].filled = 0;     
        }
    }
        
  for(i = 0; i < size; i++)
    {
      for(j = 0; j < size; j++)
        {
          box[i][j].sqNum = tempbox[j][i].sqNum;
        }
    }
        
}

void rotate90ccw()
{
  flipVert();
  rotate90cw();
}
void replace(int a, int b)
{
  int i, j;
  for(i = 0; i < size; i++)
    {
      for(j = 0; j < size; j++)
        {
          if(box[i][j].sqNum == a)
            box[i][j].sqNum = b;
                        
          else if(box[i][j].sqNum == b)
            box[i][j].sqNum =a;
        }
    }
}

void generate(void)
{
  int fn, i, blockChoice;
        
  for(i = 0; i < 10; i++) {
                
    fn = rand() % 10;
    blockChoice = rand() % 3;
                
    switch(fn)
      {
      case 0: 
        switch(blockChoice)
          {
          case 0:
            swapLittleRow(rand() % 3, rand() % 3);
            break;
                                        
          case 1:
            swapLittleRow((rand() % 3) + 3, (rand() % 3) + 3);
            break;
                                        
          case 2: 
            swapLittleRow((rand() % 3) + 6, (rand() % 3) + 6);
            break;
                                        
          }
        break;
      case 1: 
        swapBigRow((rand() % 3), (rand() % 3));
        break;
                        
      case 2: 
        switch(blockChoice)
          {
          case 0:
            swapLittleCol((rand() % 3), (rand() % 3));
            break;
                                        
          case 1:
            swapLittleCol((rand() % 3) + 3, (rand() % 3) + 3);
            break;
                                        
          case 2: 
            swapLittleCol((rand() % 3) + 6, (rand() % 3) + 6);
            break;
          }
        break;
      case 3: 
        swapBigCol((rand() % 3), (rand() % 3));
        break;
                        
      case 4: 
        flipVert();                     
        break;
                        
      case 5: 
        flipHorz();
        break;
                        
      case 6: 
        flipDiag();
        break;
                        
      case 7: 
        rotate90cw();
        break;
                        
      case 8: 
        rotate90ccw();
        break;
                        
      case 9: 
        replace((rand() % 9) + 1, (rand()  % 9)+ 1);
        break;
      }
  }
  printSudoku(0.66);
}

int main()
{
  initialize();
  srand(time(NULL));
  
  for (;;)
    generate();
  return 0;
}
