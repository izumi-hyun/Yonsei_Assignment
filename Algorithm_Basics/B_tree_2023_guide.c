// B-tree 탐색, 삽입, 삭제 프로그램
// 입력: 2 개의 회사명 파일 - Com_names1.txt,  Com_names2.txt
//       (주의: 회사명은 중간에 space 글자를 포함한 경우도 많으니 이를 감안함.)
// 먼저 두 파일 내의 모든 회사명을 각 레코드로 하여 넣는다.
//     한 레코드의 내용:  회사명(문자열),  회사명길이(정수)
// 그 다음 명령문 실행 루프로 insertion, retrieval, deletion, all records 을 실험함.

#define	_CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define D2	2			// D2는  비트리 노드내의 최대 레코드 수.
#define D	D2/2        // capacity order 이다.
#define MAX 100			// 스택 최대 원소수.
#define false 0
#define true 1

typedef struct element{     // 레코드 정의.  회사명과 그 길이로 구성됨
	char name[100];  // 회사명
	int nleng;  // 회사명 길이
}ele;

typedef struct node *nodeptr;  // *nodeptr은 node의 형태를 갖는 포인터이다. 
typedef struct node{           // 일반 노드 
	ele rec[D2];
	nodeptr ptr[D2+1];
	int fill_cnt;
}node;

typedef struct big_node{       // 일반노드보다 레코드와 포인터가 하나씩 더 큰 빅노드 [합병시 사용] 
	ele rec[D2 + 1];
	nodeptr ptr[D2+2];
}big_node;

typedef struct two_Bn {
	ele rec[2 * D2];
	nodeptr ptr[2 * D2 + 1];
} two_Bnode;

nodeptr ROOT = NULL ;        // ROOT는 전역 변수로 사용한다.  B_tree 의 루트노드를 가리킨다.

nodeptr stack[MAX];			//스택은 전역변수로 운용함. stack의 max값은 100
int top = -1;

void push(nodeptr node);
nodeptr pop();

void insert_btree();  // file 전체를 삽입 한다.  이 함수는 insert_arec을 호출하여 작업을 수행한다.
int insert_arec(ele in_rec);          // 레코드를 하나 삽입한다. 
void insert_pair_into_curr_node(nodeptr curr, ele in_rec, nodeptr child);
big_node prepare_big_node(nodeptr curr, ele in_rec, nodeptr child);
void split(nodeptr curr, nodeptr new_ptr, big_node bnode);
nodeptr retrieve(char *, int *);              // 키값을 넣어 검색한다. [깊이와 함께 출력] 
void seq_scan_btree(nodeptr curr);               // 전체 레코드들을 출력한다. 
int B_tree_deletion(char *); // Function to delete a record with a given key.
void redistribution(nodeptr father, nodeptr l_sibling, nodeptr r_sibling, char wcase, int j); // 재분배.
int merge(nodeptr father, nodeptr curr, nodeptr r_sibling, nodeptr l_sibling); // 합병.

int total_height = 0;	// B-tree 의 전체 높이 

