#ifndef INCLUDE_TBR
#define INCLUDE_TBR

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
		cout << k;

		// remaining leaves and their mappings
		list<int> leaves = F1.find_leaves();
		nodemapping twins = nodemapping(leaves);

	}

	// sync leaves
	//	* keep in a pair of vectors based on node labels for easy copying
	//
	// put in todo list
	// initialize done list
	// "root" and calculate distances for efficiency 
	// find sib pairs
	// call helper with k=0, 1, 2, ... (eventually start from approx)
	//
	//
	// helper
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

	return -1;
}

#endif
