#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>

#define MAX_VERTICES 20
typedef struct anod* TreePointer;
typedef struct anod {	// 한 노드의 구조
	int sno;			// 학번
	char name[50];		// 이름
	TreePointer leftChild, rightChild;	// 노드의 자식에 대한 포인터
}Ty_Node;

void inorder(TreePointer root);
int getheight(TreePointer root);
void search(TreePointer root, char* name);
int countLeaf(TreePointer root);
TreePointer modifiedSearch(TreePointer root, Ty_Node  asdf);
void insert_file(TreePointer* root, Ty_Node data);
void insert(TreePointer* root, Ty_Node data);
void print_child(TreePointer root, Ty_Node data);
int getNodeLevel(TreePointer root, char* key, int level);
TreePointer delete(TreePointer root, char* name);
void preorder(TreePointer root);
int main()
{
	FILE* fp = NULL;
	Ty_Node data;
	char buffer[100] = { 0, };
    char sname[50] = { 0, };
    int fs = 0;
    int no = -1;
    int height = -1;
	char* cmd = NULL;
    TreePointer root = NULL;
	fp = fopen("sdata.txt", "r");
	if (!fp)
	{
		printf("sdata.txt not found");
		return 1;
	}
    while (!feof(fp))
    {
        fs = fscanf(fp,"%d %s", &no, sname);
        if (fs < 2)
            break;
        data.sno = no;
        strcpy(data.name, sname);
        insert_file(&root, data);
    }
	while (1)
	{
		ZeroMemory(&data, sizeof(data));
		printf("수행할 작업은 (in, sp, de, se, ht, ch, le, ex) ? ");
		gets(buffer);
		cmd = strtok(buffer, " ");
		if (strcmp(cmd, "in") == 0)
		{
			cmd = strtok(NULL, " ");
            if (cmd)
            {
                data.sno = atoi(cmd);
            }
            else
            {
                continue;
            }
			cmd = strtok(NULL, " ");
            if (cmd)
            {
                strcpy(data.name, cmd);
            }
            else
            {
                continue;
            }
            insert(&root, data);
		}
		else if(strcmp(cmd, "sp") == 0)
		{
            inorder(root);
		}
		else if (strcmp(cmd, "de") == 0)
		{
			cmd = strtok(NULL, " ");
            if (cmd)
            {
                strcpy(data.name, cmd);
                delete(root, data.name);
            }
            else
            {
                continue;
            }
		}
		else if (strcmp(cmd, "se") == 0)
		{
			cmd = strtok(NULL, " ");
            if (cmd)
            {
                strcpy(data.name, cmd);
                search(root, data.name);
            }
            else
            {
                continue;
            }
		}
		else if (strcmp(cmd, "ht") == 0)
		{
            printf("height = %d\n", getheight(root));
		}
		else if (strcmp(cmd, "ch") == 0)
		{
			cmd = strtok(NULL, " ");
            if (cmd)
            {
                strcpy(data.name, cmd);
            }
            else
            {
                continue;
            }
            print_child(root, data);
		}
		else if (strcmp(cmd, "le") == 0)
		{
            int leafCount = countLeaf(root);
            printf("number of leaf nodes = %d\n", leafCount);
		}
		else if (strcmp(cmd, "ex") == 0)
		{
            printf("프로그램을 종료합니다!\n");
            system("pause");
		}
		else
		{
			printf("잘못된 명령어를 사용했습니다\n");
			continue;
		}
	}
}

TreePointer modifiedSearch(TreePointer root, Ty_Node data)
{
    TreePointer temp = root;
    while (temp)
    {
        root = temp;
        if (strcmp(data.name, root->name) < 0)
            temp = root->leftChild;
        else if (strcmp(data.name, root->name) > 0)
            temp = root->rightChild;
        else
            return NULL;
    }
    return root;
}

void insert_file(TreePointer* root, Ty_Node data)
{
    TreePointer ptr, temp = modifiedSearch(*root, data);
    if (temp || !(*root))
    {
        if (temp && strcmp(temp->name, data.name) == 0)
        {
            return;
        }
        ptr = (TreePointer)malloc(sizeof(*ptr));
        ptr->sno = data.sno;
        ptr->leftChild = ptr->rightChild = NULL;
        strcpy(ptr->name, data.name);
        if (*root)
            if (strcmp(data.name, temp->name) < 0)
                temp->leftChild = ptr;
            else
                temp->rightChild = ptr;
        else
            *root = ptr;
    }
}


void insert(TreePointer* root, Ty_Node data) {
    TreePointer ptr, temp = modifiedSearch(*root, data);
    if (temp || !(*root)) {
        if (temp && strcmp(temp->name, data.name) == 0) {
            printf("Insertion Failed: Duplicate key\n");
            return;
        }
        ptr = (TreePointer)malloc(sizeof(*ptr));
        ptr->sno = data.sno;
        ptr->leftChild = ptr->rightChild = NULL;
        strcpy(ptr->name, data.name);
        if (*root) {
            if (strcmp(data.name, temp->name) < 0)
                temp->leftChild = ptr;
            else
                temp->rightChild = ptr;
        }
        else {
            *root = ptr;
        }
        int level = getNodeLevel(*root, data.name, 1);
        printf("입력 성공! level = %d\n", level);
    }
    else {
        printf("입력 실패\n");
    }
}

