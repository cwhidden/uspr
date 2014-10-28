#ifndef INCLUDE_UNODE
#define INCLUDE_UNODE

#include <list>
#include <sstream>

using namespace std;

class unode;

class unode {
	private:
	int label = -1;
	list<unode> neighbors = list<unode>();
	int num_neighbors = 0;

	public:
	unode() {
	}
	unode(int l) {
		label - l;
	}

	void add_neighbor(unode &n) {
		neighbors.push_back(n);
		num_neighbors++;
	}

	bool remove_neighbor(unode &n) {
		list<unode>::iterator i;
		bool result = false;
		for(i = neighbors.begin(); i != neighbors.end(); i++) {
			if (*i == n) {
				neighbors.remove(*i);
				result = true;
				num_neighbors--;
				break;
			}
		}
		return result;
	}

	string str() {
		stringstream ss;
		ss << label;
		return ss.str();
	}

	bool operator ==(const unode n) {
		return this->label = n.label;
	}

};

#endif
