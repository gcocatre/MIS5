// Project: Maximum Independent Set (MIS, version 5)
// Computer Science (2016-2017), 4th semester
// Author: Guillaume Cocatre-Zilgien
// Professor: Michel Ventou

#include <iostream>
#include <fstream>
#include <future>
#include <random>
#include <stdexcept>
#include <string>
#include "Graph.hpp"

using namespace std;

// Constructor with the cardinality of a set as argument
Graph::Graph(cardinal n)
{
    Vertex vertex;
    this->numberOfEdges = 0;
    if (n == RANDOM_NUMBER_OF_VERTICES) {
        n = this->getRandomNumber(2, 40);
    }
    this->enabledCardinal = n;

    this->vertices.resize(n, vertex);
    for (vid v = 0; v < n; ++v) {
        this->vertices[v].id = v;
    }
}

// Constructor with the path to a file containing a graph as argument
Graph::Graph(const char *path)
{
    this->numberOfEdges = 0;
    this->enabledCardinal = 0;

    bool invalid {false};
    ifstream file;
    file.open(path, ios::in);
    if (file.is_open()) {
        unsigned n {0}, e {0};
        vid v, w;
        string line;
        while (getline(file, line)) {
            if (n == 0) {
                if (line.compare(0,2,"c ") == 0) continue;

                if (sscanf(line.c_str(), "p edge %u %u", &n, &e) != 2) {
                    if (sscanf(line.c_str(), "p col %u %u", &n, &e) != 2) {
                        try { n = (unsigned) stoul(line); } catch (...) { n = 0; }
                    }
                }

                if (n != 0) {
                    Vertex vertex;
                    this->enabledCardinal = n;
                    this->vertices.resize(n, vertex);
                    for (vid v = 0; v < n; ++v) {
                        this->vertices[v].id = v;
                    }
                }
            } else {
                if (line.compare(0,3,"MIS") == 0) {
                    break;
                } else if (sscanf(line.c_str(), "e %u %u", &v, &w) == 2 || sscanf(line.c_str(), "%u %u", &v, &w) == 2) {
                    if (v > 0 && w > 0) {
                        this->connectVertices((v-1), (w-1));
                    } else {
                        invalid = true;
                        break;
                    }
                }
            }
        }
        file.close();
        if (invalid) {
            cerr << "Error: invalid file " << path << endl;
            this->vertices.clear();
            this->enabledCardinal = 0;
            this->numberOfEdges = 0;
        }
    } else {
        cerr << "Error: unable to open file " << path << endl;
    }
}

unsigned Graph::getRandomNumber(unsigned min, unsigned max)
{
    static std::mt19937 rng;
    static bool seeded {false};
    if (!seeded) {
        rng.seed(std::random_device()());
    }
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
    return (unsigned) dist(rng);
}

unsigned Graph::connectRandomly(float percentage)
{
    if (percentage < 1.0 || percentage > 100.0) {
        percentage = (float) this->getRandomNumber(1, 100);
    }

    unsigned max = (unsigned) (this->vertices.size() * (this->vertices.size() - 1)) / 2;
    unsigned n = (unsigned) ((percentage * max / 100.0) + 0.5);
    unsigned p, maxPairIndex = (max - 1);
    vid v, w, maxVid = (vid) (this->vertices.size() - 1);

    if (this->numberOfEdges > 0) {
        for (auto &vertex : this->vertices) {
            this->disconnectVertex(vertex.id);
        }
    }

    // Generate the complete list of vertices that can be connected
    vector<pair<vid, vid>> pairs;
    for (v = 0; v < maxVid; ++v) {
        for (w = v + 1; w <= maxVid; ++w) {
            pairs.push_back(make_pair(v, w));
        }
    }

    // Randomly select two vertices to connect together
    while (this->numberOfEdges < n) {
        do {
            p = this->getRandomNumber(0, maxPairIndex);
        } while (pairs[p].second == 0);
        this->connectVertices(pairs[p].first, pairs[p].second);
        // It is faster to flag a couple of vertices as being "unavailable", than to erase them from the vector.
        pairs[p].second = 0;
    }

    return this->numberOfEdges;
}

void Graph::print()
{
    cout << this->vertices.size() << "\n";
    for (auto &vertex : this->vertices) {
        if (vertex.isIsolated()) {
            cout << vertex.id + 1 << "\n";
        } else {
            for (auto &w : vertex.neighbors) {
                if (w > vertex.id) {
                    cout << vertex.id + 1 << " " << w + 1 << "\n";
                }
            }
        }
    }
    cout << flush;
}

