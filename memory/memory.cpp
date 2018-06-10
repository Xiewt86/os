#include <bits/stdc++.h>
#include <unistd.h>
#include <string.h>
#include <vector>

using namespace std;

#define PROCESS_NAME_LEN 32 //进程名最大长度
#define MIN_SLICE 10 //内碎片最大大小
#define DEFAULT_MEM_SIZE 1024  //总内存大小
#define DEFAULT_MEM_START 0  //内存开始分配时的起始地址
#define DEFAULT_ALGO 1

typedef pair<int, string> My_algo;
int mem_size = DEFAULT_MEM_SIZE;
static int pid = 0;
My_algo algo[123];
int algo_idx = DEFAULT_ALGO;

struct free_block{	//空闲数据块
	int size;
	int start_addr;
	struct free_block *next;
};

struct allocated_block{ //已分配的数据块
	int pid;
	int size;
	int start_addr;
	char process_name[PROCESS_NAME_LEN];
	//int *data;
	struct allocated_block *next;
};

free_block *free_block_head; //空闲数据块首指针
allocated_block *allocated_block_head = NULL; //分配块首指针

allocated_block *find_process(int id); //寻找pid为id的分配块
free_block *init_free_block(int mem_size); //空闲块初始化
free_block *find_free_block(int size);  //寻找满足size大小的空闲块

void display_menu(); //显示选项菜单
void set_mem_size(); //设置内存大小
int allocate_mem(allocated_block *ab); //为制定块分配内存
int create_new_process(); //创建新的进程
int free_mem(allocated_block *ab); //释放分配块
void display_mem_usage(); //显示内存情况
void kill_process(); //杀死对应进程并释放其空间与结构体
void Usemy_algo(int id); //使用对应的分配算法（未使用）
void rearrange(); // 对块进行重新分配（未使用）
void swap(int *p, int *q); //交换地址（未使用）
int dispose(allocated_block *ab); //释放分配块结构体（未使用）

//主函数
int main(){
	int op;
	pid = 0;
	free_block_head = init_free_block(mem_size); //初始化一个可以使用的内存块，类似与操作系统可用的总存储空间
	algo[0] = make_pair(1, "FirstFit");
	algo[1] = make_pair(2, "BestFit");
	for(;;){
		display_menu();
		fflush(stdin);
		scanf("%d", &op);
		switch (op){
			case 1:{ 
					if (allocated_block_head == NULL)
					    set_mem_size(); 
					else{
						printf("Can not resize memory when memory has been allocated!\n");
						sleep(1);
					}
					break;
				   }
			case 2:{ 
					   printf("1) First fit,\n2) Best fit\nPlease choose a memory allocation algorithm: ");
					   scanf("%d", &algo_idx); 
					   cout<<"The memory allocation algorithm is set to "<<algo[algo_idx-1].second;
					   fflush(stdout);
					   sleep(1);
					   break; 
				   }
			case 3:{   
					   if (!create_new_process()) {
						   printf("No memory left!\n");
						   sleep(1);
					   } 
					   break; 
				   }
			case 4:{ kill_process(); break; }
			case 5:{ display_mem_usage(); break; }
			case 0:{ puts("bye...."); sleep(1); return 0; }
			default: break;
		}	
	}
}

allocated_block *find_process(int id){ //循环遍历分配块链表，寻找pid=id的进程所对应的块
	if (allocated_block_head == NULL)
		return NULL;
	else {
		allocated_block *current = allocated_block_head;
		while (current != NULL) {
			if (current->pid == id)
				return current;
			current = current->next;
		}
		return NULL;
	}
}

free_block *init_free_block(int mem_size){ //初始化空闲块，这里的mem_size表示允许的最大虚拟内存大小
	free_block *p;
	p = (free_block *)malloc(sizeof(free_block));
	if (p == NULL){
		puts("No memory left");
		return NULL;
	}
	p->size = mem_size;
	p->start_addr = DEFAULT_MEM_START;
	p->next = NULL;
	return p;
}

free_block *find_free_block(int required_size) {
	switch(algo_idx) {
		case 1:{  //First fit
				   free_block *current = free_block_head;
				   while (current != NULL) {
					   if (current->size >= required_size)
						   return current;
					   else
						   current = current->next;
				   }
				   return NULL;
			   }
		case 2:{  //Best fit
				   free_block *current = free_block_head;
				   free_block *min_free_block = NULL;
				   while (current != NULL) {
					   if (current->size >= required_size){
						   if (min_free_block == NULL)
						   	  min_free_block = current;
						   else {
							   if (min_free_block->size > current->size) {
								   min_free_block = current;
							   }
						   }

					   }
					   current = current->next;
				   }
				   return min_free_block;
			   }   	   
		default: break;
	}
}

