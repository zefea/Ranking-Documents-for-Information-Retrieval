/*
 * Zeynep Ferah Akkurt
 * 150119824
 * 
 * CSE225 Data Structures, 2020(FALL) Project 
 * Ranking Documents for Information Retrieval with Priority Queues
 * 
 * ProjectFinal.c
 * 
 */



#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

//all documents that is related with keyword
int numberOfRelevantDocs = 0;
char path[] = "C:\\Users\\lenovo\\Desktop\\Project 2\\files\\files";


//Key
typedef struct{	
	char fileName[25];	
	int frequency;
	int totalWords;
	char path[50];
	
}Document;

//node
struct node{
	
	Document key;
	int degree;
	struct node* parent;
	struct node* child;
	struct node* sibling;
};

//heap
typedef struct{
	struct node *head;
} Heap;

Heap *createHeap();
struct node* createNode(Document);
void get_a_BTk(struct node*, struct node*);
struct node *mergeRootLists(Heap*, Heap*);
struct node *merge(Heap*, Heap*);
void insertNodeToHeap(Heap*,struct node*);
void reverse_list(struct node*,Heap*);
void removeMax( Heap *, struct node *, struct node *);
struct node *extractMax(Heap *);
int readAndCount(char*,Document*);
int getFileNames(Heap** ,char *);
void printInfo(Document);
void getDocument(Document*,char *);
void getRelatedDocs(Heap *, int);
void freeHeap(Heap *);


		
//initialize heap
Heap *createHeap()
{
	Heap *heap = malloc(sizeof(Heap));
	heap->head = NULL;
	return heap;
}

//new node
struct node* createNode(Document key){

	struct node* p = (struct node*)malloc(sizeof(struct node));//new node;			
	p->key = key;
	p->degree = 0;
	p->parent = NULL;
	p->child = NULL;
	p->sibling = NULL;
	return p;
}

//Bk-1 + Bk-1 = Bk
void get_a_BTk(struct node*y, struct node*z){
	y->parent = z;
    y->sibling = z->child;
    z->child = y;
    z->degree = z->degree + 1;	
}

/* Merges the root lists of H1 &  H2 into a single linked-list
 * Merges root lists of BH1 and BH2 into one sorted by ascending degree. */
struct node *mergeRootLists(Heap *heap1, Heap *heap2 )
{
	//if heaps are empty
	if( heap1->head == NULL ) 
		return heap2->head;
	if( heap2->head == NULL ) 
		return heap1->head;

	//heads
	struct node *head;
	struct node *headof1 = heap1->head;
	struct node *headof2 = heap2->head;
	
	//B0-->B1 - the smaller degree will come first
	if( heap1->head->degree <= heap2->head->degree ){
		head = heap1->head;
		headof1 = headof1->sibling;
	}else{
		head = heap2->head;
		headof2 = headof2->sibling;
	}
	
	//set  the siblings of the head
	struct node *tempHead = head;
	while( headof1 != NULL && headof2 != NULL ){
		
		if( headof1->degree <= headof2->degree ){
			tempHead->sibling = headof1;
			headof1 = headof1->sibling;
		}else{
			tempHead->sibling = headof2;
			headof2 = headof2->sibling;
		}
		tempHead = tempHead->sibling;
	}

	if(headof1 != NULL){
		tempHead->sibling = headof1;
	}else{
		tempHead->sibling = headof2;
	}
	
	return head;
}

//merge two binomial heap
struct node *merge(Heap* heap1, Heap* heap2){	
	
	Heap *heap = createHeap();					//create an empty heap
	heap->head = mergeRootLists(heap1,heap2);	//roots merged heap
	if(heap->head == NULL)
		return NULL;
		
	struct node* q = NULL;		//prev of x
	struct node* x = heap->head;//x is the head
	struct node* r = x->sibling;//next of x
	
	//cases
	while(r != NULL){
		//case 1 and 2
		if((r->degree != x->degree)||((r->sibling != NULL) && (r->sibling->degree == x->degree))){	
			q = x;
			x = r;
		}else if(x->key.frequency > r->key.frequency){		//case 3 for maximum key
			x->sibling = r->sibling;
			get_a_BTk(r,x);	//binomial link
		}else{				//case 4
			if( q == NULL ){
				heap->head = r;
			}else{
				q->sibling = r;
			}		
			get_a_BTk(x,r);//binomial link
		}
		r = x->sibling;
	}
	return heap->head;	
}
//insert a node into heap
void insertNodeToHeap(Heap* myheap,struct node*key){
	
	//new one node heap
	Heap*newheap = createHeap();
	key->parent = NULL;
	key->child = NULL;
	key->sibling = NULL;
	key->degree = 0;
	newheap->head = key; 
	//insertion with merge: H0(one node) and current heap
	myheap->head = merge(myheap,newheap);
	
}

//reverse the order of the linked list of node’s children
void reverse_list(struct node* key,Heap* heapTemp){
    if (key->sibling != NULL) { 
        reverse_list(key->sibling,heapTemp); 
        (key->sibling)->sibling = key; 
    } 
    else
        heapTemp->head = key; 
}

//removes the maximum key from the root list of H. 
void removeMax( Heap *heap, struct node *max, struct node *maxPrev){
	//if there is no key this means heap is empty
    if (maxPrev == NULL && max->sibling == NULL) {
        heap->head = NULL; 
    }else if(maxPrev == NULL){//if mprevious node of max node is empty, then head will be next of max node and max will be removed
		heap->head = max->sibling;
    }else{					//if both previous node of max and max node is exist, then previous one should point to the next of max and max is removed between them.
        maxPrev->sibling = max->sibling; 
	}
}

