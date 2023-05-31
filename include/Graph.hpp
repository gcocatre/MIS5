// Project: Maximum Independent Set (MIS, version 5)
// Computer Science (2016-2017), 4th semester
// Author: Guillaume Cocatre-Zilgien
// Professor: Michel Ventou

#ifndef Graph_hpp
#define Graph_hpp

#include <algorithm>
#include <limits>
#include <utility>
#include <vector>
#include <set>
#include "Vertex.hpp"

// Here we decide which implementation we want to use:
// typedef ListVertex Vertex;
// typedef SetVertex Vertex;
typedef VectorVertex Vertex;

typedef std::size_t cardinal;
typedef std::pair<vid, vid> vidPair;
typedef std::set<vid> vidSet;

const unsigned RANDOM_NUMBER_OF_VERTICES = std::numeric_limits<unsigned>::max();
const unsigned NO_VERTEX_FOUND = std::numeric_limits<unsigned>::max();
const unsigned SCORE_LIMIT = std::numeric_limits<unsigned>::max();
const std::size_t BAD_MIS = 0;

enum algorithm {ALGO_EXACT, ALGO_MDG, ALGO_VSA, ALGO_MVSA, ALGO_BEST, ALGO_ENUM_SIZE};

class Graph
{
private:
    unsigned long long numberOfCalls;           // number of recursive calls for the exact algorithm
protected:
    std::vector<Vertex> vertices;               // set of vertices in the graph
    unsigned numberOfEdges;                     // number of edges in the graph
    cardinal enabledCardinal;                   // number of vertices that were NOT disabled
    vidSet MIS;                                 // Maximum Independent Set
    void constructMIS(vidSet &);                // build a MIS from the list of enabled vertices
    void disableVertex(vid);                    // disable a vertex
    void disableVertexNeighborhood(vid);        // disable the neighbors of a vertex
    void restoreVertex(Vertex &);               // re-enable a vertex
    Vertex & getNeighborByIndex(vid v, int i)   {
        auto it = vertices[v].neighbors.cbegin();
        std::advance(it, i);
        return vertices[*it];
    }
    vid getVertexOfDegree(score);               // get a vertex with a degree of a certain value
    vidPair getTwinVertices();                  // get a couple of "obvious" vertices for the exact algorithm
    vid getMaxDegreeVertex();                   // get a vertex with the highest degree
    vid getMaxSupportVertex();                  // get a vertex with the highest support value (for the VSA heuristic)
    vid getMinSupportVertexNeighbor();          // get a vertex with the smallest support value (for the MVSA heuristic)
    unsigned getRandomNumber(unsigned, unsigned);
    cardinal processObviousVertex(std::vector<Vertex> &); // process "obvious" vertices for the exact algorithm
    cardinal processTwinVertices(std::vector<Vertex> &);  // same as above
    cardinal findExactMIS();                    // exact algorithm
public:
    Graph()                                     : numberOfEdges {0}, enabledCardinal {0} {}
    Graph(cardinal);
    Graph(const char *path);
    void print();
    Vertex & operator[](vid v)                  { return vertices.at(v); }
    cardinal getSize()                          { return vertices.size(); }
    unsigned getNumberOfEdges()                 { return numberOfEdges; }
    unsigned getMaxNumberOfEdges()              { return (unsigned) ((vertices.size() * (vertices.size() - 1 )) / 2); }
    float getNumberOfEdgesPerVertex()           { return 2.0 * numberOfEdges / vertices.size(); }
    float getEdgePercentage()                   { return (numberOfEdges * 100.0 / this->getMaxNumberOfEdges()); }
    bool isIndependent()                        { return (numberOfEdges == 0); }
    vidSet getMIS()                             { return MIS; }
    unsigned connectRandomly(float);            // generate a graph randomly
    void connectVertices(vid, vid);             // connect two vertices with an edge
    void disconnectVertices(vid, vid);          // disconnect two vertices (remove the edge)
    void disconnectVertex(vid);                 // disconnect a vertex from its neighbors
    bool areVerticesConnected(vid, vid);        // are these two vertices connected by an edge?
    vidSet findMIS(algorithm);                  // find a Maximum Independent Set
    bool checkMIS(const vidSet &);              // check the validity of a MIS
    bool saveMIS(const vidSet &, const char *); // save the MIS in a file
    unsigned long long getNumberOfCalls()       { return this->numberOfCalls; }
};

#endif /* Graph_hpp */