int getNodeLevel(TreePointer root, char* key, int level) {
    if (root == NULL) {
        return -1;
    }
    int cmp = strcmp(root->name, key);
    if (cmp == 0) {
        return level;
    }
    else if (cmp > 0) {
        return getNodeLevel(root->leftChild, key, level + 1);
    }
    else {
        return getNodeLevel(root->rightChild, key, level + 1);
    }
}

TreePointer delete(TreePointer root, char* name) {
    TreePointer current, target, parent, maxLeft;
    parent = target = maxLeft = NULL;
    current = root;

    while (current != NULL) {
        if (strcmp(name, current->name) == 0) {
            target = current;
            break;
        }
        parent = current;
        if (strcmp(name, current->name) < 0) {
            current = current->leftChild;
        }
        else {
            current = current->rightChild;
        }
    }

    if (target == NULL) {
        printf("그런 학생이 없습니다.");
        return root;
    }

    if (parent != NULL) {
        if (target == parent->leftChild) {
            printf("성공적으로 삭제되었습니다.\n");
        }
        else {
            printf("성공적으로 삭제되었습니다.\n");
        }
    }

    if (target->leftChild == NULL && target->rightChild == NULL) {
        if (parent == NULL) {
            root = NULL;
        }
        else if (target == parent->leftChild) {
            parent->leftChild = NULL;
        }
        else {
            parent->rightChild = NULL;
        }
        free(target);
    }

    else if (target->leftChild == NULL || target->rightChild == NULL) {
        TreePointer child;
        if (target->leftChild != NULL) {
            child = target->leftChild;
        }
        else {
            child = target->rightChild;
        }
        if (parent == NULL) {
            root = child;
        }
        else if (target == parent->leftChild) {
            parent->leftChild = child;
        }
        else {
            parent->rightChild = child;
        }
        free(target);
    }

    else {
        maxLeft = target->leftChild;
        while (maxLeft->rightChild != NULL) {
            parent = maxLeft;
            maxLeft = maxLeft->rightChild;
        }
        printf("이동된 학생: %s\n", maxLeft->name);
        strcpy(target->name, maxLeft->name);

        if (parent == target) {
            parent->leftChild = maxLeft->leftChild;
        }
        else {
            parent->rightChild = maxLeft->leftChild;
        }

        free(maxLeft);
    }
    return root;
}


void search(TreePointer root, char* name) {
    TreePointer temp = root;
    int level = 1;

    while (temp) {
        if (strcmp(name, temp->name) == 0) {
            printf("이름: %s, 학번: %d, 레벨위치 = %d\n", temp->name, temp->sno, level);
            return;
        }
        else if (strcmp(name, temp->name) < 0) {
            temp = temp->leftChild;
        }
        else {
            temp = temp->rightChild;
        }
        level++;
    }
    printf("그런 학생 없습니다.");
}

void print_child(TreePointer root, Ty_Node data) {
    if (root == NULL)
    {
        printf("트리가 비어있습니다.\n");
        return;
    }

    TreePointer temp = root;
    while (temp != NULL)
    {
        if (strcmp(data.name, temp->name) == 0) {
            if (temp->leftChild != NULL) {
                printf("left child = %s, ", temp->leftChild->name);
            }
            if (temp->rightChild != NULL) {
                printf("right child = %s\n", temp->rightChild->name);
            }
            return;
        }
        else if (strcmp(data.name, temp->name) < 0) {
            temp = temp->leftChild;
        }
        else {
            temp = temp->rightChild;
        }
    }

    printf("%s 학생을 찾을 수 없습니다.\n", data.name);
}


void preorder(TreePointer root)
{
    if (root)
    {
        printf("%d ", root->sno);
        preorder(root->leftChild);
        preorder(root->rightChild);
    }
}

void inorder(TreePointer root)
{
    if (root)
    {
        inorder(root->leftChild);
        printf("%d %s\n", root->sno, root->name);
        inorder(root->rightChild);
    }
}

int getheight(TreePointer root)
{
    if (root == NULL)
        return 0;
    else
    {
        int left_height = getheight(root->leftChild);
        int right_height = getheight(root->rightChild);
        if (left_height > right_height)
            return left_height + 1;
        else
            return right_height + 1;
    }
}

int countLeaf(TreePointer root)
{
    if (root == NULL)
        return 0;
    else if (root->leftChild == NULL && root->rightChild == NULL)
        return 1;
    else
        return countLeaf(root->leftChild) + countLeaf(root->rightChild);
}