void display_menu(){
	puts("\n\n+============== Menu ==============+");
	printf("1) Set memory size (default = %d)\n", DEFAULT_MEM_SIZE);
	printf("2) Set memory allocation algorithm\n");
	printf("3) Create a new process\n");
	printf("4) Kill a process\n");
	printf("5) Display memory usage\n");
	printf("0) Exit\n");
	puts("+==================================+");
}

void set_mem_size(){ //更改最大内存大小
	int size;
	printf("Memory size: ");
	scanf("%d", &size);
	if (size <= 0) {
		printf("Invalid memory size!\n");
		sleep(1);
		return;
	}
	mem_size = size;
	free_block_head = init_free_block(mem_size); //初始化一个可以使用的内存块，类似与操作系统可用的总存储空间
}

int allocate_mem(allocated_block *ab){ //为块分配内存，真正的操作系统会在这里进行置换等操作
	//free_block *tmp = NULL;
	/* 将需要被分配内存的块加入allocated_block表 */
	ab->next = allocated_block_head;
	allocated_block_head = ab;

	/* 修改free_block链表 */
	free_block *fprevious = NULL;
	free_block *fcurrent = free_block_head;
	while (fcurrent->start_addr != ab->start_addr) {  //寻找ab来自哪个free_block
		fprevious = fcurrent;
		fcurrent = fcurrent->next;
	}
	if (fcurrent->size == ab->size) {  //情况1：整块free_block被allocated
		if (fcurrent == free_block_head) { //情况1.1：free_block是链表首元素
			//tmp = free_block_head;
			free_block_head = free_block_head->next;
			free(fcurrent);
			fcurrent = NULL;
		} else {  //情况1.2：free_block不是链表首元素
			fprevious->next = fcurrent->next;
			free(fcurrent);
			fcurrent = NULL;
		}
	} else {  //情况2：部分free_block被allocated
		fcurrent->start_addr = fcurrent->start_addr + ab->size;
		fcurrent->size = fcurrent->size - ab->size;
	}
	return 1;
}

int create_new_process(){ //创建新进程
	int size = 0;
	struct free_block *available_free_block;
	struct allocated_block *block;
	block = (struct allocated_block*)malloc(sizeof(struct allocated_block));
	printf("Process size(kb): ");
	scanf("%d", &size);
	available_free_block = find_free_block(size);
	if (available_free_block == NULL) {
		return 0;
	} else {
		block->pid = ++pid;
		block->size = size;
		block->start_addr = available_free_block->start_addr;
		snprintf(block->process_name, sizeof(block->process_name), "process %d", block->pid);
		block->next = NULL;
		allocate_mem(block);
		return 1;
	}
}

void swap(int *p, int *q){
	int tmp = *p;
	*p = *q;
	*q = tmp;
	return;
}

void rearrange(){ //将块按照地址大小进行排序
	free_block *tmp, *tmpx;
	puts("Rearrange begins...");
	puts("Rearrange by address...");
	tmp = free_block_head;
	while(tmp != NULL){
		tmpx = tmp->next;
		while (tmpx != NULL){
			if (tmpx->start_addr < tmp->start_addr){
				swap(&tmp->start_addr, &tmpx->start_addr);
				swap(&tmp->size, &tmpx->size);
			}
			tmpx = tmpx->next;
		}
		tmp = tmp->next;
	}
	usleep(500);
	puts("Rearrange Done.");
}


