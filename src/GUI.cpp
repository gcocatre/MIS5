// Project: Maximum Independent Set (MIS, version 5)
// Computer Science (2016-2017), 4th semester
// Author: Guillaume Cocatre-Zilgien
// Professor: Michel Ventou

#include <cmath>
#include <iostream>
#include <map>
#include <stdexcept>
#include "GUI.hpp"

using namespace std;

map<int, TTF_Font*> TrueTypeFonts;

TrueTypeText::TrueTypeText(char *t, int s, SDL_Color bgc, SDL_Color fgc, XY pos, textAlignment a)
{
    this->text = (char *) malloc(4096 * sizeof(char));
    strcpy(this->text, t);
    this->size = s;
    this->bgColor = bgc;
    this->fgColor = fgc;
    this->position = pos;
    this->alignment = a;
}

TrueTypeText::~TrueTypeText()
{
    delete this->text;
}

bool TrueTypeText::loadFont()
{
    if (TrueTypeFonts.count(size) == 0) {
        TrueTypeFonts[size] = TTF_OpenFont("DejaVuSans.ttf", size);
        if (TrueTypeFonts[size] == NULL) {
            TrueTypeFonts[size] = TTF_OpenFont("data/DejaVuSans.ttf", size);
        }
        if (TrueTypeFonts[size] == NULL) {
            TrueTypeFonts[size] = TTF_OpenFont("../data/DejaVuSans.ttf", size);
        }
        if (TrueTypeFonts[size] == NULL) {
            cerr << "Error: unable to load the required font" << endl;
            return false;
        }
    }
    return true;
}

void TrueTypeText::render(SDL_Renderer *renderer)
{
    if (!this->loadFont()) return;
    TTF_SetFontStyle(TrueTypeFonts[size], 0);
    SDL_Surface *text_surface = TTF_RenderText_Shaded(TrueTypeFonts[size], this->text, this->fgColor, this->bgColor);
    SDL_Rect box;
    box.w = text_surface->w;
    box.h = text_surface->h;
    switch (this->alignment) {
        case ALIGN_LEFT:
            box.x = this->position.x - text_surface->w;
            box.y = this->position.y - (text_surface->h / 2);
            break;

        case ALIGN_RIGHT:
            box.x = this->position.x;
            box.y = this->position.y - (text_surface->h / 2);
            break;

        case ALIGN_CENTER:
        default:
            box.x = this->position.x - (text_surface->w / 2);
            box.y = this->position.y - (text_surface->h / 2);
            break;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_RenderCopy(renderer, texture, NULL, &box);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(texture);
}

VertexBox::VertexBox(int i, int s, SDL_Color c, XY xy)
{
    this->id = i;
    this->size = s;
    this->bgColor = c;
    this->position = xy;
}

void VertexBox::render(SDL_Renderer *renderer)
{
    SDL_Rect fillRect = this->getRectangle();
    SDL_Rect border = {fillRect.x - 1, fillRect.y - 1, this->size+1, this->size+1};
    SDL_Color borderColor = {0x00, 0x00, 0x00, 0xFF};

    SDL_SetRenderDrawColor(renderer, this->bgColor.r, this->bgColor.g, this->bgColor.b, this->bgColor.a);
    SDL_RenderFillRect(renderer, &fillRect);
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(renderer, &border);

    char text[16];
    sprintf(text, "%u", this->id + 1);
    TrueTypeText t(text, (this->size * 40 / 100), this->bgColor, {0xFF,0xFF,0xFF,0xFF}, this->position, ALIGN_CENTER);
    t.render(renderer);
}

GraphWindow::GraphWindow(int w, int h, Graph *g, vidSet *s)
{
    this->width = w;
    this->height = h;
    this->graph = g;
    this->MIS = s;
    this->refreshRate = 60;
    this->bgColor = {0xDD, 0xDD, 0xDD, 0xFF};
    this->screenCenter = {(width / 2), (height / 2)};

    this->vertexSize = 30;
    this->vertexMargin = 30;
    this->vertexTextSize = 12;

    this->vertexMouseOver = NO_VERTEX_FOUND;

    for (vid v = 0; v < g->getSize(); ++v) {
        SDL_Color fillColor;
        if (this->MIS->count(v) == 1) {
            fillColor = {0xCC,0x00,0x00,0x00};
        } else {
            fillColor = {0x44,0x44,0x44,0xFF};
        }
        this->vertices.push_back(VertexBox(v, this->vertexSize, fillColor, {0,0}));
    }

    try {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw runtime_error("SDL_Init() failure");
        }
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            throw runtime_error("SDL_SetHint() failure");
        }
        if (TTF_Init() == -1) {
            throw runtime_error("TTF_Init() failure");
        }
        this->window = SDL_CreateWindow(
            "Maximum Independent Set", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN
        );
        if (this->window == NULL) {
            throw runtime_error("SDL_CreateWindow() failure");
        }
        this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
        if (this->renderer == NULL) {
            throw runtime_error("SDL_CreateRenderer() failure");
        }
        SDL_SetRenderDrawColor(this->renderer, this->bgColor.r, this->bgColor.g, this->bgColor.b, this->bgColor.a);
    } catch (runtime_error &e) {
        cerr << e.what() << endl;
    }
}

GraphWindow::~GraphWindow()
{
    for (auto &ttf : TrueTypeFonts) TTF_CloseFont(ttf.second);

    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    this->window = NULL;
    this->renderer = NULL;
    SDL_Quit();
}

