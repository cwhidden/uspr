#ifndef INCLUDE_UTREE
#define INCLUDE_UTREE

#include <vector>
#include <iostream>
#include "unode.h"

using namespace std;

class utree;

// options
bool KEEP_LABELS = false;

// prototypes
bool build_utree(utree &t, string &s, map<string, int> &label_map, map<int, string> &reverse_label_map);
int build_utree_helper(utree &t, string &s, map<string, int> &label_map, map<int, string> & reverse_label_map, int start, unode *parent, bool &valid);
void find_sibling_pairs_hlpr(utree &t, map<int, int> &sibling_pairs);
map<int, int> distances_from_leaf(utree &T1, int leaf);
void distances_from_leaf_hlpr(utree &T1, map<int, int> &distances, unode *prev, unode *current, int distance);
void distances_from_leaf_decorator(utree &T1, int leaf);
void distances_from_leaf_decorator_hlpr(utree &T1, unode *prev, unode *current, int distance);

class utree {
	protected:
		vector <unode*> internal_nodes;
		vector <unode*> leaves;
		int smallest_leaf;
	public:
		// create the tree
		utree(string &newick, map<string, int> &label_map, map<int, string> &reverse_label_map) {
			internal_nodes = vector<unode *>();
			leaves = vector<unode *>();
			build_utree(*this, newick, label_map, reverse_label_map);
		}
		utree(const utree &T) {
			// copy vectors of pointers
			internal_nodes = vector<unode *>(T.internal_nodes.size());
			leaves = vector<unode *>(T.leaves.size());
			smallest_leaf = T.smallest_leaf;
			// create new nodes
			for(int i = 0; i < T.internal_nodes.size(); i++) {
				if (T.internal_nodes[i] != NULL) {
				//	cout << "internal_nodes[" << i << "]" << endl;
					internal_nodes[i] = new unode(*(T.internal_nodes[i]));
				//	cout << "\t" << T.internal_nodes[i] << ", " << internal_nodes[i] << endl;
				}
			}
			for(int i = 0; i < T.leaves.size(); i++) {
				if (T.leaves[i] != NULL) {
				//	cout << "leaves[" << i << "]" << endl;
					leaves[i] = new unode(*(T.leaves[i]));
				//	cout << "\t" << T.leaves[i] << ", " << leaves[i] << endl;
				}
			}
			// update neighbor pointers
			for(int i = 0; i < internal_nodes.size(); i++) {
				//	cout << "neighborhood of internal_nodes[" << i << "]" << endl;
				if (internal_nodes[i] != NULL) {
					list<unode *> old_neighbors = T.internal_nodes[i]->get_neighbors();
					internal_nodes[i]->clear_neighbors();
				//	cout << "\t";
					for (unode *u : old_neighbors) {
				//		cout << u << ", ";
						internal_nodes[i]->add_neighbor(get_node(u->get_label()));
					}
					internal_nodes[i]->clear_contracted_neighbors();
					list<unode *> old_contracted_neighbors = T.internal_nodes[i]->get_contracted_neighbors();
				//	cout << "\t";
					for (unode *u : old_contracted_neighbors) {
				//		cout << u << ", ";
						internal_nodes[i]->add_contracted_neighbor(get_node(u->get_label()));
					}
				//	cout << endl;
				//	cout << "\t";
				//	for (unode *u : internal_nodes[i]->get_neighbors()) {
				//		cout << u << ", ";
				//	}
				//	cout << endl;
				}
			}
			for(int i = 0; i < leaves.size(); i++) {
				//	cout << "neighborhood of leaves[" << i << "]" << endl;
				if (leaves[i] != NULL) {
					list<unode *> old_neighbors = T.leaves[i]->get_neighbors();
					leaves[i]->clear_neighbors();
				//	cout << "\t";
					for (unode *u : old_neighbors) {
				//		cout << u << ", ";
						leaves[i]->add_neighbor(get_node(u->get_label()));
					}
					list<unode *> old_contracted_neighbors = T.leaves[i]->get_contracted_neighbors();
				//	cout << "\t";
					leaves[i]->clear_contracted_neighbors();
					for (unode *u : old_contracted_neighbors) {
				//		cout << u << ", ";
						leaves[i]->add_contracted_neighbor(get_node(u->get_label()));
					}
				//	cout << endl;
				//	cout << "\t";
				//	for (unode *u : leaves[i]->get_neighbors()) {
				//		cout << u << ", ";
				//	}
				//	cout << endl;
				}
			}
		}
		~utree() {
			int end = internal_nodes.size();
			for(int i = 0; i < end; i++) {
				if (internal_nodes[i] != NULL) {
				delete internal_nodes[i];
				}
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

	 vector<unode *> &get_leaves() {
		 return leaves;
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

	string str_subtree(unode *n) {
		stringstream ss;
		str_subtree(ss, n, n->get_parent());
		return ss.str();
	}

	void str_subtree(stringstream &s, unode *n, unode *prev) const {
		// only leaf labels
		if (n->get_label() >= 0) {
			s << n->str();
		}
		list<unode *>::const_iterator i;
		const list<unode *> &cn = n->const_neighbors();
		int count = 0;
		bool has_contracted = false;
		for(unode *i : n->const_neighbors()) {
			if (prev == NULL || (*i).get_label() != prev->get_label()) {
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
		for(unode *i : n->const_contracted_neighbors()) {
			if (prev == NULL || (*i).get_label() != prev->get_label()) {
				if (count == 0) {
					s << "<";
				}
				else {
					s << ",";
				}
				count++;
				has_contracted = true;
				str_subtree(s, i, n);
			}
		}
		if (has_contracted) {
			s << ">";
		}
		else if (count > 0) {
			s << ")";
		}
	}

	void str_subtree_with_depths(stringstream &s, unode *n, unode *prev) const {
		// only leaf labels
		if (n->get_label() >= 0) {
			s << n->str();
		}
		list<unode *>::const_iterator i;
		const list<unode *> &cn = n->const_neighbors();
		int count = 0;
		bool has_contracted = false;
		for(unode *i : n->const_neighbors()) {
			if (prev == NULL || (*i).get_label() != prev->get_label()) {
				if (count == 0) {
					s << "(";
				}
				else {
					s << ",";
				}
				count++;
				str_subtree_with_depths(s, i, n);
			}
		}
		for(unode *i : n->const_contracted_neighbors()) {
			if (prev == NULL || (*i).get_label() != prev->get_label()) {
				if (count == 0) {
					s << "<";
				}
				else {
					s << ",";
				}
				count++;
				has_contracted = true;
				str_subtree_with_depths(s, i, n);
			}
		}
		if (has_contracted) {
			s << ">";
		}
		else if (count > 0) {
			s << ")";
		}
		s << ":" << n->get_distance();
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
	unode *root = dummy.get_parent();
	root->remove_neighbor(&dummy);
	root->contract();



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
			//cout << "label=" << label << endl;
			if (KEEP_LABELS) {
				label = atoi(name.c_str());
				//cout << "label=" << label << endl;
			}
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
		loc = build_utree_helper(t, s, label_map, reverse_label_map, loc + 1, new_node, valid);
		while(s[loc] == ',') {
			loc = build_utree_helper(t, s, label_map, reverse_label_map, loc + 1, new_node, valid);
		}
		if (s[loc] != ')') {
			valid = false;
			return s.size()-1;
		}
	//	if (parent->get_label() != -1) {
			new_node->add_neighbor(parent);
			parent->add_neighbor(new_node);
	//	}
		loc++;
	}
	return loc;
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

void distances_from_leaf_decorator(utree &T1, int leaf) {
	unode *node = T1.get_leaf(leaf);
	distances_from_leaf_decorator_hlpr(T1, node, node, 0);
}

void distances_from_leaf_decorator_hlpr(utree &T1, unode *prev, unode *current, int distance) {
	current->set_distance(distance);
	for(unode *n : current->get_neighbors()) {
		if (n != prev) {
			distances_from_leaf_decorator_hlpr(T1, current, n, distance+1);
		}
	}
}

#endif
