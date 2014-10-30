#ifndef INCLUDE_UFOREST
#define INCLUDE_UFOREST

#include <iostream>
#include "unode.h"
#include "utree.h"

class uforest: public utree {
	private:
		vector<unode *> components;

	public:
		uforest(string &newick, map<string, int> &label_map, map<int, string> &reverse_label_map) : utree(newick, label_map, reverse_label_map) {
			components = vector<unode *>();
			if (leaves.size() > 0) {
				components.push_back(leaves[0]);
				leaves[0]->set_component(0);
			}
		}
		string str() const {
			stringstream ss;
			for(int i = 0; i < components.size(); i++) {
				if (i > 0) {
					ss << " ";
				}
				unode *root = components[i]->get_neighbors().front();
				str_subtree(ss, root, root);
			}
			return ss.str();
		}
		friend ostream& operator<<(ostream &os, const uforest& f);
};

ostream& operator<<(ostream &os, const uforest& f) {
	os << f.str() << ";";
	return os;
}

#endif
