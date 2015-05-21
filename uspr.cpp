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

bool DEFAULT_OPTIMIZATIONS = true;
bool PRINT_mAFS = false;
bool COUNT_mAFS = false;

bool ALL_DISTANCES = true;
bool TBR_APPROX = false;
bool TBR = false;
bool REPLUG = false;



// function prototypes


int main(int argc, char *argv[]) {
	int max_args = argc-1;
	while (argc > 1) {
		char *arg = argv[--argc];
		if (strcmp(arg, "--print-mAFs") == 0) {
			PRINT_mAFS = true;
		}
		if (strcmp(arg, "--count-mAFs") == 0) {
			COUNT_mAFS = true;
		}
		else if (strcmp(arg, "--keep-labels") == 0) {
			KEEP_LABELS = true;
				cout << "KEEP_LABELS=true" << endl;
		}
		else if (strcmp(arg, "--no-opt") == 0) {
			DEFAULT_OPTIMIZATIONS = false;
			cout << "NO OPTIMIZATIONS" << endl;
		}
		else if (strcmp(arg, "--no-protect-b") == 0) {
			OPTIMIZE_PROTECT_B = false;
		}
		else if (strcmp(arg, "--tbr-approx") == 0) {
			TBR_APPROX = true;
			ALL_DISTANCES = false;
		}
		else if (strcmp(arg, "--tbr") == 0) {
			TBR = true;
			ALL_DISTANCES = false;
		}
		else if (strcmp(arg, "--replug") == 0) {
			REPLUG = true;
			ALL_DISTANCES = false;
		}
		else if (strcmp(arg, "--help") == 0 ||
				strcmp(arg, "-h") == 0 ||
				strcmp(arg, "-help") == 0) {
			cout << USAGE;
			return 0;
		}
	}

	if (DEFAULT_OPTIMIZATIONS == false) {
		OPTIMIZE_2B = false;
		OPTIMIZE_PROTECT_A = false;
		OPTIMIZE_PROTECT_B = false;
		OPTIMIZE_BRANCH_AND_BOUND = false;
		cout << "NO OPTIMIZATIONS" << endl;
	}

	if (ALL_DISTANCES) {
		TBR_APPROX = true;
		TBR = true;
		REPLUG = true;
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
		cout << "T1: " << F1.str(false, &reverse_label_map) << endl;
		cout << "T2: " << F2.str(false, &reverse_label_map) << endl;
		// compute TBR distance
		if (TBR_APPROX) {
		cout << "a_TBR: " << tbr_high_lower_bound(F1, F2) << " <= d_TBR <= " << tbr_low_upper_bound(F1, F2) << endl;
		}
		if (TBR) {
			int distance = tbr_distance(F1, F2, true);
			cout << "d_TBR = " << distance << endl;
		}
		int count;
		if (PRINT_mAFS) {
			count = tbr_print_mAFs(F1, F2);
			cout << count << " mAFs" << endl;
		}
		else if (COUNT_mAFS) {
			count = tbr_count_mAFs(F1, F2);
			cout << count << " mAFs" << endl;
		}

		if (REPLUG) {
			int d_replug = replug_distance(F1, F2, true);
			cout << "d_R = " << d_replug << endl;
		}



	}
}
