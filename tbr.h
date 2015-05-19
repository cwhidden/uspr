#ifndef INCLUDE_TBR
#define INCLUDE_TBR

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

class nodemapping;

typedef enum {ALIVE, DEAD, SOCKET, UNKNOWN} nodestatus;
string nodestatus_name[] = {"ALIVE", "DEAD", "SOCKET", "UNKNOWN"};

typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS, boost::no_property> undirected_graph;

//#define DEBUG 1
#ifdef DEBUG
	#define debug(x) x
#else
	#define debug(x) 
#endif

//#define DEBUG_APPROX 1
#ifdef DEBUG_APPROX
	#define debug_approx(x) x
#else
	#define debug_approx(x) 
#endif

//#define DEBUG_REPLUG 1
#ifdef DEBUG_REPLUG
	#define debug_replug(x) x
#else
	#define debug_replug(x) 
#endif

//#define DEBUG_SOCKETS 1
#ifdef DEBUG_SOCKETS
	#define debug_sockets(x) x
#else
	#define debug_sockets(x) 
#endif

//#define DEBUG_PHI_NODES 1
#ifdef DEBUG_PHI_NODES
	#define debug_phi_nodes(x) x
#else
	#define debug_phi_nodes(x) 
#endif




// NOTE: okay for TBR distance only, not for all mAFs / replug
bool OPTIMIZE_2B = false;
bool OPTIMIZE_PROTECT_A = true;
bool OPTIMIZE_PROTECT_B = true;
bool OPTIMIZE_BRANCH_AND_BOUND = true;

// classes

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
			forward.erase(l1);
			forward.insert(make_pair(l1, l2));
			backward.erase(l2);
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

class socket {
	public:
	socket(int x, int y, int c, int n) {
		if (x <= y) {
			i = x;
			j = y;
		}
		else {
			i = y;
			j = x;
		}
		dead = c;
		num = n;
	}
	string str() {
		stringstream ss;
		ss << "s(";
		ss << this->i << ", ";
		ss << this->j << ", ";
		ss << this->dead  << ", ";
		ss << this->num << ")";
		return ss.str();
	}
	int i;
	int j;
	int dead;
	int num;
};

class socketcontainer {
	public:
	map<pair<int, int>, vector<socket *> > sockets;
	map<int, socket *> dead_map;

	socketcontainer(list<socket *> &socketlist) {
		sockets = map<pair<int, int>, vector<socket *> >();
		dead_map = map<int, socket *>();
		for (socket *s : socketlist) {
			dead_map[s->dead] = s;
			map<pair<int, int>, vector<socket *> >::iterator i = 
				sockets.find(make_pair(s->i, s->j));
			if (i == sockets.end()) {
				
				auto new_i = sockets.insert(make_pair(make_pair(s->i, s->j), vector<socket *>()));
					i = new_i.first;
			}
			if (i->second.size() < s->num) {
				i->second.resize(s->num);
			}
			(i->second)[s->num - 1] = s;
		}
	}

	~socketcontainer() {
		for (auto &i : sockets) {
			for (socket *s : i.second) {
				delete s;
			}
		}
		sockets.clear();
		dead_map.clear();
	}

	vector<socket *> &find(int i, int j) {
		return sockets[make_pair(i, j)];
	}

	socket *find_dead(int n) {
		return dead_map[n];
	}

	list<socket *> get_list() {
		list<socket *> socket_list = list<socket *>();
		for (auto &i : sockets) {
			for (socket *s : i.second) {
				socket_list.push_back(s);
			}
		}
		return socket_list;
	}
	 
};


// function prototypes
int tbr_distance(uforest &F1, uforest &F2, bool print_forest);
template<typename T>
int tbr_distance(uforest &T1, uforest &T2, int (*func_pointer)(uforest &F1, uforest &F2, nodemapping &twins, int k, T s), uforest **MAF1 = NULL, uforest **MAF2 = NULL);
int tbr_count_MAFs(uforest &F1, uforest &F2);
int tbr_count_mAFs(uforest &F1, uforest &F2);
int tbr_print_mAFs(uforest &F1, uforest &F2);
int tbr_count_mAFs(uforest &T1, uforest &T2, bool print);
template<typename T>
int tbr_distance(uforest &T1, uforest &T2, T t, int (*func_pointer)(uforest &F1, uforest &F2, nodemapping &twins, int k, T s), uforest **MAF1 = NULL, uforest **MAF2 = NULL);
template<typename T>
int tbr_distance_hlpr(uforest &T1, uforest &T2, int k, T t, int (*func_pointer)(uforest &F1, uforest &F2, nodemapping &twins, int k, T s), uforest **MAF1 = NULL, uforest **MAF2 = NULL);
template<typename T>
int tbr_distance_hlpr(uforest &F1, uforest &F2, int k, nodemapping &twins, map<int, int> &sibling_pairs, list<int> &singletons, T t, int (*func_pointer)(uforest &F1, uforest &F2, nodemapping &twins, int k, T s), uforest **MAF1 = NULL, uforest **MAF2 = NULL);
list<pair<int,int> > find_pendants(unode *a, unode *c);
int tbr_approx(uforest &T1, uforest &T2);
int tbr_approx(uforest &T1, uforest &T2, bool low);
int tbr_approx_hlpr(uforest &F1, uforest &F2, int k, nodemapping &twins, map<int, int> &sibling_pairs, list<int> &singletons);
int tbr_high_lower_bound(uforest &T1, uforest &T2);
int tbr_low_lower_bound(uforest &T1, uforest &T2);
int tbr_high_upper_bound(uforest &T1, uforest &T2);
int tbr_low_upper_bound(uforest &T1, uforest &T2);
int tbr_branch_bound(uforest &F1, uforest &F2, nodemapping &twins, map<int, int> &sibling_pairs, list<int> &singletons);
void find_sockets(uforest &T1, uforest &F1, list<socket *> &sockets);
void find_sockets_hlpr(unode *n, unode *prev, uforest &T, list<socket *> &sockets);
void add_sockets(unode *x, unode *y, list<socket *> &sockets);
void find_dead_components(uforest &T, socketcontainer &S, map<int, nodestatus> &T_status, vector<list<int> > &T_dead_components);
void find_dead_components_hlpr(unode *n, unode *prev, int component, uforest &T, socketcontainer &S, map<int, nodestatus> &T_status, vector<list<int> > &T_dead_components);
void update_nodemapping(nodemapping &twins, uforest &F, int original_label, int new_label, bool forward);
int check_socket_group_combination(int k, int kprime, socketcontainer &T1_sockets, socketcontainer &T2_sockets_normalized, vector<list<int> > &T1_dead_components, vector<list<int> > &T2_dead_components, vector<pair<vector<socket *> , vector<socket *> > > &socketcandidates, vector<pair<socket *, socket *> > &sockets, vector<pair<socket *, socket *> > &candidate_phi_node_sockets);
int check_socket_group_combinations(int n, int i, int j, int last, int k, int kprime, socketcontainer &T1_sockets, socketcontainer &T2_sockets_normalized, vector<list<int> > &T1_dead_components, vector<list<int> > &T2_dead_components, vector<pair<vector<socket *> , vector<socket *> > > &socketcandidates, vector<pair<socket *, socket *> > &sockets, vector<pair<socket *, socket *> > &phi_node_sockets);
int check_socket_group_combinations(int k, int kprime, socketcontainer &T1_sockets, socketcontainer &T2_sockets_normalized, vector<list<int> > &T1_dead_components, vector<list<int> > &T2_dead_components, vector<pair<vector<socket *> , vector<socket *> > > &socketcandidates, vector<pair<socket *, socket *> > &phi_node_sockets);
bool get_constraint(list<int> &dead_component, socketcontainer &T_sockets, map<socket *, int> &socket_pointer_map, vector<int> &constraint);
int solve_monotonic_2sat_2vars(vector<vector<int> > &constraints, list<int> &changed_sockets);
int solve_monotonic_2sat_2vars(vector<vector<int> > &constraints);
void add_phi_nodes(uforest &F, map<pair<int, int>, int> &F_add_phi_nodes);
// function prototypes end

// AF helpers
int dummy_mAFs(uforest &F1, uforest &F2, nodemapping &twins, int k, int dummy);
int print_mAFs(uforest &F1, uforest &F2, nodemapping &twins, int k, int dummy);
int count_mAFs(uforest &F1, uforest &F2, nodemapping &twins, int k, int *count);
int print_and_count_mAFs(uforest &F1, uforest &F2, nodemapping &twins, int k, int *count);
int replug_hlpr(uforest &F1, uforest &F2, nodemapping &twins, int k, pair<uforest, uforest> T);

// compute the TBR distance
int tbr_distance(uforest &T1, uforest &T2, bool print_forest = false) {
	bool old_value = OPTIMIZE_2B;
	// always safe for the TBR distance
	OPTIMIZE_2B = true;
	uforest *MAF1 = NULL;
	uforest *MAF2 = NULL;
	int d = tbr_distance(T1, T2, &dummy_mAFs, &MAF1, &MAF2);
	if (print_forest) {
		if (MAF1 != NULL) {
			cout << "F1: " << MAF1->str() << endl;
		}
		if (MAF2 != NULL) {
			cout << "F2: " << MAF2->str() << endl;
		}
	}
	if (MAF1 != NULL) {
		delete MAF1;
	}
	if (MAF2 != NULL) {
		delete MAF2;
	}
	OPTIMIZE_2B = old_value;
	return d;
}

template <typename T>
int tbr_distance(uforest &T1, uforest &T2, int (*func_pointer)(uforest &F1, uforest &F2, nodemapping &twins, int k, T s), uforest **MAF1, uforest **MAF2) {
	T dummy = 0;
	return tbr_distance(T1, T2, dummy, func_pointer, MAF1, MAF2);
}

template <typename T>
int tbr_distance(uforest &T1, uforest &T2, T t, int (*func_pointer)(uforest &F1, uforest &F2, nodemapping &twins, int k, T s), uforest **MAF1, uforest **MAF2) {

	int start = tbr_high_lower_bound(T1, T2);

	for(int k = start; k < 100; k++) {
			cout << "{" << k << "} ";
			cout.flush();
			// test k
			int result = tbr_distance_hlpr(T1, T2, k, t, func_pointer, MAF1, MAF2);
			if (result >= 0) {
				cout << endl;
				return k - result;
			}
	}
	return -1;
}

int tbr_count_MAFs(uforest &T1, uforest &T2) {
	int count = 0;
	int start = tbr_high_lower_bound(T1, T2);

	for(int k = start; k < 100; k++) {
			cout << "{" << k << "} ";
			cout.flush();
			// test k
			int result = tbr_distance_hlpr(T1, T2, k, &count, &count_mAFs);
			if (result >= 0) {
				cout << endl;
				return count;
			}
	}
	return count;
}

