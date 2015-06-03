#ifndef __SIMGRID_HPP__
#define __SIMGRID_HPP__

#include "routing.hpp"
#include "types.hpp"

#include "msg/msg.h" // for xbt_dynar declaration

class proc_info{
  private:
    int recv; //host receiving data ALSO DESTINATION
    int send; //host sending data
    double size; // optional to change this from default 300GB
    long time; //keeps track of time process runs for
    uint8_t pop; //push or pull
    UI input; // User input
    Routes *route; //route -> for cached version
    next_hop* nh; // Next Hop graph information
    long label; //label for push or pull value
  public:
    proc_info(int r, double sz, double t,uint8_t p){
      recv = r;
      size = sz;
      time = t;
      pop = p;
    }
    void set_UI(UI u){ input = u; }
    void set_time(long t){ time = t; }
    void set_recv(long d){ recv = d; }
    void set_send(long s){ send = s;}
    void set_route(Routes *R){ route = R;}
    void set_nh(next_hop *h){ nh = h; }
    void set_label(long l){ label = l; }

    UI get_UI(void){ return input; }
    double get_size(){ return size; }
    long get_time(){ return time; }
    int get_recv(){ return recv; }
    int get_send(){ return send; }
    uint8_t get_pop(){ return pop; }
    Routes* get_route(){ return route; }
    next_hop* get_nh(){ return nh; }
    long get_label(){ return label; }
};

class task_info{
  private:
    long label; // Genome ID for task
    int pop; // Is task pop=1 or push=0
    double start; //Starting time of the process
    long task_id; //Unique task ID
    UI input; // User input
    int dest; // Destination Node
    int src; // Source Node
    Routes *route;
    next_hop* nh; // Next Hop graph information

  public:
    long get_label(){ return label; }
    int get_pop(){ return pop; }
    double get_start(){ return start;}
    long get_id(){ return task_id; }
    int get_dest(){ return dest; }
    int get_src(){ return src; }
    Routes* get_route(){ return route; }
    next_hop* get_nh(){ return nh; }

    void set_label(long l){ label = l; }
    void set_pop(int p){ pop = p; }
    void set_start(double t){ start = t; }
    void set_dest(long d){ dest = d; }
    void set_src(long s){ src = s; }
    void set_route(Routes *R){ route = R;}
    void set_nh(next_hop *h){ nh = h; }

    void set_UI(UI u){ input = u; }
    UI get_UI(void){ return input; }

    task_info(long l, int p, int t){
      label = l;
      pop = p;
      task_id = t;
    }

    task_info(long l, int p){
      label = l;
      pop = p;
    }

    task_info(){
      label = 0;
      pop = 0;
    }
};


int recv(int argc, char *argv[]);
int send(int argc, char *argv[]);
void single_server(UI input);

int ssc_recv(int argc, char *argv[]);
int ssc_send(int argc, char *argv[]);
void single_server_cache(UI input, Routes *R, next_hop *H);

void sim(UI input, Policy P, Routes *R, next_hop *H);

void write_platform(Graph *G, next_hop *H, UI in);
void write_deploy(Graph *G, next_hop *H);

#endif
