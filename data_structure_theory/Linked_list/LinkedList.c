#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
//하나의 데이터 아이템의 정의는 다음과 같다
typedef struct Listdata {
	char name[30];
	int id;
	char phone[20];
	char province[30];
}listData;

//하나의 리스트 노드의 정의는 다음과 같다.
typedef struct Listnode* type_listNode_pointer;
typedef struct Listnode {
	listData data;
    type_listNode_pointer link;
}listNode;
//리스트의 첫 번째 노드를 가리키는 head는 다음과 같이 정의한다.
typedef struct LinkedList {
	type_listNode_pointer head;
	int length;
}linkedList;

//입력함수
void insertLast(linkedList* L, listData item);
void insert(linkedList* L, listNode* pre, listData x);
void delete(linkedList* L, int x);
void reverse(linkedList* L);
type_listNode_pointer search_insert_position(type_listNode_pointer first, int stunum);
type_listNode_pointer search_prev_delete(type_listNode_pointer first, int stunum, int* ptr_found);

//search 함수
type_listNode_pointer search(linkedList* L, int x);

// 링크드리스트 길이 함수
int getLength(linkedList* L);

//출력함수
void printList(linkedList* L);

// 문자열 관련 함수
int read_number(char buf[], int* pos);
void read_name(char buf[], int* pos, char name[]);

void main() {
    linkedList LL = { NULL, 0 };  // 헤더 변수. 이것은 구조체임.
    listData anitem;
    type_listNode_pointer prev, prev2, np, y;
    char sname[30]; int snum; char sphone[20]; char sprovince[30];
    int res, found;
    FILE* fp;
    int ib;
    int exit_flag = 0, stu_num1, stu_num2;
    char buffer[100], command[30];
    char stu_name[30];
    int length = 0;
    // 파일의 데이터를 모두 연결리스트에 넣자.
    if (!(fp = fopen("./student_data.txt", "r")))
    {
        printf("student_data.txt를 찾을수 없습니다.");
        return 1;
    }
    while (1) {
        res = fscanf(fp, "%s %d %s %s", sname, &snum, sphone, sprovince);
        if (res < 4) break;
        strcpy(anitem.name, sname); 
        strcpy(anitem.phone, sphone);
        strcpy(anitem.province, sprovince);
        anitem.id = snum;
        insertLast(&LL, anitem);
    }
    fclose(fp);
    printf("student_data.txt을 읽어왔습니다\n");
    printList(&LL);
    while (exit_flag == 0)
    {
        printf("명령문을 넣으시오>\n");
        printf("[print]\n");
        printf("[search 학번]\n");
        printf("[insert 기준학번 학번 이름 번호 지역]\n");
        printf("[delete 학번]\n");
        printf("[reverse]\n");
        printf("[getLength]\n");
        gets(buffer);
        ib = 0;
        read_name(buffer, &ib, command);
        if(strcmp(command, "print") == 0)
        {
            system("cls");
            printf("print 명령결과:\n");
            printList(&LL);
        }
        else if(strcmp(command, "search") == 0)
        {
            system("cls");
            printf("search 명령결과:\n");
            stu_num1 = read_number(buffer, &ib);
            if (stu_num1 == -99999)continue;
            np = search(&LL, stu_num1);
            if (np)
            {
                printf("%s, %d, %s, %s\n", np->data.name, np->data.id, np->data.phone, np->data.province);
            }
            else
            {
                printf("찾지 못하였습니다.\n");
            }
        }
        else if (strcmp(command, "insert") == 0)
        {
            system("cls");
            printf("insert 명령결과:\n");
            stu_num1 = read_number(buffer, &ib);
            if (stu_num1 == -1) continue;
            stu_num2 = read_number(buffer, &ib);
            if (stu_num2 == -1) continue;
            read_name(buffer, &ib, stu_name);
            read_name(buffer, &ib, sphone);
            read_name(buffer, &ib, sprovince);
            if (stu_name[0] == '\0' || sphone[0] == '\0' || sprovince[0] == '\0')
            {
                printf("이름 또는 번호 또는 지역을 읽기 실패.\n");
                continue;
            }
            prev = search(&LL, stu_num1);
            prev2 = search(&LL, stu_num2);
            if (prev != NULL && prev2 == NULL)
            {
                strcpy(anitem.name, stu_name);
                anitem.id = stu_num2;
                strcpy(anitem.phone, sphone);
                strcpy(anitem.province, sprovince);
                insert(&LL, prev, anitem);
                printList(&LL);
            }
            else
            {
                printf("삽입 실패\n");
            }
        }
        else if(strcmp(command, "delete") == 0)
        {
            system("cls");
            printf("delete 명령결과:\n");
            stu_num1 = read_number(buffer, &ib);
            if (stu_num1 == -99999) continue;;
            delete(&LL, stu_num1);
            printList(&LL);
        }
        else if(strcmp(command, "reverse") == 0)
        {
            system("cls");
            reverse(&LL);
            printList(&LL);
        }
        else if (strcmp(command, "getLength") == 0)
        {
            system("cls");
            length = getLength(&LL);
            printf("노드의 갯수는 %d개입니다.\n", length);
        }
        else if(strcmp(command, "exit") == 0)
        {
            //exit(0);
            exit_flag = 1;
        }
        else
        {
            system("cls");
            printf("명령어를 찾을수없습니다.\n");
        }
    }
} // main

