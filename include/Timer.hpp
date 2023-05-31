// Simplified tool for measuring the execution time of a piece of code
// Author: Guillaume Cocatre-Zilgien
// Computer Science (2016-2017), 4th semester

#ifndef Timer_hpp
#define Timer_hpp

#include <chrono>

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::time_point<Time> timePoint;
typedef std::chrono::duration<double> seconds;

class Timer
{
protected:
    timePoint startTime;
    seconds getSeconds()                { return (Time::now() - startTime); }
public:
    Timer()                             { startTime = Time::now(); }
    void reset()                        { startTime = Time::now(); }
    double countSeconds()               { return this->getSeconds().count(); }
    double countMilliseconds()          { return (this->getSeconds().count() * 1000.0); }
};

#endif /* Timer_hpp */
