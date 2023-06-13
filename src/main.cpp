// Project: Maximum Independent Set (MIS, version 5)
// Computer Science (2016-2017), 4th semester
// Author: Guillaume Cocatre-Zilgien
// Professor: Michel Ventou

#include <cstdio>
#include <unistd.h>
#include <iostream>
#include "Timer.hpp"
#include "Graph.hpp"
#include "GUI.hpp"

using namespace std;

#define SHOW_N_VERTICES 60

void computeMIS(Graph &original, Graph &g, vidSet &MIS, algorithm algo)
{
    Timer t;
    switch (algo) {
        case ALGO_MDG:  printf("MDG:   ");  MIS = g.findMIS(ALGO_MDG);   break;
        case ALGO_VSA:  printf("VSA:   ");  MIS = g.findMIS(ALGO_VSA);   break;
        case ALGO_MVSA: printf("MVSA:  ");  MIS = g.findMIS(ALGO_MVSA);  break;
        case ALGO_BEST: printf("BEST:  ");  MIS = g.findMIS(ALGO_BEST);  break;

        case ALGO_EXACT:
        default:        printf("EXACT: "); MIS = g.findMIS(ALGO_EXACT); break;
    }
    float secs = t.countSeconds();
    printf("[ %3lu ]", MIS.size());

    if (secs < 1.0) {
        printf(" \t%5.1f ms\n", secs * 1000.0);
    } else {
        printf(" \t%5.1f s\n", secs);
    }

    if (!original.checkMIS(MIS)) {
        fprintf(stderr, "Error: invalid MIS!\n");
    }
}

void printHelp()
{
    printf("Usage: ./MIS -n VERTEX_COUNT  [ -p EDGE_PERCENTAGE ] [ -e ] [ -o result.txt ]\n"
           "   or: ./MIS -i graph.txt [ -e ] [ -o result.txt ]\n\n"
           "    -h          Display help\n"
           "    -e          Run the exact algorithm\n"
           "    -i FILE     Load a file containing a graph\n"
           "    -o FILE     Save the graph in a file\n"
           "    -n NUMBER   Generate a random graph with N vertices\n"
           "    -p NUMBER   Set the percentage of edges when generating a graph\n"
           "    -x PIXELS   Window width in pixels (800 by default)\n"
           "    -y PIXELS   Window height in pixels (600 by default)\n"
           "\nA few notes: only graphs with %d vertices or less, will be displayed graphically.\n"
           "Heuristics are always executed.\n"
           "In order to generate a graph with a random number of vertices, specify -n 0.\n", SHOW_N_VERTICES);
}

