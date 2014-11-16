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
		uforest(const uforest &F) : utree(F) {
			// copy vector of pointers
			components = vector<unode *>(F.components.size());
			// update with the new nodes
			for(int i = 0; i < F.components.size(); i++) {
//				cout << "components[" << i << "]" << endl;
				components[i] = get_node(F.components[i]->get_label());
//				cout << "\t" << F.components[i] << ", " << components[i] << endl;
			}
		}
		string str() const {
			stringstream ss;
			for(int i = 0; i < components.size(); i++) {
				if (i > 0) {
					ss << " ";
				}
				unode *root = components[i];
				if (root->get_label() > -1) {
					if (root->is_leaf()) {
						root = root->get_neighbors().front();
					}
					else if (!(root->get_contracted_neighbors().empty())){
						root = root->get_contracted_neighbors().front();
					}
				}
				str_subtree(ss, root, root);
				ss << ";";
			}
			return ss.str();
		}
		friend ostream& operator<<(ostream &os, const uforest& f);

		pair<int,int> cut_edge(int x, int y) {
			unode *X, *Y;
			X = get_node(x);
			Y = get_node(y);
			cout << "d_X: " << X->get_distance() << endl;
			cout << "d_Y: " << Y->get_distance() << endl;
			bool swapped = false;
			if (Y->get_distance() > X->get_distance()) {
				cout << "AHH!" << endl;
				X = get_node(y);
				Y = get_node(x);
				swapped = true;
			}
			X->remove_neighbor(Y);
			Y->remove_neighbor(X);

			unode *Xprime = X->contract();
			unode *Yprime = Y->contract();


			if (Xprime->get_component() > -1) {
				cout << "boo" << endl;
				add_component(Yprime);
				update_component(Xprime->get_component(), Xprime);
			}
			else {
				cout << "urns" << endl;
				add_component(Xprime);
				if (Yprime->get_component() > -1) {
					update_component(Yprime->get_component(), Yprime);
				}
			}
			if (swapped) {
				return make_pair(Yprime->get_label(),Xprime->get_label());
			}
			else {
				return make_pair(Xprime->get_label(),Yprime->get_label());
			}
		}

		void update_component(int c, int l) {
			components[c] = get_node(l);
		}

		void update_component(int c, unode *n) {
			components[c] = n;
		}

		void add_component(unode *C) {
			C->set_component(components.size());
			components.push_back(C);
		}
};

ostream& operator<<(ostream &os, const uforest& f) {
	os << f.str();
	return os;
}

#endif
