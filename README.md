# MIS5

This was a university project, with comments translated from the original French.
I haven't compiled it in years, and while I remember managing to compile it on OS X,
it will probably be easier to compile it on a Linux system.

## The Project

This project is my implementation of an NP-complete problem in graph theory.

Given a set of *vertices*, some of which are connected by *edges*, some of which are not,
we want to find a subset of vertices that are not connected to any other vertex,
with a cardinality that is as large as possible. Such a subset is called a
[**Maximum Independent Set**](https://en.wikipedia.org/wiki/Independent_set_(graph_theory)).
There may be more than one subset with the same cardinality, but we only care about finding *one*.

The *degree* of a vertex is the number of vertices it is connected to. In other words, it represents its number of
edges.
An isolated vertex has a degree of *0*.
A vertex connected to one other vertex has a degree of *1*.
A vertex connected to two other vertices has a degree of *2*, and so on.

The *support* of a vertex is the sum of the degrees of its immediate neighbors.
E.g. a vertex that is connected to a vertex of degree 2 and to another vertex of degree 3, has a support value of 5.

In order to find a *MIS*, we use both *heuristics* (algorithms that are fast and provide a solution that is *not* exact, but close
enough) and one *exact* algorithm (much slower but that provides a result that is known to be correct, i.e. there cannot
be a better result).

## My Implementation

This particular implementation runs 3 heuristics in parallel, and uses the result as a *shortcut* for the exact
algorithm (it ignores subsets that are equal or inferior in cardinality):

- Max Degree (MDG): no known inventor. We find the vertex with the highest degree, remove it from the set, and repeat
until we are left with an independent set (no edges remain).

- Vertex Support Algorithm (VSA): S. Balaji, K. Kannan and Swaminathan Venkatasubramanian. We find the vertex with the
highest support, remove it from the set, and repeat until we end up with an independent set.

- Modified Vertex Support Algorithm (MVSA): Khan Imran and Khan Hasham. We find the vertex with the *lowest* support,
select its direct neighbors, find among those the vertex with the lowest support, take that one out of the set, and
repeat until no edges remain.

### The Exact Algorithm

Loop over all vertices. There are two possibilities with each vertex:
1. It is part of a Maximum Independent Set.
2. It is *not* part of a MIS.

We then explore the two possibilities, recursively, until we end up with a subset (of unconnected vertices) that is as
large, or larger, as any other. In case (1), we remove all of its neighbors (they cannot be part of a MIS, since they
are connected to at least one vertex). In case (2), we only remove that particular vertex (it is not part of a MIS, and
we have to evaluate its direct neighbors independently - no pun intended).

As one can intuitively see right away, that algorithm is not only slow, but one possibility is much faster to evaluate
fully than the other. Case (1) is obviously much faster than case (2), since we often remove more than one vertex at a
time. That makes it quite difficult to effectively parallelize (one thread will finish much earlier than the other).

We can, however, and I do, run the 3 heuristics in parallel, and use the best result's cardinality to ignore all
subsets with an equal or lower cardinality (we don't even bother to evaluate them). It helps running the exact algorithm
a bit faster, but its execution time (or complexity) remains exponential.

One major finding (it was a surprise to me, and it was a surprise as well to my other class mates when they subsequently
observed the same phenomenon in their own implementations, on different hardware), was that **half of the execution time
of the exact algorithm was spent in RAM**, saving and copying subsets for evaluation. **Read / write operations in RAM
are not free**, even with fast RAM. That is one of the reasons why Apple's M1 and M2 chips are so efficient: they put
RAM, CPU and GPU cores as close to each other as possible.

## Usage

```
./MIS -n VERTEX_COUNT [ -p EDGE_PERCENTAGE ] [ -e ] [ -o result.txt ]
   or: ./MIS -i graph.txt [ -e ] [ -o result.txt ]

    -h          Display help
    -e          Run the exact algorithm
    -i FILE     Load a file containing a graph
    -o FILE     Save the graph in a file
    -n NUMBER   Generate a random graph with N vertices
    -p NUMBER   Set the percentage of edges when generating a graph
    -x PIXELS   Window width in pixels (800 by default)
    -y PIXELS   Window height in pixels (600 by default)

A few notes: only graphs with 60 vertices or less, will be displayed graphically.
Heuristics are always executed.
In order to generate a graph with a random number of vertices, specify -n 0.
```