void Graph::connectVertices(vid v, vid w)
{
    this->vertices[v].addNeighbor(w);
    this->vertices[w].addNeighbor(v);
    ++this->numberOfEdges;
}

void Graph::disconnectVertices(vid v, vid w)
{
    this->vertices[v].removeNeighbor(w);
    this->vertices[w].removeNeighbor(v);
    --this->numberOfEdges;
}

void Graph::disconnectVertex(vid v)
{
    Vertex &vertex {this->vertices[v]};
    this->numberOfEdges -= vertex.degree;
    for (auto &w : vertex.neighbors) {
        this->vertices[w].removeNeighbor(v);
    }
    vertex.clearNeighbors();
}

bool Graph::areVerticesConnected(vid v, vid w)
{
    if (this->vertices[v].degree < this->vertices[w].degree) {
        return this->vertices[v].isConnectedTo(w);
    } else {
        return this->vertices[w].isConnectedTo(v);
    }
}

void Graph::disableVertex(vid v)
{
    Vertex &vertex {this->vertices[v]};
    this->numberOfEdges -= vertex.degree;
    for (auto &w : vertex.neighbors) {
        this->vertices[w].removeNeighbor(v);
    }
    vertex.disable();
    --this->enabledCardinal;
}

void Graph::disableVertexNeighborhood(vid v)
{
    Vertex &vertex {this->vertices[v]};
    this->enabledCardinal -= vertex.degree;
    for (auto &w : vertex.neighbors) {
        Vertex &neighbor {this->vertices[w]};
        this->numberOfEdges -= neighbor.degree;
        for (auto &x : neighbor.neighbors) {
            if (x != v) { // don't affect the first loop
                this->vertices[x].removeNeighbor(w);
            }
        }
        neighbor.disable();
    }
    vertex.clearNeighbors();
}

void Graph::restoreVertex(Vertex &vertex)
{
    if (!this->vertices[vertex.id].enabled) {
        this->vertices[vertex.id].enabled = true;
        ++this->enabledCardinal;
    }

    for (auto &w : vertex.neighbors) {
        if (!this->vertices[w].enabled) {
            this->vertices[w].enabled = true;
            ++this->enabledCardinal;
        }
        if (!this->areVerticesConnected(vertex.id, w)) {
            this->connectVertices(vertex.id, w);
        }
    }
}

vid Graph::getMaxDegreeVertex()
{
    vid candidate {NO_VERTEX_FOUND};
    score maxDegree {0};
    for (auto &vertex : this->vertices) {
        if (vertex.degree > maxDegree) {
            candidate = vertex.id;
            maxDegree = vertex.degree;
        }
    }
    return candidate;
}

vid Graph::getMaxSupportVertex()
{
    vid candidate {NO_VERTEX_FOUND};
    score support, maxSupport {0}, maxDegree {0};
    for (auto &vertex : this->vertices) {
        if (vertex.isEnabled()) {
            support = 0;
            for (auto &w : vertex.neighbors) {
                support += this->vertices[w].degree;
            }

            if (support > maxSupport) {
                candidate = vertex.id;
                maxSupport = support;
                maxDegree = vertex.degree;
            } else if (support == maxSupport && vertex.degree > maxDegree) {
                candidate = vertex.id;
                maxDegree = vertex.degree;
            }
        }
    }
    return candidate;
}

vid Graph::getMinSupportVertexNeighbor()
{
    vid candidate {NO_VERTEX_FOUND};
    score support {0}, minSupport {SCORE_LIMIT};
    vector<vid> minSupportVertices;

    // Create a list of vertices with the smallest non-null "support" value
    for (auto &vertex : this->vertices) {
        if (vertex.isEnabled() && vertex.degree > 0) {
            support = 0;
            for (auto &w : vertex.neighbors) {
                support += this->vertices[w].degree;
            }

            if (support < minSupport) {
                minSupportVertices.clear();
                minSupportVertices.push_back(vertex.id);
                minSupport = support;
            } else if (support == minSupport) {
                minSupportVertices.push_back(vertex.id);
            }
        }
    }

    // Find among their neighbors the vertex with the smallest "support" value
    if (minSupportVertices.size() > 0) {
        minSupport = SCORE_LIMIT;
        for (auto &v : minSupportVertices) {
            Vertex &vertex {this->vertices[v]};
            for (auto &w : vertex.neighbors) {
                Vertex &neighbor {this->vertices[w]};
                support = 0;
                for (auto &x : neighbor.neighbors) {
                    support += this->vertices[x].degree;
                }
                if (support < minSupport) {
                    candidate = w;
                    minSupport = support;
                }
            }
        }
    }
    return candidate;
}