int main(int argc, char * argv[]) {
    if (argc == 1) {
        printHelp();
        exit(1);
    }

    Graph g;
    cardinal n {0};
    float percentage {50.0};
    char *inputFile {nullptr}, *outputFile {nullptr};
    Timer t;
    bool forceExact {false};
    vector<vidSet> sets(ALGO_ENUM_SIZE);
    int x {0}, y {0};

    // parameter handling
    int c; opterr = 0;
    while ((c = getopt(argc, argv, "hei:o:n:p:x:y:")) != -1) {
        switch (c) {
            case 'h':
                printHelp();
                exit(0);
                break;

            case 'e':
                forceExact = true;
                break;

            case 'i':
                inputFile = optarg;
                g = Graph(inputFile);
                break;

            case 'o':
                outputFile = optarg;
                break;

            case 'n':
                n = (unsigned) atoi(optarg);
                if (n > 5000) {
                    fprintf(stderr, "Error: the number of vertices must be 5000 or less.\n");
                    exit(1);
                } else if (n > 1) {
                    g = Graph(n);
                } else {
                    g = Graph(RANDOM_NUMBER_OF_VERTICES);
                }
                break;

            case 'p':
                printf("Generating a random graph… ");
                percentage = atof(optarg);
                t.reset();
                if (percentage > 0.999 && percentage < 100.001)
                    g.connectRandomly(percentage);
                else
                    g.connectRandomly(0.0);
                printf("in %.1f seconds\n", t.countSeconds());
                break;

            case 'x':
                x = atoi(optarg);
                if (x < 640 || x > 1920) {
                    fprintf(stderr, "Error: the window must be anywhere from 640 to 1920 pixels wide.\n");
                    exit(1);
                }
                break;

            case 'y':
                y = atoi(optarg);
                if (y < 480 || y > 1200) {
                    fprintf(stderr, "Error: the window must be anywhere from 480 to 1200 pixels tall.\n");
                    exit(1);
                }
                break;

            case '?':
                fprintf(stderr, "Invalid parameter: -%c\n", optopt);
                exit(1);
                break;
        }
    }

    if (optind < argc) {
        fprintf(stderr, "Invalid argument: %s\n", argv[optind]);
        exit(1);
    }

    if (g.getSize() == 0) {
        fprintf(stderr, "A problem occured while generating the graph…\n");
        exit(1);
    }

    if (g.getNumberOfEdges() == 0) {
        printf("Generating a random graph… ");
        t.reset();
        g.connectRandomly(0.0);
        printf("in %.1f seconds\n", t.countSeconds());
    }
    n = g.getSize();

    // We keep a copy of the original, unprocessed graph, in order to test the validity of the resulting MIS
    // (and potentially spot bugs in the code)
    Graph copy(g);

    printf("%lu vertices, %u edges (%.1f%% of max = %u, %.1f per vertex on average)\n\n",
        n, g.getNumberOfEdges(), g.getEdgePercentage(), g.getMaxNumberOfEdges(), g.getNumberOfEdgesPerVertex());

    // Always execute heuristics (they cost next to nothing in processing time)
    for (auto a : {ALGO_BEST, ALGO_MDG, ALGO_VSA, ALGO_MVSA}) {
        computeMIS(copy, g, sets[a], a);
    }

    // Execute the exact algorithm if the user asked for it, or if the graph is small
    if (n <= SHOW_N_VERTICES || forceExact) {
        computeMIS(copy, g, sets[ALGO_EXACT], ALGO_EXACT);
    }

    printf("\nCardinalities of Maximum Independent Sets that were found:\n");
    for (auto a : {ALGO_MDG, ALGO_VSA, ALGO_MVSA, ALGO_EXACT}) {
        if (sets[a].size() > 0) {
            switch (a) {
                case ALGO_VSA:   printf("VSA:   "); break;
                case ALGO_MDG:   printf("MDG:   "); break;
                case ALGO_MVSA:  printf("MVSA:  "); break;
                case ALGO_BEST:  printf("BEST:  "); break;
                case ALGO_EXACT: printf("EXACT: "); break;
                default: break;
            }
            for (vid v : sets[a]) {
                if (n <= 100)       printf("%2u ", v+1);
                else if (n <= 1000) printf("%3u ", v+1);
                else                printf("%4u ", v+1);
            }
            printf("\n");
        }
    }

    printf("\nNumber of recursive calls: %llu\n", g.getNumberOfCalls());

    // Sort the list of sets in order to find the largest one
    sort(sets.begin(), sets.end(), [](vidSet a, vidSet b) { return a.size() > b.size(); });
    auto MISpt = sets.begin(); // pointer to the largest MIS

    // Save the graph in a file that was specified by the user
    if (outputFile != nullptr && MISpt->size() > 0) {
        g.saveMIS(*MISpt, outputFile);
    }

    // If the graph is small enough, let's display it graphically
    if (n <= SHOW_N_VERTICES) {
        if (x == 0 && y == 0) {
            x = 800, y = 600;
        } else if (y == 0) {
            y = (x * 3 / 4);
        } else if (x == 0) {
            x = (y * 4 / 3);
        }
        GraphWindow w(x, y, &g, &(*MISpt)); // MISpt is NOT a pointer to vidSet
        w.run();
    }

    return 0;
}
