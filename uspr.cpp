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
#include <ctime>
#include <cstdlib>
#include "utree.h"
#include "unode.h"
#include "uforest.h"
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
		if (strcmp(arg, "--keep-labels") == 0) {
			KEEP_LABELS = true;
				cout << "KEEP_LABELS=true" << endl;
		}
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
		uforest F1 = uforest(T1_line, label_map, reverse_label_map);
		uforest F2 = uforest(T2_line, label_map, reverse_label_map);
		cout << "T1: " << F1 << endl;
		cout << "T2: " << F2 << endl;
		// compute TBR distance
//		cout << "a_TBR high upper bound = " << tbr_high_upper_bound(F1, F2) << endl;
//		cout << "a_TBR high lower bound = " << tbr_high_lower_bound(F1, F2) << endl;
//		cout << "a_TBR low upper bound = " << tbr_low_upper_bound(F1, F2) << endl;
//		cout << "a_TBR low lower bound = " << tbr_low_lower_bound(F1, F2) << endl;
		cout << "a_TBR: " << tbr_high_lower_bound(F1, F2) << " <= d_TBR <= " << tbr_low_upper_bound(F1, F2) << endl;
		int distance = tbr_distance(F1, F2, &print_mAFs);
		cout << "d_TBR = " << distance << endl;
	}
}
