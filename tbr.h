#ifndef INCLUDE_TBR
#define INCLUDE_TBR

class nodemapping;

// function prototypes
int tbr_distance(uforest F1, uforest F2);
int tbr_distance_hlpr(uforest &F1, uforest &F2, int k, nodemapping &twins, map<int, int> &sibling_pairs, list<int> &singletons);
list<pair<int,int> > find_pendants(unode *a, unode *c);

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
int tbr_distance(uforest F1, uforest F2) {

	for(int k = 0; k < 5; k++) {
		if (k > 0) {
			cout << " ";
		}
		cout << "<" << k << ">";

		// remaining leaves and their mappings
		list<int> leaves = F1.find_leaves();
		nodemapping twins = nodemapping(leaves);

		// sibling pairs
		map<int,int> sibling_pairs = F1.find_sibling_pairs();

		// singletons
		list<int> singletons = list<int>();

		// "root" the trees
		// TODO: make this normalize "leaves" as well
		F1.root(F1.get_smallest_leaf());
		F2.root(F2.get_smallest_leaf());

		// set leaves as terminal
		for(unode *u : F1.get_leaves()) {
			u->set_terminal(true);
		}
		for(unode *u : F2.get_leaves()) {
			u->set_terminal(true);
		}

		distances_from_leaf_decorator(F1, F1.get_smallest_leaf());
		distances_from_leaf_decorator(F2, F2.get_smallest_leaf());
		cout << endl;
		cout << F1 << endl;
		for(int i : F1.find_leaves()) {
			cout << i << ": " << F1.get_node(i)->get_distance() << endl;
		}

		cout << F2 << endl;
		for(int i : F2.find_leaves()) {
			cout << i << ": " << F2.get_node(i)->get_distance() << endl;
		}


		// test k
		int result = tbr_distance_hlpr(F1, F2, k, twins, sibling_pairs, singletons);
		if (result >= 0) {
			return k - result;
		}

	}

	return -1;
}

