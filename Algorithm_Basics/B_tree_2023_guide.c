// B-tree Ž��, ����, ���� ���α׷�
// �Է�: 2 ���� ȸ��� ���� - Com_names1.txt,  Com_names2.txt
//       (����: ȸ����� �߰��� space ���ڸ� ������ ��쵵 ������ �̸� ������.)
// ���� �� ���� ���� ��� ȸ����� �� ���ڵ�� �Ͽ� �ִ´�.
//     �� ���ڵ��� ����:  ȸ���(���ڿ�),  ȸ������(����)
// �� ���� ��ɹ� ���� ������ insertion, retrieval, deletion, all records �� ������.

#define	_CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define D2	2			// D2��  ��Ʈ�� ��峻�� �ִ� ���ڵ� ��.
#define D	D2/2        // capacity order �̴�.
#define MAX 100			// ���� �ִ� ���Ҽ�.
#define false 0
#define true 1

typedef struct element{     // ���ڵ� ����.  ȸ���� �� ���̷� ������
	char name[100];  // ȸ���
	int nleng;  // ȸ��� ����
}ele;

typedef struct node *nodeptr;  // *nodeptr�� node�� ���¸� ���� �������̴�. 
typedef struct node{           // �Ϲ� ��� 
	ele rec[D2];
	nodeptr ptr[D2+1];
	int fill_cnt;
}node;

typedef struct big_node{       // �Ϲݳ�庸�� ���ڵ�� �����Ͱ� �ϳ��� �� ū ���� [�պ��� ���] 
	ele rec[D2 + 1];
	nodeptr ptr[D2+2];
}big_node;

typedef struct two_Bn {
	ele rec[2 * D2];
	nodeptr ptr[2 * D2 + 1];
} two_Bnode;

nodeptr ROOT = NULL ;        // ROOT�� ���� ������ ����Ѵ�.  B_tree �� ��Ʈ��带 ����Ų��.

nodeptr stack[MAX];			//������ ���������� �����. stack�� max���� 100
int top = -1;

void push(nodeptr node);
nodeptr pop();

void insert_btree();  // file ��ü�� ���� �Ѵ�.  �� �Լ��� insert_arec�� ȣ���Ͽ� �۾��� �����Ѵ�.
int insert_arec(ele in_rec);          // ���ڵ带 �ϳ� �����Ѵ�. 
void insert_pair_into_curr_node(nodeptr curr, ele in_rec, nodeptr child);
big_node prepare_big_node(nodeptr curr, ele in_rec, nodeptr child);
void split(nodeptr curr, nodeptr new_ptr, big_node bnode);
nodeptr retrieve(char *, int *);              // Ű���� �־� �˻��Ѵ�. [���̿� �Բ� ���] 
void seq_scan_btree(nodeptr curr);               // ��ü ���ڵ���� ����Ѵ�. 
int B_tree_deletion(char *); // Function to delete a record with a given key.
void redistribution(nodeptr father, nodeptr l_sibling, nodeptr r_sibling, char wcase, int j); // ��й�.
int merge(nodeptr father, nodeptr curr, nodeptr r_sibling, nodeptr l_sibling); // �պ�.

int total_height = 0;	// B-tree �� ��ü ���� 

int main(void)	{
				
	char line[200], *res_gets ;
	ele one_rec;
	char name_s[100];
	int lleng ;
	int i, k, r;
	nodeptr tp;

	// ���� ������ ���ϳ��� ��� ���ڵ带 B-tree �� �����Ѵ�.
	insert_btree();		 

	// ��ɹ� ���� ������ �����Ѵ�.
	while(1){
		fflush(stdin);			// buffer clear 
		printf("\n����� �����ÿ�\n");
	    printf("insert: i �̸�  / delete : d �̸�  /  retrieve : r �̸�  /  ��ü���: p   /  ����: e  >> ");
		res_gets = gets (line); 
		if (!res_gets)
			break; // no input.

		lleng = strlen(line);
		if (lleng <= 0)
			continue; // empty line input. try again.

		i = 1;
		if (line[0] == 'E' || line[0] == 'e') { // ���� ���.
			printf("�������� ������!\n\n");   return 0;
		}

		else if (line[0] == 'I' || line[0] == 'i')		{ // ���ڵ� �Ѱ� ���� ���.

			// ���� ȸ����� ������.
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

			top = -1;  // ������ �ʱ�ȭ.
			r = insert_arec(one_rec);	//���ڵ� �ϳ��� �ִ´�.
			if (r == 0) 
				printf("���� ����.\n", r);
			else
				printf("���� ����.\n", r);

		}	else if (line[0] == 'D' || line[0] == 'd') {  // ���� ���.
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
				printf("������û ���� Ű�� ���ڵ尡 �������� ����.\n", r);
			else
				printf("���� ����.\n");

		}	else if (line[0]=='R' || line[0] == 'r')	{	// Ž�� ���.
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
				printf("Ž������.  Name: %s,  ����: %d\n", tp->rec[i].name, tp->rec[i].nleng);
			else
				printf("Ž����û ���� Ű�� ���� ���ڵ尡 �������� ����.\n");

		}	else if(line[0]=='P' || line[0] == 'p') // ����Ʈ ���.
			seq_scan_btree(ROOT);
		else
			printf("input error \n");		
	}	//while
}	//main

