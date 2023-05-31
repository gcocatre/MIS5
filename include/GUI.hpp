// Project: Maximum Independent Set (MIS, version 5)
// Computer Science (2016-2017), 4th semester
// Author: Guillaume Cocatre-Zilgien
// Professor: Michel Ventou

#ifndef GUI_hpp
#define GUI_hpp

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

#include <vector>
#include <cstring>
#include "Graph.hpp"

enum textAlignment {ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT};
enum vertexType {NORMAL_VERTEX, INDEPENDENT_VERTEX, ANY_VERTEX};

typedef struct {
    int x;
    int y;
} XY;

class TrueTypeText
{
    friend class VertexBox;
    friend class GraphWindow;
protected:
    char *text;
    int size;
    SDL_Color bgColor, fgColor;
    XY position;
    textAlignment alignment;
    bool loadFont();
public:
    TrueTypeText();
    TrueTypeText(char *t, int s, SDL_Color bgc, SDL_Color fgc, XY pos, textAlignment a);
    ~TrueTypeText();
    void render(SDL_Renderer*);
};

class VertexBox
{
    friend class GraphWindow;
protected:
    int id, size;
    SDL_Color bgColor;
    XY position;
    void drawID();
    SDL_Rect getRectangle() {
        SDL_Rect r = {(position.x - (size / 2)), (position.y - (size / 2)), size, size};
        return r;
    }
public:
    VertexBox();
    VertexBox(int i, int s, SDL_Color c, XY xy);
    void render(SDL_Renderer*);
};

class GraphWindow
{
protected:
    int width, height, refreshRate;
    int vertexSize, vertexMargin, vertexTextSize;
    XY screenCenter;
    Graph *graph;
    vidSet *MIS;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Color bgColor;
    std::vector<XY> vertexCoordinates;
    std::vector<VertexBox> vertices;
    vid vertexMouseOver;
    void calculateVertexCoordinates();
    void drawVertexConnections(vid, vertexType);
    void handleEvent(SDL_Event*);
    void printText();
public:
    GraphWindow(int, int, Graph*, vidSet*);
    ~GraphWindow();
    void run();
};

#endif /* GUI_hpp */
