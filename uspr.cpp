// TODO: basic data structures
// TODO: mAF iteration
// TODO: mEAF iteration
// TODO: USPR distance


// includes
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include "utree.h"
#include "unode.h"
#include "tbr.h"

using namespace std;

// constants
//
string USAGE =
"uspr, version 0.0.1\n";



// function prototypes


int main(int argc, char *argv[]) {
	int max_args = argc-1;
	while (argc > 1) {
		char *arg = argv[--argc];
		if (strcmp(arg, "--help") == 0 ||
				strcmp(arg, "-h") == 0 ||
				strcmp(arg, "-help") == 0) {
			cout << USAGE;
			return 0;
		}
	}

	// label maps to allow string labels
	map<string, int> label_map= map<string, int>();
	map<int, string> reverse_label_map = map<int, string>();

	// set random seed
	srand(unsigned(time(0)));

	// read input trees
	string T1_line = "";
	string T2_line = "";
	while (getline(cin, T1_line) && getline(cin, T2_line)) {
		// load into data structures
		utree T1 = utree(T1_line, label_map, reverse_label_map);
		utree T2 = utree(T2_line, label_map, reverse_label_map);
		// compute TBR distance
		int distance = tbr_distance(T1, T2);
		cout << T1 << endl;
		cout << T2 << endl;
		cout << distance << endl;
	}
}
