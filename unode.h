#ifndef INCLUDE_UNODE
#define INCLUDE_UNODE

#include <list>
#include <sstream>

using namespace std;

class unode;

class unode {
	private:
	int label;
	list<unode *> neighbors;
	list<unode *> contracted_neighbors;
	int num_neighbors;
	int component;
	bool terminal;
	int distance;

	public:
	unode() {
		label = -1;
		neighbors = list<unode *>();
		contracted_neighbors = list<unode *>();
		num_neighbors = 0;
		component = -1;
		terminal = false;
		distance = -1;
	}
	unode(int l) {
		label = l;
		neighbors = list<unode *>();
		contracted_neighbors = list<unode *>();
		num_neighbors = 0;
		num_neighbors = 0;
		component = -1;
		terminal = false;
		distance = -1;
	}
	unode(const unode &n) {
		label = n.label;
		neighbors = list<unode *>(n.neighbors);
		contracted_neighbors = list<unode *>(n.contracted_neighbors);
		num_neighbors = 0;
		num_neighbors = n.num_neighbors;
		component = n.component;
		terminal = n.terminal;
		distance = n.distance;
	}

	void add_neighbor(unode *n) {
		neighbors.push_back(n);
		num_neighbors++;
	}

	void add_contracted_neighbor(unode *n) {
		contracted_neighbors.push_back(n);
	}

	void add_parent(unode *n) {
		neighbors.push_front(n);
		num_neighbors++;
	}

	bool remove_neighbor(unode *n) {
		list<unode *>::iterator i;
		bool result = false;
		for(i = neighbors.begin(); i != neighbors.end(); i++) {
			if ((*i) == n) {
				result = true;
				break;
			}
		}
		if (result) {
			neighbors.remove(*i);
			num_neighbors--;
		}
		return result;
	}

	bool contract_neighbor(unode *n) {
		bool ret = remove_neighbor(n);
		if (ret) {
			contracted_neighbors.push_back(n);
		}
		return ret;
	}

	string str() const {
		stringstream ss;
		ss << label;
		return ss.str();
	}

	bool operator ==(const unode n) const {
		return this->label == n.label;
	}
	bool operator !=(const unode n) const {
		return this->label != n.label;
	}


	int get_label() const {
		return label;
	}

	const list<unode *> &const_neighbors() const {
		return neighbors;
	}

	const list<unode *> &const_contracted_neighbors() const {
		return contracted_neighbors;
	}

	list<unode *> &get_neighbors() {
		return neighbors;
	}

	list<unode *> &get_contracted_neighbors() {
		return contracted_neighbors;
	}

	int get_num_neighbors() {
		return num_neighbors;
	}

	bool is_leaf() {
		return (num_neighbors == 1);
	}

	void set_component(int c) {
		component = c;
	}

	int get_component() {
		return component;
	}

	void root(int l) {
		unode *p = NULL;
		for (unode *n : get_neighbors()) {
			if (n->get_label() == l) {
				p = n;
			}
			else {
				n->root(get_label());
			}
		}
		if (p != NULL) {
			neighbors.remove(p);
			neighbors.push_front(p);
		}
	}

	void rotate(int l) {
		unode *p = NULL;
		for (unode *n : get_neighbors()) {
			if (n->get_label() == l) {
				p = n;
			}
		}
		if (p != NULL) {
			neighbors.remove(p);
			neighbors.push_front(p);
		}
	}

	unode *get_parent() {
		if (neighbors.empty()) {
			return NULL;
		}
		return neighbors.front();
	}

	unode *get_neighbor_not(unode *a, unode *b) {
		cout << "neighbors: " << get_neighbors().size() << endl;
		cout << "c_neighbors: " << get_contracted_neighbors().size() << endl;
		for (unode *x : get_neighbors()) {
			if (*x != *a && *x != *b) {
				return x;
			}
		}
		return NULL;
	}

	void set_terminal(bool t) {
		terminal = t;
	}

	bool get_terminal() {
		return terminal;
	}

	unode *get_sibling() {
		unode *parent = get_parent();
		int count = 0;
		for (unode *x : parent->get_neighbors()) {
			if (count > 0 && x != this) {
				return x;
			}
			count++;
		}
	}

	void clear_neighbors() {
		neighbors.clear();
		num_neighbors = 0;
	}

	void clear_contracted_neighbors() {
		contracted_neighbors.clear();
	}

	unode *contract() {
		if (num_neighbors == 1 && contracted_neighbors.empty()) {
			unode *p = neighbors.front();
			if (p->is_leaf() && this->get_label() < -1) {
				p->remove_neighbor(this);
				if (component > -1) {
					p->set_component(component);
				}
				return p;
			}
		}
		else if (num_neighbors == 2 && contracted_neighbors.empty()) {
			unode *p = neighbors.front();
			unode *c = *(next(neighbors.begin(), 1));
			cout << "contracting:" << endl;
			cout << p << endl;
			cout << c << endl;
			if (!p->is_leaf() || !c->is_leaf()) {
				clear_neighbors();
				p->remove_neighbor(this);
				c->remove_neighbor(this);
				c->add_parent(p);
				p->add_neighbor(c);
				c->set_distance(distance);
				if (!get_terminal()) {
					p->set_terminal(false);
				}
				if (component > -1) {
					p->set_component(component);
				}
				return p;
			}
		}
		return this;
	}

	void set_distance(int d) {
		distance = d;
	}

	int get_distance() {
		return distance;
	}
	bool is_singleton() {
		if (num_neighbors == 0) {
			return true;
		}
		return false;
		/*
		if (get_parent()->get_distance() <= get_distance()) {
			return false;
		}
		bool ret = this->get_terminal();
		for (unode *c : this->get_neighbors()) {
			if (c->get_terminal() == false) {
				ret = false;
			}
		}
		return ret;
		*/
	}

};

#endif