int tbr_distance_hlpr(uforest &F1, uforest &F2, int k, nodemapping &twins, map<int, int> &sibling_pairs, list<int> &singletons) {

	if (k < 0) {
		return -1;
	}

	cout << "tbr_distance_hlpr(" << k << ")" << endl;

	// if (sib pair list is not empty, k>=0) {
	while (!sibling_pairs.empty()) {

		// Case 1 : Isolated Subtree
		while (!singletons.empty()) {

			cout << "Case 1" << endl;

			unode *F2_a = F2.get_node(singletons.front());
			singletons.pop_front();
			unode *F1_a = F1.get_node(twins.get_backward(F2_a->get_label()));
			cout << "F1: " << F1.str() << endl;
			cout << "F1_a: " << F1.str_subtree(F1_a) << endl;

			cout << "F2: " << F2.str() << endl;
			cout << "F2_a: " << F2.str_subtree(F2_a) << endl;

			// remove from sibling pairs if necessary
			map<int,int>::iterator i = sibling_pairs.find(F1_a->get_label());
			if (i != sibling_pairs.end()) {
				sibling_pairs.erase(i->second);
				sibling_pairs.erase(i->first);
			}

			cout << F1 << endl;
			pair<int,int> components = F1.cut_edge(F1_a->get_label(), F1_a->get_parent()->get_label());
			cout << F1 << endl;

			// check for new sibling pair
			cout << F1.str_subtree(F1.get_node(components.first)) << endl;
			cout << F1.str_subtree(F1.get_node(components.second)) << endl;
//			cout << F1_new_terminal->get_parent()->get_distance() << endl;
//			cout << F1_new_terminal->get_distance() << endl;
			// root is terminal
			unode *F1_new_node = F1.get_node(components.second)->get_parent();
			vector<int> new_sibling_pair = vector<int>();
			for (unode *u : F1_new_node->get_neighbors()) {
				if (u->get_terminal()) {
					new_sibling_pair.push_back(u->get_label());
				}
			}
			for(int i = 0; i + 1 < new_sibling_pair.size(); i++) {
				sibling_pairs.insert(make_pair(new_sibling_pair[i], new_sibling_pair[i+1]));
				sibling_pairs.insert(make_pair(new_sibling_pair[i+1], new_sibling_pair[i]));
			}

/*			if (F1_new_terminal->get_parent()->get_terminal()) {
				new_sibling = F1_new_terminal->get_parent();
			}
			else {
				if (F1_new_terminal->get_parent()->get_distance() > F1_new_terminal->get_distance()) {
					// root
					new_sibling = F1_new_terminal->get_parent();
				}
				else {
					new_sibling = F1_new_terminal->get_sibling();
				}
			}
				
			cout << F1.str_subtree(new_sibling) << endl;
			cout << new_sibling->get_terminal() << endl;
			if (new_sibling->get_terminal() == true) {
			}
			*/

		}

		cout << "sibling pairs: " << endl; 
		for (pair<int, int> p: sibling_pairs) {
			cout << p.first << ", " << p.second << endl;
		}

		// get sibling pair (a,c) in F1
		map<int, int>::iterator spi = sibling_pairs.begin();
		unode *F1_a = F1.get_node(spi->first);
		unode *F1_c = F1.get_node(spi->second);
		sibling_pairs.erase(F1_a->get_label());
		sibling_pairs.erase(F1_c->get_label());

		// find a and c in F2
		unode *F2_a = F2.get_node(twins.get_forward(F1_a->get_label()));
		unode *F2_c = F2.get_node(twins.get_forward(F1_c->get_label()));

		// a is the deeper of the pair
		if (F2_a->get_distance() < F2_c->get_distance()) {

			unode *temp = F1_a;
			F1_a = F1_c;
			F1_c = temp;

			temp = F2_a;
			F2_a = F2_c;
			F2_c = temp;
		}

		cout << "F1: " << F1.str() << endl;
		cout << "F1_a: " << F1.str_subtree(F1_a) << endl;
		cout << "F1_c: " << F1.str_subtree(F1_c) << endl;


		cout << "F2: " << F2.str() << endl;
		cout << "F2_a: " << F2.str_subtree(F2_a) << endl;
		cout << "F2_c: " << F2.str_subtree(F2_c) << endl;




		
		// Case 2 : Compatible Sibling Pair

		// TODO: need to reroot after (or check reroot, because it isn't working on trees5.tre)

		
		if (F2_a->get_parent() == F2_c->get_parent()) {

			cout << "Case 2" << endl;
			// make terminal in F1
			unode *F1_new_terminal = F1_c->get_parent();
			F1_new_terminal->set_terminal(true);

			// rotate if F1_c or F1_a was the root
			if (F1_new_terminal->get_distance() > F1_c->get_distance() ||
			F1_new_terminal->get_distance() > F1_a->get_distance()) {
				F1_new_terminal->rotate(F1_new_terminal->get_neighbor_not(F1_a, F1_c)->get_label());
				F1_c->set_distance(F1_a->get_distance());
				// update component root after rotate
				if (F1_c->get_component() > -1) {
					F1.update_component(F1_c->get_component(), F1_new_terminal->get_label());
				}
				else if (F1_a->get_component() > -1) {
					F1.update_component(F1_a->get_component(), F1_new_terminal->get_label());
				}
				F2_c->set_distance(F2_a->get_distance());
			}

			// check for new sibling pair
			unode *new_sibling;
			cout << F1_new_terminal->get_parent()->get_distance() << endl;
			cout << F1_new_terminal->get_distance() << endl;
			// root is terminal
			if (F1_new_terminal->get_parent()->get_terminal()) {
				new_sibling = F1_new_terminal->get_parent();
			}
			else {
				if (F1_new_terminal->get_parent()->get_distance() > F1_new_terminal->get_distance()) {
					// root
					new_sibling = F1_new_terminal->get_neighbor_not(F1_a, F1_c);
				}
				else {
					new_sibling = F1_new_terminal->get_sibling();
				}
			}
				
			cout << F1.str_subtree(new_sibling) << endl;
			cout << new_sibling->get_terminal() << endl;
			if (new_sibling->get_terminal() == true) {
				sibling_pairs.insert(make_pair(F1_new_terminal->get_label(),new_sibling->get_label()));
				sibling_pairs.insert(make_pair(new_sibling->get_label(),F1_new_terminal->get_label()));
			}
			
			// make terminal in F2
			unode *F2_new_terminal = F2_a->get_parent();
			F2_new_terminal->set_terminal(true);

			// rotate if F2_c was the root
			if (F2_new_terminal->get_distance() > F2_c->get_distance()) {
				F2_new_terminal->rotate(F2_new_terminal->get_neighbor_not(F2_a, F2_c)->get_label());
				// update component root after rotate
				if (F2_c->get_component() > -1) {
					F2.update_component(F2_c->get_component(), F2_new_terminal->get_label());
				}
				F2_c->set_distance(F2_a->get_distance());
			}

			// add to nodemapping
			twins.add(F1_new_terminal->get_label(), F2_new_terminal->get_label());

			// TODO: this is broken!
			// TODO: fix for e_x singleton identification !
			// need to check that all neighbors are singletons?
			//
			// check for singleton
			if (F2_new_terminal->get_parent()->get_distance() > F2_new_terminal->get_distance()) {
				if (F2_new_terminal->is_singleton()) {
					singletons.push_back(F2_new_terminal->get_label());
				}
			}




		}

		else if (F2_a->get_parent() == F2_c) {
			cout << "Case 2.5" << endl;
		}

		// Case 3 : Cutting

		else {

			if (k <= 0) {
				return -1;
			}

			cout << "Case 3" << endl;

			int result = -1;

			// find pendant edges between a and c in F2
			// TODO: need distances from "root" to do this efficiently
			
			list<pair<int,int> > pendants = find_pendants(F2_a, F2_c);
			cout << "pendants: " << endl;
			for (auto p : pendants) {
				cout << "\t" << F2.str_subtree(F2.get_node(p.first)) << "\t" << F2.str_subtree(F2.get_node(p.second)) << endl;
			}

			cout << endl;

			// Cut F2_a
			{
				pair <int, int> e_a = make_pair(F2_a->get_label(), F2_a->get_parent()->get_label());
	
				cout  << "cut e_a" << endl;
	
				// copy the trees
				uforest F1_copy = uforest(F1);
				uforest F2_copy = uforest(F2);
				nodemapping twins_copy = nodemapping(twins);
				map<int,int> sibling_pairs_copy = map<int, int>(sibling_pairs);
				list<int> singletons_copy = list<int>(singletons);
	
				cout << F2_copy << endl;
				pair<int,int> components = F2_copy.cut_edge(e_a.first, e_a.second);
				cout << F2_copy << endl;
				// check for singleton
				cout << "checking if " << F2_copy.str_subtree(F2_copy.get_node(components.first)) << " is a singleton" << endl;
				if (F2_copy.get_node(components.first)->is_singleton()) {
					cout << "it is" << endl;
					singletons_copy.push_back(components.first);
				}
				cout << "checking if " << F2_copy.str_subtree(F2_copy.get_node(components.second)) << " is a singleton" << endl;
				if (F2_copy.get_node(components.second)->is_singleton()) {
					cout << "it is" << endl;
					singletons_copy.push_back(components.second);
				}
//				if (F2_copy.get_node(components.first)->get_terminal()) {
//					singletons_copy.push_back(components.first);
//				}
//				if (F2_copy.get_node(components.second)->get_terminal()) {
//					singletons_copy.push_back(components.second);
//				}
				int branch_a = tbr_distance_hlpr(F1_copy, F2_copy, k-1, twins_copy, sibling_pairs_copy, singletons_copy);
	
				if (branch_a > result) {
					result = branch_a;
				}
			}

			// Cut F2_c
			{
				pair <int, int> e_c = make_pair(F2_c->get_label(), F2_c->get_parent()->get_label());
	
				cout  << "cut e_c" << endl;
	
				// copy the trees
				uforest F1_copy = uforest(F1);
				uforest F2_copy = uforest(F2);
				nodemapping twins_copy = nodemapping(twins);
				map<int, int> sibling_pairs_copy = map<int, int>(sibling_pairs);
				list<int> singletons_copy = list<int>(singletons);
	
				cout << F2_copy << endl;
				pair<int, int> components = F2_copy.cut_edge(e_c.first, e_c.second);
				cout << F2_copy << endl;
				if (F2_copy.get_node(components.first)->is_singleton()) {
					singletons_copy.push_back(components.first);
				}
				if (F2_copy.get_node(components.second)->is_singleton()) {
					singletons_copy.push_back(components.second);
				}
				int branch_c = tbr_distance_hlpr(F1_copy, F2_copy, k-1, twins_copy, sibling_pairs_copy, singletons_copy);
	
				if (branch_c > result) {
					result = branch_c;
				}
			}

			/*
			// Cut F2_b
			{
				pair <int, int> e_b = pendants.front();
	
				cout  << "cut e_b" << endl;
	
				// copy the trees
				uforest F1_copy = uforest(F1);
				uforest F2_copy = uforest(F2);
				nodemapping twins_copy = nodemapping(twins);
				map<int, int> sibling_pairs_copy = map<int, int>(sibling_pairs);
				sibling_pairs_copy.insert(make_pair(F1_a->get_label(), F1_c->get_label()));
				sibling_pairs_copy.insert(make_pair(F1_c->get_label(), F1_a->get_label()));
				list<int> singletons_copy = list<int>(singletons);
	
				cout << F2_copy << endl;
				pair<int, int> components = F2_copy.cut_edge(e_b.first, e_b.second);
				cout << F2_copy << endl;
				if (F2_copy.get_node(components.first)->get_terminal()) {
					singletons_copy.push_back(components.first);
				}
				if (F2_copy.get_node(components.second)->get_terminal()) {
					singletons_copy.push_back(components.second);
				}
				int branch_b = tbr_distance_hlpr(F1_copy, F2_copy, k-1, twins_copy, sibling_pairs_copy, singletons_copy);
	
				if (branch_b > result) {
					result = branch_b;
				}
			}
			*/
/*
			// Cut F2_d
			{
				pair <int, int> e_d = pendants.back();
	
				cout  << "cut e_d" << endl;
	
				// copy the trees
				uforest F1_copy = uforest(F1);
				uforest F2_copy = uforest(F2);
				nodemapping twins_copy = nodemapping(twins);
				map<int, int> sibling_pairs_copy = map<int, int>(sibling_pairs);
				sibling_pairs_copy.insert(make_pair(F1_a->get_label(), F1_c->get_label()));
				sibling_pairs_copy.insert(make_pair(F1_c->get_label(), F1_a->get_label()));
				list<int> singletons_copy = list<int>(singletons);
	
				cout << F2_copy << endl;
				pair<int, int> components = F2_copy.cut_edge(e_d.first, e_d.second);
				cout << F2_copy << endl;
				if (F2_copy.get_node(components.first)->get_terminal()) {
					singletons_copy.push_back(components.first);
				}
				if (F2_copy.get_node(components.second)->get_terminal()) {
					singletons_copy.push_back(components.second);
				}
				int branch_d = tbr_distance_hlpr(F1_copy, F2_copy, k-1, twins_copy, sibling_pairs_copy, singletons_copy);
	
				if (branch_d > result) {
					result = branch_d;
				}
			}
			*/
			return result;
		}
	}

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

	cout << "ANSWER FOUND" << endl;
	return k;
}