int main(void)	{
				
	char line[200], *res_gets ;
	ele one_rec;
	char name_s[100];
	int lleng ;
	int i, k, r;
	nodeptr tp;

	// 먼저 데이터 파일내의 모든 레코드를 B-tree 에 삽입한다.
	insert_btree();		 

	// 명령문 수행 루프를 수행한다.
	while(1){
		fflush(stdin);			// buffer clear 
		printf("\n명령을 넣으시오\n");
	    printf("insert: i 이름  / delete : d 이름  /  retrieve : r 이름  /  전체출력: p   /  종료: e  >> ");
		res_gets = gets (line); 
		if (!res_gets)
			break; // no input.

		lleng = strlen(line);
		if (lleng <= 0)
			continue; // empty line input. try again.

		i = 1;
		if (line[0] == 'E' || line[0] == 'e') { // 종료 명령.
			printf("종료명령이 들어왔음!\n\n");   return 0;
		}

		else if (line[0] == 'I' || line[0] == 'i')		{ // 레코드 한개 삽입 명령.

			// 먼저 회사명을 가져옴.
			k = 0;
			while (line[i] == ' ')	i++;
			while (i < lleng) {
				name_s[k] = line[i]; k++; i++;
			}
			name_s[k] = '\0';

			if (strlen(name_s) == 0)
				continue;

			strcpy(one_rec.name,name_s);
			one_rec.nleng = strlen(name_s);

			top = -1;  // 스택을 초기화.
			r = insert_arec(one_rec);	//레코드 하나를 넣는다.
			if (r == 0) 
				printf("삽입 실패.\n", r);
			else
				printf("삽입 성공.\n", r);

		}	else if (line[0] == 'D' || line[0] == 'd') {  // 삭제 명령.
			k = 0;
			while (line[i] == ' ')	i++;
			while (i < lleng) {
				name_s[k] = line[i]; k++; i++;
			}
			name_s[k] = '\0';

			if (strlen(name_s) == 0)
				continue;

			r = B_tree_deletion(name_s);
			if (r==0)
				printf("삭제요청 받은 키의 레코드가 존재하지 않음.\n", r);
			else
				printf("삭제 성공.\n");

		}	else if (line[0]=='R' || line[0] == 'r')	{	// 탐색 명령.
			k = 0;
			while (line[i] == ' ')	i++;
			while (i < lleng) {
				name_s[k] = line[i]; k++; i++;
			}
			name_s[k] = '\0';

			if (strlen(name_s) == 0)
				continue;
			tp = retrieve(name_s, &i);
			if (tp)
				printf("탐색성공.  Name: %s,  길이: %d\n", tp->rec[i].name, tp->rec[i].nleng);
			else
				printf("탐색요청 받은 키를 가진 레코드가 존재하지 않음.\n");

		}	else if(line[0]=='P' || line[0] == 'p') // 프린트 명령.
			seq_scan_btree(ROOT);
		else
			printf("input error \n");		
	}	//while
}	//main

/**************** stack 관련 함수들 ********************/
void push(nodeptr node){
	if ( top > MAX ) ///>>>  MAX-1 이상( >= ) 이어야 하지 않나?
    {
		printf("stack is full\n");
		return;
    }
	top++;            
    stack[top] = node; 
    
}

nodeptr pop(){
	int temp=0;
	if ( top < 0 )   
	{
		printf("stack is empty.\n");
		return 0;
	}    
	
	temp = top; ///>>> 현재 top 이 가리키는 곳에 가장 최근의 데이타가 저장되어 있다. 따라서,
	top--;               
	return stack[temp];  
}

// 한 레코드와 한 포인터로 구성된 쌍을 노드에 넣는 작업.
void insert_pair_into_curr_node(nodeptr curr, ele in_rec, nodeptr child) {
	int i, j;

	/****************
	to
	be
	filled
	*****************/
	for (i = 0; i < curr->fill_cnt; i++) {
		int cmp = strcmp(in_rec.name, curr->rec[i].name);
		if (cmp == 0) {
			return;
		}
		else if (cmp < 0) {
			for (j = curr->fill_cnt; j > i; j--) {
				curr->rec[j] = curr->rec[j - 1];
				curr->ptr[j + 1] = curr->ptr[j];
			}
			curr->rec[i] = in_rec;
			curr->ptr[i + 1] = child;
			curr->fill_cnt++;
			return;
		}
	}

	curr->rec[curr->fill_cnt] = in_rec;
	curr->ptr[curr->fill_cnt + 1] = child;
	curr->fill_cnt++;
	return;
}	//end of insert_pair_into_curr_node.

// curr 의 내용과 (in_rec, child) 쌍을 넣은 big node 를 준비하여 반환한다.
big_node prepare_big_node(nodeptr curr, ele in_rec, nodeptr child) {
	big_node bnode = { NULL, NULL };
	int i, j;

	/****************
	to
	be
	filled
	*****************/
	for (i = 0; i < D2; i++) {
		bnode.rec[i] = curr->rec[i];
		bnode.ptr[i] = curr->ptr[i];
	}
	bnode.ptr[D2] = curr->ptr[D2];

	for (i = 0; i < D2; i++) {
		int cmp = strcmp(in_rec.name, bnode.rec[i].name);
		if (cmp < 0) {
			for (j = D2 - 1; j >= i; j--) {
				bnode.rec[j + 1] = bnode.rec[j];
				bnode.ptr[j + 2] = bnode.ptr[j + 1];
			}
			bnode.rec[i] = in_rec;
			bnode.ptr[i + 1] = child;
			return bnode;
		}
	}

	bnode.rec[D2] = in_rec;
	bnode.ptr[D2 + 1] = child;
	return bnode;
	return bnode;
} // end of prepare_big_node

