
#ifndef INCLUDE_USPR
#define INCLUDE_USPR


#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <memory>
#include <ctime>
#include <cstdlib>
#include "utree.h"
#include "unode.h"
#include "uforest.h"
#include "tbr.h"
#include "uspr_neighbors.h"

// options
bool USE_TBR_APPROX_ESTIMATE = true;
bool USE_TBR_ESTIMATE = true;
bool USE_REPLUG_ESTIMATE = true;

// classes

typedef enum {REPLUG, TBR, TBR_APPROX, BFS} estimator_t;
string estimator_t_name[] = {"REPLUG", "TBR", "TBR_APPROX", "BFS"};

// distance estimate
class tree_distance {
	public:
	// cost to reach this tree
	int cost;
	// estimate of distance to destination
	int estimate;
	// cost + estimate
	int distance;
	// the tree
	string tree;
	// estimator used
	estimator_t estimator;

	tree_distance(int c, int d, string t, estimator_t e) {
		cost = c;
		estimate = d;
		distance = c + d;
		tree = t;
		estimator = e;
	}

	friend bool operator < (tree_distance a, tree_distance b);
	friend bool operator <= (tree_distance a, tree_distance b);

};

// prefer better estimates when equal
bool operator < (tree_distance a, tree_distance b) {
	if (a.distance + a.distance == b.distance) {
		return a.estimator < b.estimator;
	}
	return a.distance < b.distance; } 
bool operator <= (tree_distance a, tree_distance b) {
	if (a.distance == b.distance) {
		return a.estimator <= b.estimator;
	}
	return a.distance <= b.distance;
}


// function prototypes
int uspr_distance(uforest &T1, uforest &T2);

// functions
int uspr_distance(uforest &T1, uforest &T2) {

	// normalize tree order
	T1.normalize_order();
	T2.normalize_order();

	// set of visited trees
	set<string> visited_trees = set<string>();

	// target string
	string target = T2.str();


	// priority queue of trees
	multiset<tree_distance> distance_priority_queue = multiset<tree_distance>();

	// check if trees are equal
	// TODO: normalize strings
	if (T1.str() == target) {
		return 0;
	}

	// start with the first distance
	visited_trees.insert(T1.str());
	distance_priority_queue.insert(tree_distance(0, 1, T1.str(), BFS));



	// final estimator
	estimator_t final_estimator = BFS;
	if (USE_TBR_APPROX_ESTIMATE) {
		final_estimator = TBR_APPROX;
	}
	if (USE_TBR_ESTIMATE) {
		final_estimator = TBR;
	}
	if (USE_REPLUG_ESTIMATE) {
		final_estimator = REPLUG;
	}

	// explore the next tree
	while (!distance_priority_queue.empty()) {
		multiset<tree_distance>::iterator it = distance_priority_queue.begin();

		// debugging
		cout << it->distance << ": " << it->cost << " + " << it->estimate << " using " << estimator_t_name[it->estimator] << endl;
		cout << "\t" << it->tree << endl;

		// remove the old entry
		int cost = it->cost;
		string tree = it->tree;
		estimator_t prev_estimator = it->estimator;
		distance_priority_queue.erase(it);

		// build the tree
		uforest T = uforest(tree);

		// check if the distance estimate is final
		if (prev_estimator != final_estimator) {
			// if not, compute the next estimate and insert it into the queue
			int distance = 1;
			if (prev_estimator > TBR_APPROX &&
					USE_TBR_APPROX_ESTIMATE) {
				distance = tbr_high_lower_bound(T, T2);
				distance_priority_queue.insert(tree_distance(cost, distance, tree, TBR_APPROX));
			}
			else if (prev_estimator > TBR &&
					USE_TBR_ESTIMATE) {
				distance = tbr_distance(T, T2);
				distance_priority_queue.insert(tree_distance(cost, distance, tree, TBR));
			}
			else if (prev_estimator > REPLUG &&
					USE_REPLUG_ESTIMATE) {
				distance = replug_distance(T, T2);
				distance_priority_queue.insert(tree_distance(cost, distance, tree, REPLUG));
			}
			continue;
		}

		// if final, get the tree neighborhood
		// TODO: enumerate SPRs
		// note: valid SPRs - move one endpoint to anywhere within its subtree
		// i.e. enumerate edges, then enumerate over each subtree
		// what about duplicates? similar to RSPR, NNIs?
		// for each tree
			// check if it has already been seen (visited_trees)
			// if not then insert it into the queue (initial BFS - cost + 1)
			// TODO: stop immediately if we find T2?

		list<utree *> neighbors = get_neighbors(&T);




	}

	return -1;
}

#endif