int tbr_print_mAFs(uforest &T1, uforest &T2) {
	return tbr_count_mAFs(T1, T2, true);
}

int tbr_count_mAFs(uforest &T1, uforest &T2) {
	return tbr_count_mAFs(T1, T2, false);
}

int tbr_count_mAFs(uforest &T1, uforest &T2, bool print) {
	int count = 0;
	int start = tbr_high_lower_bound(T1, T2);

	for(int k = start; k < 100; k++) {
			cout << "{" << k << "} ";
			cout.flush();
			// test k
			int new_count = 0;
			int result;
			if (print) {
				result = tbr_distance_hlpr(T1, T2, k, &new_count, &print_and_count_mAFs);
			}
			else {
				result = tbr_distance_hlpr(T1, T2, k, &new_count, &count_mAFs);
			}
			if (result >= 0) {
				cout << endl;
				cout << "found " << new_count << " mAFs" << endl;
				if (count < new_count) {
					count = new_count;
				}
				else {
					cout << endl;
					return count;
				}
			}
	}
	return count;
}

int replug_distance(uforest &T1, uforest &T2, bool print_forest = false) {
	// may be needed
	T1.root(T1.get_smallest_leaf());
	T2.root(T2.get_smallest_leaf());
	distances_from_leaf_decorator(T1, T1.get_smallest_leaf());
	distances_from_leaf_decorator(T2, T2.get_smallest_leaf());
	uforest *MAF1 = NULL;
	uforest *MAF2 = NULL;
	int d = tbr_distance(T1, T2, make_pair(T1, T2), &replug_hlpr, &MAF1, &MAF2);
	if (print_forest) {
		if (MAF1 != NULL) {
			cout << "F1: " << MAF1->str() << endl;
		}
		if (MAF2 != NULL) {
			cout << "F2: " << MAF2->str() << endl;
		}
	}
	if (MAF1 != NULL) {
		delete MAF1;
	}
	if (MAF2 != NULL) {
		delete MAF2;
	}
	return d;
}


template <typename T>
int tbr_distance_hlpr(uforest &T1, uforest &T2, int k, T t, int (*func_pointer)(uforest &F1, uforest &F2, nodemapping &twins, int k, T s), uforest **MAF1 = NULL, uforest **MAF2 = NULL) {
	uforest F1 = uforest(T1);
	uforest F2 = uforest(T2);

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
		if (u != NULL) {
			u->set_terminal(true);
		}
	}
	for(unode *u : F2.get_leaves()) {
		if (u != NULL) {
			u->set_terminal(true);
		}
	}

	distances_from_leaf_decorator(F1, F1.get_smallest_leaf());
	distances_from_leaf_decorator(F2, F2.get_smallest_leaf());
	debug(
		cout << endl << F1 << endl;
		for(int i : F1.find_leaves()) {
			cout << i << ": " << F1.get_node(i)->get_distance() << endl;
		}

		cout << F2 << endl;
		for(int i : F2.find_leaves()) {
			cout << i << ": " << F2.get_node(i)->get_distance() << endl;
		}
	)


	return tbr_distance_hlpr(F1, F2, k, twins, sibling_pairs, singletons, t, func_pointer, MAF1, MAF2);
}