void Graph::constructMIS(vidSet &set)
{
    set.clear();
    for (auto &vertex : this->vertices) {
        if (vertex.isEnabled()) {
            set.insert(vertex.id);
        }
    }
}

vid Graph::getVertexOfDegree(score d)
{
    for (auto &vertex : this->vertices) {
        if (vertex.degree == d) {
            return vertex.id;
        }
    }
    return NO_VERTEX_FOUND;
}

/**
 *  A vertex with a degree of 1 is always part of a MIS.
 *  If such vertices are found, they are processed directly
 *  and the result is returned to the calling method.
 */
cardinal Graph::processObviousVertex(vector<Vertex> &backup)
{
    vid candidate;
    while ((candidate = this->getVertexOfDegree(1)) != NO_VERTEX_FOUND) {
        Vertex &neighbor {this->getNeighborByIndex(candidate, 0)};
        backup.push_back(neighbor);
        this->disableVertex(neighbor.id);
        if (this->isIndependent()) {
            if (this->enabledCardinal > this->MIS.size()) {
                this->constructMIS(this->MIS);
            }
            for (auto &vertex : backup) {
                this->restoreVertex(vertex);
            }
            return this->enabledCardinal - 1;
        } else {
            return BAD_MIS;
        }
    }
    return BAD_MIS;
}

/**
 *  A vertex with a degree of 2, whose neighbors are connected to each other,
 *  is always part of a MIS. The same can be observed with a pair of vertices with a degree of 2,
 *  who share the same neighbors. If such vertices are found, they are processed directly
 *  and the result is returned to the calling method.
 */
vidPair Graph::getTwinVertices()
{
    for (auto &vertex : this->vertices) {
        if (vertex.degree == 2) {
            Vertex &n1 {this->getNeighborByIndex(vertex.id, 0)}, &n2 {this->getNeighborByIndex(vertex.id, 1)};
            if (n1.isConnectedTo(n2.id)) {
                return make_pair(n1.id, n2.id); // the neighbors are connected to each other
            } else {
                for (auto &w : n1.neighbors) {
                    if (this->vertices[w].degree == 2 && w != vertex.id && this->vertices[w].isConnectedTo(n2.id)) {
                        return make_pair(n1.id, n2.id); // 2 vertices with a degree of 2 share the same neighbors
                    }
                }
            }
        }
    }
    return make_pair(NO_VERTEX_FOUND, NO_VERTEX_FOUND);
}

cardinal Graph::processTwinVertices(vector<Vertex> &backup)
{
    vidPair candidates, notFound(NO_VERTEX_FOUND, NO_VERTEX_FOUND);
    while ((candidates = this->getTwinVertices()) != notFound) {
        backup.push_back(this->vertices[candidates.first]);
        backup.push_back(this->vertices[candidates.second]);
        this->disableVertex(candidates.first);
        this->disableVertex(candidates.second);
        if (this->isIndependent()) {
            if (this->enabledCardinal > this->MIS.size()) {
                this->constructMIS(this->MIS);
            }
            for (auto &vertex : backup) {
                this->restoreVertex(vertex);
            }
            return this->enabledCardinal - 2;
        } else {
            return BAD_MIS;
        }
    }
    return BAD_MIS;
}

/**
 *  Vertices with a degree of 1 or 2 are processed first (see previous methods).
 *  Next, c1 <= c2 is always true, so even if a MIS with a cardinality of c2 is found,
 *  we MUST check g because it might contain a larger MIS. That is why g is checked before copy,
 *  even if the latter is faster to check.
 */
