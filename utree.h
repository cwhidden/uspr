#ifndef INCLUDE_UTREE
#define INCLUDE_UTREE

#include "unode.h"

using namespace std;

class utree;

// prototypes
void build_utree(utree &t, string &s, map<string, int> &label_map, map<int, string> &reverse_label_map);
void build_utree_helper(utree &t, string &s, map<string, int> &label_map, map<int, string> & reverse_label_map, int start, unode &parent);

class utree {
	private:
		vector<unode> internal_nodes = vector<unode>();
		vector<unode> leaves = vector<unode>();
		int num_leaves = 0;
	public:
		// create the tree
		utree(string &newick, map<string, int> &label_map, map<int, string> &reverse_label_map) {
			build_utree(*this, newick, label_map, reverse_label_map);
			cout << newick << endl;
		}
	friend ostream& operator<<(ostream &os, const utree& t);

	 int add_internal_node() {
		int label = -(internal_nodes.size() + 2);
		internal_nodes.push_back(unode(label));
		return label;
	}

	int add_leaf(int label) {
		if (leaves.size() < label) {
			leaves.resize(label);
		}
		leaves[label] = unode(label);
	}

	 unode &get_internal_node(int label) {
		 return internal_nodes[-(label + 2)];
	 }

	 unode &get_leaf(int label) {
		 return leaves[label];
	 }
};

ostream& operator<<(ostream &os, const utree& t) {
	os << "not implemented" << endl;
	return os;
}

void build_utree(utree &t, string &s, map<string, int> &label_map, map<int, string> &reverse_label_map) {
	unode &root = t.get_internal_node(t.add_internal_node());
	build_utree_helper(t, s, label_map, reverse_label_map, 0, root);
}

void build_utree_helper(utree &t, string &s, map<string, int> &label_map, map<int, string> & reverse_label_map, int start, unode &parent) {
	// next special char
	int loc = s.find_first_of("(,)", start);
	if (loc == string::npos) {
		return loc;
	}
	else if (s[loc] == '(') {
		// internal node
	}
	else {
		// leaf
		// node label
		// ignore white space
		while(s[start] == ' ' || s[start] == '\t')
			start++;
		int end = loc;
		while(s[end] == ' ' || s[end] == '\t')
			end--;
		string name = s.substr(start, end - start);
		map<string, int>::iterator m = label_map.find(name);
		int label = -1;
		if (m != label_map.end()) {
			label = m->second;
		}
		else {
			// TODO
		}

	}
}

#endif
