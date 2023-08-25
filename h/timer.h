#ifndef TIMER_AK_H
#define TIMER_AK_H


////////////////////////////////////////////////////////////////////////////////

/*** 
    All the functionality of the std::chrono library for measuring code
    execution time is encapsulated in the Timer class. 
                                                      ***/


////////////////////////////////////////////////////////////////////////////////

#include <chrono>


////////////////////////////////////////////////////////////////////////////////

class Timer {
private:
   using clock_t = std::chrono::steady_clock; // Type alias.
   using second_t = std::chrono::duration<double, std::ratio<1>>; // Type alias.
   std::chrono::time_point<clock_t> start;
public:
   Timer() : start(clock_t::now()) {}
   void Reset() { start = clock_t::now(); }
   double Elapsed() const {
      return std::chrono::duration_cast<second_t>(clock_t::now()-start).count();
   }
};


////////////////////////////////////////////////////////////////////////////////

#endif
