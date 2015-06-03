#include <stdio.h>
#include "boost/tuple/tuple.hpp"
#include "types.hpp"
#include "bgl_graph_info.hpp"
#include "routing.hpp"

using namespace std;

/* Print help menu */
void print_help(){
  fprintf(stderr,"Cloud Data Distribution Help Menu:\n");
  fprintf(stderr,"main [b:c:C:d:D:hHl:p:P:r:s:v:w:W:]\n");
  fprintf(stderr,"\t[b] | {175000} | Window size (in KB)\n");
  fprintf(stderr,"\t[c] | {10,10} | tail circuit capacity, [C] core circuit capacity (Gb/s)\n");
  fprintf(stderr,"\t[d] | {5} | Degree of each node in graph\n");
  fprintf(stderr,"\t[D] | {1} | Data Distribution Policy\n");
  fprintf(stderr,"\t\t[1] Single Server Policy\n");
  fprintf(stderr,"\t\t[2] Single Server Policy with Caching\n");
  fprintf(stderr,"\t[h]elp, for this help menu\n");
  fprintf(stderr,"\t[H]elp, prints out all simulation variable values\n");
  fprintf(stderr,"\t[l] | {1.0} | Length of simulation (in days)\n");
  fprintf(stderr,"\t[m] | 250 | Per genome max total pulls\n");
  fprintf(stderr,"\t[p] | 556.0 | Push rate - total pushes/day\n");
  fprintf(stderr,"\t[P] | 8.33 | Pull rate - per genome pulls/day\n");
  fprintf(stderr,"\t[r] | {1} | Routing Algorithm to Use\n");
  fprintf(stderr,"\t\t[1] OSPF\n");
  fprintf(stderr,"\t[s] | {100} | Seed, pass the seed value\n");
  fprintf(stderr,"\t[v] | {10}  | Vertices in the graph to be generated\n");
  fprintf(stderr,"\t[w] | {5,25} | Minimum weight, [W] maximum weight - Uniform\n");
  fprintf(stderr,"\t\t Link weights in milliseconds\n");
  return;
}

/*prints simulation variable values*/
void print_input(UI in,Policy P){
  char* pol;
  char* route;
  switch(P->get_policy()){
    case 1:
      pol = (char*)"Single Server - No Caching";
      break;
    case 2:
      pol = (char*)"Single Server - Caching Enabled";
      break;
  }
  switch(in->get_route()){
    case 1:
      route = (char*)"OSPF";
      break;
  }
  printf("Simulation Modifiers\n");
  printf("\tSEED INPUT: %d\n",in->get_seed());
  printf("\tWindow Size: %u\n",in->get_winsize());
  printf("\t# of PULLS per object: %ld\n",in->get_pulls()/in->get_objects());
  printf("\t# of OBJECTS: %ld\n",in->get_objects());
  printf("\tObject Push Rate: %4f\n", in->get_pushrate());
  printf("\tObject Pull Rate: %4f\n", in->get_pullrate());
  printf("\tObject Size (MB): %0.0f\n", in->get_size());
  printf("\tPolicy: %d (%s)\n",P->get_policy(),pol);
  printf("Graph Modifiers\n");
  printf("\t# of VERTICES: %d\n",in->get_vertices());
  printf("\tDegree: %f\n", (in->get_edgechance()*(in->get_vertices()-1)));
  printf("\tWeight: [%4f,%4f]\n",in->get_minw(),in->get_maxw());
  printf("\tCapacity (Gb/s): [%ld,%ld]\n",in->get_mincap()/125000000,
					  in->get_maxcap()/125000000);
  printf("\tTotal pushes per day: %f\n", in->get_pushPerDay());
  printf("\tPer genome pulls per day: %f\n", in->get_pullPerDay());
  printf("\tPer genome max total pulls: %d\n", in->get_maxpulls());
  printf("\tLength of simulation (days): %f\n", in->get_days());
  printf("\tRouting Algorithm: %s\n",route);
}

/* print out the graph*/
void print_graph(Graph G){
  std::pair<edge_iter, edge_iter> ep;
  edge_iter ei, ei_end;
  /* FIX TO PRINT EDGE AND VERIFY GRAPH. */ 
  for (tie(ei, ei_end) = boost::edges(G); ei != ei_end; ++ei){
    edge_info prop = G[*ei];//.edge_info.weight;
    DEBUGE("Edge: %s - %s :: Weight: %d , Capacity: %ld\n", 
          G[boost::source(*ei, G)].name.c_str(), G[target(*ei, G)].name.c_str(), 
          prop.weight,prop.capacity);
    }
}
