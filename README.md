uspr version 1.0.0
====

Software for computing the unrooted subtree Prune-and-Regraft (uSPR) distance between unrooted binary topologies in Newick format. Can also compute the Tree Bisection-and-Reconnection (TBR) distance and an intermediate distance called the replug distance. The uSPR distance is a natural distance metric with respect to phylogenetic tree search, as common tree search and sampling software mainly use SPR operations (or NNI operations, a subset of SPR operations). The uSPR distance is also a lower bound on the number of lateral gene transfer events required to explain the difference between a reference/species tree and a gene tree.

Please note that the uSPR distance is NP-hard to compute (as are the TBR and replug distances) and most of the algorithms used in this software scale exponentially with the distance computed. This version of uspr is aimed at trees with up to 50 leaves and uSPR distances up to 14.

If you are interested in comparing rooted trees in terms of SPR operations, you should use rspr instead (https://github.com/cwhidden/rspr). rspr is also much more efficient and can easily handle pairs of binary rooted trees with 200+ leaves and distances > 50.

This is the code for the preprint "Calculating the Unrooted Subtree-Prune-and-Regraft Distance" by Chris Whidden and Frederick A. Matsen IV. Please see "http://arxiv.org/abs/1511.07529" for more information on the motivation behind this project, the algorithms used and their expected performance.

Copying
====
```
Copyright 2015 Chris Whidden
cwhidden@fredhutch.org
https://github.com/cwhidden/uspr
December 10, 2015
Version 1.0.0
```

This file is part of uspr.

uspr is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
uspr is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with uspr.  If not, see <http://www.gnu.org/licenses/>.

Usage
====

Usage: uspr [OPTIONS]

Input: pairs of unrooted binary trees in Newick format on standard input, one tree per line .

Output:
By default, uspr will compute 4 distance metrics:
- an approximation of the TBR distance that will provide a rough range. Note that the uSPR  distance is at most double the TBR distance, so doubling this range provides a rough estimate of the uSPR distance. This can be used to determine if the trees are too disimilar to be compared using this software.
- the TBR distance, and an associated maximum agreement forest. The agreement forest shows which parts of the trees remain stable under one minimal set of TBR operations that transform one tree into the other. Tree edges not included in the agreement forest are those modified by these operations.
- the replug distance, an intermediate distance between the TBR and uSPR distances and an associatied maximum endpoint agreement forest. This agreement forest indicates endpoints of modified edges that remain stable with asterisks (\*). The replug distance is similar to the uSPR distance, but does not guarantee that intermediate operations result in a tree.
- the uSPR distance and the number of trees that were explored to compute it using a progressive A\* search.

Example Output
====
```
./uspr < test_trees/replug_example_3.tre
T1: (a1,a2,(a3,(a4,(b1,(b2,(b3,(b4,(c1,(c2,(c3,c4))))))))));
T2: (a1,(a2,(a3,a4)),(((b1,b2),(b3,(c1,(c2,(c3,c4))))),b4));
a_TBR: 2 <= d_TBR <= 6
{2}
d_TBR = 2
F1: ((a1,a2),a3,a4); (b1,b2); (b3,(((c3,c4),c2),c1),b4);
F2: ((a1,a2),a3,a4); (b4,(((c3,c4),c2),c1),b3); (b1,b2);
{2} {3}
d_R = 3
F1: ((a1,a2),a3,a4); (b1,b2,(b3,b4)); ((*,c1),(c3,c4),c2);
F2: ((a1,a2),a3,a4); ((b3,b4),b1,b2); ((*,c1),(c3,c4),c2);
examined 347 trees
d_USPR = 3

./uspr < test_trees/replug_example_4.tre
T1: (a1,a2,(a3,(a4,(b1,(b2,(b3,b4))))));
T2: (a1,(a2,(a3,a4)),(((b1,b2),b3),b4));
a_TBR: 1 <= d_TBR <= 3
{1}
d_TBR = 1
F1: ((a1,a2),a3,a4); (b1,b2,(b3,b4));
F2: ((a1,a2),a3,a4); ((b4,b3),b1,b2);
{1} {2}
d_R = 2
F1: ((a1,a2),a3,a4); (b1,b2,(b3,b4));
F2: ((a1,a2),a3,a4); ((b4,b3),b1,b2);
examined 170 trees
d_USPR = 2
```

Options
====
```
Basic options

-h --help              Print program information and exit.

--tbr-approx
--tbr
--replug
--uspr                 By default, uspr will compute all 4 distances. If any of these
                       options are specified then uspr will compute only the specified
                       distances.
                       
--print-mAFs           Print all maximal agreement forests found during execution of
                       the program.
--count-mAFs           Count all maximal agreement forests found during execution of
                       the program.

Algorithm Options

--no-approx-estimate
--no-tbr-estimate
--no-replug-estimate   Disable the TBR approximation, TBR distance, or replug distance
                       heuristics when computing the SPR distance. In most cases these
                       options will greatly increase the time required by uspr.
                       
--no-opt
--no-protect-b         Disable all MAF optimizations or just the edge protection
                       optimization for enumerating agreement forests. In most
                       cases these options will greatly increase the time required
                       by uspr.
```

uspr_neighbors
====
`uspr_neighbors` is a utility program that calculates all trees that can be obtained from an unrooted binary tree using a single subtree prune and regraft operation. This is called the SPR neighborhood of a tree. Input one tree in newick format on STDIN. Note that this program will output or count the input tree unless the `--ignore_original` option is specified.
```
Options

-h --help              Print program information and exit.

--ignore_original      Do not output or count the input tree.

--size_only            Count the number of SPR neighbors instead of printing them.
```

Files
====
|File|Description|
|----|-----------|
|COPYING|            The GPL License version 3|
|libs/boost/graph|   Boost Graph Libraries, a required dependency|
|Makefile|           Makefile|
|README.md|          This README|
|tbr.h|              Library to calculate TBR and replug distances|
|test_trees|         A folder of test tree pairs (*.tre) and expected output (*.test)|
|uforest.h|          Forest data structure|
|unode.h|            Tree node data structure|
|uspr|               The main executable after compilation|
|uspr.cpp|           Main file and interface code|
|uspr.h|             Library to calculate SPR distances|
|uspr_neighbors|     A sub executable for computing all SPR neighbors of a tree|
|uspr_neighbors.cpp| Main uspr_neighbors file and interface code|
|uspr_neighbors.h|   uspr_neighbors library code|
|utree.h|            Tree data structure|

Dependencies
====
uspr requires the Boost Graph Library, part of the Boost C++ Libraries. The Boost Graph library is available from https://github.com/boostorg/graph and is also available in the package manager of most Linux distributions. The library is included as a submodule of uspr on github. If you obtained uspr directly from the github repository and do not have the Boost Libraries installed on your system, then the command `git submodule update --init --recursive` can be used to download the libraries.


Installation
====
uspr is a command-line program written in C++. To use it, compile uspr.cpp and execute the resulting program. On systems with the g++ compiler and make program, the included make file will compile uspr; simply run `make`. You can then run `make test` to confirm that the program was built and is running correctly.

Citing uspr
====
If you use uspr in your research, please cite:

Chris Whidden and Frederick A. Matsen IV. Calculating the Unrooted Subtree-Prune-and-Regraft Distance. eprint arXiv:1511.07529. http://arxiv.org/abs/1511.07529
