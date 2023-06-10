#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define m 12
#define p 12
#define Max_dir 8
#define Timestep 1000
#define MAX_STACKSIZE 100

typedef struct AAoff {
	short int vert;
	short int horiz;
} offsets;

typedef struct St_element {
	short int row;
	short int col;
	short int dir;
} ty_element;
ty_element stack[MAX_STACKSIZE];
ty_element reverse_stack[MAX_STACKSIZE];
typedef struct ListNode* listPointer;
typedef struct ListNode {
	ty_element data;
	listPointer link;
} listNode;

offsets move[Max_dir]{ {-1,0 },{-1, 1}, {0,1}, {1, 1}, {1,0}, {1, -1}, {0, -1}, {-1, -1} };

int maze[m + 2][p + 2];
int mark[m + 2][p + 2];

void gotoxy(int x, int y)
{
	COORD Pos = { x ,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void push(listPointer *top, ty_element pos)
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
	//int newline = 0;
	//int back = 0;

	if (maze[sy][sx] == 1 || maze[dy][dx] == 1)
	{
		printf(" Է¿    :       ̳    ǥ             Դϴ .\n");
		return 0;
	}
	if (sy == dy && sx == dx)
	{
		printf(" Է¿    :           ǥ          ϴ .\n");
		return 0;
	}
	//       ̷θ  ȭ 鿡  ׸   .
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

	for (i = 0; i < m; i++)
	{
		for (j = 0; j < p; j++)
		{
			mark[1 + i][1 + j] = 0;
		}
		printf("\n");
	}

	char ch;
	for (i = 0; i < m + 2; i++) {
		for (j = 0; j < p + 2; j++) {
			gotoxy(j + basex, i + basey);
			if (maze[i][j] == 1)
				ch = '1';
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

		while (dir < 8 && !found)
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
				position.dir = ++dir;
				push(&top, position);
				mark[nextRow][nextCol] = 1;
				row = nextRow;
				col = nextCol;
				dir = 0;
			}
			else
			{
				++dir;
				if (dir == 8)
				{
					gotoxy(col + basex, row + basey);
					printf("$");
					mark[row][col] = 1;
					num_bktrack++;
					Sleep(Timestep);
				}
			}
			
			Sleep(Timestep / 10);
		}
	}

	gotoxy(basex, basey + m + 2);
	if (found)
	{
		//gotoxy(basex, basey + m + 2);
		printf("\nã      (row,col):\n");
		path_length = 0;
		/*               Ѱſ            
		while (top)
		{
			position = pop(&top);
			push(&rtop, position);
		}
		while (rtop)
		{
			position = pop(&rtop);
			if (newline >= 6)
			{
				printf("\n");
				newline = 0;
			}
			newline++;
			printf("[%d,%d]", position.row, position.col);
			++path_length;
		}
		*/
		reverse(&top);
		while (top)
		{
			
			position = pop(&top);
			if (ocount >= 6)
			{
				printf("\n");
				ocount = 0;
			}
			ocount++;
			printf("[%2d,%2d]", position.row, position.col);
			++path_length;
		}
		printf("[%2d,%2d]\n", dy, dx);
		++path_length;
		printf("  α    = %d,   Ʈ  ŷ  = %d", path_length, num_bktrack);
	}
	else
	{
		//gotoxy(basex, basey + m + 2);
		printf("  ξ   \n");
	}

}


int main()
{
	// (fopen, fscanf  ̿ ).
	FILE* fp = NULL;
	fp = fopen("maze_14_2.txt", "r");
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
		printf("     ,   ǥ    ?(4         ;       Control+DEnter): ");
		res = scanf("%d%d%d%d", &starty, &startx, &endy, &endx);
		if (res != 4) break;
		path(starty, startx, endy, endx);
	}
}
