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
#include "uspr.h"

using namespace std;

// constants
//
bool IGNORE_ORIGINAL = false;
bool SIZE_ONLY = false;

string USAGE =
"uspr_neighbors, version 0.0.1\n";


// function prototypes


int main(int argc, char *argv[]) {
	int max_args = argc-1;
	while (argc > 1) {
		char *arg = argv[--argc];
		if (strcmp(arg, "--size_only") == 0) {
			SIZE_ONLY = true;
		}
		else if (strcmp(arg, "--ignore_original") == 0) {
			IGNORE_ORIGINAL = true;
		}
		else if (strcmp(arg, "--help") == 0 ||
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
	string T_line = "";
	while (getline(cin, T_line)) {
		// load into data structures
		uforest F1 = uforest(T_line, &label_map, &reverse_label_map);
		F1.normalize_order();
		if (!IGNORE_ORIGINAL) {
			cout << F1.str(false, &reverse_label_map) << endl;
		}

		// compute neighbors
		list<utree> neighbors = get_neighbors(&F1);
		if (SIZE_ONLY) {
			int size = neighbors.size();
			cout << size << " neighbors" << endl;
		}
		else {
			for (utree T: neighbors) {
				cout << T.str(false, &reverse_label_map) << endl;
			}
		}
	}
}