/**************** stack ���� �Լ��� ********************/
void push(nodeptr node){
	if ( top > MAX ) ///>>>  MAX-1 �̻�( >= ) �̾�� ���� �ʳ�?
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
	
	temp = top; ///>>> ���� top �� ����Ű�� ���� ���� �ֱ��� ����Ÿ�� ����Ǿ� �ִ�. ����,
	top--;               
	return stack[temp];  
}

// �� ���ڵ�� �� �����ͷ� ������ ���� ��忡 �ִ� �۾�.
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

// curr �� ����� (in_rec, child) ���� ���� big node �� �غ��Ͽ� ��ȯ�Ѵ�.
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

// big ����� ������ ��� ���ڵ带 �������� ������ curr ��, ������ new node  �� ������ �ִ´�.
void split(nodeptr curr, nodeptr new_ptr, big_node bnode) {
	int i;
	// ��� �����κ��� curr �� ������ �ǰ� �Ѵ�.

	/****************
	to
	be
	filled
	*****************/

	// big node �� ��� ���ڵ� �ڸ� new node �� ���� 

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
// ���ڵ� �ϳ��� �����ϴ� �Լ��̴�.
// ��ȯ��: 0: ���Խ���,  1, 2: ���Լ��� (1: ������ ����, 2:�� �� �� �þ� ��.)

int insert_arec(ele in_rec){	//�ϳ��� ���ڵ带 ����  key = ȸ���  

	int i,j;
	nodeptr curr, child, new_ptr, tptr=NULL;
	ele empty = {"\0",0};
	big_node bnode;
	
	if(!ROOT){		//ROOT�� NULL�̸� btree�� �������. �� ù ��带 ����� ���⿡ �ִ´�.
		ROOT = (nodeptr) malloc(sizeof(node));	//nodeptr���·� nodeũ�⸸ŭ �Ҵ�޾� �����ּҴ� ROOT�� ����   
		ROOT -> rec[0] = in_rec;				//key���� ROOT->rec[0]�� �ִ´�. 
		ROOT -> ptr[0] = ROOT -> ptr[1] = NULL; //p0�� p1�� NULL�� �ִ´�. 
		ROOT -> fill_cnt = 1; 
		return 1;  // ù ��带 ����� �ְ� ������. 
	}

    //ROOT�� null�� �ƴ� ��� �̰�����.
	curr = ROOT;								//ROOT�� ���� curr�� �ִ´�. 
	
	do{  	
		for(i = 0; i< curr->fill_cnt ; i++) {     //curr�� ��ī��Ʈ�� i���� ���� �� ���� i�� ����  
			if (strcmp(in_rec.name, curr->rec[i].name) < 0) // (in_rec.name < curr->rec[i].name)
				break; 
			else if (strcmp(in_rec.name, curr->rec[i].name) == 0) //���� Ű�� ���ڵ尡 �̹� �����Ͽ� �� ����.
			{	printf("����Ű �̹� ����� �� ����: %s\n", in_rec.name); 
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
		// The pair <in_rec, child>  �� curr �� �־�� �Ѵ�. (��: in_rec, child �� ���� ä������ ����� �´�.)

		if (curr->fill_cnt < D2)	{	// curr ��忡 �� �ڸ��� �ϳ� �̻� �����Ƿ� �� curr ��忡 �ִ´�.

			insert_pair_into_curr_node(curr, in_rec, child); 
			return 1; // ���Լ��� (���� 1: ��Ʈ�� �߰� ���� ������).
		}
		else {	//  curr ��尡 full ��. �� ���ڸ��� ����.  ���� �̿��Ͽ� split and elevation ����.

			// big ��忡 curr �� ����� ���� ���� ������ �°� �ִ´�.
			bnode = prepare_big_node(curr, in_rec, child);

			// big ��带 ��� ���ڵ� ����, ��� ���ڵ�, ��� ���ڵ� ������ 3 �κ����� ������.
			
			// ��� ���ڵ� �ڸ� ����  new node �� �Ҵ��Ѵ�.
			new_ptr = (nodeptr) malloc(sizeof(node)); 

			// split �۾��� �����Ѵ�: ����� ������ curr ��忡, ����� ������ new node �� �ִ´�.			
			split(curr, new_ptr, bnode);

			// �θ�� elevation�� (�ø�) ���� �غ��Ѵ�.
			in_rec = bnode.rec[D]; // �ø� ���ڵ�
			child = new_ptr;       // �ø� ������
			
			// elevation �� �����Ѵ�.
			if(top >=0){  // curr �� �θ� �ִ� ���.
				curr = pop();  // curr �� �θ�� curr�� ������. �÷����� ���� ������ in_rec, child �� �غ��Ͽ���.
				// �θ� �ִ� (�� elevation �ϴ�) �۾��� do �� �պκп��� ������ ����.
			}
			else{ // ������ empty �̸� curr �� �θ� ����. �� curr �� ROOT �����.
				// �� ROOT ��带 ����� curr �� �θ�� �ϰ� ���⿡ (in_rec, child) ���� elevation ��Ŵ.
				tptr = (nodeptr)malloc(sizeof(node));
				tptr->rec[0] = in_rec;
				tptr->ptr[0] = ROOT;
				tptr->ptr[1] = child;
				tptr->fill_cnt = 1;
				ROOT = tptr;  // ROOT �� ���� �Ҵ��� ���� ������
				total_height++;
				return 2; // ���� ���� (���� 2: �� ��Ʈ�� ����)
 			} // else.

		} // else.
	}while (1);

	return 0; 
} //�Լ� insert_arec

void insert_btree(){							//������ü�� ���ڵ带 ���� ->insert_arec �� ȣ�� 
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
			break;  // ���� ��� ����.

		lineleng = strlen(line);  // line �� ������ ���ڴ� newline ������.
		if (lineleng - 1  >= 100 )
			continue; // ȸ����� �ʹ� �� ����
		line[lineleng - 1] = '\0'; //  ������ newline ���� ����.

		strcpy(data.name, line);  // ������ ���ڵ� �غ�.
		data.nleng = strlen(line);

		top = -1;  // ������ ����·� �ʱ�ȭ.
		r = insert_arec(data);   // �� ���ڵ带 ��Ʈ���� �����Ѵ�.
		if (r != 0) 
			count++; // ���Լ��� ī��Ʈ ����.
	} //while 

	fp = fopen("Com_names2.txt", "r");
	if (fp == NULL) {
		printf("Cannot open this file :  Com_names2.txt\n");
		scanf("%d", &check);
	} //if 

	while (1) {
		ret_fgets = fgets(line, 200, fp);
		if (!ret_fgets)
			break;  // ���� ��� ����.

		lineleng = strlen(line);  // line �� ������ ���ڴ� newline ������.
		if (lineleng - 1 >= 100)
			continue; // ȸ����� �ʹ� �� ����
		line[lineleng - 1] = '\0'; //  ������ newline ���� ����.

		strcpy(data.name, line);  // ������ ���ڵ� �غ�.
		data.nleng = strlen(line);

		top = -1;  // ������ ����·� �ʱ�ȭ.
		r = insert_arec(data);   // �� ���ڵ带 ��Ʈ���� �����Ѵ�.
		if (r != 0)
			count++; // ���Լ��� ī��Ʈ ����.

	} //while 


	printf("���� ������ ���ڵ� �� = %d \n\n",count);
	fclose(fp);
}   //  �Լ� insert_btree


nodeptr  retrieve(char *skey, int *idx_found ) {	//�˻� �Լ� 
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

// ����������� ����, �θ��� ���ڵ�, �������� ������ �޾Ƽ� ��й踦 �ϴ� �Լ�.
// �Ű����� ����:
//      father: �θ���, l_sibling, r_sibling: ��й��� �� ���� ����,
//      wcase: curr �� ���������� ��й��̸� ��L��, curr �� ���������� ��й��̸� ��R��.
//      j :  father �ȿ��� curr�� ����Ű�� �������� �ε�����.
void redistribution(nodeptr father, nodeptr l_sibling, nodeptr r_sibling, char wcase, int j) {
	int i, k, m, n, h;
	two_Bnode twoB; //  twobnode(bnode�� 2���� ����)

	if (wcase == 'L')
		j = j - 1; // father���� l_sibing �� curr(=r_sibling) ������ ���ڵ��� index.
	else if (wcase == 'R')
		j = j; // father���� curr(=l_sibling) �� r_sibling ������ ���ڵ��� index.

	//copy l_sibling's content, intermediate key in father, r_sibling's content to twobnode;

	// ����: j �� father ������ �ڽ� l_sibling �� ���� �������� index �� ��� ����.
	// ����: j�� father ������ �ڽ� l_sibling�� ���� �������� index�� ��� ����.
	i = 0;
	twoB.rec[i] = father->rec[j]; // �θ𿡼��� �߰�Ű�� ������.
	i++; // ���� ���ڵ� ��ȣ�� �ǰ� ��.

	for (k = 0; k < l_sibling->fill_cnt; k++, i++) {
		twoB.ptr[i] = l_sibling->ptr[k]; twoB.rec[i] = l_sibling->rec[k];
	}

	twoB.ptr[i] = l_sibling->ptr[k]; // i�� twoB ����� ���ڵ� ���� ����.

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

// curr ��带 �������� �Ǵ� ���������� �պ��ϰ� �Ѵ�.
// �պ��� �θ��� �߰����ڵ� �� ���������� ������ ���������� ������ �޼��Ѵ�.
// ��ȯ��:  father ���� ������ ���ڵ��� ��ġ.
int merge(nodeptr father, nodeptr curr, nodeptr r_sibling, nodeptr l_sibling) {
	int i, j, k;
	nodeptr leftptr, rightptr;

	////  leftptr :  �պ��Ǵ� �� ���� �� ���� ������ ����Ű�� ��.
	////  rightptr : �պ��Ǵ� �� ���� �� ���� ������ ����Ű�� ��.
	if (r_sibling) {  // ���������� �����ϹǷ� �� ������ �պ��Ѵ�.
		leftptr = curr;
		rightptr = r_sibling;
	}
	else { // ������ �ȵǹǷ� ���������� �պ��Ѵ�.
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
			free(rightptr); // ���� ������ �����մϴ�.
			break; // ���ڵ带 ã�����Ƿ� ���� ����
		}
	}
	if (father != NULL && father->fill_cnt == 0) {
		free(rightptr);
	}

	//free(rightptr); // ���������� �����Ѵ�.
	return i; // �� ������ ����Ű�� ������ ���̿� �ִ� �θ� ���� ���ڵ��� ��ġ�� ��ȯ.
}  // end of merge

// ������ �����ϴ� �Լ�. ������ ȸ���(���ڿ�)�� ���� �����͸� �Ű����� out_key �� �޴´�.
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
			break;  //����: ���� i�� ã�� ��ġ�� ��� ����.
		else {  // curr �� ����. child �� ���� ���� �Ѵ�.
			Pt = curr->ptr[i];
			if (Pt) { 
				push(curr); 
				curr = Pt; // curr �� �ڽ����� �����ϹǷν� �������� �޼���.
			} 
			else 
				return 0; // ���� �� �ڽ��� ����. ���� Ű�� ���� ���ڵ尡 ���� �ʴ´�.
		}
	} while (1);

	// Comes here when the key is found. d_rec (record to be deleted) is the record i in node curr.
	// Step (1):  find successor of d_rec.
	if (curr->ptr[0]) {   // curr node is a nonleaf node  �� ���.
		// We need to find successor of the record to be deleted ;
		Pt = curr->ptr[i + 1];
		push(curr);
		// ���� ���� �����͸� ���󳻷� ����.
		while (Pt->ptr[0]) {
			push(Pt);
			Pt = Pt->ptr[0];
		}

		// Pt �� ������带 ����Ų��. 
		// Pt �� succ ����̴�. �� ����� ù ���ڵ尡 d_rec �� successor ���ڵ��̴�.
		curr->rec[i] = Pt->rec[0]; // successor �� d_rec �� �����. �� d_rec �� ���ŵȴ�.

		// ���� Pt �� ù ���ڵ� (�� ��ġ 0�� ���ڵ�)�� ����� ������ �ȴ�.
		curr = Pt; // curr �� �Ͽ��� ������ ���ڵ尡 �ִ� ��带 ����Ű�� �Ѵ�.
		i = 0; // ���� successor ���ڵ���  index �� ���� i �� ������ �ְ� ��.
	} //end if

	// ���⿡ ���� curr ��忡�� index �� i �� ���ڵ�� �� ���� �����͸� �����Ͽ��� �Ѵ�.
	do {
		// Step (2):
		// curr ��忡�� ��ġ i �� ���ڵ�� �� �ٷ� ������ �����͸� �����ϴ� �۾��� �Ѵ�. 
		for (j = i + 1; j < curr->fill_cnt; j++) { // i ���� ������ �͵��� �� ĭ�� �������� �̵�.
			curr->rec[j - 1] = curr->rec[j]; 
			curr->ptr[j] = curr->ptr[j + 1];
		}
		curr->fill_cnt = curr->fill_cnt - 1;

		// Step (3):
		if (curr == ROOT) {  // ��Ʈ��忡�� ������ �Ͼ� ����.
			if (curr->fill_cnt == 0) {  // ��Ʈ��尡 ����ִ�. ���� ���� ��Ʈ��带 �����Ѵ�.
				ROOT = ROOT->ptr[0]; 
				free(curr); 
			}
			return 1; // deletion succeeded.
		}

		// 	���⿡ �����ϸ� curr �� ROOT �� �ƴ� ����̴�.
		// Step (4):
		if (curr->fill_cnt >= D)  // ���� �Ŀ��� �� �̻��� ä���� ��Ȳ��.
			return 2;  // ���� �������� �˰��� �����Ѵ�.

		// ���� �����ϸ� curr �� ���̻� ä������ ���� ��Ȳ�̴�..
		// Step (5):
		father = pop(); // curr �� �θ��带 father �� ����Ű�� �Ѵ�..

		// r-sibling : curr �� ���������� ���� �����͸� ������ ��. (������ NULL �� ������ ��.)
		// l-sibling : curr �� ���������� ���� �����͸� ������ ��. (������ NULL �� ������ ��.)
		
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

		// ����: father �� ptr[j] �� curr �� ����.
		//  r_sibling or l_sibling  �� �ϳ��� D ���� ���� ���ڵ带 ������ ��й谡 �����ϴ�.
		r_OK = 0;  l_OK = 0;
		if (r_sibling && r_sibling->fill_cnt > D) 
			r_OK = 1; // ���� ������ ��й� �� �� �� ����.
		else if (l_sibling && l_sibling->fill_cnt > D) 
			l_OK = 1; // ���������� ����� �� �� �� ����.

		if (r_OK || l_OK) { // ���� �Ǵ� ���� ������ ��й� �� �� �� ����.
			if (r_OK) {  // ���������� �����ϸ� ���� ���������κ��� ��й踦 �޴´�.
				redistribution(father, curr, r_sibling, 'R', j);
			}
			else if (l_OK) {  // ���������� �ȵǹǷ� ���� �����κ��� ��й踦 �޴´�.
				redistribution(father, l_sibling, curr, 'L', j);
			}
			printf("Redistribution has been done.\n");
			return 3; // Deletion succeeded with redistribution.
		}
		else {   //  step 6. ��й谡 �Ұ����� ��Ȳ�̴�.  �պ��� �ʿ��ϴ�.

			i = merge(father, curr, r_sibling, l_sibling); // i �� father �� ������ ���ڵ� ��ġ
			printf("Merging has been done.\n");

			curr = father;
			// �� �������� ���� i �� �� ���ο� curr ��忡�� ������ ���ڵ��� ��ġ�� ������ ����.. 
			// ���� ������ �� ������ ���� �κп��� ������ ����.
		} // else.
	} while (1);  // end of do-while ��.

} // B_tree_deletion
