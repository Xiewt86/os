#include <bits/stdc++.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#define GET_ARRAY_LEN(array, len) {len = (sizeof(array) / sizeof(array[0]));}

using namespace std;

vector<int> tracks;
int init_track;
int capacity;
int max_track;

void FCFS ();
void SSTF ();
void SCAN ();
void C_SCAN ();
void LOOK ();
void CLOOK ();
void C_LOOK ();
void print (vector<int> out, int dist, string algo);

int main ()
{
	cin >> init_track;
	cin >> capacity;
	cin >> max_track;
	tracks.push_back(init_track);
	for (int i = 0; i < max_track; ++i)
	{
		int tmp;
		cin >> tmp;
		tracks.push_back(tmp);
	}
	FCFS ();
	SSTF ();
	SCAN ();
	C_SCAN ();
	LOOK ();
	C_LOOK ();
	return 0;
}

void FCFS ()
{
	int dist = 0;
	vector<int> out;
	for (int i = 0; i < tracks.size(); ++i)
	{
		out.push_back(tracks[i]);
		int last;
		if (i == 0)
		{
			last = init_track;
		}
		else
		{
			last = out[i-1];
		}
		dist += abs(tracks[i] - last);
	}
	print(out, dist, "FCFS");
}

void SSTF ()
{
	vector<int> tmp_tracks(tracks); 
	int dist = 0;
	vector<int> out;
	int len = tmp_tracks.size();
	int last = init_track;
	for (int i = 0; i < len; ++i)
	{
		int min_dist = abs(tmp_tracks[0] - last);
		vector<int>::iterator min_itr = tmp_tracks.begin();
		for (vector<int>::iterator it = tmp_tracks.begin(); it != tmp_tracks.end(); ++it)
		{
			if (abs(*it - last) < min_dist)
			{
				min_itr = it;
				min_dist = abs(*it - last);
			}
		}
		out.push_back(*min_itr);
		dist += abs(*min_itr - last);
		last = *min_itr;
		tmp_tracks.erase(min_itr);
	}
	print(out, dist, "SSTF");
}

void SCAN ()
{
	vector<int> out;
	vector<int> records (capacity, 0);
	int last = init_track;
	int dist = 0;
	int cnt = -1;
	int flag = 1;
	int j;
	for (int i = 0; i < tracks.size(); ++i)
	{
		records[tracks[i]] += 1;
	}
	j = init_track;
	while (j >= 0)
	{
		if (cnt == max_track)
		{
			flag = 0;
			break;
		}
		if (records[j] != 0)
		{
			records[j]--;
			cnt++;
			out.push_back(j);
			last = j;
		}
		if (records[j] == 0)
		{
			if (j > 0)
			{
				dist++;
			}
			j--;
		}
	}
	if (flag && records[0] == 0)
	{
		out.push_back(0);
	}
	dist += init_track;
	j = init_track;
	while (j < capacity)
	{
		if (cnt == max_track)
		{
			break;
		}
		if (records[j] != 0)
		{
			records[j]--;
			cnt++;
			out.push_back(j);
			last = j;
		}
		if (records[j] == 0)
		{
			if (cnt != max_track)
			{
				dist++;
				//cout << j << endl;
			}
			j++;
		}
	}
	print(out, dist, "SCAN");

}

void C_SCAN()
{
	int flag = 1;
	int cnt = -1;
	vector<int> out;
	int dist = 0;
	vector<int> records (capacity, 0);
	for (int i = 0; i < tracks.size(); ++i)
	{
		records[tracks[i]] += 1;
	}
	int j = init_track;
	while (j >= 0)
	{
		if (cnt == max_track)
		{
			flag = 0;
			break;
		}
		if (records[j] != 0)
		{
			//cout << j << endl;
			records[j]--;
			cnt++;
			out.push_back(j);
		}
		if (records[j] == 0)
		{
			if (j > 0)
			{
				dist++;
			}
			j--;
		}
	}
	if (flag)
	{
		if (records[0] == 0)
		{
			out.push_back(0);
		}
		if (records[capacity - 1] == 0)
		{
			out.push_back(capacity - 1);
		}
	}
	j = capacity - 1;
	while (j > init_track)
	{
		if (cnt == max_track)
		{
			break;
		}
		if (records[j] != 0)
		{
			//cout << j << endl;
			records[j]--;
			cnt++;
			out.push_back(j);
		}
		if (records[j] == 0)
		{
			if (cnt != max_track)
				dist++;
			j--;
			//cout << j << endl;
		}
	}
	print(out, dist, "C-SCAN");
}

void LOOK ()
{
    vector<int> out;
	vector<int> records (capacity, 0);
	int last = init_track;
	int dist = 0;
	int cnt = -1;
	int min = capacity - 1;
	int j;
	for (int i = 0; i < tracks.size(); ++i)
	{
		records[tracks[i]] += 1;
		if (tracks[i] < min)
		{
			min = tracks[i];
		}
	}
	j = init_track;
	while (j >= 0)
	{
		if (cnt == max_track || j < min)
		{
			break;
		}
		if (records[j] != 0)
		{
			records[j]--;
			cnt++;
			out.push_back(j);
			last = j;
		}
		if (records[j] == 0)
		{
			if (j > min)
			{
				dist++;
			}
			j--;
		}
	}
	j = init_track + 1;
	dist += abs (j - last);
	while (j < capacity)
	{
		if (cnt == max_track)
		{
			break;
		}
		if (records[j] != 0)
		{
			records[j]--;
			cnt++;
			out.push_back(j);
			last = j;
		}
		if (records[j] == 0)
		{
			if (cnt != max_track)
			{
				dist++;
			}
			j++;
		}
	}
	print(out, dist, "LOOK");
}

void C_LOOK ()
{
    vector<int> out;
	vector<int> records (capacity, 0);
	int last = init_track;
	int dist = 0;
	int cnt = -1;
	int min = capacity - 1;
	int max = 0;
	int j;
	for (int i = 0; i < tracks.size(); i++)
	{
		records[tracks[i]] += 1;
		if (tracks[i] < min)
		{
			min = tracks[i];
		}
		if (tracks[i] > max)
		{
			max = tracks[i];
		}
	}
	j = init_track;
	while (j >= 0)
	{
		if (cnt == max_track || j < min)
		{
			break;
		}
		if (records[j] != 0)
		{
			records[j]--;
			cnt++;
			out.push_back(j);
			last = j;
		}
		if (records[j] == 0)
		{
			if (j > min)
			{
				dist++;
			}
			j--;
		}
	}
	j = max;
	while (j > init_track)
	{
		if (cnt == max_track)
		{
			break;
		}
		if (records[j] != 0)
		{
			records[j]--;
			cnt++;
			out.push_back(j);
			last = j;
		}
		if (records[j] == 0)
		{
			if (cnt != max_track)
			{
				dist++;
			}
			j--;
		}
	}
	print(out, dist, "C-LOOK");
}


void print (vector<int> out, int dist, string algo)
{
	cout << algo << "\n";
	cout << dist << endl;
	int len = out.size();
	for (int i = 0; i < len; ++i)
	{
		//if (1)
		if (i == len - 1)
		{
			cout << out[i] << "\n" << dist << endl;
			break;
		}
		cout << out[i] << " -> ";
	}
} 

