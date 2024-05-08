#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define m 14
#define p 14
#define Max_dir 4 // 총 가능한 이동방향의 수

typedef struct Aoff {
    short int vert;
    short int horiz;
} offsets;

typedef struct St_element { //스택에 저장할 데이터
    short int row;
    short int col;
    short int dir;
} ty_element;

typedef struct ListNode* listPointer;
typedef struct ListNode { //연결스택의 노드
    ty_element data;
    listPointer link;
} listNode;

offsets move[Max_dir]{ {0,1}, {0,-1},{1,0}, {-1,0} }; //E W S N

int maze[m + 2][p + 2]; // maze 행렬. 전역변수로 선언
int mark[m + 2][p + 2]; // mark 행렬

void gotoxy(int x, int y)
{
    COORD Pos = { x ,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void push(listPointer* top, ty_element pos)
{
    listPointer temp = (listPointer)malloc(sizeof(listNode));
    temp->data.row = pos.row;
    temp->data.col = pos.col;
    temp->data.dir = pos.dir;
    temp->link = *top;
    *top = temp;
}
ty_element pop(listPointer* top)
{
    listPointer temp = *top;
    ty_element pdata = temp->data;
    *top = temp->link;
    free(temp);
    return pdata;
}
void reverse(listPointer* top)
{
    listPointer temp, curr, next;

    temp = NULL;
    curr = *top;
    do {
        next = curr->link;
        curr->link = temp;
        temp = curr;
        curr = next;
    } while (curr);
    *top = temp;
}

int path(int sy, int sx, int dy, int dx)
{
    listPointer top = NULL;
    listPointer rtop = NULL;
    int i = 0, j = 0, row = 0, col = 0, nextRow = 0, nextCol = 0, dir = 0, basex = 0, basey = 0, ocount = 0;
    int found = 0, num_bktrack = 0, path_length = 0;
    int EXIT_ROW = dy, EXIT_COL = dx;
    ty_element position;
    int stop = 0;
    if (maze[sy][sx] == 1 || maze[dy][dx] == 1)
    {
        printf("입력오류 : 출발점이나 목표점이 막힌 셀입니다.\n");
        return 0;
    }
    if (sy == dy && sx == dx)
    {
        printf("입력오류 : 출발점과 목표점이 같습니다.\n");
        return 0;
    }

    // 먼지 미로를 화면에 그린다.
    CONSOLE_SCREEN_BUFFER_INFO presentCur;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &presentCur);
    basex = presentCur.dwCursorPosition.X;
    basey = presentCur.dwCursorPosition.Y;

    for (i = 0; i < m + 2; i++)
    {
        for (j = 0; j < p + 2; j++)
        {
            gotoxy(j + basex, i + basey);
            printf("%1d", maze[i][j]);
        }
    }

    position.row = sy;
    position.col = sx;
    position.dir = 0;
    push(&top, position);

    for (i = 0; i < m + 2; i++)
    {
        for (j = 0; j < p + 2; j++)
        {
            mark[1 + i][1 + j] = 0;
        }
    }

    char ch;
    for (i = 0; i < m + 2; i++) {
        for (j = 0; j < p + 2; j++) {
            gotoxy(j + basex, i + basey);
            if (maze[i][j] == 1)
                ch = '1';
            else if (maze[i][j] == 2)
                ch = '2';
            else
                ch = '0';
            printf("%c", ch);
        }
    }
    while (!found && top)
    {
        position = pop(&top);
        row = position.row;
        col = position.col;
        dir = position.dir;

        while (dir < Max_dir && !found)
        {
            nextRow = row + move[dir].vert;
            nextCol = col + move[dir].horiz;

            gotoxy(col + basex, row + basey);
            if (row == sy && col == sx)
            {
                printf(">");
                mark[row][col] = 1;
            }
            else
            {
                printf("*");
            }

            if (nextRow == EXIT_ROW && nextCol == EXIT_COL)
            {
                mark[row][col] = 1;
                gotoxy(nextCol + basex, nextRow + basey);
                printf("<");
                position.row = row; position.col = col;
                position.dir = ++dir;
                push(&top, position);
                found = TRUE;
            }
            else if (!maze[nextRow][nextCol] && !mark[nextRow][nextCol])
            {

                position.row = row;
                position.col = col;
                position.dir = dir;
                push(&top, position);
                mark[nextRow][nextCol] = 1;
                row = nextRow;
                col = nextCol;
                dir = 0;
            }
            else if (maze[nextRow][nextCol] == 2)
            {
                mark[row][col] = 1;
                gotoxy(nextCol + basex, nextRow + basey);
                printf("!");
                position.row = row; position.col = col;
                position.dir = ++dir;
                push(&top, position);
                stop = 1;
                break;
            }
            else
            {
                ++dir;
                if (dir >= Max_dir)
                {
                    gotoxy(col + basex, row + basey);
                    printf("$");
                    mark[row][col] = 1;
                    num_bktrack++;
                }
            }
        }
        if (stop == 1)
        {
            break;
        }
    }
    gotoxy(col + basex+4, row + basey+4);
    if (stop == 1)
    {
        printf("\n장애물 발견으로 중단");
    }
    printf("\n찾은 경로(row,col):\n");
    path_length = 0;
    reverse(&top);
    while (top)
    {
        position = pop(&top);
        printf("[%2d,%2d] ", position.row, position.col);
        ++path_length;
    }
    ++path_length;
    printf("[%2d,%2d]", nextRow, nextCol);
    printf("\n경로길이 = %d, 백트래킹수= %d", path_length, num_bktrack);
}


int main()
{
    // (fopen, fscanf 이용).
    FILE* fp = NULL;
    fp = fopen("maze.txt", "r");
    if (!fp)
    {
        printf("maze_14_2.txt not found");
        return 1;
    }
    int starty; int startx;
    int endy; int endx;
    int res;
    int i = 0;
    while (!feof(fp))
    {
        for (int j = 0; j < p + 2; j++)
        {
            fscanf(fp, "%d", &maze[i][j]);
        }
        i++;
    }
    while (TRUE) {


        printf("\n");
        printf("출발점, 목표점은?(4개의 정수; 종료는 Control+DEnter): ");
        res = scanf("%d%d%d%d", &starty, &startx, &endy, &endx);
        if (res != 4) break;
        path(starty, startx, endy, endx);
    }
}
