#ifndef INCLUDE_TBR
#define INCLUDE_TBR

class nodemapping;

// function prototypes
int tbr_distance(uforest &F1, uforest &F2);
int tbr_distance_hlpr(uforest &F1, uforest &F2, int k, nodemapping &twins, map<int, int> &sibling_pairs);

class nodemapping {
	private:
		map <int,int> forward;
		map <int,int> backward;
	public:
		nodemapping(list<int> &leaves) {
			for(int l : leaves) {
				forward.insert(make_pair(l,l));
				backward.insert(make_pair(l,l));
			}
		}
		void add(int l1, int l2) {
			forward.insert(make_pair(l1, l2));
			backward.insert(make_pair(l2, l1));
		}
		int get_forward(int l) {
			map<int, int>::iterator result = forward.find(l);
			if (result != forward.end()) {
				return result->second;
			}
			else {
				return -1;
			}
		}
		int get_backward(int l) {
			map<int, int>::iterator result = backward.find(l);
			if (result != backward.end()) {
				return result->second;
			}
			else {
				return -1;
			}
		}
};

// compute the tbr distance
//
int tbr_distance(uforest &F1, uforest &F2) {

	for(int k = 0; k < 10; k++) {
		if (k > 0) {
			cout << " ";
		}
		cout << "<" << k << ">";

		// remaining leaves and their mappings
		list<int> leaves = F1.find_leaves();
		nodemapping twins = nodemapping(leaves);

		// sibling pairs
		map<int,int> sibling_pairs = F1.find_sibling_pairs();

		// "root" the trees
		// TODO: make this normalize "leaves" as well
		F1.root();
		F2.root();

		// TODO: use distance from root to speed up the approximation
//		map<int,int> distances = 


		// test k
		int result = tbr_distance_hlpr(F1, F2, k, twins, sibling_pairs);

	}

	return -1;
}

int tbr_distance_hlpr(uforest &F1, uforest &F2, int k, nodemapping &twins, map<int, int> &sibling_pairs) {

	if (k < 0) {
		return -1;
	}

	while (!sibling_pairs.empty()) {
		// get sibling pair (a,c) in F1
		map<int, int>::iterator spi = sibling_pairs.begin();
		unode *F1_a = F1.get_node(spi->first);
		unode *F1_c = F1.get_node(spi->second);
		sibling_pairs.erase(F1_a->get_label());
		sibling_pairs.erase(F1_c->get_label());

		cout << "F1_a: " << F1_a->get_label() << endl;
		cout << "F1_c: " << F1_c->get_label() << endl;

		// find a and c in F2
		unode *F2_a = F2.get_node(twins.get_forward(F1_a->get_label()));
		unode *F2_c = F2.get_node(twins.get_forward(F1_c->get_label()));

		cout << "F2_a: " << F2_a->get_label() << endl;
		cout << "F2_c: " << F2_c->get_label() << endl;

		// find pendant edges between a and c in F2
		// TODO: need distances from "root" to do this efficiently

		cout << endl;
	}

	// if (sib pair list is not empty, k>=0) {
		// get next sibling pair (a,c) from F1
		// find a and c in F2
		// set a to be the "lower" of the pair
		// find path between a and c, if it exists
		//
		// TODO: optimization if q=2 then we don't need to cut a or c
		// make this an option (we may need all mAFs)
		// cases:
		// 	case a (unless q=2)
		// 	case b (unless sep comps) set b_only flag
		// 	case d (unless sep comps) set b_only flag
		// 	case c (unless q=2) protect a
		// note: need to copy the tree, todo list, and done list for each
		// }
		// else {
		// 	if (k < 0) {
		// 		return false
		// 	}
		// 	else {
		// 		return AF
		// 	}
		// }
		//
		// note: need to maintain component representatives when cutting and merging (initially smallest leaf)

	return k;
}

#endif
