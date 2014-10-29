#ifndef INCLUDE_UTREE
#define INCLUDE_UTREE

#include "unode.h"

using namespace std;

class utree;

// prototypes
bool build_utree(utree &t, string &s, map<string, int> &label_map, map<int, string> &reverse_label_map);
int build_utree_helper(utree &t, string &s, map<string, int> &label_map, map<int, string> & reverse_label_map, int start, unode &parent, bool &valid);
void str_subtree(stringstream &s, const unode &n, const unode &prev);

class utree {
	private:
		vector<unode> internal_nodes;
		vector<unode> leaves;
	public:
		// create the tree
		utree(string &newick, map<string, int> &label_map, map<int, string> &reverse_label_map) {
			internal_nodes = vector<unode>();
			leaves = vector<unode>();
			cout << "building: " << newick << endl;
			build_utree(*this, newick, label_map, reverse_label_map);
		}
	friend ostream& operator<<(ostream &os, const utree& t);

	 int add_internal_node() {
		int label = -(internal_nodes.size() + 2);
		internal_nodes.push_back(unode(label));
		cout << "adding internal node with label: " << label << endl;
		return label;
	}

	int add_leaf(int label) {
		cout << "adding leaf " << label << endl;
		cout << leaves.size() << endl;
		if (leaves.size() <= label) {
			leaves.resize(label+1);
		}
		cout << leaves.size() << endl;
		leaves[label] = unode(label);
		cout << leaves[label].str() << endl;
		return label;
	}

	 unode &get_internal_node(int label) {
		 return internal_nodes[-label - 2];
	 }

	 unode &get_leaf(int label) {
		 return leaves[label];
	 }

	int num_leaves() const {
		return leaves.size();
	}

	string str() const {
		stringstream s;
		int start = -1;
		int end = num_leaves();
		for(int i = 0; i < end; i++) {
			if (leaves[i].get_label() != -1) {
				start = i;
				break;
			}
		}
		if (start == -1) {
			return "empty tree";
		}
		str_subtree(s, leaves[0], leaves[0]);
		return s.str();
	 }
};

ostream& operator<<(ostream &os, const utree& t) {
	os << t.str() << endl;
	return os;
}

bool build_utree(utree &t, string &s, map<string, int> &label_map, map<int, string> &reverse_label_map) {
	unode &root = t.get_internal_node(t.add_internal_node());
	bool valid = true;
	build_utree_helper(t, s, label_map, reverse_label_map, 0, root, valid);
	return valid;
}

int build_utree_helper(utree &t, string &s, map<string, int> &label_map, map<int, string> &reverse_label_map, int start, unode &parent, bool &valid) {
	// next special char
	cout << "build_u_tree_helper(" << start << ")" << endl;
	int loc = s.find_first_of("(,)", start);
	if (loc == string::npos) {
		return loc;
	}
	cout << "\t" << s[loc] << endl;
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
		cout << "\t" << "leaf: " << name << ", " << label << endl;
		cout << t.num_leaves() << endl;
		unode &new_node = t.get_leaf(t.add_leaf(label));
		cout << "foo1" << endl;
		parent.add_neighbor(&new_node);
		cout << "foo2" << endl;
		cout << parent.get_label() << endl;
		cout << new_node.get_label() << endl;
		new_node.add_neighbor(&parent);
		cout << "foo3" << endl;
	}
	else {
		// internal node
		unode &new_node = t.get_internal_node(t.add_internal_node());
		loc = build_utree_helper(t, s, label_map, reverse_label_map, loc + 1, new_node, valid);
		while(s[loc] == ',') {
			loc = build_utree_helper(t, s, label_map, reverse_label_map, loc + 1, new_node, valid);
		}
		if (s[loc] != ')') {
			valid = false;
			return s.size()-1;
		}
	}
	return loc;
}

void str_subtree(stringstream &s, const unode &n, const unode &prev) {
	s << n.str();
	cout << "str_subtree" << endl;
	cout << "<" << n.get_label() << ">" << endl;
	cout << s.str() << endl;
	list<unode *>::const_iterator i;
	const list<unode *> &cn = n.const_neighbors();
	for(i = cn.begin(); i != cn.end(); i++) {
		if (**i != prev) {
			str_subtree(s, **i, n);
		}
	}
}

#endif
