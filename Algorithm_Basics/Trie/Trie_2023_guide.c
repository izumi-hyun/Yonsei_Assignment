#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node *nodeptr;
typedef struct node
{
    char ch;       // character in this node
    long int bp;   // a byte position in the master file
    nodeptr right; // a horizontal pointer
    nodeptr below; // a vertical poiner
} nodetype;
typedef struct record
{
    char word[50];
    int widx;
    int wfreq;
} ty_rec;
typedef struct correct_result
{
    char word[100];     // the result of correction
    int ns, nd, ni, nt; // number of substitution, deletion, insertion, transposistion
    float penalty;      // penalty score
} ty_correction;

///////////////////////////////////////////////////////////////////////////////////////////////
// Global variables가 주어진다.
///////////////////////////////////////////////////////////////////////////////////////////////
nodeptr root = NULL;
ty_correction cwords[20000];
int nres = 0;
int found_perfect_match = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
// 아래 함수는 guide로 주어지며 별도로 작성할 필요가 없다.
///////////////////////////////////////////////////////////////////////////////////////////////
void sort_results(ty_correction cwords[]);
int printed(char printed_words[][100], char aword[], int total);
// void main(); 함수 또한 별도로 작성할 필요가 없다.

///////////////////////////////////////////////////////////////////////////////////////////////
// 아래 선언된 함수는 아래 정의에서 직접 작성하거나 수정한다.
///////////////////////////////////////////////////////////////////////////////////////////////
int search_trie(char key[], nodeptr *curr, nodeptr *prev);
nodeptr hang_down(char key[], nodeptr tn, int i);
nodeptr insert_trie(char key[50]);

void transposition(nodeptr root_sub_trie, char key[], char corr_key[], int ki, int ci, int n_s, int n_d, int n_i, int n_t);
void substitution(nodeptr root_sub_trie, char key[], char corr_key[], int ki, int ci, int n_s, int n_d, int n_i, int n_t);
void deletion(nodeptr root_sub_trie, char key[], char corr_key[], int ki, int ci, int n_s, int n_d, int n_i, int n_t);
void insertion(nodeptr root_sub_trie, char key[], char corr_key[], int ki, int ci, int n_s, int n_d, int n_i, int n_t);

float get_penalty(int n_s, int n_d, int n_i, int n_t);
void find_with_spell_correction(nodeptr root_sub_trie, char key[], char corr_key[], int ki, int ci, int n_s, int n_d, int n_i, int n_t);
void output_top_corrections();

///////////////////////////////////////////////////////////////////////////////////////////////
// 아래는 위에서 선언된 함수의 정의부 이다. 직접 작성하거나 수정하여 완성한다.
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// 1단계: (레코드 저장) 주어진 파일의 정보를 모두 삽입하는 코드를 작성한다.
///////////////////////////////////////////////////////////////////////////////////////////////

int search_trie(char key[], nodeptr* curr, nodeptr* prev)
{
    int i = 0; *prev = NULL; *curr = root;
    if (root == NULL) return i;
    do { // search vertically by the next while stmt
        while (key[i] != '\0' && (*curr)->ch == key[i]) {
            *prev = *curr; i++; *curr = (*curr)->below;
        }
        // see if search completed with success
        if ((*curr)->ch == '\0' && key[i] == '\0') return -1;
        // search horizontally by skipping over letters before it
        while (*curr != NULL && (*curr)->ch < key[i]) {
            *prev = *curr; *curr = (*curr)->right;
        }
        // see if search has failed
        if (*curr == NULL || (*curr)->ch > key[i]) {
            return i;
        }
    } while (1);

} // search_trie

/* Fill codes */
nodeptr hang_down(char key[], nodeptr tn, int i)
{
    nodeptr newn;
    while (key[i] != '\0')
    {
        newn = (nodeptr)malloc(sizeof(nodetype));
        /*
         * Fill
         * codes
         */
        newn->ch = key[i]; // 현재 문자 설정
        newn->right = NULL; // 오른쪽 포인터는 NULL로 초기화
        newn->below = NULL; // 아래쪽 포인터도 NULL로 초기화
        i++;
        tn->below = newn; // 현재 노드의 아래쪽 포인터를 새 노드로 설정
        tn = newn; // 현재 노드를 새 노드로 업데이트
    }

    // 종료 노드 추가
    newn = (nodeptr)malloc(sizeof(nodetype));
    /*
     * Fill
     * codes
     */
    newn->ch = '\0'; // 종료 문자 설정
    newn->right = NULL; // 오른쪽 포인터는 NULL
    newn->below = NULL; // 아래쪽 포인터도 NULL
    tn->below = newn; // 마지막 노드의 아래쪽 포인터를 종료 노드로 설정

    return newn; // 마지막으로 추가된 종료 노드 반환
} // hang_down