type_listNode_pointer search(linkedList* L, int x) {
    type_listNode_pointer first = L->head;
    while (first)
    {
        if (first->data.id == x)
        {
            return first;
        }
        else
        {
            first = first->link;
        }
    }
    
    return NULL;
} // search

type_listNode_pointer search_insert_position(type_listNode_pointer first, int stunum) {
    type_listNode_pointer curr, prev;
    prev = NULL;
    if (!first) return NULL;
    curr = first;
    while (curr && curr->data.id < stunum) {
        prev = curr; curr = curr->link;
    }
    return prev;
} // search_insert_position


type_listNode_pointer search_prev_delete(type_listNode_pointer first, int stunum, int* ptr_found) {
    type_listNode_pointer prev, curr;
    curr = first; prev = NULL;
    while (curr != NULL) {
        if (curr->data.id == stunum) {
            *ptr_found = 1;
            return prev;
        }
        else {
            prev = curr; curr = curr->link;
        }
    }
    *ptr_found = 0;
    return NULL;
} // search_prev_delete

void printList(linkedList* L)
{
    type_listNode_pointer curr;
    curr = L->head;
    for (; curr; curr = curr->link)
    {
        printf("%s %4d %s %s\n", curr->data.name, curr->data.id, curr->data.phone, curr->data.province);
    }
    printf("\n");
} // printList

void insert(linkedList* L, listNode* pre, listData x)
{
    type_listNode_pointer temp;
    temp = (type_listNode_pointer)malloc(sizeof(listNode));
    temp->data = x;
    if (L->head)
    {
        if (pre)
        {
            temp->link = pre->link;
            pre->link = temp;
        }
        else {
            temp->link = L->head;
            L->head = temp;
        }
    }
    else {
        temp->link = NULL;
        L->head = temp;
    }
    L->length++;
} // insert

void insertLast(linkedList* L, listData item)
{
    type_listNode_pointer temp;
    type_listNode_pointer prev = search_insert_position(L, item.id); // 학번을 키로 하자.
    temp = (type_listNode_pointer)malloc(sizeof(listNode));
    temp->data = item;
    if (L->head)
    {
        if (prev)
        {
            temp->link = prev->link;
            prev->link = temp;
            
        }
        else {
            temp->link = L->head;
            L->head = temp;
        }

    }
    else {
        temp->link = NULL;
        L->head = temp;
    }
    L->length++;
} // insertLast


void delete(linkedList* L, int x)
{
    type_listNode_pointer trail = NULL, curr;
    int found;
    trail = search_prev_delete(L->head, x, &found);
    if (found == 0)
        printf("이 학생이 존재하지 않아 삭제하지 못함.\n");
    else
    {
        if (trail)
        {
            curr = trail->link;
            trail->link = curr->link;
        }
        else
        {
            curr = L->head;
            L->head = curr->link;
        }
        free(curr);
        L->length--;
    }
} // delete

void reverse(linkedList* L) {
    type_listNode_pointer temp, curr, next;
    if (L->head == NULL)
        return;
    temp = NULL;
    curr = L->head;
    do {
        next = curr->link;
        curr->link = temp;
        temp = curr;
        curr = next;
    } while (curr);
    L->head = temp;
    return;
} // reverse


int getLength(linkedList* L)
{
    /*
    type_listNode_pointer curr;
    curr = L->head;
    int Llength = 0;
    for (; curr; curr = curr->link)
    {
        Llength++;
    }
    */
    
    return L->length;
} // getLength

int read_number(char buf[], int* pos)
{
    int n = 0, i = 0;
    char numstr[30];

    while (buf[*pos] == ' ' || buf[*pos] == ',')
    {
        (*pos)++;
    }

    while (isdigit(buf[*pos]))
    {
        numstr[i] = buf[*pos];
        i++;
        (*pos)++;
    }
    numstr[i] = '\0';

    if (i == 0)
    {
        printf("수를 읽지 못했어요. \n");
        return -1;
    }
    else {
        n = atoi(numstr);
        return n;
    }
} // read_number

void read_name(char buf[], int* pos, char name[])
{
    int i = 0;
    //공백문자나 컴마는 지나간다
    while (buf[*pos] == ' ' || buf[*pos] == ',') (*pos)++;
    while (buf[*pos] != ' ' && buf[*pos] != ', ' && buf[*pos] != '\0')
    {
        name[i] = buf[*pos];
        i++;
        (*pos)++;
    }
    name[i] = '\0';
} // read_name