int free_mem(allocated_block *ab){ //释放某一块的内存
	int start_addr;
	int size;
	free_block *tmp;
	allocated_block *current = allocated_block_head;
	allocated_block *previous = NULL;
	struct free_block *fblock = NULL;
	free_block *fcurrent = free_block_head;
	start_addr = ab->start_addr;
	size = ab->size;
	while (current->next != NULL) {
		if (current->next == ab) {
			previous = current;
			break;
		}
		current = current->next;
	}
	if (previous != NULL) 
		previous->next = ab->next;
	else 
		allocated_block_head = ab->next;
	free(ab);
	ab = NULL;
	if (free_block_head == NULL) {  //情况0：free_block链表为空
		fblock = (free_block*)malloc(sizeof(free_block));
		fblock->start_addr = start_addr;
		fblock->size = size;
		fblock->next = NULL;
		free_block_head = fblock;
		return 1;
	} else if (free_block_head->start_addr >= start_addr + size) { //情况1：需要释放的内存块在free_block_head之前
		if (start_addr+size == free_block_head->start_addr) {  //情况1.1: 需要释放的内存块与free_block_head相邻
			free_block_head->start_addr = start_addr;
			free_block_head->size = free_block_head->size + size;
		} else {  //情况1.2：需要释放的内存块与free_block不相邻
			fblock = (free_block *)malloc(sizeof(free_block));
			fblock->start_addr = start_addr;        
			fblock->size = size;
			fblock->next = free_block_head;	
			free_block_head = fblock;
		}
		return 1;
	}
	while (fcurrent->next != NULL) { //情况2：需要释放的内存块在free_block_head之后，但在最后一个free_block之前
		if (fcurrent->start_addr + fcurrent->size <= start_addr && fcurrent->next->start_addr >= start_addr + size) { //找到需要释放的内存块在哪两块free_block之间
			
			if (fcurrent->start_addr+fcurrent->size == start_addr && fcurrent->next->start_addr == start_addr + size) {  //情况2.1：需要释放的内存块与前后两块free_block均相邻
				fcurrent->size = fcurrent->size + size + fcurrent->next->size;
				tmp = fcurrent->next;
				fcurrent->next = fcurrent->next->next;
				free(tmp);
				tmp = NULL;
			} else if (fcurrent->start_addr+fcurrent->size == start_addr) {  //情况2.2：需要释放的内存块在某两块free_block之间并且与前一块相邻
				fcurrent->size = fcurrent->size + size;
			} else if (fcurrent->next->start_addr == start_addr + size) {  //情况2.3：需要释放的内存块在某两块free_block之间并与后一块相邻
				fcurrent->next->start_addr = start_addr;
				fcurrent->next->size = fcurrent->next->size + size;
			} else {  //情况2.4:需要释放的内存块在某两块free_block之间并且不与其他块相邻
				fblock = (free_block *)malloc(sizeof(free_block)); 
				fblock->start_addr = start_addr;                   
				fblock->size = size;  
				fblock->next = fcurrent->next;
				fcurrent->next = fblock;
			}
			return 1;
		}
		fcurrent = fcurrent->next;
	}
	if (fcurrent->next == NULL) {  //情况3：需要释放的内存块在最后一个free_block之后
		if (fcurrent->start_addr+fcurrent->size == start_addr) {  //情况3.1：需要释放的内存块与最后一个free_block相邻
			fcurrent->size = fcurrent->size + size;                                                                
		} else { //情况3.2：需要释放的内存块与最后一个free_block不相邻
			fblock = (free_block *)malloc(sizeof(free_block)); 
			fblock->start_addr = start_addr;                   
			fblock->size = size;  
			fblock->next = NULL;
			fcurrent->next = fblock;
		}
		return 1;
	}
	return 1;
}

int dispose(allocated_block *fab){ //释放结构体所占的内存
	allocated_block *pre, *ab;
	if (fab == allocated_block_head){
		allocated_block_head = allocated_block_head->next;
		free(fab);
		return 1;
	}
	pre = allocated_block_head;
	ab = allocated_block_head->next;
	while (ab != fab){ pre = ab; ab = ab->next;}
	pre->next = ab->next;
	free(ab);
	return 2;
}

void display_mem_usage(){
	free_block *fb = free_block_head;
	allocated_block *ab = allocated_block_head;
	puts("+---------- Free Memory ----------+");
	printf("%10s %20s\n", "start_addr", "size");
	int cnt = 0;
	while (fb != NULL){
		cnt++;
		printf("%10d %20d\n", fb->start_addr, fb->size);
		fb = fb->next;
	}
	if (!cnt) puts("No Free Memory");
	else printf("Totaly %d free blocks\n", cnt);
	puts("+---------------------------------+");  
	puts("");
	puts("+------------------------------ Used Memory -----------------------------+");
	printf("%10s %20s %20s %20s\n", "PID", "process_name", "start_addr", "size");
	cnt = 0;
	while (ab != NULL){
		cnt++;
		printf("%10d %20s %20d %20d\n", ab->pid, ab->process_name, ab->start_addr, ab->size);
		ab = ab->next;
	}
	if (!cnt) puts("No allocated block");
	else printf("Totaly %d allocated blocks\n", cnt);
	puts("+------------------------------------------------------------------------+");  
	return;
}

void kill_process(){ //杀死某个进程
	allocated_block *ab;
	int pid;
	puts("Please input the pid of Killed process");
	scanf("%d", &pid);
	ab = find_process(pid);
	if (ab != NULL){
		free_mem(ab);
		//dispose(ab);
	} else {
		printf("Process (pid=%d) does not exist.\n", pid);
		sleep(1);
	}
}