// big 노드의 내용을 가운데 레코드를 기준으로 좌측은 curr 에, 우측은 new node  에 나누어 넣는다.
void split(nodeptr curr, nodeptr new_ptr, big_node bnode) {
	int i;
	// 가운데 이전부분이 curr 의 내용이 되게 한다.

	/****************
	to
	be
	filled
	*****************/

	// big node 의 가운데 레코드 뒤를 new node 에 저장 

	/****************
	to
	be
	filled
	*****************/
	for (i = 0; i < D; i++) {
		curr->rec[i] = bnode.rec[i];
		curr->ptr[i] = bnode.ptr[i];
	}
	curr->ptr[D] = bnode.ptr[D]; // Leftmost pointer of new_ptr

	for (i = 0; i < D2 - D; i++) {
		new_ptr->rec[i] = bnode.rec[i + D + 1];
		new_ptr->ptr[i] = bnode.ptr[i + D + 1];
	}
	new_ptr->ptr[i] = bnode.ptr[D2 + 1]; // Rightmost pointer of new_ptr

	// Update fill counts for curr and new_ptr
	curr->fill_cnt = D;
	new_ptr->fill_cnt = D2 - D;

	return;
} // split

/*******************************************************/
// 레코드 하나를 삽입하는 함수이다.
// 반환값: 0: 삽입실패,  1, 2: 삽입성공 (1: 층증가 없이, 2:한 층 더 늘어 남.)

int insert_arec(ele in_rec){	//하나의 레코드를 삽입  key = 회사명  

	int i,j;
	nodeptr curr, child, new_ptr, tptr=NULL;
	ele empty = {"\0",0};
	big_node bnode;
	
	if(!ROOT){		//ROOT가 NULL이면 btree가 비어있음. 맨 첫 노드를 만들어 여기에 넣는다.
		ROOT = (nodeptr) malloc(sizeof(node));	//nodeptr형태로 node크기만큼 할당받아 시작주소는 ROOT가 가짐   
		ROOT -> rec[0] = in_rec;				//key값을 ROOT->rec[0]에 넣는다. 
		ROOT -> ptr[0] = ROOT -> ptr[1] = NULL; //p0과 p1에 NULL을 넣는다. 
		ROOT -> fill_cnt = 1; 
		return 1;  // 첫 노드를 만들어 넣고 종료함. 
	}

    //ROOT가 null이 아닐 경우 이곳으로.
	curr = ROOT;								//ROOT의 값을 curr에 넣는다. 
	
	do{  	
		for(i = 0; i< curr->fill_cnt ; i++) {     //curr의 필카운트가 i보다 작을 때 까지 i를 증가  
			if (strcmp(in_rec.name, curr->rec[i].name) < 0) // (in_rec.name < curr->rec[i].name)
				break; 
			else if (strcmp(in_rec.name, curr->rec[i].name) == 0) //동일 키의 레코드가 이미 존재하여 안 넣음.
			{	printf("동일키 이미 존재로 안 넣음: %s\n", in_rec.name); 
				return 0;
			}
		}
		child = curr ->ptr[i];  
		if(child)		
		{	push(curr);
			curr = child;  
		}
		else 
			break;
	}while(1); 

	do { 
		// The pair <in_rec, child>  를 curr 에 넣어야 한다. (주: in_rec, child 는 값이 채워져서 여기로 온다.)

		if (curr->fill_cnt < D2)	{	// curr 노드에 빈 자리가 하나 이상 있으므로 이 curr 노드에 넣는다.

			insert_pair_into_curr_node(curr, in_rec, child); 
			return 1; // 삽입성공 (종류 1: 루트의 추가 없이 가능함).
		}
		else {	//  curr 노드가 full 임. 즉 빈자리가 없음.  빅노드 이용하여 split and elevation 수행.

			// big 노드에 curr 의 내용과 넣을 쌍을 순서에 맞게 넣는다.
			bnode = prepare_big_node(curr, in_rec, child);

			// big 노드를 가운데 레코드 이전, 가운데 레코드, 가운데 레코드 이후의 3 부분으로 나눈다.
			
			// 가운데 레코드 뒤를 넣을  new node 를 할당한다.
			new_ptr = (nodeptr) malloc(sizeof(node)); 

			// split 작업을 수행한다: 가운데의 좌측은 curr 노드에, 가운데의 우측은 new node 에 넣는다.			
			split(curr, new_ptr, bnode);

			// 부모로 elevation할 (올릴) 쌍을 준비한다.
			in_rec = bnode.rec[D]; // 올릴 레코드
			child = new_ptr;       // 올릴 포인터
			
			// elevation 을 수행한다.
			if(top >=0){  // curr 가 부모가 있는 경우.
				curr = pop();  // curr 의 부모로 curr를 변경함. 올려보낼 쌍은 위에서 in_rec, child 에 준비하였음.
				// 부모에 넣는 (즉 elevation 하는) 작업은 do 의 앞부분에서 수행할 것임.
			}
			else{ // 스택이 empty 이면 curr 의 부모가 없음. 즉 curr 는 ROOT 노드임.
				// 새 ROOT 노드를 만들어 curr 의 부모로 하고 여기에 (in_rec, child) 쌍을 elevation 시킴.
				tptr = (nodeptr)malloc(sizeof(node));
				tptr->rec[0] = in_rec;
				tptr->ptr[0] = ROOT;
				tptr->ptr[1] = child;
				tptr->fill_cnt = 1;
				ROOT = tptr;  // ROOT 를 새로 할당한 노드로 변경함
				total_height++;
				return 2; // 삽입 성공 (종류 2: 새 루트가 생김)
 			} // else.

		} // else.
	}while (1);

	return 0; 
} //함수 insert_arec