template <typename T>
int tbr_distance_hlpr(uforest &F1, uforest &F2, int k, nodemapping &twins, map<int, int> &sibling_pairs, list<int> &singletons, T t, int (*func_pointer)(uforest &F1, uforest &F2, nodemapping &twins, int k, T s), uforest **MAF1 = NULL, uforest **MAF2 = NULL) {

	if (k < 0) {
		return -1;
	}

	debug(cout << "tbr_distance_hlpr(" << k << ")" << endl);

	// if (sib pair list is not empty, k>=0) {
	while (!sibling_pairs.empty() || !singletons.empty()) {

		// Case 1 : Isolated Subtree
		while (!singletons.empty()) {

			debug(cout << "Case 1" << endl);

			unode *F2_a = F2.get_node(singletons.front());
			singletons.pop_front();
			unode *F1_a = F1.get_node(twins.get_backward(F2_a->get_label()));
			debug(
				cout << "F1: " << F1.str() << endl;
				cout << "F1_a: " << F1.str_subtree(F1_a) << endl;
	
				cout << "F2: " << F2.str() << endl;
				cout << "F2_a: " << F2.str_subtree(F2_a) << endl;
			)

			// remove from sibling pairs if necessary
			map<int,int>::iterator spi = sibling_pairs.find(F1_a->get_label());
//			map<int,int>::iterator j;
			if (spi != sibling_pairs.end()) {
//				j = sibling_pairs.find(i->second);
//				if (j != sibling_pairs.end() && j->second == i->first) {
					sibling_pairs.erase(spi->second);
//				}
				sibling_pairs.erase(spi->first);
			}

			debug(cout << F1 << endl);

			if (F1_a->get_parent() == NULL) {
				continue;
			}
			int first_label = F1_a->get_label();
			int second_label = F1_a->get_parent()->get_label();
			pair<int,int> components = F1.cut_edge(first_label, second_label);
			update_nodemapping(twins, F1, first_label, components.first, true);
			update_nodemapping(twins, F1, second_label, components.second, true);

			debug(cout << F1 << endl);

			// check for new sibling pair
			debug(
				cout << F1.str_subtree(F1.get_node(components.first)) << endl;
				cout << F1.str_subtree(F1.get_node(components.second)) << endl;
//			cout << F1_new_terminal->get_parent()->get_distance() << endl;
//			cout << F1_new_terminal->get_distance() << endl;
				cout << "checking for sibling pair" << endl;
			)

			// check for sibling pair
			unode *F1_new_node = F1.get_node(components.second);//->get_parent();
			vector<int> new_sibling_pair = vector<int>();
			if (F1_new_node != NULL) {
				for (unode *u : F1_new_node->get_neighbors()) {
					if (u->get_terminal()) {
						new_sibling_pair.push_back(u->get_label());
					}
				}
			}
			int i = new_sibling_pair.size();
			debug(cout << new_sibling_pair.size() << endl);
			if (i >= 2) {
				if (sibling_pairs.find(new_sibling_pair[i-1]) == sibling_pairs.end() && sibling_pairs.find(new_sibling_pair[i-2]) == sibling_pairs.end()) {
					debug(cout << "sibling_pair found" << endl);
					sibling_pairs.insert(make_pair(new_sibling_pair[i-2], new_sibling_pair[i-1]));
					sibling_pairs.insert(make_pair(new_sibling_pair[i-1], new_sibling_pair[i-2]));
				}
			}

		}

		if (sibling_pairs.empty()) {
			break;
		}

		debug(
			cout << "sibling pairs: " << sibling_pairs.size() << endl; 
			for (pair<int, int> p: sibling_pairs) {
				cout << p.first << ", " << p.second << endl;
			}
		)

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
		if (F2_a->get_distance() < F2_c->get_distance() ||
				(F2_a->get_distance() == F2_c->get_distance() &&
				 F2_a->get_parent()->get_distance() < F2_c->get_parent()->get_distance())
				) {

			unode *temp = F1_a;
			F1_a = F1_c;
			F1_c = temp;

			temp = F2_a;
			F2_a = F2_c;
			F2_c = temp;
		}

		debug(
			cout << "F1: " << F1.str() << endl;
			cout << "F1_a: " << F1.str_subtree(F1_a) << endl;
			cout << "F1_c: " << F1.str_subtree(F1_c) << endl;


			cout << "F2: " << F2.str() << endl;
			cout << "F2_a: " << F2.str_subtree(F2_a) << endl;
			cout << "F2_c: " << F2.str_subtree(F2_c) << endl;
		)




		
		// Case 2 : Compatible Sibling Pair

		if (F2_a->get_parent() == F2_c->get_parent() ||
				F2_a->get_parent() == F2_c ||
				F2_c->get_parent() == F2_a) {

			debug(cout << "Case 2" << endl);

			// make terminal in F1
			// contract F1_a and F1_c
			unode *F1_new_terminal = F1_a->get_parent();
			debug(cout << "F1_new_terminal: " << F1.str_subtree(F1_new_terminal) << endl);
			F1_new_terminal->set_terminal(true);
			F1_new_terminal->contract_neighbor(F1_a);
			F1_new_terminal->contract_neighbor(F1_c);


			if (F1_c->get_component() > -1) {
				F1.update_component(F1_c->get_component(), F1_new_terminal->get_label());
			}
			else if (F1_a->get_component() > -1) {
				F1.update_component(F1_a->get_component(), F1_new_terminal->get_label());
			}

			// check for sibling pair
			unode *F1_new_node = F1_new_terminal->get_parent();
			vector<int> new_sibling_pair = vector<int>();
			if (F1_new_node != NULL) {
				for (unode *u : F1_new_node->get_neighbors()) {
					if (u->get_terminal()) {
						new_sibling_pair.push_back(u->get_label());
					}
				}
			}
			int i = new_sibling_pair.size();
			if (i >= 2) {
				if (sibling_pairs.find(new_sibling_pair[i-1]) == sibling_pairs.end() && sibling_pairs.find(new_sibling_pair[i-2]) == sibling_pairs.end()) {
					sibling_pairs.insert(make_pair(new_sibling_pair[i-1], new_sibling_pair[i-2]));
					sibling_pairs.insert(make_pair(new_sibling_pair[i-2], new_sibling_pair[i-1]));
				}
			}
			
			// make terminal in F2
			// contract F2_a and F2_c
			debug(
				cout << "d(F2_a): " << F2_a->get_distance() << endl;
				cout << "d(F2_c): " << F2_c->get_distance() << endl;
			)
			unode *F2_new_terminal = F2_a->get_parent();
			if (F2_c->get_parent() == F2_a &&
					F2_a->get_label() < -1) {
				F2_new_terminal = F2_a;
			}
			F2_new_terminal->set_terminal(true);

			if (F2_new_terminal != F2_a) {
				F2_new_terminal->contract_neighbor(F2_a);
			}
			if (F2_new_terminal != F2_c) {
				F2_new_terminal->contract_neighbor(F2_c);
			}

			if (F2_c->get_component() > -1) {
				F2.update_component(F2_c->get_component(), F2_new_terminal->get_label());
			}
			else if (F2_a->get_component() > -1) {
				F2.update_component(F2_a->get_component(), F2_new_terminal->get_label());
			}

			// add to nodemapping
			twins.add(F1_new_terminal->get_label(), F2_new_terminal->get_label());

			// check for singleton
			if (F2_new_terminal->is_singleton()) { //get_parent()->get_distance() > F2_new_terminal->get_distance()) 
				singletons.push_back(F2_new_terminal->get_label());
			}




		}

		else if (F2_a->get_parent() == F2_c) {
			debug(cout << "Case 2.5" << endl);
		}

		// TODO: 2.5 where (F2_c->get_parent() == F2_a) ?

		// Case 3 : Cutting

		else {

			debug(cout << "Case 3" << endl);

			if (k <= 0) {
				return -1;
			}
			
			if (OPTIMIZE_BRANCH_AND_BOUND) {
				int lower_bound = tbr_branch_bound(F1, F2, twins, sibling_pairs, singletons);
				if (k < lower_bound) {
					return -1;
				}
			}

			int result = -1;

			// find pendant edges between a and c in F2
			// TODO: need distances from "root" to do this efficiently

			debug(cout << F2.str_with_depths() << endl);
			
			list<pair<int,int> > pendants = find_pendants(F2_a, F2_c);
			int num_pendants = pendants.size();
			debug(
				cout << "pendants: " << endl;
				for (auto p : pendants) {
					cout << "\t" << F2.str_subtree(F2.get_node(p.first)) << "\t" << F2.str_subtree(F2.get_node(p.second)) << endl;
				}
				cout << endl;
			)

			bool cut_a = true;
			bool cut_c = true;
			bool cut_b = true;

			if (num_pendants < 2 || num_pendants > (k+1)) {
				cut_b = false;
			}
			// safe for getting the TBR distance but not for exploring all mAFs
			else if (OPTIMIZE_2B && num_pendants == 2) {
				cut_a = false;
				cut_c = false;
			}

			if (OPTIMIZE_PROTECT_A && F2_a->is_protected()) {
				cut_a = false;
			}
			if (OPTIMIZE_PROTECT_A && F2_c->is_protected()) {
				cut_c = false;
			}

			// Cut F2_a
			if (cut_a) {
				pair <int, int> e_a = make_pair(F2_a->get_label(), F2_a->get_parent()->get_label());
	
				debug(cout  << "cut e_a" << endl);
	
				// copy the trees
				uforest F1_copy = uforest(F1);
				uforest F2_copy = uforest(F2);
				uforest *MAF1_copy = NULL;
				uforest *MAF2_copy = NULL;
				nodemapping twins_copy = nodemapping(twins);
				map<int,int> sibling_pairs_copy = map<int, int>(sibling_pairs);
				list<int> singletons_copy = list<int>(singletons);
	
				debug(cout << F2_copy << endl);
				int first_label = e_a.first;
				int second_label = e_a.second;
				pair<int,int> components = F2_copy.cut_edge(first_label, second_label);
				update_nodemapping(twins_copy, F2_copy, first_label, components.first, false);
				update_nodemapping(twins_copy, F2_copy, second_label, components.second, false);
				debug(
					cout << F2_copy << endl;
					cout << components.first << endl;
					cout << F2_copy.get_node(components.first) << endl;
					cout << components.second<< endl;
					cout << F2_copy.get_node(components.second) << endl;
				)
				// check for singleton
				debug(cout << "checking if " << F2_copy.str_subtree(F2_copy.get_node(components.first)) << " is a singleton" << endl);
				if (F2_copy.get_node(components.first)->is_singleton()) {
					debug(cout << "it is" << endl);
					singletons_copy.push_back(components.first);
				}
				debug(cout << "checking if " << F2_copy.str_subtree(F2_copy.get_node(components.second)) << " is a singleton" << endl);
				if (F2_copy.get_node(components.second)->is_singleton()) {
					debug(cout << "it is" << endl);
					singletons_copy.push_back(components.second);
				}
				int branch_a = tbr_distance_hlpr(F1_copy, F2_copy, k-1, twins_copy, sibling_pairs_copy, singletons_copy, t, func_pointer, &MAF1_copy, &MAF2_copy);
	
				if (branch_a > result) {
					if (*MAF1 != NULL) {
						delete *MAF1;
					}
					if (*MAF2 != NULL) {
						delete *MAF2;
					}
					*MAF1 = MAF1_copy;
					*MAF2 = MAF2_copy;
					result = branch_a;
				}
				else {
					if (MAF1_copy != NULL) {
						delete MAF1_copy;
					}
					if (MAF2_copy != NULL) {
						delete MAF2_copy;
					}
				}
			}

			// Cut F2_c
			if (cut_c) {
				pair <int, int> e_c = make_pair(F2_c->get_label(), F2_c->get_parent()->get_label());
	
				debug(cout  << "cut e_c: " << F2.str_subtree(F2_c) << endl);
	
				// copy the trees
				uforest F1_copy = uforest(F1);
				uforest F2_copy = uforest(F2);
				uforest *MAF1_copy = NULL;
				uforest *MAF2_copy = NULL;
				nodemapping twins_copy = nodemapping(twins);
				map<int, int> sibling_pairs_copy = map<int, int>(sibling_pairs);
				list<int> singletons_copy = list<int>(singletons);
	
				debug(cout << F2_copy << endl);
				int first_label = e_c.first;
				int second_label = e_c.second;
				pair<int,int> components = F2_copy.cut_edge(first_label, second_label);
				update_nodemapping(twins_copy, F2_copy, first_label, components.first, false);
				update_nodemapping(twins_copy, F2_copy, second_label, components.second, false);
				if (OPTIMIZE_PROTECT_A) {
					F2_copy.get_node(F2_a->get_label())->set_protected(true);
				}
				debug(cout << F2_copy << endl);
				if (F2_copy.get_node(components.first)->is_singleton()) {
					singletons_copy.push_back(components.first);
				}
				if (F2_copy.get_node(components.second)->is_singleton()) {
					singletons_copy.push_back(components.second);
				}
				int branch_c = tbr_distance_hlpr(F1_copy, F2_copy, k-1, twins_copy, sibling_pairs_copy, singletons_copy, t, func_pointer, &MAF1_copy, &MAF2_copy);
	
				if (branch_c > result) {
					if (*MAF1 != NULL) {
						delete *MAF1;
					}
					if (*MAF2 != NULL) {
						delete *MAF2;
					}
					*MAF1 = MAF1_copy;
					*MAF2 = MAF2_copy;
					result = branch_c;
				}
				else {
					if (MAF1_copy != NULL) {
						delete MAF1_copy;
					}
					if (MAF2_copy != NULL) {
						delete MAF2_copy;
					}
				}
			}

			// Cut each F2_b but one, for each possible choice
			if (cut_b) {
				debug(cout << "k=" << k << endl);
				for (int i = 0; i < num_pendants; i++) {
					debug(cout << "cut e_b except for e_{b_" << i << "}" << endl);
					bool valid = true;

					// copy the trees
					uforest F1_copy = uforest(F1);
					uforest F2_copy = uforest(F2);
					uforest *MAF1_copy = NULL;
					uforest *MAF2_copy = NULL;
					nodemapping twins_copy = nodemapping(twins);
					map<int, int> sibling_pairs_copy = map<int, int>(sibling_pairs);
					sibling_pairs_copy.insert(make_pair(F1_a->get_label(), F1_c->get_label()));
					sibling_pairs_copy.insert(make_pair(F1_c->get_label(), F1_a->get_label()));
					list<int> singletons_copy = list<int>(singletons);

					debug(cout << F2_copy << endl);

					int j = 0;
					for(pair<int, int> e_b : pendants) {
						if ( j != i) {
							debug(cout << "cut e_{b_" << j << "}" << endl);
							if (OPTIMIZE_PROTECT_A) {
								unode *x = F2_copy.get_node(e_b.first);
								unode *y = F2_copy.get_node(e_b.second);
								if (y->get_distance() > x->get_distance()) {
									x = F2_copy.get_node(e_b.second);
									y = F2_copy.get_node(e_b.first);
								}
								if (x->is_protected()) {
									valid = false;
								}
							}

							int first_label = e_b.first;
							int second_label = e_b.second;
							pair<int,int> components = F2_copy.cut_edge(first_label, second_label);
							update_nodemapping(twins_copy, F2_copy, first_label, components.first, false);
							update_nodemapping(twins_copy, F2_copy, second_label, components.second, false);
							debug(cout << F2_copy << endl);
							if (F2_copy.get_node(components.first)->is_singleton()) {
								singletons_copy.push_back(components.first);
							}
							if (F2_copy.get_node(components.second)->is_singleton()) {
								singletons_copy.push_back(components.second);
							}
						}
						else {
							if (OPTIMIZE_PROTECT_B && i < num_pendants) {
								unode *x = F2_copy.get_node(e_b.first);
								unode *y = F2_copy.get_node(e_b.second);
								if (y->get_distance() > x->get_distance()) {
									x = F2_copy.get_node(e_b.second);
									y = F2_copy.get_node(e_b.first);
								}
								x->set_protected(true);
							}
						}
						j++;
					}
					int branch_b = -1;
					if (valid) {
						branch_b = tbr_distance_hlpr(F1_copy, F2_copy, k-(num_pendants-1), twins_copy, sibling_pairs_copy, singletons_copy, t, func_pointer, &MAF1_copy, &MAF2_copy);
					}
					if (branch_b > result) {
						if (*MAF1 != NULL) {
							delete *MAF1;
						}
						if (*MAF2 != NULL) {
							delete *MAF2;
						}
						*MAF1 = MAF1_copy;
						*MAF2 = MAF2_copy;
						result = branch_b;
					}
					else {
						if (MAF1_copy != NULL) {
							delete MAF1_copy;
						}
						if (MAF2_copy != NULL) {
							delete MAF2_copy;
						}
					}
				}

			}

			// Cut F2_b
			if (false && cut_b) {
				pair <int, int> e_b = pendants.front();
	
				debug(cout  << "cut e_b" << endl);
	
				// copy the trees
				uforest F1_copy = uforest(F1);
				uforest F2_copy = uforest(F2);
				uforest *MAF1_copy = NULL;
				uforest *MAF2_copy = NULL;
				nodemapping twins_copy = nodemapping(twins);
				map<int, int> sibling_pairs_copy = map<int, int>(sibling_pairs);
				sibling_pairs_copy.insert(make_pair(F1_a->get_label(), F1_c->get_label()));
				sibling_pairs_copy.insert(make_pair(F1_c->get_label(), F1_a->get_label()));
				list<int> singletons_copy = list<int>(singletons);
	
				debug(cout << F2_copy << endl);
				int first_label = e_b.first;
				int second_label = e_b.second;
				pair<int,int> components = F2_copy.cut_edge(first_label, second_label);
				update_nodemapping(twins_copy, F2_copy, first_label, components.first, false);
				update_nodemapping(twins_copy, F2_copy, second_label, components.second, false);
				debug(cout << F2_copy << endl);
				if (F2_copy.get_node(components.first)->is_singleton()) {
					singletons_copy.push_back(components.first);
				}
				if (F2_copy.get_node(components.second)->is_singleton()) {
					singletons_copy.push_back(components.second);
				}
				int branch_b = tbr_distance_hlpr(F1_copy, F2_copy, k-1, twins_copy, sibling_pairs_copy, singletons_copy, t, func_pointer, &MAF1_copy, &MAF2_copy);
	
				if (branch_b > result) {
					if (*MAF1 != NULL) {
						delete *MAF1;
					}
					if (*MAF2 != NULL) {
						delete *MAF2;
					}
					*MAF1 = MAF1_copy;
					*MAF2 = MAF2_copy;
					result = branch_b;
				}
				else {
					if (MAF1_copy != NULL) {
						delete MAF1_copy;
					}
					if (MAF2_copy != NULL) {
						delete MAF2_copy;
					}
				}
			}
			// Cut F2_d
			if (false && cut_b) {
				pair <int, int> e_d = pendants.back();
	
				debug(cout  << "cut e_d" << endl);
	
				// copy the trees
				uforest F1_copy = uforest(F1);
				uforest F2_copy = uforest(F2);
				uforest *MAF1_copy = NULL;
				uforest *MAF2_copy = NULL;
				nodemapping twins_copy = nodemapping(twins);
				map<int, int> sibling_pairs_copy = map<int, int>(sibling_pairs);
				sibling_pairs_copy.insert(make_pair(F1_a->get_label(), F1_c->get_label()));
				sibling_pairs_copy.insert(make_pair(F1_c->get_label(), F1_a->get_label()));
				list<int> singletons_copy = list<int>(singletons);
	
				debug(cout << F2_copy << endl);
				int first_label = e_d.first;
				int second_label = e_d.second;
				pair<int,int> components = F2_copy.cut_edge(first_label, second_label);
				update_nodemapping(twins_copy, F2_copy, first_label, components.first, false);
				update_nodemapping(twins_copy, F2_copy, second_label, components.second, false);

				debug(cout << F2_copy << endl);
				if (F2_copy.get_node(components.first)->is_singleton()) {
					singletons_copy.push_back(components.first);
				}
				if (F2_copy.get_node(components.second)->is_singleton()) {
					singletons_copy.push_back(components.second);
				}
				int branch_d = tbr_distance_hlpr(F1_copy, F2_copy, k-1, twins_copy, sibling_pairs_copy, singletons_copy, t, func_pointer, &MAF1_copy, &MAF2_copy);
	
				if (branch_d > result) {
					if (*MAF1 != NULL) {
						delete *MAF1;
					}
					if (*MAF2 != NULL) {
						delete *MAF2;
					}
					*MAF1 = MAF1_copy;
					*MAF2 = MAF2_copy;
					result = branch_d;
				}
				else {
					if (MAF1_copy != NULL) {
						delete MAF1_copy;
					}
					if (MAF2_copy != NULL) {
						delete MAF2_copy;
					}
				}
			}
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

	debug(
		cout << "ANSWER FOUND" << endl;
		cout << "\t" << F1.str() << endl;
		cout << "\t" << F2.str() << endl;
	)
	int ret_k = k;
	// apply a secondary branching step. Note: may modify the AF (e.g. to a phi-forest)
	if (func_pointer != NULL) {
		ret_k = (*func_pointer)(F1, F2, twins, k, t);
	}
	// save the AFs if requested
	if (MAF1 != NULL) {
		*MAF1 = new uforest(F1);
	}

	if (MAF2 != NULL) {
		*MAF2 = new uforest(F2);
	}
	return ret_k;
}

// compute the tbr distance approximation
//
int tbr_approx(uforest &T1, uforest &T2) {
	return tbr_approx(T1, T2, 0);
}

int tbr_high_lower_bound(uforest &T1, uforest &T2) {
	return (tbr_approx(T1, T2, 0) + 3) / 4;
}

int tbr_low_lower_bound(uforest &T1, uforest &T2) {
	return (tbr_approx(T1, T2, 1) + 3) / 4;
}

int tbr_high_upper_bound(uforest &T1, uforest &T2) {
	return tbr_approx(T1, T2, 0);
}

int tbr_low_upper_bound(uforest &T1, uforest &T2) {
	return tbr_approx(T1, T2, 1);
}

int tbr_branch_bound(uforest &F1, uforest &F2, nodemapping &twins, map<int, int> &sibling_pairs, list<int> &singletons) {

	uforest F1_copy = uforest(F1);
	uforest F2_copy = uforest(F2);
	nodemapping twins_copy = nodemapping(twins);
	map<int,int> sibling_pairs_copy = map<int, int>(sibling_pairs);
	list<int> singletons_copy = list<int>(singletons);

	int result = tbr_approx_hlpr(F1_copy, F2_copy, 0, twins_copy, sibling_pairs_copy, singletons_copy);
	return (result + 3) / 4;
}

int tbr_approx(uforest &T1, uforest &T2, bool low) {
	uforest F1 = uforest(T1);
	uforest F2 = uforest(T2);

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
		if (u != NULL) {
			u->set_terminal(true);
		}
	}
	for(unode *u : F2.get_leaves()) {
		if (u != NULL) {
			u->set_terminal(true);
		}
	}

	distances_from_leaf_decorator(F1, F1.get_smallest_leaf());
	distances_from_leaf_decorator(F2, F2.get_smallest_leaf());
	debug_approx(
		cout << endl << F1 << endl;
		for(int i : F1.find_leaves()) {
			cout << i << ": " << F1.get_node(i)->get_distance() << endl;
		}

		cout << F2 << endl;
		for(int i : F2.find_leaves()) {
			cout << i << ": " << F2.get_node(i)->get_distance() << endl;
		}
	)


	// compute approximation
	int result = tbr_approx_hlpr(F1, F2, 0, twins, sibling_pairs, singletons);
	if (low) {
		return (F2.num_components() - 1);
	}
	return result;
}

int tbr_approx_hlpr(uforest &F1, uforest &F2, int k, nodemapping &twins, map<int, int> &sibling_pairs, list<int> &singletons) {

	debug_approx(cout << "tbr_approx_hlpr(" << k << ")" << endl);

	while (!sibling_pairs.empty() || !singletons.empty()) {

		// Case 1 : Isolated Subtree
		while (!singletons.empty()) {

			debug_approx(cout << "Case 1" << endl);

			unode *F2_a = F2.get_node(singletons.front());
			singletons.pop_front();
			unode *F1_a = F1.get_node(twins.get_backward(F2_a->get_label()));
			debug_approx(
				cout << "F1: " << F1.str() << endl;
				cout << "F1_a: " << F1.str_subtree(F1_a) << endl;
	
				cout << "F2: " << F2.str() << endl;
				cout << "F2_a: " << F2.str_subtree(F2_a) << endl;
			)

			// remove from sibling pairs if necessary
			map<int,int>::iterator spi = sibling_pairs.find(F1_a->get_label());
//			map<int,int>::iterator j;
			if (spi != sibling_pairs.end()) {
//				j = sibling_pairs.find(i->second);
//				if (j != sibling_pairs.end() && j->second == i->first) {
					sibling_pairs.erase(spi->second);
//				}
				sibling_pairs.erase(spi->first);
			}

			debug_approx(cout << F1 << endl);

			if (F1_a->get_parent() == NULL) {
				continue;
			}
				int first_label = F1_a->get_label();
				int second_label = F1_a->get_parent()->get_label();
				pair<int,int> components = F1.cut_edge(first_label, second_label);
				update_nodemapping(twins, F1, first_label, components.first, true);
				update_nodemapping(twins, F1, second_label, components.second, true);



			debug_approx(cout << F1 << endl);

			// check for new sibling pair
			debug_approx(
				cout << F1.str_subtree(F1.get_node(components.first)) << endl;
				cout << F1.str_subtree(F1.get_node(components.second)) << endl;
//			cout << F1_new_terminal->get_parent()->get_distance() << endl;
//			cout << F1_new_terminal->get_distance() << endl;
				cout << "checking for sibling pair" << endl;
			)

			// check for sibling pair
			unode *F1_new_node = F1.get_node(components.second);//->get_parent();
			vector<int> new_sibling_pair = vector<int>();
			if (F1_new_node != NULL) {
				for (unode *u : F1_new_node->get_neighbors()) {
					if (u->get_terminal()) {
						new_sibling_pair.push_back(u->get_label());
					}
				}
			}
			int i = new_sibling_pair.size();
			debug_approx(cout << new_sibling_pair.size() << endl);
			if (i >= 2) {
				if (sibling_pairs.find(new_sibling_pair[i-1]) == sibling_pairs.end() && sibling_pairs.find(new_sibling_pair[i-2]) == sibling_pairs.end()) {
					debug_approx(cout << "sibling_pair found" << endl);
					sibling_pairs.insert(make_pair(new_sibling_pair[i-2], new_sibling_pair[i-1]));
					sibling_pairs.insert(make_pair(new_sibling_pair[i-1], new_sibling_pair[i-2]));
				}
			}

		}

		if (sibling_pairs.empty()) {
			break;
		}

		debug_approx(
			cout << "sibling pairs: " << sibling_pairs.size() << endl; 
			for (pair<int, int> p: sibling_pairs) {
				cout << p.first << ", " << p.second << endl;
			}
		)

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
		if (F2_a->get_distance() < F2_c->get_distance() ||
				(F2_a->get_distance() == F2_c->get_distance() &&
				 F2_a->get_parent()->get_distance() < F2_c->get_parent()->get_distance())
				) {

			unode *temp = F1_a;
			F1_a = F1_c;
			F1_c = temp;

			temp = F2_a;
			F2_a = F2_c;
			F2_c = temp;
		}

		debug_approx(
			cout << "F1: " << F1.str() << endl;
			cout << "F1_a: " << F1.str_subtree(F1_a) << endl;
			cout << "F1_c: " << F1.str_subtree(F1_c) << endl;


			cout << "F2: " << F2.str() << endl;
			cout << "F2_a: " << F2.str_subtree(F2_a) << endl;
			cout << "F2_c: " << F2.str_subtree(F2_c) << endl;
		)




		
		// Case 2 : Compatible Sibling Pair

		if (F2_a->get_parent() == F2_c->get_parent() ||
				F2_a->get_parent() == F2_c ||
				F2_c->get_parent() == F2_a) {

			debug_approx(cout << "Case 2" << endl);

			// make terminal in F1
			// contract F1_a and F1_c
			unode *F1_new_terminal = F1_a->get_parent();
			debug_approx(cout << "F1_new_terminal: " << F1.str_subtree(F1_new_terminal) << endl);
			F1_new_terminal->set_terminal(true);
			F1_new_terminal->contract_neighbor(F1_a);
			F1_new_terminal->contract_neighbor(F1_c);


			if (F1_c->get_component() > -1) {
				F1.update_component(F1_c->get_component(), F1_new_terminal->get_label());
			}
			else if (F1_a->get_component() > -1) {
				F1.update_component(F1_a->get_component(), F1_new_terminal->get_label());
			}

			// check for sibling pair
			unode *F1_new_node = F1_new_terminal->get_parent();
			vector<int> new_sibling_pair = vector<int>();
			if (F1_new_node != NULL) {
				for (unode *u : F1_new_node->get_neighbors()) {
					if (u->get_terminal()) {
						new_sibling_pair.push_back(u->get_label());
					}
				}
			}
			int i = new_sibling_pair.size();
			if (i >= 2) {
				if (sibling_pairs.find(new_sibling_pair[i-1]) == sibling_pairs.end() && sibling_pairs.find(new_sibling_pair[i-2]) == sibling_pairs.end()) {
					sibling_pairs.insert(make_pair(new_sibling_pair[i-1], new_sibling_pair[i-2]));
					sibling_pairs.insert(make_pair(new_sibling_pair[i-2], new_sibling_pair[i-1]));
				}
			}
			
			// make terminal in F2
			// contract F2_a and F2_c
			debug_approx(
				cout << "d(F2_a): " << F2_a->get_distance() << endl;
				cout << "d(F2_c): " << F2_c->get_distance() << endl;
			)
			unode *F2_new_terminal = F2_a->get_parent();
			if (F2_c->get_parent() == F2_a &&
					F2_a->get_label() < -1) {
				F2_new_terminal = F2_a;
			}
			F2_new_terminal->set_terminal(true);

			if (F2_new_terminal != F2_a) {
				F2_new_terminal->contract_neighbor(F2_a);
			}
			if (F2_new_terminal != F2_c) {
				F2_new_terminal->contract_neighbor(F2_c);
			}

			if (F2_c->get_component() > -1) {
				F2.update_component(F2_c->get_component(), F2_new_terminal->get_label());
			}
			else if (F2_a->get_component() > -1) {
				F2.update_component(F2_a->get_component(), F2_new_terminal->get_label());
			}

			// add to nodemapping
			twins.add(F1_new_terminal->get_label(), F2_new_terminal->get_label());

			// check for singleton
			if (F2_new_terminal->is_singleton()) { //get_parent()->get_distance() > F2_new_terminal->get_distance()) 
				singletons.push_back(F2_new_terminal->get_label());
			}




		}

		else if (F2_a->get_parent() == F2_c) {
			debug_approx(cout << "Case 2.5" << endl);
		}

		// TODO: 2.5 where (F2_c->get_parent() == F2_a) ?

		// Case 3 : Cutting

		else {

			debug_approx(cout << "Case 3" << endl);

			// to avoid finding the pendant edges between a and c, note that cutting any two neighbors of p_a / p_c results in the same forest

			unode *F2_b1 = F2_a->get_parent()->get_neighbor_not(F2_a);
			unode *F2_b2 = F2_a->get_parent()->get_neighbor_not(F2_a, F2_b1);
			unode *F2_d1 = F2_c->get_parent()->get_neighbor_not(F2_c);
			unode *F2_d2 = F2_c->get_parent()->get_neighbor_not(F2_c, F2_d1);
			
			bool cut_a = true;
			bool cut_b = true;
			bool cut_c = true;
			bool cut_d = true;

			if (F2_b1 == NULL || F2_b2 == NULL) {
				cut_b = false;
				cut_d = false;
			}

			if (F2_d1 == NULL || F2_d2 == NULL) {
				cut_b = false;
				cut_d = false;
			}

			if (F2_a->get_parent()->get_parent() == F2_c->get_parent()) {
				cut_a = false;
				cut_c = false;
				F2_b2 = F2_a->get_parent();
				F2_d1 = F2_c->get_parent();
			}

			// OPTIMIZATIONS ?
			/*
			if (num_pendants < 2 || num_pendants > k) {
				cut_b = false;
			}
			else if (OPTIMIZE_2B && num_pendants == 2) {
				cut_a = false;
				cut_c = false;
			}
			*/

//				int branch_a = tbr_distance_hlpr(F1_copy, F2_copy, k-1, twins_copy, sibling_pairs_copy, singletons_copy, func_pointer);

			// Cut F2_a
			if (cut_a) {
				pair <int, int> e_a = make_pair(F2_a->get_label(), F2_a->get_parent()->get_label());
	
				debug_approx(cout  << "cut e_a" << endl);
	
				debug_approx(cout << F2 << endl;)
				int first_label = e_a.first;
				int second_label = e_a.second;
				pair<int,int> components = F2.cut_edge(first_label, second_label);
				update_nodemapping(twins, F2, first_label, components.first, false);
				update_nodemapping(twins, F2, second_label, components.second, false);
				debug_approx(
					cout << F2 << endl;
					cout << components.first << endl;
					cout << F2.get_node(components.first) << endl;
					cout << components.second<< endl;
					cout << F2.get_node(components.second) << endl;
				)
				// check for singleton
				debug_approx(cout << "checking if " << F2.str_subtree(F2.get_node(components.first)) << " is a singleton" << endl);
				if (F2.get_node(components.first)->is_singleton()) {
					debug_approx(cout << "it is" << endl);
					singletons.push_back(components.first);
				}
				debug_approx(cout << "checking if " << F2.str_subtree(F2.get_node(components.second)) << " is a singleton" << endl);
				if (F2.get_node(components.second)->is_singleton()) {
					debug_approx(cout << "it is" << endl);
					singletons.push_back(components.second);
				}
	
			}

			// Cut F2_c
			if (cut_c) {
				pair <int, int> e_c = make_pair(F2_c->get_label(), F2_c->get_parent()->get_label());
	
				debug_approx(cout  << "cut e_c" << endl);
	
				debug_approx(cout << F2 << endl;)
				int first_label = e_c.first;
				int second_label = e_c.second;
				pair<int,int> components = F2.cut_edge(first_label, second_label);
				update_nodemapping(twins, F2, first_label, components.first, false);
				update_nodemapping(twins, F2, second_label, components.second, false);
				debug_approx(
					cout << F2 << endl;
					cout << components.first << endl;
					cout << F2.get_node(components.first) << endl;
					cout << components.second<< endl;
					cout << F2.get_node(components.second) << endl;
				)
				// check for singleton
				debug_approx(cout << "checking if " << F2.str_subtree(F2.get_node(components.first)) << " is a singleton" << endl);
				if (F2.get_node(components.first)->is_singleton()) {
					debug_approx(cout << "it is" << endl);
					singletons.push_back(components.first);
				}
				debug_approx(cout << "checking if " << F2.str_subtree(F2.get_node(components.second)) << " is a singleton" << endl);
				if (F2.get_node(components.second)->is_singleton()) {
					debug_approx(cout << "it is" << endl);
					singletons.push_back(components.second);
				}
	
			}

			// Cut F2_b
			if (cut_b) {
				pair <int, int> e_b = make_pair(F2_b1->get_label(), F2_b2->get_label());
	
				debug_approx(cout  << "cut e_b" << endl);
	
				debug_approx(cout << F2 << endl;)
				int first_label = e_b.first;
				int second_label = e_b.second;
				pair<int,int> components = F2.cut_edge(first_label, second_label);
				update_nodemapping(twins, F2, first_label, components.first, false);
				update_nodemapping(twins, F2, second_label, components.second, false);
				debug_approx(cout << F2 << endl;)

				if (components.first != -1 && components.second != -1) {
					debug_approx(
						cout << components.first << endl;
						cout << F2.get_node(components.first) << endl;
						cout << components.second<< endl;
						cout << F2.get_node(components.second) << endl;
					)
					// check for singleton
					debug_approx(cout << "checking if " << F2.str_subtree(F2.get_node(components.first)) << " is a singleton" << endl);
					if (F2.get_node(components.first)->is_singleton()) {
						debug_approx(cout << "it is" << endl);
						singletons.push_back(components.first);
					}
					debug_approx(cout << "checking if " << F2.str_subtree(F2.get_node(components.second)) << " is a singleton" << endl);
					if (F2.get_node(components.second)->is_singleton()) {
						debug_approx(cout << "it is" << endl);
						singletons.push_back(components.second);
					}
				}
			}
			// Cut F2_d
			if (cut_d) {
				pair <int, int> e_d = make_pair(F2_d1->get_label(), F2_d2->get_label());
	
				debug_approx(cout  << "cut e_d" << endl);
	
				debug_approx(cout << F2 << endl;)
				int first_label = e_d.first;
				int second_label = e_d.second;
				pair<int,int> components = F2.cut_edge(first_label, second_label);
				update_nodemapping(twins, F2, first_label, components.first, false);
				update_nodemapping(twins, F2, second_label, components.second, false);
				debug_approx(cout << F2 << endl;)

				if (components.first != -1 && components.second != -1) {
					debug_approx(
						cout << components.first << endl;
						cout << F2.get_node(components.first) << endl;
						cout << components.second<< endl;
						cout << F2.get_node(components.second) << endl;
					)
					// check for singleton
					debug_approx(cout << "checking if " << F2.str_subtree(F2.get_node(components.first)) << " is a singleton" << endl);
					if (F2.get_node(components.first)->is_singleton()) {
						debug_approx(cout << "it is" << endl);
						singletons.push_back(components.first);
					}
					debug_approx(cout << "checking if " << F2.str_subtree(F2.get_node(components.second)) << " is a singleton" << endl);
					if (F2.get_node(components.second)->is_singleton()) {
						debug_approx(cout << "it is" << endl);
						singletons.push_back(components.second);
					}
				}
			}
			k += 4;
		}
	}

	debug_approx(
		cout << "ANSWER FOUND" << endl;
		cout << "\t" << F1.str() << endl;
		cout << "\t" << F2.str() << endl;
	)
	return k;
}

// assume a is deeper than c
list<pair<int,int> > find_pendants(unode *a, unode *c) {
	debug(cout << "find_pendants()" << endl);
	list<pair<int,int> > pendants = list<pair<int,int> >();
	// don't forget c's grandparent
	// move through "parents" until we reach the same node
	// use a list of pairs of ints instead?
	bool same_component = true;
	while(a->get_parent() != c->get_parent()) {
		debug(cout << "a:" << a->get_distance() << endl;
		cout << "c:" << c->get_distance() << endl;)
		if (a->get_distance() > c->get_distance() ||
				 (a->get_parent()->get_distance() >= c->get_parent()->get_distance())) {
			unode *prev_a = a;
			a = a->get_parent();
			if (a->get_distance() > prev_a->get_distance()) {
				same_component = false;
				break;
			}
			debug(
				cout << prev_a->get_distance() << endl;
				cout << a->get_distance() << endl;
				cout << a->get_parent()->get_distance() << endl;
				//cout << a->get_neighbor_not(prev_a, a->get_parent())->get_distance() << endl;
				cout << endl;
			)
			if (a->get_parent() != NULL && a->get_neighbor_not(prev_a, a->get_parent()) != NULL) {
				pendants.push_back(make_pair(a->get_label(), a->get_neighbor_not(prev_a, a->get_parent())->get_label()));
			}
		}
		else {
			unode *prev_c = c;
			c = c->get_parent();
			if (c->get_distance() > prev_c->get_distance()) {
				debug(cout << "c's new distance is " << c->get_distance() << endl;)
				same_component = false;
				break;
			}
			debug(
				cout << prev_c->get_distance() << endl;
				cout << c->get_distance() << endl;
				cout << c->get_parent()->get_distance() << endl;
				//cout << a->get_neighbor_not(prev_a, a->get_parent())->get_distance() << endl;
				cout << endl;
			)
			if (c->get_parent() != NULL && c->get_neighbor_not(prev_c, c->get_parent()) != NULL) {
				pendants.push_back(make_pair(c->get_label(), c->get_neighbor_not(prev_c, c->get_parent())->get_label()));
			}
		}
	}
	// return empty list if they are in different components
	if (!same_component) {
		pendants.clear();
	}
	// add the final parent edge (or last
	else {
		debug(
			cout << "FOO: " << a->get_label() << endl;
			cout << "FOO: " << c->get_label() << endl;
			cout << "FOO: " << c->get_parent()->get_label() << endl;
			cout << "FOO: " << a->get_parent()->get_neighbor_not(a,c) << endl;
	//		cout << "FOO: " << a->get_parent()->get_neighbor_not(a,c)->get_label() << endl;
		)
		if (a->get_parent()->get_distance() > a->get_distance()) {
			// a = c = root
			if (a->get_neighbor_not(a,c) != NULL) {
				pendants.push_back(make_pair(a->get_label(), a->get_neighbor_not(a,c)->get_label()));
		 }
		}
		else {
			// a = c = not root
			if (a->get_parent()->get_neighbor_not(a,c) != NULL) {
				pendants.push_back(make_pair(a->get_parent()->get_label(), a->get_parent()->get_neighbor_not(a,c)->get_label()));

			}
		}
	}
	return pendants;
}

int print_mAFs(uforest &F1, uforest &F2, nodemapping &twins, int k, int dummy) {
	cout << "ANSWER FOUND" << endl;
	cout << "\t" << F1.str() << endl;
	cout << "\t" << F2.str() << endl;
	return k;
}

int count_mAFs(uforest &F1, uforest &F2, nodemapping &twins, int k, int *count) {
	(*count)++;
	return k;
}

int print_and_count_mAFs(uforest &F1, uforest &F2, nodemapping &twins, int k, int *count) {
	cout << "ANSWER FOUND" << endl;
	cout << "\t" << F1.str() << endl;
	cout << "\t" << F2.str() << endl;
	(*count)++;
	return k;
}

int dummy_mAFs(uforest &F1, uforest &F2, nodemapping &twins, int k, int dummy) {
	return k;
}

int replug_hlpr(uforest &F1, uforest &F2, nodemapping &twins, int k, pair<uforest, uforest> T) {

	// tree alias
	uforest &T1 = T.first;
	uforest &T2 = T.second;

	// cleanup AFs - TODO: make this unnecessary
	F1.uncontract();
	F2.uncontract();
	F1.contract_degree_two();
	F2.contract_degree_two();

	int kprime = F1.num_components()-1;

	debug_replug(
		cout << endl << "REPLUG_HLPR" << endl;
		cout << "\t" << "k:  " << k << endl;
		cout << "\t" << "T1: " << T1 << endl;
		cout << "\t" << "T1: " << T1.str(true) << endl;
		cout << "\t" << "T2: " << T2 << endl;
		cout << "\t" << "T2: " << T2.str(true) << endl;
		cout << "\t" << "F1: " << F1.str() << endl;
		cout << "\t" << "F1: " << F1.str(true) << endl;
		cout << "\t" << "F2: " << F2.str() << endl;
		cout << "\t" << "F2: " << F2.str(true) << endl;

		cout << "\t" << "F1 num: nodemapping (distance)" << endl;
		for (unode *i: F1.get_node_list()) {
			cout << "\t\t";
			cout << i->get_label();
			cout << ": ";
			cout << twins.get_forward(i->get_label());
			cout << "  (";
			cout << i->get_distance();
			cout << ")";
			cout << endl;
		}

		cout << "\t" << "F2 num: nodemapping (distance)" << endl;
		for (unode *i: F2.get_node_list()) {
			cout << "\t\t";
			cout << i->get_label();
			cout << ": ";
			cout << twins.get_backward(i->get_label());
			cout << "  (";
			cout << i->get_distance();
			cout << ")";
			cout << endl;
		}
	)



	// node status
	map<int, nodestatus> T1_status = map<int, nodestatus>();
	map<int, nodestatus> T2_status = map<int, nodestatus>();

	// initialize unknown status
	for (unode *n : T1.get_node_list()) {
		T1_status.insert(make_pair(n->get_label(), UNKNOWN));
	}
	for (unode *n : T2.get_node_list()) {
		T2_status.insert(make_pair(n->get_label(), UNKNOWN));
	}

	// 1. Map alive nodes T1 <-> F1 and T2 <-> F2
	for (unode *n : F1.get_alive_nodes()) {
		T1_status[n->get_label()] = ALIVE;
	}
	for (unode *n : F2.get_alive_nodes()) {
		T2_status[n->get_label()] = ALIVE;
	}

	// 2. Map sockets (T nodes on F paths)
	// 	Socket format: s(i,j,T,l)
	// 	(i,j) - F edge
	// 	T - corresponding tree, 1 or 2
	// 	l - socket order from i to j
	//
	// 	note: need to normalize with F1 <-> F2 mapping
	list <socket *> T1_socketlist = list<socket *>();
	list <socket *> T2_socketlist = list<socket *>();

	find_sockets(T1, F1, T1_socketlist);
	find_sockets(T2, F2, T2_socketlist);

	socketcontainer T1_sockets = socketcontainer(T1_socketlist);
	socketcontainer T2_sockets = socketcontainer(T2_socketlist);

	for (socket *s: T1_socketlist) {
		T1_status[s->dead] = SOCKET;
	}

	for (socket *s: T2_socketlist) {
		T2_status[s->dead] = SOCKET;
	}

	debug_replug(
		cout << "T1 sockets: " << endl;
		for (socket *s: T1_socketlist) {
			cout << "\t" << s->str() << endl;
		}
		cout << endl;
		cout << "T2 sockets: " << endl;
		for (socket *s: T2_socketlist) {
			cout << "\t" << s->str() << endl;
		}
		cout << endl;
	)

	// 3. Map dead nodes (not alive or sockets)
	for (pair<const int, nodestatus> &p : T1_status) {
		if (p.second == UNKNOWN) {
			// ignore a node not in T1
			if (T1.get_node(p.first)->get_num_neighbors() > 0) {
				p.second = DEAD;
			}
		}
	}
	for (pair<const int, nodestatus> &p : T2_status) {
		if (p.second == UNKNOWN) {
			// ignore a node not in T2
			if (T2.get_node(p.first)->get_num_neighbors() > 0) {
				p.second = DEAD;
			}
		}
	}

	// test node status
	debug_replug(
		cout << "T1 node status" << endl;
		for (unode *n : T1.get_node_list()) {
			cout << n->get_label() << ": " <<
				nodestatus_name[T1_status[n->get_label()]] << endl;
		}
		cout << endl;

		cout << "T2 node status" << endl;
		for (unode *n : T2.get_node_list()) {
			cout << n->get_label() << ": " <<
				nodestatus_name[T2_status[n->get_label()]] << endl;
		}
		cout << endl;
	)


	// 4. Identify dead components and corresponding socket sets
	//
	vector<list<int> > T1_dead_components = vector<list<int> >();
	vector<list<int> > T2_dead_components = vector<list<int> >();

	find_dead_components(T1, T1_sockets, T1_status, T1_dead_components);
	find_dead_components(T2, T2_sockets, T2_status, T2_dead_components);

	// test dead components
	int i = 0;
	debug_replug(
		cout << "T1 dead components" << endl;
		for(list<int> &l : T1_dead_components) {
			i++;
			cout << "\t" << i << ":" << endl;
			cout << "\t\t";
			for (int x : l) {
				cout << x << ",";
			}
			cout << endl;
		}
	
		i = 0;
		cout << "T2 dead components" << endl;
		for(list<int> &l : T2_dead_components) {
			i++;
			cout << "\t" << i << ":" << endl;
			cout << "\t\t";
			for (int x : l) {
				cout << x << ",";
			}
			cout << endl;
		}
	)

	// 4.5. Identify Multifurcating socket resolutions
	//
	// Normalize T2 sockets
	list<socket *> T2_normalized_socketlist = list<socket *>();
	for (socket *s : T2_socketlist) {
		int new_i = twins.get_backward(s->i);
		int new_j = twins.get_backward(s->j);
		T2_normalized_socketlist.push_back(new socket(new_i, new_j, s->dead, s->num));
	}

	socketcontainer T2_sockets_normalized = socketcontainer(T2_normalized_socketlist);

	//
	// identify sets of T1 and T2 sockets that map to the same AF edge
	vector<pair<vector<socket *> , vector<socket *> > > socketcandidates = vector<pair<vector<socket *> , vector<socket *> > >();
	i = 0;
	int max_sockets = 0;
	for (pair<pair<int, int>, vector <socket*> > socketgroup : T1_sockets.sockets) {
		i++;
		int start = socketgroup.first.first;
		int end = socketgroup.first.second;
		vector <socket *> &T1_group = socketgroup.second;
		vector <socket *> &T2_group = T2_sockets_normalized.find(start, end);
		debug_replug(
			cout << "socket group " << i << ": " << start << ", " << end << endl;
			cout << "\t" << "T1: " << T1_group.size() << endl;
			cout << "\t" << "T2: " << T2_group.size() << endl;
		)
		if (T1_group.size() < T2_group.size()) {
			max_sockets += T1_group.size();
		}
		else {
			max_sockets += T2_group.size();
		}
		if (!T1_group.empty() && !T2_group.empty()) {
			socketcandidates.push_back(make_pair(T1_group, T2_group));
		}
	}

	debug_replug(
		cout << socketcandidates.size() << " socket groups " << endl;
	)

	// Branch and bound. We have a lower bound of kprime - max # of sockets
	int lower_bound = kprime - max_sockets;
	if (lower_bound < 0) {
		lower_bound = 0;
	}
	debug_replug(
		cout << "lower_bound: " << lower_bound + kprime << endl;
		cout << "allowed: " << k + kprime << endl;
	)

	if (lower_bound > k) {
		return -1;
	}

	// phi-node sockets
	vector<pair<socket *, socket *> > phi_node_sockets = vector<pair<socket *, socket *> >();

	// 5. Test each combination of socket assignments for the maximum number
	// 		of phi-nodes
	k = check_socket_group_combinations(k, kprime, T1_sockets, T2_sockets_normalized, T1_dead_components, T2_dead_components, socketcandidates, phi_node_sockets);


	// number of phi-nodes to add to each edge
	map<pair<int, int>, int> F1_add_phi_nodes = map<pair<int, int>, int>();
	map<pair<int, int>, int> F2_add_phi_nodes = map<pair<int, int>, int>();

	for(pair<socket *, socket *> p : phi_node_sockets) {
		F1_add_phi_nodes[make_pair(p.first->i, p.first->j)]++;
		socket *T2_p = T2_sockets.find_dead(p.second->dead);
		F2_add_phi_nodes[make_pair(T2_p->i, T2_p->j)]++;
	}

	debug_replug(
		cout << endl;
		cout << "F1 refresher:" << F1.str(true) << endl;
		cout << "F2 refresher:" << F2.str(true) << endl;
		cout << endl;

		cout << "F1 phi-nodes to add:" << endl;
		for(pair<pair<int, int>, int> phi_node_count : F1_add_phi_nodes) {
			cout << "\t" << phi_node_count.first.first << ", " << phi_node_count.first.second << ": " << phi_node_count.second << endl;
		}

		cout << "F2 phi-nodes to add:" << endl;
		for(pair<pair<int, int>, int> phi_node_count : F2_add_phi_nodes) {
			cout << "\t" << phi_node_count.first.first << ", " << phi_node_count.first.second << ": " << phi_node_count.second << endl;
		}
	)

	add_phi_nodes(F1, F1_add_phi_nodes);
	add_phi_nodes(F2, F2_add_phi_nodes);

	debug_replug(
		cout << "F1: " << F1.str() << endl;
		cout << "F2: " << F1.str() << endl;
	)


	// 6. return distance

	return k;
}


int check_socket_group_combinations(int k, int kprime, socketcontainer &T1_sockets, socketcontainer &T2_sockets_normalized, vector<list<int> > &T1_dead_components, vector<list<int> > &T2_dead_components, vector<pair<vector<socket *> , vector<socket *> > > &socketcandidates, vector<pair<socket *, socket *> > &phi_node_sockets) {

	vector<pair<socket *, socket *> > sockets = vector<pair<socket *, socket *> >();
	return check_socket_group_combinations(0, 0, 0, 0, k, kprime, T1_sockets, T2_sockets_normalized, T1_dead_components, T2_dead_components, socketcandidates, sockets, phi_node_sockets);

	return k;
}

// enumerate each combination of socket pairings recursively
int check_socket_group_combinations(int n, int i, int j, int last, int k, int kprime, socketcontainer &T1_sockets, socketcontainer &T2_sockets_normalized, vector<list<int> > &T1_dead_components, vector<list<int> > &T2_dead_components, vector<pair<vector<socket *> , vector<socket *> > > &socketcandidates, vector<pair<socket *, socket *> > &sockets, vector<pair<socket *, socket *> > &phi_node_sockets) {
/*	cout << "combinations(";
	cout << n << ", "; 
	cout << i << ", "; 
	cout << j << ", "; 
	cout << last << ")"; 
	cout << endl;
	*/
	// test this combination
	if (n >= socketcandidates.size()) {
		return check_socket_group_combination(k, kprime, T1_sockets, T2_sockets_normalized, T1_dead_components, T2_dead_components, socketcandidates, sockets, phi_node_sockets);
	}
	// move to next socket group
	if (i >= socketcandidates[n].first.size() ||
			j >= socketcandidates[n].second.size()) {
		// only advance if there is no open assignment
		if (last != 0) {
			return -1;
		}
		else {
			n++;
			return check_socket_group_combinations(n, 0, 0, 0, k, kprime, T1_sockets, T2_sockets_normalized, T1_dead_components, T2_dead_components, socketcandidates, sockets, phi_node_sockets);
		}
	}

	int best_k = k - kprime;
	vector<pair<socket *, socket *> > candidate_phi_node_sockets = vector<pair<socket *, socket *> >();

	// match i and j
	sockets.push_back(make_pair(socketcandidates[n].first[i], socketcandidates[n].second[j]));
	candidate_phi_node_sockets.clear();
	int k1 = check_socket_group_combinations(n, i+1, j+1, 0, k, kprime, T1_sockets, T2_sockets_normalized, T1_dead_components, T2_dead_components, socketcandidates, sockets, candidate_phi_node_sockets);
	if (k1 > best_k) {
		best_k = k1;
		phi_node_sockets.swap(candidate_phi_node_sockets);
	}
	sockets.pop_back();

	// skip i, can't skip j next time
	int k2 = -1;
	if (last != 1) {
		candidate_phi_node_sockets.clear();
		k2 = check_socket_group_combinations(n, i+1, j, -1, k, kprime, T1_sockets, T2_sockets_normalized, T1_dead_components, T2_dead_components, socketcandidates, sockets, candidate_phi_node_sockets);
		if (k2 > best_k) {
			best_k = k2;
			phi_node_sockets.swap(candidate_phi_node_sockets);
		}
	}

	// skip j, can't skip i next time
	int k3 = -1;
	if (last != -1) {
		candidate_phi_node_sockets.clear();
		k3 = check_socket_group_combinations(n, i, j+1, 1, k, kprime, T1_sockets, T2_sockets_normalized, T1_dead_components, T2_dead_components, socketcandidates, sockets, candidate_phi_node_sockets);
		if (k3 > best_k) {
			best_k = k3;
			phi_node_sockets.swap(candidate_phi_node_sockets);
		}
	}

	return best_k;
}

int check_socket_group_combination(int k, int kprime, socketcontainer &T1_sockets, socketcontainer &T2_sockets_normalized, vector<list<int> > &T1_dead_components, vector<list<int> > &T2_dead_components, vector<pair<vector<socket *> , vector<socket *> > > &socketcandidates, vector<pair<socket *, socket *> > &sockets, vector<pair<socket *, socket *> > &candidate_phi_node_sockets) {

	debug_phi_nodes(
		cout << "check_socket_group_combination()" << endl;
		for (pair<socket *, socket *> &p : sockets) {
				cout << p.first->str() << "\t" << p.second->str() << endl;
		}
	)

	// map of socket pointers to paired socket numbers
	map<socket *, int> socket_pointer_map = map<socket *, int>();
	int i = 0;
	for(int i = 0; i < sockets.size(); i++) {
		socket_pointer_map[sockets[i].first] = i;
		socket_pointer_map[sockets[i].second] = i;
	}

	// vector of vectors with socket constraints
	// one socket from each constraint cannot have a phi node
	vector<vector<int> > constraints = vector<vector<int> >();


	for(list<int> &dead_component : T1_dead_components) {
		vector<int> constraint = vector<int>();
		bool trivial = get_constraint(dead_component, T1_sockets, socket_pointer_map, constraint);
		if (trivial != true) {
			constraints.push_back(constraint);
		}
	}
	for(list<int> &dead_component : T2_dead_components) {
		vector<int> constraint = vector<int>();
		bool trivial = get_constraint(dead_component, T2_sockets_normalized, socket_pointer_map, constraint);
		if (trivial != true) {
			constraints.push_back(constraint);
		}
	}
	debug_phi_nodes(
		cout << "found " << constraints.size() << " constraints" << endl; 
	)

	int non_phi_nodes = 0;
	// determine the number of non_phi_nodes
	list<int> changed_sockets = list<int>();
	if (constraints.size() > 0) {
		non_phi_nodes = solve_monotonic_2sat_2vars(constraints, changed_sockets);
	}
	int phi_nodes = sockets.size() - non_phi_nodes;

	// find the phi nodes
	vector<bool> changed_sockets_vector = vector<bool>(sockets.size(), false);
	for (int socket : changed_sockets) {
		changed_sockets_vector[socket] = true;
	}
	debug_phi_nodes(
		cout << "phi-node sockets:" << endl;
	)
	for (int i = 0; i < sockets.size(); i++) {
		if (!changed_sockets_vector[i]) {
			candidate_phi_node_sockets.push_back(sockets[i]);
			debug_phi_nodes(cout << sockets[i].first->str() << "\t" << sockets[i].second->str() << endl;)
		}
	}
	debug_phi_nodes(
		cout << phi_nodes << " phi_nodes" << endl;
		cout << non_phi_nodes << " non_phi_nodes" << endl;
		cout << "replug_distance: " << (2 * kprime) - phi_nodes << endl;
	)
	return k - (kprime - phi_nodes);

}

// determine the minimum number of variables that must be false to satisfy a monotonic 2 SAT set of CNF constraints where each variable occurs at most twice
// works by converting to a maximum edge cover problem where each vertex is a clause and each edge is a variable spanning two clauses
int solve_monotonic_2sat_2vars(vector<vector<int> > &constraints, list<int> &changed_sockets) {

	debug_phi_nodes(cout << "solve_monotonic_2sat_2vars()" << endl;)

	// inverse map sockets to constraints
	map<int, vector<int> > constraint_map = map<int, vector<int> >();
	for(int i = 0; i < constraints.size(); i++) {
		for(int socket : constraints[i]) {
			constraint_map[socket].push_back(i);
		}
	}

	// build the constraint graph
	undirected_graph G(constraints.size());
	for(int i = 0; i < constraints.size(); i++) {
		for(int socket : constraints[i]) {
			for (int constraint : constraint_map[socket]) {
				if (constraint != i) {
			// for constraints containing this socket
				if (i < constraint) {
					add_edge(i, constraint, G);
				}
				}
			}
		}
	}


	// print the graph
	typedef boost::graph_traits<undirected_graph>::edge_iterator edge_iter;
	edge_iter ei;
	edge_iter ei_end;

	debug_phi_nodes(
		for (boost::tie(ei, ei_end) = boost::edges(G); ei != ei_end; ei++) {
			cout << *ei << endl;
		}
		cout << endl;
	)


	// find a maximum matching
	
	// matching data structure
	vector<boost::graph_traits<undirected_graph>::vertex_descriptor> mate(constraints.size());
	bool success = boost::checked_edmonds_maximum_cardinality_matching(G, &mate[0]);
	assert(success);

	int matching_size = boost::matching_size(G, &mate[0]);
	debug_phi_nodes(cout << "found a matching of size " << matching_size << endl;)


	// expand to an edge cover by adding (#vertices - (2 * size of matching))
	// total is #vertices - matching_size
	int edge_cover_size = constraints.size() - matching_size;
	debug_phi_nodes(cout << "found an edge cover of size " << edge_cover_size << endl;)


	// determine the sockets that move
	vector<bool> handled_constraints = vector<bool>(constraints.size(), false);
	typedef boost::graph_traits<undirected_graph>::vertex_iterator vertex_iter;
	vertex_iter vi;
	vertex_iter vi_end;
	for(boost::tie(vi, vi_end) = vertices(G); vi != vi_end; vi++) {
		// vertex is matched
		if (mate[*vi] != boost::graph_traits<undirected_graph>::null_vertex() && *vi < mate[*vi]) {
			debug_phi_nodes(cout << "{" << *vi << ", " << mate[*vi] << "}" << endl;)
			handled_constraints[*vi] = true;
			handled_constraints[mate[*vi]] = true;
			// find a socket in both constraints
			bool done = false;
			for(int socket : constraints[*vi]) {
				if (done) {
					break;
				}
				for(int constraint : constraint_map[socket]) {
					if (constraint == mate[*vi]) {
						debug_phi_nodes(cout << "both contain " << socket << endl;)
						changed_sockets.push_back(socket);
						done = true;
						break;
					}
				}
			}

		}
		else if (!handled_constraints[*vi]) {
			// vertex is not matched, pick an arbitrary socket
			int chosen_socket = constraints[*vi][0];
			debug_phi_nodes(
				cout << "{" << *vi << "}" << endl;
				cout << "contains " << chosen_socket << endl;
			)
			changed_sockets.push_back(chosen_socket);
		}
	}

	debug_phi_nodes(
		cout << changed_sockets.size();
		if (changed_sockets.size() == edge_cover_size) {
			cout << " == ";
		}
		else {
			cout << " !- ";
		}
		cout << edge_cover_size << endl;

		cout << "unmatched: " << endl;
		for (int socket : changed_sockets) {
			cout << "\t" << socket << endl;
		}
	)

	return edge_cover_size;

}

int solve_monotonic_2sat_2vars(vector<vector<int> > &constraints) {
	list<int> changed_sockets = list<int>();
	return solve_monotonic_2sat_2vars(constraints, changed_sockets);
}

bool get_constraint(list<int> &dead_component, socketcontainer &T_sockets, map<socket *, int> &socket_pointer_map, vector<int> &constraint) {

	bool trivial = false;
	for(int dead : dead_component) {
		socket *s = T_sockets.find_dead(dead);
//		cout << "dead: " << dead << endl;
//		cout << "socket: " << s->str() << endl;
		map<socket *, int>::iterator socket_pointer_map_iterator =
				socket_pointer_map.find(s);
		if (socket_pointer_map_iterator != socket_pointer_map.end()) {
			int socket_num = socket_pointer_map_iterator->second;
			constraint.push_back(socket_num);
		}
		else {
			trivial = true;
//					break;
//					debugging only
				constraint.push_back(-1);
		}
		
	}
	debug_phi_nodes(
		cout << "constraint: ";
		for(int i = 0; i < constraint.size(); i++) {
			if (i != 0) {
				cout << ",";
			}
			cout << constraint[i]+1;
		}
		cout << endl;
	)
	return trivial;
}


void find_sockets(uforest &T, uforest &F, list<socket *> &sockets) {
	for (unode *c : F.get_components()) {
		// leaf component
		if (c->get_neighbors().empty()) {
			find_sockets_hlpr(c, c, T, sockets);
		}
		// cherry component
		if (c->get_neighbors().size() == 2) {
			unode *lc = T.get_node(c->get_neighbors().front()->get_label());
			unode *rc = T.get_node(c->get_neighbors().back()->get_label());
			add_sockets(lc, rc, sockets);
		}
		// general case
		else {
			find_sockets_hlpr(c, NULL, T, sockets);
		}
	}
}
void find_sockets_hlpr(unode *n, unode *prev, uforest &T, list<socket *> &sockets) {
	for (unode *x : n->get_neighbors()) {
		if (x != prev) {
			find_sockets_hlpr(x, n, T, sockets);
		}
	}
	// follow path of sockets
	if (prev != NULL) {
		add_sockets(T.get_node(n->get_label()), T.get_node(prev->get_label()), sockets);
	}
}

void add_sockets(unode *xstart, unode *ystart, list<socket *> &sockets) {
	int start, end;
	unode *x, *y;
	if (xstart->get_label() <= ystart->get_label()) {
		x = xstart;
		y = ystart;
	}
	else {
		x = ystart;
		y = xstart;
	}
	start = x->get_label();
	end = y->get_label();
	debug_sockets(
		cout << "add_sockets(" << x->get_label() << ", " << y->get_label() << ")" << endl;
	)

	// store each side of the walk separately to maintain the correct order
	list<socket *> x_path = list<socket *>();
	list<socket *> y_path = list<socket *>();

	// singleton leaf or cherry component
	if (x == y) {
		x_path.push_back(new socket(start, end, start, -1));
		debug_sockets(
			cout << "\t" << "finding socket s(";
			cout << start << ", ";
			cout << end << ", ";
			cout << start;
			cout << ")" << endl;
		)
	}


	// TODO: How to identify a socket adjacent to LCA(xstart, ystart) ?
	//  easy for xstart==ystart
	//  Is there always a socket at a component root unless it is the original root?

	while (x != y) {
		debug_sockets(
			cout << "\t" << "x: " << x->get_label() << "  (" << x->get_distance() << ")" << endl;
			cout << "\t" << "y: " << y->get_label() << "  (" << y->get_distance() << ")" << endl;
		)
		if (x->get_distance() >= y->get_distance()) {
			unode *next = x->get_parent();
			if (next != y) {
				x_path.push_back(new socket(start, end, next->get_label(), -1));
				debug_sockets(
					cout << "\t" << "finding socket s(";
					cout << start << ", ";
					cout << end << ", ";
					cout << next->get_label();
					cout << ")" << endl;
				)
			}
			x = next;
		}
		else {
			unode *next = y->get_parent();
			if (next != x) {
				y_path.push_front(new socket(start, end, next->get_label(), -1));
				debug_sockets(
					cout << "\t" << "finding socket s(";
					cout << start << ", ";
					cout << end << ", ";
					cout << next->get_label();
					cout << ")" << endl;
				)
			}
			y = next;
		}
	}

	int count = 0;

	x_path.splice(x_path.end(), y_path);

	for (socket *s : x_path) {
		s->num = ++count;
		debug_sockets(
			cout << "\t" << "adding socket s(";
			cout << s->i << ", ";
			cout << s->j << ", ";
			cout << s->dead  << ", ";
			cout << s->num << ")" << endl;
		)
	}

	sockets.splice(sockets.end(), x_path);


}

void find_dead_components(uforest &T, socketcontainer &S, map<int, nodestatus> &T_status, vector<list<int> > &T_dead_components) {
	unode *root = T.get_leaf(T.get_smallest_leaf());
	find_dead_components_hlpr(root, NULL, -1, T, S, T_status, T_dead_components);
}

// TODO: problem when a node has multiple sockets

void find_dead_components_hlpr(unode *n, unode *prev, int component, uforest &T, socketcontainer &S, map<int, nodestatus> &T_status, vector<list<int> > &T_dead_components) {
	if (prev != NULL) {
		int n_label = n->get_label();
		int prev_label = prev->get_label();
//		cout << "checking " << prev_label << " -> " << n_label << endl;
		if (T_status[prev_label] == SOCKET) {
			// found a new dead component
			if (T_status[n_label] == SOCKET) {
				// check that this isn't an adjacent socket
				socket *n_socket = S.find_dead(n_label);
				socket *prev_socket = S.find_dead(prev_label);
				if (n_socket->i != prev_socket->i ||
						n_socket->j != prev_socket->j) {

//					cout << "found" << endl;
				
					component = T_dead_components.size();
					T_dead_components.push_back(list<int>());
					T_dead_components[component].push_back(prev_label);
					T_dead_components[component].push_back(n_label);
					component = -1;
				}
			}
			// found a new dead component
			else if (T_status[n_label] == DEAD) {
				component = T_dead_components.size();
				T_dead_components.push_back(list<int>());
				T_dead_components[component].push_back(prev_label);
			}
		}
		else if (T_status[prev_label] == DEAD) {
			// found end of a dead component
			if (T_status[n_label] == SOCKET) {
				T_dead_components[component].push_back(n_label);
				component = -1;
			}
		}
	}
	// explore neighbors, carrying a dead component number if necessary
	for (unode *x : n->get_neighbors()) {
		if (x != prev) {
			find_dead_components_hlpr(x, n, component, T, S, T_status, T_dead_components);
		}
	}
}

void update_nodemapping(nodemapping &twins, uforest &F, int original_label, int new_label, bool forward) {
	// odd bug
	if (new_label == -1) {
		return;
	}
	if (original_label != new_label) {
//		cout << endl << "FOO" << endl << endl;
//		cout << "\t" << original_label << "->" << new_label << endl;
		int twin;
		if (forward) {
			twin = twins.get_forward(original_label);
		}
		else {
			twin = twins.get_backward(original_label);
		}
//		cout << "\t" << twin << endl;
		if (twin != -1) {
			int parent_label = new_label;
			if (F.get_node(new_label)->get_parent() != NULL) {
				parent_label = F.get_node(new_label)->get_parent()->get_label();
			}
			if (forward) {
				twins.add(parent_label, twin);
			}
			else {
				twins.add(twin, parent_label);
			}
		}
	}
}

// add the set of phi nodes to the forest
// TODO: include original socket numbers to reuse nodes?
void add_phi_nodes(uforest &F, map<pair<int, int>, int> &F_add_phi_nodes) {
//	cout << "F: " << F.str(true) << endl;
	for(pair<pair<int, int>, int> phi_node_count : F_add_phi_nodes) {
		int start_id = phi_node_count.first.first;
		int end_id = phi_node_count.first.second;
		int count = phi_node_count.second;
//		cout << "adding " << start_id << ", " << end_id << ": " << count << endl;

		unode *start = F.get_node(start_id);
		unode *end = F.get_node(end_id);

		bool skip_last = false;
		// if start and end are the same, don't connect twice
		if (start == end) {
			skip_last = true;
		}

		while(count > 0) {
			start->remove_neighbor(end);
			end->remove_neighbor(start);
			unode *new_socket = F.get_node(F.add_internal_node());
			unode *new_phi_node = F.get_node(F.add_phi_node());
			start->add_neighbor(new_socket);
			new_socket->add_neighbor(start);
			new_socket->add_neighbor(new_phi_node);
			new_phi_node->add_neighbor(new_socket);
			if (count > 1 || !skip_last) {
				new_socket->add_neighbor(end);
				end->add_neighbor(new_socket);
			}
			start = new_socket;
			count--;
//			cout << "F: " << F.str(true) << endl;
		}
	}
	return;
}





#endif
