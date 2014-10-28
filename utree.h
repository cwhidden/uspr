#ifndef INCLUDE_UTREE
#define INCLUDE_UTREE

#include "unode.h"

using namespace std;

class utree {
	private:
		vector<unode> nodes = vector<unode>();
		int num_leaves = 0;
	public:
		utree(string newick) {
			// create the tree
			cout << newick << endl;
		}
	friend ostream& operator<<(ostream &os, const utree& t);
};

ostream& operator<<(ostream &os, const utree& t) {
	os << "not implemented" << endl;
	return os;
}

#endif