cardinal Graph::findExactMIS()
{
    static unsigned long long x {0};
    vector<Vertex> backup0;
    cardinal c0 {0};
    if ((c0 = this->processObviousVertex(backup0)) != BAD_MIS) {
        return c0;
    } else if ((c0 = this->processTwinVertices(backup0)) != BAD_MIS) {
        return c0;
    }

    vid candidate {this->getMaxDegreeVertex()};
    cardinal c1 {this->enabledCardinal - 1}, c2 {(this->enabledCardinal - this->vertices[candidate].degree)};

    if (c1 > this->MIS.size()) {
        Vertex backup1(this->vertices[candidate]); // keep a copy of the vertex
        this->disableVertex(candidate); // disable the vertex with the highest degree
        if (!this->isIndependent()) { // terminal condition
            c1 = this->findExactMIS();
        } else if (this->enabledCardinal > this->MIS.size()) {
            this->constructMIS(this->MIS);
        }
        this->restoreVertex(backup1); // restore the disabled vertex
    } else {
        c1 = 0;
    }

    if (c2 > this->MIS.size() && c2 > c1) {
        Graph copy(*this);
        copy.disableVertexNeighborhood(candidate); // keep the vertex with the highest degree
        if (!copy.isIndependent()) { // terminal condition
            c2 = copy.findExactMIS();
            if (copy.MIS.size() > this->MIS.size()) {
                this->MIS = copy.MIS;
            }
        } else if (copy.enabledCardinal > this->MIS.size()) {
            copy.constructMIS(this->MIS);
        }
    } else {
        c2 = 0;
    }

    this->numberOfCalls = ++x;
    for (auto &vertex : backup0) this->restoreVertex(vertex);
    return ((c1 | c2) == 0) ? BAD_MIS : ((c1 > c2) ? c1 : c2);
}

/**
 *  ALGO_BEST: run the best heuristics in parallel (rudimentary multithreading) and return the best result.
 *
 *  ALGO_EXACT: find a large MIS fast, thanks to heuristics; ignore candidates with a smaller cardinality
 *  when searching for a MIS (that is actually maximum, with a 100% certainty).
 *
 *  Heuristics: select a vertex and disable it; rince and repeat until no edges remain (we get a MIS).
 */
vidSet Graph::findMIS(algorithm algo)
{
    vidSet set;
    if (this->isIndependent()) {
        if (this->enabledCardinal > this->MIS.size()) {
            this->constructMIS(this->MIS);
        }
    } else if (algo == ALGO_BEST) {
        vector<vidSet> sets;
        vector<future<vidSet>> threads;
        for (auto heuristic : {ALGO_MVSA, ALGO_MDG, ALGO_VSA}) {
            threads.push_back(async(&Graph::findMIS, this, heuristic));
        }
        for (auto &thread : threads) {
            sets.push_back(thread.get());
        }
        sort(sets.begin(), sets.end(), [](vidSet a, vidSet b) { return a.size() > b.size(); });
        set = *(sets.begin()); // pointer to the largest MIS
    } else if (algo == ALGO_EXACT) {
        if (this->MIS.size() == 0) {
            this->findMIS(ALGO_MVSA);
            this->findMIS(ALGO_MDG);
        }
        this->findExactMIS();
    } else {
        Graph g(*this);
        switch (algo) {
            case ALGO_MDG:  while (!g.isIndependent()) g.disableVertex(g.getMaxDegreeVertex()); break;
            case ALGO_VSA:  while (!g.isIndependent()) g.disableVertex(g.getMaxSupportVertex()); break;
            case ALGO_MVSA: while (!g.isIndependent()) g.disableVertex(g.getMinSupportVertexNeighbor()); break;
            default: throw std::invalid_argument("Invalid algorithm"); break;
        }
        g.constructMIS(set);
    }

    if (set.size() > this->MIS.size()) {
        this->MIS = set;
    }
    return (set.size() > 0) ? set : this->MIS;
}

bool Graph::checkMIS(const vidSet &MIS)
{
    for (vid v : MIS) {
        for (vid w : MIS) {
            if (v < w && this->areVerticesConnected(v, w)) {
                return false;
            }
        }
    }
    return true;
}

bool Graph::saveMIS(const vidSet &MIS, const char *path)
{
    ofstream file;
    file.open(path, ios::out);
    if (!file.is_open()) {
        cerr << "Error while opening file " << path << endl;
        return false;
    }

    file << this->vertices.size() << "\n";
    for (auto &vertex : this->vertices) {
        if (vertex.isIsolated()) {
            file << vertex.id + 1 << "\n";
        } else {
            for (auto &w : vertex.neighbors) {
                if (w > vertex.id) {
                    file << vertex.id + 1 << " " << w + 1 << "\n";
                }
            }
        }
    }

    file << "\nMIS: " << MIS.size() << "\n";
    for (vid v : MIS) {
        file << v + 1 << " ";
    }
    file << endl;

    file.close();
    return true;
}