nodeptr insert_trie(char key[50])
{
    int i;
    char c;
    nodeptr tn, curr, prev, last = NULL; // used to indicate insertion position

    // if root is NULL, this is special case of empty trie.
    if (root == NULL)
    {
        // nothing in the trie. insert the first one.
        tn = (nodeptr)malloc(sizeof(nodetype));
        tn->ch = key[0];
        root = tn;
        tn->right = NULL;
        last = hang_down(key, tn, 1);
        return last;
    }

    // insert전, search를 실행하여 탐색을 진행한다.
    // curr, prev 는 탐색이 정지한 곳을 가리킨다.
    // curr, prev 새로 넣을 노드를 넣도록 한다.
    i = search_trie(key, &curr, &prev);

    // search를 수행했을때, 삽입하고자 하는 값인 key가 TRIE에서 존재하는 것을 확인한 경우 -1을 반환한다.
    // 이때, 현재 삽입하고자 하는 값인 key를 삽입하지 않고 insert를 수행하지 않고 반환한다.
    if (i == -1)
    {
        printf("insert fails since it already exists in trie. key = %s\n", key);
        return NULL; // NULL을 반환하여 삽입 실패를 알린다.
    }

    // i != -1인경우 아래 코드를 실행한다.
    // case 0
    if (prev == NULL)
    {
        /*
         * Fill
         * codes
         */
        tn = (nodeptr)malloc(sizeof(nodetype));
        tn->ch = key[i];
        tn->right = root;
        root = tn;
        last = hang_down(key, tn, i + 1);
    }
    // case 1
    else if ((prev->below != NULL) && prev->below == curr)
    {
        /*
         * Fill
         * codes
         */
        tn = (nodeptr)malloc(sizeof(nodetype));
        tn->ch = key[i];
        prev->below = tn;
        tn->right = curr;
        last = hang_down(key, tn, i + 1);
    }
    // case 2
    else if (prev->right == curr && curr != NULL)
    {
        /*
         * Fill
         * codes
         */
        tn = (nodeptr)malloc(sizeof(nodetype));
        tn->ch = key[i];
        prev->right = tn;
        tn->right = curr;
        last = hang_down(key, tn, i + 1);
    }
    // case 3
    else if (prev->right == curr && curr == NULL)
    {
        /*
         * Fill
         * codes
         */
        tn = (nodeptr)malloc(sizeof(nodetype));
        tn->ch = key[i];
        prev->right = tn;
        tn->right = NULL;
        last = hang_down(key, tn, i + 1);
    }
    else
    {
        printf("Logic error in insertion. Control reaches the position where it can't.\n");
        c = getchar();
    }

    return last; // success of insertion. return pointer to the last node.
} // function  insert_trie.

///////////////////////////////////////////////////////////////////////////////////////////////
//  2단계: (기능 개발) TRIE를 탐색하는 코드를 작성한다.
///////////////////////////////////////////////////////////////////////////////////////////////
void substitution(nodeptr root_sub_trie, char key[], char corr_key[], int ki, int ci, int n_s, int n_d, int n_i, int n_t)
{
    nodeptr np = root_sub_trie;
    char new_corr_key[100];
    strcpy(new_corr_key, corr_key); // 새로운 교정된 문자열 공간을 만들어 지금까지의 결과를 저장한다.

    do {
        nodeptr start_ptr = np->below;
        new_corr_key[ci] = np->ch; // 현재 노드의 문자로 치환
        new_corr_key[ci + 1] = '\0';
        if (start_ptr) {
            // 재귀적으로 find_with_spell_correction 호출
            find_with_spell_correction(start_ptr, key, new_corr_key, ki + 1, ci + 1, n_s + 1, n_d, n_i, n_t);
        }
        np = np->right; // 다음 노드로 이동
    } while (np != NULL); // np가 NULL이 될 때까지 반복

    return;
} // substitution



void deletion(nodeptr root_sub_trie, char key[], char corr_key[], int ki, int ci, int n_s, int n_d, int n_i, int n_t)
{
    nodeptr np = root_sub_trie;
    char new_corr_key[100];
    strcpy(new_corr_key, corr_key); // corr_key 문자열을 복사하여 변경으로부터 보호

    do {
        nodeptr start_ptr = np->below;
        new_corr_key[ci] = np->ch; // key[ki]에 대한 올바른 문자를 복원
        new_corr_key[ci + 1] = '\0';
        if (start_ptr) {
            // 재귀적으로 find_with_spell_correction 호출
            find_with_spell_correction(start_ptr, key, new_corr_key, ki, ci + 1, n_s, n_d + 1, n_i, n_t);
        }
        np = np->right; // 다음 노드로 이동
    } while (np != NULL); // np가 NULL이 될 때까지 반복

    return;
} // deletion



