#ifndef INCLUDE_UTREE
#define INCLUDE_UTREE

#include <vector>
#include <iostream>
#include "unode.h"

using namespace std;

class utree;

// prototypes
bool build_utree(utree &t, string &s, map<string, int> &label_map, map<int, string> &reverse_label_map);
int build_utree_helper(utree &t, string &s, map<string, int> &label_map, map<int, string> & reverse_label_map, int start, unode *parent, bool &valid);
void str_subtree(stringstream &s, unode *n, unode *prev);
void find_sibling_pairs_hlpr(utree &t, map<int, int> &sibling_pairs);
map<int, int> distances_from_leaf(utree &T1, int leaf);
void distances_from_leaf_hlpr(utree &T1, map<int, int> &distances, unode *prev, unode *current, int distance);

class utree {
	protected:
		vector <unode*> internal_nodes;
		vector <unode*> leaves;
		int smallest_leaf = -1;
	public:
		// create the tree
		utree(string &newick, map<string, int> &label_map, map<int, string> &reverse_label_map) {
			internal_nodes = vector<unode *>();
			leaves = vector<unode *>();
			build_utree(*this, newick, label_map, reverse_label_map);
		}
		~utree() {
			int end = internal_nodes.size();
			for(int i = 0; i < end; i++) {
				delete internal_nodes[i];
			}
			end = leaves.size();
			for(int i = 0; i < end; i++) {
				if (leaves[i] != NULL) {
					delete leaves[i];
				}
			}
		}
	friend ostream& operator<<(ostream &os, const utree& t);

	 int add_internal_node(unode *foo) {
		int label = -(internal_nodes.size() + 2);
		 unode f = unode(label);
		internal_nodes.push_back( new unode(label));
		return label;
	}

	int add_leaf(int label) {
		int start = leaves.size() + 1;
		if (leaves.size() <= label) {
			leaves.resize(label+1);
		}
		for (int i = start; i < label; i++) {
			leaves[i] = NULL;
		}
		leaves[label] = new unode(label);
		return label;
	}

	 unode *get_internal_node(int label) {
		 return internal_nodes[-(label) - 2];
	 }

	 vector<unode *> &get_internal_nodes() {
		 return internal_nodes;
	 }

	 unode *get_leaf(int label) {
		 return leaves[label];
	 }

	 unode *get_node(int label) {
		 if (label < 0) {
			 return get_internal_node(label);
		 }
		 else {
			 return get_leaf (label);
		 }
	 }

	int num_leaves() const {
		return leaves.size();
	}

	string str() const {
		stringstream s;
		int start = smallest_leaf;
		if (start == -1) {
			return "empty tree";
		}
		unode *root = leaves[start]->get_neighbors().front();
		str_subtree(s, root, root);
		return s.str();
	 }

	list<int> find_leaves() {
		list<int> leaf_list = list<int>();
		for (unode *i : leaves) {
			if (i != NULL) {
				leaf_list.push_back(i->get_label());
			}
		}
		return leaf_list;
	}

	map<int, int> find_sibling_pairs() {
		map<int, int> sibling_pairs = map<int, int>();
		find_sibling_pairs_hlpr(*this, sibling_pairs);
		return sibling_pairs;
	}

	void set_smallest_leaf(int l) {
		smallest_leaf = l;
	}

	int get_smallest_leaf() {
		return smallest_leaf;
	}

	void root() {
		root(smallest_leaf);
	}

	void root(int l) {
		unode *n = get_leaf(l);
		if (n != NULL) {
			n->root(n->get_label());
		}
	}

	void root(unode *n) {
		if (n != NULL) {
			n->root(n->get_label());
		}
	}
};

ostream& operator<<(ostream &os, const utree& t) {
	os << t.str() << ";";
	return os;
}

bool build_utree(utree &t, string &s, map<string, int> &label_map, map<int, string> &reverse_label_map) {
	bool valid = true;
	unode dummy = unode(-1);
	build_utree_helper(t, s, label_map, reverse_label_map, 0, &dummy, valid);
	int end = t.num_leaves();
	int start = -1;
	for(int i = 0; i < end; i++) {
		if (t.get_leaf(i) != NULL) {
			start = i;
			break;
		}
	}
	t.set_smallest_leaf(start);
	return valid;
}

int build_utree_helper(utree &t, string &s, map<string, int> &label_map, map<int, string> &reverse_label_map, int start, unode *parent, bool &valid) {
	// next special char
	int loc = s.find_first_of("(,)", start);
	if (loc == string::npos) {
		return loc;
	}
	if (s[loc] != '(') {
		// leaf
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
			label = label_map.size();
			label_map.insert(make_pair(name, label));
			reverse_label_map.insert(make_pair(label, name));
		}
		unode *new_node = t.get_leaf(t.add_leaf(label));
		parent->add_neighbor(new_node);
		new_node->add_neighbor(parent);
	}
	else {
		// internal node
	int l = t.add_internal_node(parent);
		unode *new_node = t.get_internal_node(l);
		if (parent->get_label() != -1) {
			new_node->add_neighbor(parent);
			parent->add_neighbor(new_node);
		}
		loc = build_utree_helper(t, s, label_map, reverse_label_map, loc + 1, new_node, valid);
		while(s[loc] == ',') {
			loc = build_utree_helper(t, s, label_map, reverse_label_map, loc + 1, new_node, valid);
		}
		if (s[loc] != ')') {
			valid = false;
			return s.size()-1;
		}
		loc++;
	}
	return loc;
}

void str_subtree(stringstream &s, unode *n, unode *prev) {
	// only leaf labels
	if (n->get_label() >= 0) {
		s << n->str();
	}
	list<unode *>::const_iterator i;
	const list<unode *> &cn = n->const_neighbors();
	int count = 0;
	for(unode *i : n->const_neighbors()) {
		if ((*i).get_label() != prev->get_label()) {
			if (count == 0) {
				s << "(";
			}
			else {
				s << ",";
			}
			count++;
			str_subtree(s, i, n);
		}
	}
	if (count > 0) {
		s << ")";
	}
}

void find_sibling_pairs_hlpr(utree &t, map<int, int> &sibling_pairs) {
	int i = 3;
	for(int l : t.find_leaves()) {
		unode *n = t.get_leaf(l);
		unode *p = n->get_neighbors().front();
		for (unode *u : p->get_neighbors()) {
			int ul = u->get_label();
			if (u->is_leaf() && ul > l) {
				sibling_pairs.insert(make_pair(l,ul));
				sibling_pairs.insert(make_pair(ul,l));
			}
		}
	}
	return;
}

map<int, int> distances_from_leaf(utree &T1, int leaf) {
	map<int, int> distances = map<int, int>();
	unode *node = T1.get_leaf(leaf);
	distances_from_leaf_hlpr(T1, distances, node, node, 0);
	return distances;
}

void distances_from_leaf_hlpr(utree &T1, map<int, int> &distances, unode *prev, unode *current, int distance) {
	distances.insert(make_pair(current->get_label(), distance));
	for(unode *n : current->get_neighbors()) {
		if (n != prev) {
			distances_from_leaf_hlpr(T1, distances, current, n, distance+1);
		}
	}
}

#endif