// assume a is deeper than c
list<pair<int,int> > find_pendants(unode *a, unode *c) {
	list<pair<int,int> > pendants = list<pair<int,int> >();
	// don't forget c's grandparent
	// move through "parents" until we reach the same node
	// use a list of pairs of ints instead?
	bool same_component = true;
	while(a->get_parent() != c->get_parent()) {
//		cout << "a:" << a->get_distance() << endl;
//		cout << "c:" << c->get_distance() << endl;
		if (a->get_distance() > c->get_distance()) {
			unode *prev_a = a;
			a = a->get_parent();
			if (a->get_distance() > prev_a->get_distance()) {
				same_component = false;
				break;
			}
//			cout << prev_a->get_distance() << endl;
//			cout << [a->get_distance() << endl;
//			cout << a->get_parent()->get_distance() << endl;
//			cout << a->get_neighbor_not(prev_a, a->get_parent())->get_distance() << endl;
			pendants.push_back(make_pair(a->get_label(), a->get_neighbor_not(prev_a, a->get_parent())->get_label()));
		}
		else {
			unode *prev_c = c;
			c = c->get_parent();
			if (c->get_distance() > prev_c->get_distance()) {
				same_component = false;
				break;
			}
			pendants.push_back(make_pair(c->get_label(), c->get_neighbor_not(prev_c, c->get_parent())->get_label()));
		}
	}
	// return empty list if they are in different components
	if (!same_component) {
		pendants.clear();
	}
	// add the final parent edge (or last
	else {
		pendants.push_back(make_pair(a->get_parent()->get_label(), a->get_parent()->get_neighbor_not(a,c)->get_label()));
	}
	return pendants;
}

#endif