void insert_btree(){							//파일전체의 레코드를 삽입 ->insert_arec 을 호출 
	FILE *fp;	
	ele data;
	char line[200];
	char* ret_fgets;
	int r;
	int n=0, lineleng;
	int check,count=0;
	fp = fopen("Com_names1.txt","r");
	if(fp==NULL){
		printf("Cannot open this file :  Com_names1.txt\n");
		scanf("%d",&check);
	} //if 
	
	ROOT = NULL;
	while(1){

		ret_fgets = fgets(line, 200, fp);
		if (!ret_fgets)
			break;  // 파일 모두 읽음.

		lineleng = strlen(line);  // line 의 마지막 글자는 newline 글자임.
		if (lineleng - 1  >= 100 )
			continue; // 회사명이 너무 길어서 무시
		line[lineleng - 1] = '\0'; //  마지막 newline 글자 제거.

		strcpy(data.name, line);  // 삽입할 레코드 준비.
		data.nleng = strlen(line);

		top = -1;  // 스택의 빈상태로 초기화.
		r = insert_arec(data);   // 한 레코드를 비트리에 삽입한다.
		if (r != 0) 
			count++; // 삽입성공 카운트 증가.
	} //while 

	fp = fopen("Com_names2.txt", "r");
	if (fp == NULL) {
		printf("Cannot open this file :  Com_names2.txt\n");
		scanf("%d", &check);
	} //if 

	while (1) {
		ret_fgets = fgets(line, 200, fp);
		if (!ret_fgets)
			break;  // 파일 모두 읽음.

		lineleng = strlen(line);  // line 의 마지막 글자는 newline 글자임.
		if (lineleng - 1 >= 100)
			continue; // 회사명이 너무 길어서 무시
		line[lineleng - 1] = '\0'; //  마지막 newline 글자 제거.

		strcpy(data.name, line);  // 삽입할 레코드 준비.
		data.nleng = strlen(line);

		top = -1;  // 스택의 빈상태로 초기화.
		r = insert_arec(data);   // 한 레코드를 비트리에 삽입한다.
		if (r != 0)
			count++; // 삽입성공 카운트 증가.

	} //while 


	printf("삽입 성공한 레코드 수 = %d \n\n",count);
	fclose(fp);
}   //  함수 insert_btree


