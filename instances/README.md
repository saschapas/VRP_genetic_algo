## Instance format VRP

An instance with `n` nodes is given by a file with `n+3` lines:

- The first line contains the number of nodes `n`
- The second line contains the number of vehicles `m`
- The third line contains the vehicle capacity `C`
- The next `n` lines describe the node demands and the two-dimensional coordinates of the nodes, where each line corresponds to a node and consists of three integers: the first integer corresponds to the node demand, the second integer describes the horizontal coordinate, while the third integer corresponds to the vertical coordinate

It is implicitly assumed that the first node (i.e., the node described by the first line in the node section) corresponds to the depot.
The costs between nodes have to be calculated with the Euclidean metric, taking into account the given two-dimensional coordinates.
Non-integer values `x` in the calculated travel costs must be rounded according to formula $int(x+0.5)$, with $int$ describing the type conversion from a floating point number to an integer as implemented in most programming languages like Java or C++.
Numbers in the instance files are separated using whitespace characters.

The following table displays the objective values of <b>best-known solutions</b> (BKS) to compare with for each given instance, and also indicates if it was verified as optimally solved.

| Instance   | BKS      | Opt      |
|:----------:|:--------:|:--------:|
| A-n32-k5   | 784      | True     |
| A-n48-k7   | 1073     | True     |
| A-n61-k9   | 1034     | True     |
| A-n69-k9   | 1159     | True     |
| A-n80-k10  | 1763     | True     |
| M-n121-k7  | 1034     | True     |
| M-n200-k17 | 1373     | False    |
| P-n101-k4  | 681      | True     |
| P-n16-k8   | 450      | True     |
| P-n76-k5   | 627      | True     |

For more details on the instances, see <https://www.coin-or.org/SYMPHONY/branchandcut/VRP/data/index.htm.old#F>.