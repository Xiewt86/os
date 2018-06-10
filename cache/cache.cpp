#include<bits/stdc++.h>
#include<unistd.h>
#include<string>
#include<iostream>
#include<fstream>

using namespace std;

const double eps = 1e-8;
int dcmp(const double &x){ if (fabs(x) < eps) return 0; return x < 0 ? -1 : 1; }

#define DEFAULT_CACHE_SIZE 123
#define DEFAULT_ALGORITHM 0

int cache_size, working_algorithm, hit, miss, page_size;
vector<int> pages;

void init();
void print_menu();
void set_cache_size();
void print_algorithms();
void set_algorithm();
void add_an_algorithm();
void generate_input();
void generate_randomly(int n);
void generate_by_hand(int n);
void print_pages();
void FIFO_algorithm();
void run_algorithm();
void read_pages();
void LRU_algorithm();
void MIN_algorithm();
void scd_chance_algorithm();
void CLOCK_algorithm();
int next_call(int page, vector<int> q, int position);
void LRU_algorithm_new();

int main(){
    init();
	run_algorithm();
}


void init(){
    srand((unsigned int)time(0));
    cache_size = DEFAULT_CACHE_SIZE;
	cin >> cache_size;
    working_algorithm = 0;
	cin >> working_algorithm;
	cin >> page_size;
}

void FIFO_algorithm(){
	string s;
	int page;
	int cnt;
	hit = 0;
	miss = 0;
	while (true)
	{
		cin >> s;
		page = stoi(s);
		vector<int>::iterator itr;
		itr = find(pages.begin(), pages.end(), page);
		if (itr == pages.end())  // Cache miss.
		{
			miss++;
			if (pages.size() == cache_size)  // If the fifo is full.
			{
				vector<int>::iterator k = pages.begin();
				pages.erase(k);
				pages.push_back(page);
			}
			else  // If the fifo has a free block.
			{
				pages.push_back(page);
			}
		}
		else  // Cache hit.
		{
			hit++;
		}
		if (++cnt == page_size) 
		{
			//cout << "Time to return" << endl;
			return;
		}
	}
}


void LRU_algorithm() {
	vector<int> records;
	string s;
	int page;
	int cnt = 0;
	hit = 0;
	miss = 0;
	while (true)
	{
		cin >> s;
		page = stoi(s);
		vector<int>::iterator itr;
		itr = find(pages.begin(), pages.end(), page);
		for (int i = 0; i < records.size(); ++i)
		{
			if (records[i] != -1)
				records[i]++;
		}
		if (itr == pages.end())  // Cache miss.
		{
			miss++;
			if (pages.size() == cache_size)  // If the fifo is full.
			{
				int idx;
				vector<int>::iterator max = max_element(begin(records), end(records));
				idx = distance(records.begin(), max);
				pages[idx] = page;
				records[idx] = 0;

			}
			else  // If the fifo has a free block.
			{
				pages.push_back(page);
				records.push_back(0);
			}
		}
		else  // Cache hit.
		{
			hit++;
			int idx = distance(pages.begin(), itr);
			records[idx] = 0;
		}
		if (++cnt == page_size)
		{
			return;
		}
	}
}


void MIN_algorithm() {
	int elem;
	vector<int> records;
	vector<int> q;
	int usage_record[cache_size];
	int cnt = -1;
	for (int i = 0; i < page_size; i++)
	{
		cin >> elem;
		q.push_back(elem);
	}
	for (int i = 0; i < q.size(); ++i)
	{
		for (int j = 0; j < records.size(); ++j)
		{
			records[j]--;
		}
		++cnt;
	    int page = q[i];
 		vector<int>::iterator itr;
		itr = find(pages.begin(), pages.end(), page);
		if (itr == pages.end())  // Cache miss.
		{
			miss++;
			if (pages.size() == cache_size)  // If the cache is full
			{
				vector<int>::iterator max = max_element(records.begin(), records.end());
				int idx = distance(records.begin(), max);
				pages[idx] = page;
				records[idx] = next_call(page, q, i);		
			}
			else  // If the cache has a free block.
			{
				pages.push_back(page);
				records.push_back(next_call(page, q, i));
			}
		}
		else // Cache hit.
		{
			hit++;
			int idx = distance(pages.begin(), itr);
			records[idx] = next_call(page, q, i);
		}	
	}
}