nodeptr  retrieve(char *skey, int *idx_found ) {	//검색 함수 
	nodeptr curr = ROOT;
	nodeptr P;
	int i;
	do {
		for (i = 0; i < curr->fill_cnt; i++) {
			if (strcmp(skey, curr->rec[i].name) < 0) 
				break;
			else if (strcmp(skey, curr->rec[i].name) == 0) {
				*idx_found = i;
				return curr;
			}
			else
				; // do next i.
		} // for i= 
		P = curr->ptr[i];
		if (P) {
			curr = P;
		}
	} while (P);

	return NULL;

}//retrieve

void seq_scan_btree(nodeptr curr){
	int check_stack=0;
	int i;
	int n;
	if(curr) {
		n= curr->fill_cnt;
		for(i=0;i<n;i++) {
			seq_scan_btree(curr->ptr[i]);
			printf("Name : %s\n",curr->rec[i].name );
		}
		seq_scan_btree(curr->ptr[i]);
		return;
	} 
	else if(!curr)
		return;

} //seq_scan_btree

// 좌측형제노드 내용, 부모의 레코드, 우측형제 내용을 받아서 재분배를 하는 함수.
// 매개변수 설명:
//      father: 부모노드, l_sibling, r_sibling: 재분배할 두 형제 노드들,
//      wcase: curr 가 좌측형제와 재분배이면 ‘L’, curr 가 우측형제와 재분배이면 ‘R’.
//      j :  father 안에서 curr를 가리키는 포인터의 인덱스임.
void redistribution(nodeptr father, nodeptr l_sibling, nodeptr r_sibling, char wcase, int j) {
	int i, k, m, n, h;
	two_Bnode twoB; //  twobnode(bnode의 2배의 공간)

	if (wcase == 'L')
		j = j - 1; // father에서 l_sibing 과 curr(=r_sibling) 사이의 레코드의 index.
	else if (wcase == 'R')
		j = j; // father에서 curr(=l_sibling) 과 r_sibling 사이의 레코드의 index.

	//copy l_sibling's content, intermediate key in father, r_sibling's content to twobnode;

	// 주의: j 에 father 내에서 자식 l_sibling 에 대한 포인터의 index 가 들어 있음.
	// 주의: j에 father 내에서 자식 l_sibling에 대한 포인터의 index가 들어 있음.
	i = 0;
	twoB.rec[i] = father->rec[j]; // 부모에서의 중간키를 가져옴.
	i++; // 다음 레코드 번호가 되게 함.

	for (k = 0; k < l_sibling->fill_cnt; k++, i++) {
		twoB.ptr[i] = l_sibling->ptr[k]; twoB.rec[i] = l_sibling->rec[k];
	}

	twoB.ptr[i] = l_sibling->ptr[k]; // i는 twoB 노드의 레코드 수와 같음.

	for (k = j + 1; k < father->fill_cnt; k++, i++) {
		twoB.ptr[i] = father->ptr[k];
		twoB.rec[i] = father->rec[k - 1];
	}

	twoB.ptr[i] = father->ptr[k];

	for (k = 0; k < r_sibling->fill_cnt; k++, i++) {
		twoB.ptr[i] = r_sibling->ptr[k];
		twoB.rec[i] = r_sibling->rec[k];
	}
	//Split twobnode into first half, middle record, second half;
	h = i / 2;  // h is the index of middle record.

	//copy first half to left node;
	for (m = 0; m < h; m++) {
		l_sibling->ptr[m] = twoB.ptr[m];
		l_sibling->rec[m] = twoB.rec[m];
	}

	l_sibling->ptr[m] = twoB.ptr[m];
	l_sibling->fill_cnt = h;

	// copy second half to r_sibling;
	n = 0;
	n++;
	for (m = 0; m < (i - h - 1); m++, n++) {
		r_sibling->ptr[m] = twoB.ptr[n]; r_sibling->rec[m] = twoB.rec[n];
	}

	r_sibling->ptr[m] = twoB.ptr[n]; r_sibling->fill_cnt = i - h - 1;

	r_sibling->ptr[n] = twoB.ptr[m];
	r_sibling->fill_cnt = i - h - 1;

	//move the middle record to father ;
	father->rec[j] = twoB.rec[h];
	father->ptr[j + 1] = r_sibling;
	return;
} // end of redistribution