void insertion(nodeptr root_sub_trie, char key[], char corr_key[], int ki, int ci, int n_s, int n_d, int n_i, int n_t)
{
    nodeptr start_ptr;
    char new_corr_key[100];
    strcpy(new_corr_key, corr_key); //   corr_key string is copied to another space to protect corr_key from later change.
    start_ptr = root_sub_trie;

    // ki+1 이후의 suffix 를 실패한 partial trie 에서 찾아 보도록 한다.
    /*
     * Fill
     * codes
     */
    if (start_ptr)
        find_with_spell_correction(start_ptr, key, new_corr_key, ki + 1, ci, n_s, n_d, n_i + 1, n_t);
    return;
} // insertion
 // insertion

void transposition(nodeptr root_sub_trie, char key[], char corr_key[], int ki, int ci, int n_s, int n_d, int n_i, int n_t)
{
    nodeptr p, s;
    int temp;
    char new_corr_key[100];
    strcpy(new_corr_key, corr_key); //   corr_key string is copied to another space to protect corr_key from later change.
    p = root_sub_trie;

    do
    {
        if (p->ch == key[ki + 1])
        {
            s = p->below;
            do
            {
                if (s->ch == key[ki]) {
                    new_corr_key[ci] = key[ki + 1];
                    new_corr_key[ci + 1] = key[ki];
                    new_corr_key[ci + 2] = '\0';

                    find_with_spell_correction(s->below, key, new_corr_key, ki + 2, ci + 2, n_s, n_d, n_i, n_t + 1);
                    break;
                }
                s = s->right;
            } while (s);
            break;
        }
        else
            p = p->right;
    } while (p);
    return;
} // transposition


float get_penalty(int n_s, int n_d, int n_i, int n_t)
{
    // 각 종류마다 오류 1개당 벌점을 곱해 합산한 값을 반환한다.
    ////////////////////////////////////////////////////////
    //  아래 코드를 완성한다
    ////////////////////////////////////////////////////////
    float w[4] = { 0, };
    w[0] = 1.1f;
    w[1] = 1.3f;
    w[2] = 1.6f;
    w[3] = 1.9f;
    return n_s * w[0] + n_d * w[1] + n_i * w[2] + n_t * w[3];
    //return n_s * /* Fill your code */ +n_d * /* Fill your code */ +n_i * /* Fill your code */ +n_t * /* Fill your code */;
}

void find_with_spell_correction(nodeptr root_sub_trie, char key[], char corr_key[], int ki, int ci, int n_s, int n_d, int n_i, int n_t)
{
    nodeptr curr = root_sub_trie;
    char new_corr_key[100], c;
    if (!curr)
        return; // impossible situation. So no result is registered.

    if (curr->ch == '\0' && key[ki] == '\0')
    { // a result found. So register the result.
        strcpy(cwords[nres].word, corr_key);
        cwords[nres].penalty = get_penalty(n_s, n_d, n_i, n_t);
        cwords[nres].ns = n_s;
        cwords[nres].nd = n_d;
        cwords[nres].ni = n_i;
        cwords[nres].nt = n_t;
        if (get_penalty(n_s, n_d, n_i, n_t) == 0)
            found_perfect_match = 1;
        nres++;
        if (nres >= 20000)
        {
            printf("too many possible corrections.\n");
            c = getchar();
        }
        return;
    }

    // First we try normal processing. So we try to consume key[ki] normally by trying matching.
    while (curr && curr->ch < key[ki])
        curr = curr->right;
    if (curr && curr->ch == key[ki])
    { // matching key[ki] is OK.
        strcpy(new_corr_key, corr_key);
        new_corr_key[ci] = key[ki];
        new_corr_key[ci + 1] = '\0';
        ////////////////////////////////////////////////////////
        //  아래 코드를 완성한다
        ////////////////////////////////////////////////////////
        find_with_spell_correction(curr->below, key, new_corr_key, ki + 1, ci + 1, n_s, n_d, n_i, n_t);
        //find_with_spell_correction(/* Fill your codes */, key, /* Fill your codes */, /* Fill your codes */, /* Fill your codes */, n_s, n_d, n_i, n_t);
    }

    // Secondly, we assume all possible errors at ki.
    if (!found_perfect_match && n_s + n_d + n_i + n_t <= 2)
    { // If 3 errors found, no more attempts will be made.
        // We assume all possible situations.
        if (ki <= strlen(key) - 1)
            substitution(root_sub_trie, key, corr_key, ki, ci, n_s, n_d, n_i, n_t);
        if (ki <= strlen(key))
            deletion(root_sub_trie, key, corr_key, ki, ci, n_s, n_d, n_i, n_t);
        if (ki <= strlen(key) - 1)
            insertion(root_sub_trie, key, corr_key, ki, ci, n_s, n_d, n_i, n_t);
        if (ki <= strlen(key) - 2) // At least we need to have two or more letters left for transposition error processing.
            transposition(root_sub_trie, key, corr_key, ki, ci, n_s, n_d, n_i, n_t);
    }
    return;
} // end find_with_spell_correction