int next_call(int page, vector<int> q, int position) {
	for (int i = position + 1; i < q.size(); ++i)
	{
		if (q[i] == page)
		{
			return i-position;
		}
	}
	return q.size() + 1;
}


void CLOCK_algorithm() {
	bool full_flag = 0;
	vector<int> clock(cache_size, -1);
	vector<int> flags(cache_size, 1);
	int pointer = -1;
	int cnt = 0;
	int page;
	while (true)
	{
		cin >> page;
		vector<int>::iterator itr = find(clock.begin(), clock.end(), page);
		if (itr == clock.end()) // Cache miss.
		{
			miss++;
			while (true)
			{
				pointer++;
				if (pointer >= cache_size)
				{
					pointer = 0;
				}
				if (flags[pointer] == 0)
				{
					break;
				}
				flags[pointer] = 0;
			}
			clock[pointer] = page;
			flags[pointer] = 1;
		}
		else  // Cache hit.
		{
			hit++;
			//pointer = distance(clock.begin(), itr);
			//flags[pointer] = 1;
			flags[distance(clock.begin(), itr)] = 1;
		}		
		if (++cnt == page_size)
		{
			return;
		}
	}
}


void scd_chance_algorithm() {
	int cnt = 0;
	vector<int> lru, records, fifo;
	//int size_lru = cache_size / 2;
	//int size_fifo = cache_size - size_lru;
	int size_fifo = cache_size / 2;
	int size_lru = cache_size - size_fifo;
	while (true)
	{
		for (int i = 0; i < records.size(); ++i)
		{
			records[i]++;
		}
		bool lru_flag, fifo_flag;
		int page;
		cin >> page;
		vector<int>::iterator lru_itr = find(lru.begin(), lru.end(), page);
		vector<int>::iterator fifo_itr = find(fifo.begin(), fifo.end(), page);
		if (lru_itr == lru.end())  // lru miss
		{
			lru_flag = 0;
		}
		else
		{
			lru_flag = 1;
		}
		if (fifo_itr == fifo.end())  // fifo miss
		{
			fifo_flag = 0;
		}
		else
		{
			fifo_flag = 1;
		}
		if (! (lru_flag || fifo_flag))  // Cache miss.
		{
			miss++;
			if (fifo.size() < size_fifo)  // fifo is not full.
			{
				fifo.push_back(page);
			}
			else if (lru.size() < size_lru)  // fifo is full & lru is not full.
			{
				vector<int>::iterator k = fifo.begin();
				int e = *k;
				fifo.erase(k);
				lru.push_back(e);
				records.push_back(0);
				fifo.push_back(page);
			}
			else  // fifo is full & lru is full
			{
				vector<int>::iterator k = fifo.begin();
				int e = *k;
				fifo.erase(k);
				fifo.push_back(page);
				vector<int>::iterator max = max_element(records.begin(), records.end());
				int idx = distance(records.begin(), max);
				lru[idx] = e;
				records[idx] = 0;
			}
		}
		else  // Cache hit.
		{
			hit++;
			if (lru_flag)  // If hit happens in the lru list.
			{
				int idx = distance(lru.begin(), lru_itr);
				vector<int>::iterator k = fifo.begin();
				int e = *k;
				fifo.erase(k);
				fifo.push_back(lru[idx]);
				lru[idx] = e;
				records[idx] = 0;
			}
			else
			{
				//int e = *fifo_itr;
				//fifo.erase(fifo_itr);
				//fifo.push_back(e);
			}
		}
		if (++cnt == page_size)
		{
			return;
		}
	}
}

void print_status(){
    //cout << "cache size is: " << cache_size << endl;
    //print_pages();
    //cout << "working algorithm is " << working_algorithm << ":" << algorithms[working_algorithm].second << endl;
    //cout << "hit times = " << hit << endl << "miss times = " << miss << endl;
	//cout << setprecision (2);
    cout << "Hit ratio = " << fixed << setprecision (2) << (double)hit * 100 / (miss + hit) << "%" << endl;
}


void run_algorithm(){
	pages.clear();
    switch (working_algorithm){
        case 0: FIFO_algorithm(); break;
        case 1: LRU_algorithm(); break;
		case 2: MIN_algorithm(); break;
		case 3: CLOCK_algorithm(); break;
		case 4: scd_chance_algorithm(); break;
        default: break;
    }
    print_status();
}