// curr 노드를 우측형제 또는 좌측형제와 합병하게 한다.
// 합병은 부모의 중간레코드 및 우측형제의 내용을 좌측형제로 보내서 달성한다.
// 반환값:  father 에서 삭제될 레코드의 위치.
int merge(nodeptr father, nodeptr curr, nodeptr r_sibling, nodeptr l_sibling) {
	int i, j, k;
	nodeptr leftptr, rightptr;

	////  leftptr :  합병되는 두 형제 중 좌측 형제를 가리키게 함.
	////  rightptr : 합병되는 두 형제 중 우측 형제를 가리키게 함.
	if (r_sibling) {  // 우측형제가 존재하므로 이 형제와 합병한다.
		leftptr = curr;
		rightptr = r_sibling;
	}
	else { // 우측이 안되므로 좌측형제와 합병한다.
		leftptr = l_sibling;
		rightptr = curr;
	}

	for (i = leftptr->fill_cnt, k = 0; k < rightptr->fill_cnt; i++, k++) {
		leftptr->rec[i] = rightptr->rec[k];
		leftptr->ptr[i + 1] = rightptr->ptr[k + 1];
	}

	leftptr->fill_cnt += rightptr->fill_cnt;

	for (j = 0; j <= father->fill_cnt; j++) {
		if (father->ptr[i] == rightptr) {
			free(rightptr); // 우측 형제를 해제합니다.
			break; // 레코드를 찾았으므로 루프 종료
		}
	}
	if (father != NULL && father->fill_cnt == 0) {
		free(rightptr);
	}

	//free(rightptr); // 우측형제는 제거한다.
	return i; // 두 형제를 가리키는 포인터 사이에 있는 부모 내의 레코드의 위치를 반환.
}  // end of merge