//extracts the node with the maximum key and returns the key
struct node *extractMax(Heap *heap){
	
	//heap is empty
	if( heap->head == NULL ) 
		return NULL;
		
	struct node *max = heap->head;
	struct node *max_prev = NULL;
	struct node *next = max->sibling;
	struct node *next_prev = max;

	//find maximum node
	while( next != NULL ){
		if( next->key.frequency > max->key.frequency ){
			max = next;
			max_prev = next_prev;
		}
		next_prev = next;
		next = next->sibling;
	}
	//remove node
	removeMax(heap, max, max_prev);
	
	//new heap
	Heap*temp = createHeap();
	if (max->child != NULL) { 
		//reverse the list
        reverse_list(max->child,temp); 
       (max->child)->sibling = NULL; 
    } 
    //merge the heaps
	heap->head = merge(temp,heap);
	free(temp);
	return max;
}
//reads the document and counts the frequency of given keyword
int readAndCount(char* keyword,Document *doc){
	
	//printf("inside readfile\n")
	int frequency = 0;
	int total=0;
	
	FILE *f; 
	f = fopen(doc->path,"r");
	if(f == NULL){
		printf("file could not open!\n");
		return 0;
	} 
	char *word = (char*)calloc(15,sizeof(char));
	if(word == NULL){
		printf("no memory");
		exit(-1);
	}
	
	char c;
	int count = 0;
	char prev;

	while((c = fgetc(f)) != EOF){   
		if(isalpha(c) || c == '\'' || isalnum(c) || c == '-'){		
			strncat(word,&c,1);
			count++;			
		}else if(c == '$' || prev == '$'){
			prev = c;
			strncat(word,&c,1);
			count++;
		}else {	
			if(count != 0){
				total++;
				if(strcmp(word,keyword)==0){	//word is found
					frequency++;					
				}
			}
			count = 0;
			strncpy(word,"",strlen(word));
		}		
	}
	doc->totalWords = total;
	doc->frequency = frequency;
	
	fclose(f);	
	free(word);
	return 1;
}
//gets the filenames in given path
int getFileNames(Heap** heap ,char *keyword)
{
    struct dirent *dp;
    DIR *dir = opendir(path);

    // Unable to open directory stream
    if (!dir){ 
        return 0;
    }  
   
    int total = 0;
    while ((dp = readdir(dir)) != NULL){		
		if(isalnum((dp->d_name)[0])){
			Document doc;
			strcpy(doc.fileName,dp->d_name);
			strcpy(doc.path,path);
			strcat(doc.path,"\\");
			strcat(doc.path,doc.fileName);	
			
			if(readAndCount(keyword,&doc)){
				printInfo(doc);
				int n = doc.totalWords;
				total += n;
				if(doc.frequency != 0 )
					numberOfRelevantDocs++;
							
				struct node *nodeKey = createNode(doc);//new node
				insertNodeToHeap(*heap,nodeKey); //insert the node to the heap
			}	
		}		
	}
	
    // Close directory stream
    closedir(dir);
    return total;

}

//prints information of a document
void printInfo(Document doc){
	
	printf("\n****************************************\n");
	printf("Informations of document: \n");
	printf("filename: %.25s\n",doc.fileName);
	printf("Frequency: %d \n" , doc.frequency);
	printf("Total number of words: %d \n" ,doc.totalWords);
}

//get the document text
void getDocument(Document *doc,char *path){
	FILE *f; 
	
	strcat(path,"\\");
	strcat(path,doc->fileName);	
	
	f = fopen(path,"r");
	if(f == NULL){
		printf("file could not open!\n");
		return ;
	} 
	char c;
	while((c = fgetc(f)) != EOF){ 
		printf("%c",c);   
	}
	
}


//prints number of related documents with filename and frequecy
void getRelatedDocs(Heap * heap, int number){
	
	char *temp = (char*)malloc(sizeof(path));
	strcpy(temp,path);
	struct node* maxes[number];
	
	for(int i=0; i<number; ++i){
		struct node* max = extractMax(heap);
		maxes[i] = max;
		if(maxes[i]->key.frequency != 0){
			printf("filename: %.25s: %d\n", max->key.fileName, max->key.frequency);
		}
	}
	for(int i=0; i<number; ++i){
		if(maxes[i]->key.frequency != 0){
			printf("\n\n************************Document %.25s: %d:************************\n",maxes[i]->key.fileName,maxes[i]->key.frequency);
			getDocument(&maxes[i]->key,temp);
			strcpy(temp,path);	
		}
	}
}

//free heap
void freeHeap(Heap *heap){
	//extract max removes a node from the list, after removing, it should be free.
	while(heap->head != NULL){
		struct node* freeNode = extractMax(heap);
		free(freeNode);
	}
		
}

int main(int argc, char **argv)
{
	char keyword[25];
	
	int number = 0;		//keeps how many related documents wanted to be seen 
	//Input path from user
    printf("Please enter the keyword: ");
    scanf("%25s", keyword);
    
    printf("Please enter the number related documents to see: ");
    scanf("%d",&number);
    
	Heap *heap = createHeap();
	//read the files and set the frequencies
	int total = getFileNames(&heap,keyword);
	if(total != 0){
		printf("\n****************RESULTS*******************\n");
		printf("The keyword: %.25s\n",keyword);
		printf("Total number of words in all documents: %d\n",total);
		printf("NUMBER OF RELEVANT DOCUMENTS: %d\n",numberOfRelevantDocs);
		printf("The most relevant documents in order: \n");
		//getdocuments
		getRelatedDocs(heap,number);
	}	
	free(heap);
	return 0;
}

