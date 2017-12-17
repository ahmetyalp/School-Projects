#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cstdio>
#include <math.h>
#include <mpi.h>

using namespace std;

int power(int q) {
	int a = 1;
	for (int i = 0; i < q; i++)
		a *= 2;
	return a;
}

struct Pair
{
	char s[20];
	int k;
};
typedef struct Pair pp;

int compare_pair(const void* a, const void* b) {
	return strcmp(((pp*)a)->s, ((pp*)b)->s);
}

int main(int argc, char **argv) {
	/* Init MPI*/
	int id,np; //Current id and number of processors
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	MPI_Status status;

	/* Define MPI_Pair Data Type*/
	pp myobject;
	MPI_Datatype MPI_Pair;
	int blocklengths[2]; MPI_Datatype types[2];
	MPI_Aint displacements[2];
	blocklengths[0] = 20; types[0] = MPI_CHAR;
	displacements[0] = (size_t)&(myobject.s[0]) - (size_t)&myobject;
	blocklengths[1] = 1; types[1] = MPI_INT;
	displacements[1] = (size_t)&(myobject.k) - (size_t)&myobject;
	MPI_Type_create_struct(2, blocklengths, displacements, types, &MPI_Pair);
	MPI_Type_commit(&MPI_Pair);

	if (id == 0) { //Master

		freopen("speech_tokenized.txt", "r", stdin);
		freopen("out", "w", stdout);
		vector<string> v;
		string s;
		int wc;
		while (getline(std::cin, s))
			v.push_back(s);
		char *c = (char*)calloc(20 * v.size(), sizeof(char));
		char * it;
		it = c;
		wc = v.size();
		int i;
		for (i = 0; i < wc; i++) {
			strcpy(it, v[i].c_str());
			it += 20;
		}

		// Split the input to slaves then collect it again
		int avg_wc = wc / (np - 1);
		int first, last;
		for (int _id = 1; _id < np; _id++) {
			first = (_id - 1)*avg_wc;
			last = _id*avg_wc - 1;
			MPI_Send(&avg_wc, 1, MPI_INT, _id, 1234, MPI_COMM_WORLD);
			MPI_Send(&c[first * 20], 20 * avg_wc, MPI_CHAR, _id, 1234, MPI_COMM_WORLD);
		}

		pp* map = (pp*)calloc(wc, sizeof(pp));
		if (last + 1 != wc) { // Remaining input for master
			for ( i = last + 1; i < wc; i++) {
				strcpy(map[i].s, &c[i * 20]);
				map[i].k = 1;
			}
		}
		for (int _id = 1; _id < np; _id++) {
			MPI_Recv(&map[(_id - 1)*avg_wc], avg_wc, MPI_Pair, _id, 1235, MPI_COMM_WORLD, &status);
		}

		
		// Split mapped data for sorting
		for (int _id = 1; _id < np; _id++) {
			first = (_id - 1)*avg_wc;
			last = _id*avg_wc - 1;
			MPI_Send(&map[first], avg_wc, MPI_Pair, _id, 1234, MPI_COMM_WORLD);
		}

		int flag,_flag;
		vector<int> busy;
		vector<int>::iterator ii;
		for (i = 1; i < np; i++)
			busy.push_back(i);
		while (busy.size() > 1) {
			ii = busy.begin();
			while (ii != busy.end()) {
				flag = *ii;
				ii++;
				if (ii == busy.end())
					break;
				_flag = *ii;
				ii = busy.erase(ii);
				MPI_Send(&flag, 1, MPI_INT, _flag, 2222, MPI_COMM_WORLD);
			}
		}
		MPI_Recv(map, avg_wc*(np-1), MPI_Pair, 1, 9999, MPI_COMM_WORLD, &status);
		
		if (last + 1 != wc) {
			qsort(&map[last + 1], wc - last - 1, sizeof(pp), compare_pair);
			pp* u = (pp*)calloc(wc, sizeof(pp));
			int x1, x2, x3;
			x1 = 0; x2 = last + 1; x3 = 0;
			while (x1 <= last && x2 < wc) {
				if (strcmp(map[x1].s, map[x2].s) > 0) {
					strcpy(u[x3].s, map[x2].s);
					u[x3].k = 1;
					x2++;
					x3++;
				}
				else {
					strcpy(u[x3].s, map[x1].s);
					u[x3].k = 1;
					x1++;
					x3++;
				}
			}
			if (x1 == last + 1) {
				while (x2 < wc) {
					strcpy(u[x3].s, map[x2].s);
					u[x3].k = 1;
					x2++;
					x3++;
				}
			}
			else if (x2 == wc) {
				while (x1 <= last) {
					strcpy(u[x3].s, map[x1].s);
					u[x3].k = 1;
					x1++;
					x3++;
				}
			}
			free(map);
			map = u;
		}
		vector<pair<char*, int> > reduced_map;
		int x1, x2;
		x1 = 0;
		int count;
		//char* g = (char*)calloc(20, sizeof(char));
		while (x1 < wc ) {
			count = map[x1].k;
			for (x2 = x1 + 1; x2 < wc; x2++) {
				if (strcmp(map[x1].s, map[x2].s) != 0)
					break;
				count += map[x2].k;
			}
			reduced_map.push_back(make_pair(map[x1].s, count));
			x1 = x2;
		}
		for (auto i : reduced_map)
			cout << i.first <<":"<< i.second << endl;
	}
	else { //Slaves
		// Mapping words and send back to master
		int wc;
		MPI_Recv(&wc, 1, MPI_INT, MPI_ANY_SOURCE, 1234, MPI_COMM_WORLD, &status);
		char * c = (char*)calloc(20 * wc, sizeof(char));
		MPI_Recv(c, 20 * wc, MPI_CHAR, MPI_ANY_SOURCE, 1234, MPI_COMM_WORLD, &status);
		
		pp* map = (pp*)calloc(wc, sizeof(pp));
		pp* it;
		it = map;
		char* itt;
		itt = c;
		for (int i = 0; i < wc; i++) {
			strcpy(it->s, itt);
			it->k = 1;
			it++;
			itt += 20;
		}
		free(c);
		MPI_Send(map, wc, MPI_Pair, 0, 1235, MPI_COMM_WORLD);

		//Get data from master and sort it with builtin qsort
		MPI_Recv(map, wc, MPI_Pair, MPI_ANY_SOURCE, 1234, MPI_COMM_WORLD, &status);
		qsort(map, wc, sizeof(pp), compare_pair);
		if (id == 1) { //Root of the merging tree
			int p = (int) log2(np - 2) + 1; // Number of times when this process recive data for merging
			int ppp = p - 1;
			pp* map2; // Pair structures for merge operations
			pp* u; // Pair structures for merge operations
			int size; //Size of the coming data
			int x1, x2, x3; 
			while (p--) {
				map2 = (pp*)calloc(wc, sizeof(pp));
				MPI_Recv(map2, wc, MPI_Pair,id + power(ppp - p), 4444, MPI_COMM_WORLD, &status);
				MPI_Get_count(&status, MPI_Pair, &size);
				u = (pp*)calloc(wc + size, sizeof(pp));
				x1 = 0; x2 = 0; x3 = 0;
				while (x1 < wc && x2 < size) { //Merge data
					if (strcmp(map[x1].s, map2[x2].s) > 0) {
						strcpy(u[x3].s, map2[x2].s);
						u[x3].k = 1;
						x2++;
						x3++;
					}
					else {
						strcpy(u[x3].s, map[x1].s);
						u[x3].k = 1;
						x1++;
						x3++;
					}
				}
				if (x1 == wc) {
					while (x2 < size) {
						strcpy(u[x3].s, map2[x2].s);
						u[x3].k = 1;
						x2++;
						x3++;
					}
				}
				else if (x2 == size) {
					while (x1 < wc) {
						strcpy(u[x3].s, map[x1].s);
						u[x3].k = 1;
						x1++;
						x3++;
					}
				}
				free(map);
				free(map2);
				map = u;
				wc += size;

			}
			MPI_Send(map, wc, MPI_Pair, 0, 9999, MPI_COMM_WORLD); // Send back to master for reducing
			free(map);
		}
		else { //Internal nodes of mergong tree
			int p, z;
			p = 0;
			z = id - 1;
			while (z % 2 == 0 && (id-1)+power(p) <= np-2) {
				p++;
				z /= 2;
			} // P i number of times this process recive data for merging
			int ppp = p - 1;
			pp* map2; // Pair structures for merge operations
			pp* u; // Pair structures for merge operations
			int size; // Size of coming data
			int x1, x2, x3;
			while (p--) {
				map2 = (pp*)calloc(wc, sizeof(pp));
				MPI_Recv(map2, wc, MPI_Pair, id + power(ppp - p), 4444, MPI_COMM_WORLD, &status);
				MPI_Get_count(&status, MPI_Pair, &size);
				u = (pp*)calloc(wc + size, sizeof(pp));
				x1 = 0; x2 = 0; x3 = 0;
				while (x1 < wc && x2 < size) { //Merge
					if (strcmp(map[x1].s, map2[x2].s) > 0) {
						strcpy(u[x3].s, map2[x2].s);
						u[x3].k = 1;
						x2++;
						x3++;
					}
					else {
						strcpy(u[x3].s, map[x1].s);
						u[x3].k = 1;
						x1++;
						x3++;
					}
				}
				if (x1 == wc) {
					while (x2 < size) {
						strcpy(u[x3].s, map2[x2].s);
						u[x3].k = 1;
						x2++;
						x3++;
					}
				}
				else if (x2 == size) {
					while (x1 < wc) {
						strcpy(u[x3].s, map[x1].s);
						u[x3].k = 1;
						x1++;
						x3++;
					}
				}
				free(map);
				free(map2);
				map = u;
				wc += size;
				
			}
			int dest;
			MPI_Recv(&dest, 1, MPI_INT, 0, 2222, MPI_COMM_WORLD, &status);
			MPI_Send(map, wc, MPI_Pair, dest, 4444, MPI_COMM_WORLD);
			free(map);
		}
	}
	MPI_Finalize();
}