// 삭제를 수행하는 함수. 삭제할 회사명(문자열)에 대한 포인터를 매개변수 out_key 로 받는다.
int B_tree_deletion(char *out_key) {
	nodeptr  curr, r_sibling, l_sibling, father, Pt, leftptr, rightptr ;
	int i, j, k, r_OK, l_OK, found ;
	curr = ROOT;

	// Step (0): search for a record (to be deleted) whose key equals out_key.
	found = 0;
	do {
		for (i = 0; i < curr->fill_cnt; i++)
			if (strcmp(out_key, curr->rec[i].name) < 0) 
				break;
			else if (strcmp(out_key, curr->rec[i].name) == 0) {
				found = 1;
				break; 
			}
		if (found == 1) 
			break;  //주의: 변수 i에 찾은 위치가 들어 있음.
		else {  // curr 에 없다. child 로 내려 가야 한다.
			Pt = curr->ptr[i];
			if (Pt) { 
				push(curr); 
				curr = Pt; // curr 를 자식으로 갱신하므로써 내려감을 달성함.
			} 
			else 
				return 0; // 내려 갈 자식이 없다. 지울 키를 가진 레코드가 존재 않는다.
		}
	} while (1);

	// Comes here when the key is found. d_rec (record to be deleted) is the record i in node curr.
	// Step (1):  find successor of d_rec.
	if (curr->ptr[0]) {   // curr node is a nonleaf node  인 경우.
		// We need to find successor of the record to be deleted ;
		Pt = curr->ptr[i + 1];
		push(curr);
		// 가장 왼쪽 포인터를 따라내려 간다.
		while (Pt->ptr[0]) {
			push(Pt);
			Pt = Pt->ptr[0];
		}

		// Pt 는 리프노드를 가리킨다. 
		// Pt 가 succ 노드이다. 이 노드의 첫 레코드가 d_rec 의 successor 레코드이다.
		curr->rec[i] = Pt->rec[0]; // successor 로 d_rec 를 덮어쓴다. 즉 d_rec 이 제거된다.

		// 이젠 Pt 의 첫 레코드 (즉 위치 0의 레코드)를 지우는 문제가 된다.
		curr = Pt; // curr 로 하여금 삭제할 레코드가 있는 노드를 가리키게 한다.
		i = 0; // 지울 successor 레코드의  index 를 변수 i 가 가지고 있게 함.
	} //end if

	// 여기에 오면 curr 노드에서 index 가 i 인 레코드와 그 우측 포인터를 삭제하여야 한다.
	do {
		// Step (2):
		// curr 노드에서 위치 i 의 레코드와 그 바로 우측의 포인터를 삭제하는 작업을 한다. 
		for (j = i + 1; j < curr->fill_cnt; j++) { // i 보다 우측의 것들을 한 칸씩 좌측으로 이동.
			curr->rec[j - 1] = curr->rec[j]; 
			curr->ptr[j] = curr->ptr[j + 1];
		}
		curr->fill_cnt = curr->fill_cnt - 1;

		// Step (3):
		if (curr == ROOT) {  // 루트노드에서 삭제가 일어 났다.
			if (curr->fill_cnt == 0) {  // 루트노드가 비어있다. 따라서 기존 루트노드를 제거한다.
				ROOT = ROOT->ptr[0]; 
				free(curr); 
			}
			return 1; // deletion succeeded.
		}

		// 	여기에 도달하면 curr 는 ROOT 가 아닌 경우이다.
		// Step (4):
		if (curr->fill_cnt >= D)  // 지운 후에도 반 이상이 채워진 상황임.
			return 2;  // 삭제 성공으로 알고리즘 종료한다.

		// 여기 도달하면 curr 가 반이상 채워지지 않은 상황이다..
		// Step (5):
		father = pop(); // curr 의 부모노드를 father 가 가리키게 한다..

		// r-sibling : curr 의 우측형제에 대한 포인터를 가지게 함. (없으면 NULL 을 가지게 함.)
		// l-sibling : curr 의 좌측형제에 대한 포인터를 가지게 함. (없으면 NULL 을 가지게 함.)
		
		for (j = 0; j <= father->fill_cnt; j++) 
			if (father->ptr[j] == curr) // find ptr of father which goes down to curr.
				break; 
		if (j >= 1) 
			l_sibling = father->ptr[j - 1]; 
		else 
			l_sibling = NULL;

		if (j < father->fill_cnt) 
			r_sibling = father->ptr[j + 1]; 
		else 
			r_sibling = NULL;

		// 주의: father 의 ptr[j] 가 curr 과 같음.
		//  r_sibling or l_sibling  중 하나가 D 보다 많은 레코드를 가지면 재분배가 가능하다.
		r_OK = 0;  l_OK = 0;
		if (r_sibling && r_sibling->fill_cnt > D) 
			r_OK = 1; // 우측 형제가 재분배 해 줄 수 있음.
		else if (l_sibling && l_sibling->fill_cnt > D) 
			l_OK = 1; // 좌측형제가 재분해 해 줄 수 있음.

		if (r_OK || l_OK) { // 우측 또는 좌측 형제가 재분배 해 줄 수 있음.
			if (r_OK) {  // 우측형제가 가능하면 먼저 우측형제로부터 재분배를 받는다.
				redistribution(father, curr, r_sibling, 'R', j);
			}
			else if (l_OK) {  // 우측형제가 안되므로 좌측 형제로부터 재분배를 받는다.
				redistribution(father, l_sibling, curr, 'L', j);
			}
			printf("Redistribution has been done.\n");
			return 3; // Deletion succeeded with redistribution.
		}
		else {   //  step 6. 재분배가 불가능한 상황이다.  합병이 필요하다.

			i = merge(father, curr, r_sibling, l_sibling); // i 는 father 의 삭제될 레코드 위치
			printf("Merging has been done.\n");

			curr = father;
			// 이 시점에서 변수 i 는 이 새로운 curr 노드에서 삭제될 레코드의 위치를 가지고 있음.. 
			// 실제 삭제는 이 루프의 시작 부분에서 수행할 것임.
		} // else.
	} while (1);  // end of do-while 문.

} // B_tree_deletion
