// Project: Maximum Independent Set (MIS, version 5)
// Computer Science (2016-2017), 4th semester
// Author: Guillaume Cocatre-Zilgien
// Professor: Michel Ventou

#ifndef Vertex_hpp
#define Vertex_hpp

#include <algorithm>
#include <forward_list>
#include <limits>
#include <set>
#include <stdexcept>
#include <vector>

typedef unsigned vid;
typedef unsigned score;

// Generic class for selecting whichever implementation we want to use;
// it enabled me to test the performance of 3 different implementations.
template <typename vidList>
class GenericVertex
{
    friend class Graph;
protected:
    vid id;
    score degree;
    vidList neighbors;
    bool enabled;
    void disable()                          { enabled = false; neighbors.clear(); degree = 0; }
    void clearNeighbors()                   { neighbors.clear(); degree = 0; }
    virtual void addNeighbor(vid) = 0;
    virtual void removeNeighbor(vid) = 0;
public:
    GenericVertex()                         : id {0}, degree {0}, enabled {true} {}
    GenericVertex(vid n)                    : id {n}, degree {0}, enabled {true} {}
    vid operator[](int i) {
        if (i < degree) {
            auto it = neighbors.cbegin() + i; return *it;
        } else {
            throw std::out_of_range("No such neighbor");
        }
    }
    vid getID()                             { return id; }
    void setID(vid id)                      { this->id = id; }
    score getDegree()                       { return degree; }
    bool isEnabled()                        { return enabled; }
    bool isIsolated()                       { return neighbors.empty(); }
    bool isConnectedTo(vid v)               { return find(neighbors.cbegin(), neighbors.cend(), v) != neighbors.cend(); }
};

// Vertex class that uses a list of neighbors (turned out to be too slow)
class ListVertex : public GenericVertex<std::forward_list<vid>>
{
    friend class Graph;
protected:
    void addNeighbor(vid v)                 { neighbors.push_front(v); ++degree; }
    void removeNeighbor(vid v)              { neighbors.remove(v); --degree; }
};

// Vertex class using a vector of neighbors (turned out to be the fastest)
class VectorVertex : public GenericVertex<std::vector<vid>>
{
    friend class Graph;
protected:
    void addNeighbor(vid v)                 { neighbors.push_back(v); ++degree; }
    void removeNeighbor(vid v)              { neighbors.erase(find(neighbors.cbegin(), neighbors.cend(), v)); --degree; }
};

// Vertex class using a set of neighbors (turned out to be too slow)
class SetVertex : public GenericVertex<std::set<vid>>
{
    friend class Graph;
protected:
    void addNeighbor(vid v)                 { neighbors.insert(v); ++degree; }
    void removeNeighbor(vid v)              { neighbors.erase(v); --degree; }
};

#endif /* Vertex_hpp */