void GraphWindow::calculateVertexCoordinates()
{
    XY a;
    cardinal n = this->graph->getSize();
    int radius = ((this->width < this->height) ? this->width : this->height / 2) - this->vertexMargin;
    double angle = (2 * 3.1415957) / n;
    this->vertexCoordinates.resize(n);

    for (vid v = 0; v < n; ++v) {
        a.x = (int) (this->screenCenter.x + (radius * cos(v * angle)));
        a.y = (int) (this->screenCenter.y + (radius * sin(v * angle)));
        this->vertexCoordinates[v] = a;
        this->vertices[v].position = a;
    }
}

void GraphWindow::drawVertexConnections(vid v, vertexType type)
{
    SDL_Color c {0x00,0x00,0x00,0xFF};
    XY a, b;
    a = this->vertexCoordinates[v];
    for (vid w = 0; w < this->graph->getSize(); ++w) {
        if (this->graph->areVerticesConnected(v, w)) {
            switch (type) {
                case NORMAL_VERTEX:
                    if (this->MIS->count(v) != 0 || this->MIS->count(w) != 0) continue;
                    c = {0xBB,0xBB,0xBB,0xFF};
                    break;

                case INDEPENDENT_VERTEX:
                    if (this->MIS->count(v) == 0 && this->MIS->count(w) == 0) continue;
                    c = {0x00,0x00,0x00,0xFF};
                    break;

                case ANY_VERTEX:
                    c = {0x00,0x00,0x00,0xFF};
                    break;
            }
            b = this->vertexCoordinates[w];
            SDL_SetRenderDrawColor(this->renderer, c.r, c.g, c.b, c.a);
            SDL_RenderDrawLine(this->renderer, a.x, a.y, b.x, b.y);
        }
    }
}

void GraphWindow::handleEvent(SDL_Event *e)
{
    if (e->type == SDL_MOUSEMOTION) {
        SDL_Rect r;
        XY pos;
        SDL_GetMouseState(&pos.x, &pos.y);

        bool mouseIsOverVertex {false};
        for (vid v = 0; v < this->graph->getSize(); ++v) {
            r = this->vertices[v].getRectangle();
            if (pos.x >= r.x && pos.x <= (r.x + r.w) && pos.y >= r.y && pos.y <= (r.y + r.h)) {
                // inside vertex v
                this->vertexMouseOver = v;
                mouseIsOverVertex = true;
                break;
            }
        }
        if (!mouseIsOverVertex) {
            this->vertexMouseOver = NO_VERTEX_FOUND;
        }
    }
}

void GraphWindow::printText()
{
    cardinal n = this->graph->getSize();
    char str[256];
    SDL_Color black {0x00, 0x00, 0x00, 0xFF};

    sprintf(
        str, "%lu vertices, %u edges (%.1f%%)", n, this->graph->getNumberOfEdges(), this->graph->getEdgePercentage()
    );
    TrueTypeText statsText1(str, 12, this->bgColor, black, {this->vertexMargin,this->vertexMargin}, ALIGN_RIGHT);
    sprintf(str, "Maximum Independent Set: %lu", this->MIS->size());
    TrueTypeText statsText2(str, 12, this->bgColor, black, {this->vertexMargin,this->vertexMargin+12}, ALIGN_RIGHT);
    sprintf(str, "Hover your mouse");
    TrueTypeText mouseText1(
        str, 12, this->bgColor, black, {this->width - this->vertexMargin, this->vertexMargin}, ALIGN_LEFT
    );
    sprintf(str, "over a vertex!");
    TrueTypeText mouseText2(
        str, 12, this->bgColor, black, {this->width - this->vertexMargin, this->vertexMargin+12}, ALIGN_LEFT
    );

    statsText1.render(this->renderer);
    statsText2.render(this->renderer);
    mouseText1.render(this->renderer);
    mouseText2.render(this->renderer);
}

void GraphWindow::run()
{
    bool quit = false;
    SDL_Event e;
    cardinal n = this->graph->getSize();
    vid previousMouseOver = NO_VERTEX_FOUND;

    this->calculateVertexCoordinates();

    while (!quit) {
        SDL_Delay(1000 / this->refreshRate);
        while (SDL_PollEvent(&e) != 0) {
            quit = (e.type == SDL_QUIT);
            this->handleEvent(&e);

            if (previousMouseOver == NO_VERTEX_FOUND || previousMouseOver != this->vertexMouseOver) {
                previousMouseOver = this->vertexMouseOver;
                SDL_SetRenderDrawColor(
                    this->renderer, this->bgColor.r, this->bgColor.g, this->bgColor.b, this->bgColor.a
                );
                SDL_RenderClear(this->renderer);

                this->printText();

                // We loop twice so that black lines get drawn over gray lines
                if (this->vertexMouseOver == NO_VERTEX_FOUND) {
                    for (vid v = 0; v < n; v++) {
                        if (this->MIS->count(v) == 0) {
                            this->drawVertexConnections(v, NORMAL_VERTEX);
                        }
                    }
                    for (vid v = 0; v < n; v++) {
                        if (this->MIS->count(v) == 1) {
                            this->drawVertexConnections(v, INDEPENDENT_VERTEX);
                        }
                    }
                } else {
                    for (vid v = 0; v < n; v++) {
                        if (this->vertexMouseOver == v) {
                            this->drawVertexConnections(v, ANY_VERTEX);
                        }
                    }
                }

                for (vid v = 0; v < n; v++) {
                    this->vertices[v].render(this->renderer);
                }

                SDL_RenderPresent(this->renderer);
            }
        }
    }
}
