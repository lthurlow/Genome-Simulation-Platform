#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/lexical_cast.hpp>

#include "bgl_graph_info.hpp"
#include "randvars2.hpp"
#include "routing.hpp"

/*comment out DEBUG define line to remove debug code!*/
#define PRINT_DEBUG 1  //comment out this line to remove debug code

#ifdef PRINT_DEBUG //defines debug
#define DEBUGO(...)  fprintf(stdout, __VA_ARGS__)
#define DEBUGE(...)  fprintf(stderr, __FILE__ ": " __VA_ARGS__)
#else
#define DEBUGO(...)
#define DEBUGE(...)
#endif

#define MAX_POLICIES 2

using std::pair;

/*
 * Policy implementations:
 * 0 : Undefined - ERROR
-> * 1 : Single Server
-> * 2 : Single Server with Caching
*************************************
 * X : Multiple Servers
 * X : Cache (copy full) as you go
 * X : Cache (copy segment) as you go
 * X : Cache (copy varying amounts) as you go
 * X : Priority Pushing (Routing off link capacity)
 * X : Pull while Pushing (forced to be <= push bottleneck rate)
*/
class policy{
  private:
    int type;
    int amount; //not fully thought out, #5
  public:
    int  get_policy(){ return type; }
    void set_policy(int t){ type = t; }
};

typedef policy* Policy;

/*
 * Class for passing around user's input to other functions
*/
class user_input{
  private:
    int	     vertices;   // # of nodes in the graph
    unsigned seed;       // Seed for PRNG
    unsigned win_size;	 // TCP window size
    int	     route;      // routing algorithm to use
    long     num_pulls;  // MAX set of pulls allowed in simulation
    long     num_objs;   // MAX set of objects allowed in simulation
    long     min_cap;    // Minimum link capacity
    long     max_cap;    // Maximum link capacity
    double   obj_size;   // Size of the object
    float    min_weight; //Minimum link weight (*.0002 for latency)
    float    max_weight; //Maximum link weight
    float    chance_of_edge; // Degree of graph
    double   push_rate;  // EXP() for arrival rate for pushes
    double   pull_rate;  // EXP() for arrival rate for pulls
    float    pushPerDay;
    float    pullPerDay;
    int	     maxPulls;	 // Per genome maximum total pulls
    float    days;
  public:
    int    get_vertices(){ return vertices; }
    unsigned get_winsize(){ return win_size; }
    unsigned get_seed(){ return seed; }
    int    get_route(){ return route; }
    long   get_pulls(){ return num_pulls; }
    long   get_objects(){ return num_objs; }
    long   get_mincap(){ return min_cap; }
    long   get_maxcap(){ return max_cap; }
    double get_size(){ return obj_size; }
    float  get_minw(){ return min_weight; }
    float  get_maxw(){ return max_weight; }
    float  get_edgechance(){ return chance_of_edge; }
    double get_pushrate(){ return push_rate; }
    double get_pullrate(){ return pull_rate; }
    float  get_pushPerDay(){ return pushPerDay; }
    float  get_pullPerDay(){ return pullPerDay; }
    float  get_days(){ return days; }
    int	   get_maxpulls(){ return maxPulls; }

    void   set_vertices(int v){ vertices = v; }
    void   set_winsize (unsigned b){ win_size = b; }
    void   set_seed (unsigned s){ seed = s; }
    void   set_route(int r){ route = r; }
    void   set_pulls(long p){ num_pulls = p; }
    void   set_objects(long o){ num_objs = o; }
    void   set_mincap(long mc){ min_cap = mc; }
    void   set_maxcap(long mc){ max_cap = mc; }
    void   set_size (double s){ obj_size = s; }
    void   set_minw(float mw){ min_weight = mw; }
    void   set_maxw(float mw){ max_weight = mw; }
    void   set_edgechance(float coe){ chance_of_edge = coe; }
    void   set_pushrate(double pr){ push_rate = pr; }
    void   set_pullrate(double pr){ pull_rate = pr; }
    void   set_pushPerDay(float ppd){ pushPerDay = ppd; }
    void   set_pullPerDay(float ppd){ pullPerDay = ppd; }
    void   set_days(float d){ days = d; }
    void   set_maxpulls(int mp){ printf("XXX %d\n", mp); maxPulls = mp; }
};

typedef user_input* UI;

//file_format.cc functions
void gen_graph(UI in, unsigned s);
Graph con_graph(UI in);

//util functions
void print_help(void);
void print_input(UI in, Policy P);
void print_graph(Graph G);

/* Special Exit Output code */
inline void exit_fail(char* file, int line, int status, char* err){
  if (status != 0){
    fprintf(stderr,"Recieved ERROR status: %d from %s at %d\n", status, file, line);
    fprintf(stderr,"%s\n", err);
  }
  exit(status);
}

/* Special Exit Output code */
inline void exit_fail(char* file, int line, int status){
  if (status != 0){
    fprintf(stderr,"Exit Status: %d from %s at %d\n", status, file, line);
  }
  exit(status);
}


#endif