void output_top_corrections()
{
    char printed_words[500][100];
    int i, n_printed = 0, num_show = 10;
    for (i = 0; i < nres; i++)
    {
        if (!printed(printed_words, cwords[i].word, n_printed))
        { // if not printed before, print it.
            ////////////////////////////////////////////////////////
            //  아래 코드를 완성한다
            ////////////////////////////////////////////////////////
            //printf("<%2d>: %s penalty:%7.3f, substi:%d,  delet:%d,  inser:%d,  transp:%d\n", n_printed, /* Fill your codes */, /* Fill your codes */, /* Fill your codes */, /* Fill your codes */, /* Fill your codes */, /* Fill your codes */);
            printf("<%2d>: %s penalty:%7.3f, substi:%d,  delet:%d,  inser:%d,  transp:%d\n", n_printed, cwords[i].word, cwords[i].penalty, cwords[i].ns, cwords[i].nd, cwords[i].ni, cwords[i].nt);

            strcpy(printed_words[n_printed], cwords[i].word); // record a word printed now.
            n_printed++;
            if (n_printed >= num_show)
                break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// 아래 함수는 주어진 것이며 별도로 작성 및 수정할 필요가 없다.
///////////////////////////////////////////////////////////////////////////////////////////////
void sort_results(ty_correction cwords[])
{
    int i, j, midx;
    float minval;
    ty_correction temp;
    for (i = 0; i < nres - 1; i++)
    {
        minval = cwords[i].penalty;
        midx = i;
        for (j = i + 1; j < nres; j++)
        {
            if (cwords[j].penalty < minval)
            {
                minval = cwords[j].penalty;
                midx = j;
            }
        }
        temp = cwords[i];
        cwords[i] = cwords[midx];
        cwords[midx] = temp;
    }
}
int printed(char printed_words[][100], char aword[], int total)
{
    int i, found = 0;
    for (i = 0; i < total; i++)
    {
        if (strcmp(printed_words[i], aword) == 0)
            return 1;
    }
    return 0; // not found.
}

int main()
{
    int i;
    int insertion_cnt, res, widx, wcnt;
    char c, command, word[200];

    // ki: 다음 처리할 key 의 글자 인덱스; ci: 다음에 넣을 corr_key 의 인덱스.
    int ki = 0, ci = 0;

    ty_rec record;
    nodeptr last, curr, prev;

    // file open
    FILE *fpr, *fpw, *fpr1;

    fpr = fopen("Corpus_dictionary_AP_Penn_RARE.txt", "r");
    if (!fpr)
    {
        printf("file open error of file Corpus_dictionary_AP_Penn_RARE.txt.\n");
        return 0;
    }

    fpw = fopen("records.bin", "wb");
    if (!fpw)
    {
        printf("file open error of file records.bin for writing.\n");
        return 0;
    }

    // initialize correction penalties.
    for (i = 0; i < 60; i++)
    {
        cwords[i].penalty = 0;
    }

    insertion_cnt = 0;

    while (1)
    {
        res = fscanf(fpr, "%s %d %d", word, &widx, &wcnt);
        if (res != 3)
            break;

        // prepare a record in a variable
        // if length of word > 49, too long to store in a record.
        if (strlen(word) > 49)
        {
            printf("Too long word. Skip Word = <%s> \n", word);
            continue;
        }

        strcpy(record.word, word);
        record.widx = widx;
        record.wfreq = wcnt;

        last = insert_trie(word);
        if (!last)
        {
            // printf ("word insertion error. word = %s\n", word) ;
            // c = getchar () ;
        }
        else
        {
            insertion_cnt++;
            last->bp = ftell(fpw);                   // store bypt position of the data file.
            fwrite(&record, sizeof(ty_rec), 1, fpw); // store the record in the data file.
            fflush(fpw);
        }

        if (insertion_cnt > 94000) //,insertion_cnt % 5000 == 0))
        {
            printf("insertion count = %d | \t %s\n", insertion_cnt, word);
        }
    } // while

    printf("Finish recording. \n");
    fclose(fpw);
    fclose(fpr);

    char key[50], corr_key[50];

    do
    {
        nres = 0;
        printf("\nTYPE A KEY: ");
        gets(key);
        sscanf(key, "%s", key);
        if (key[0] == '$' && key[1] == '\0')
            break;

        corr_key[0] = '\0';
        found_perfect_match = 0;
        ki = 0, ci = 0;

        find_with_spell_correction(root, key, corr_key, ki, ci, 0, 0, 0, 0);
        sort_results(cwords);
        printf("total number of corrected results = %d\n\n", nres);

        output_top_corrections();
    } while (1);

    return 0;
}
