#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <gsl/gsl_rng.h>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/lexical_cast.hpp>

#include "types.hpp"
#include "randvars2.hpp"
#include "bgl_graph_info.hpp"

using namespace std;
using namespace boost;


static const size_t npos = -1; //defined in string::npos, for find() not found

/*
 * Generate the graph to run simulation on.
 * Make a preference file that itm can then use to generate the graph
 * Prefernece file has special values, see comments below.
 *
*/

void gen_graph(user_input *i,
	       unsigned	   seed){

  int syscheck = 0; //return value for system calls

  /* Makes gt-itm not crash.*/
  fprintf(stderr, "Exporting MALLOC_CHECK_=0\n");
  syscheck = system("export MALLOC_CHECK_=0"); //makes itm not crash
  if (syscheck != 0){
    fprintf(stderr, "Export Command Failed!");
    exit(1);
  }

  /* create a file to use for prefrences for gt-itm*/
  ofstream pref;
  pref.open("pref");

  /* 
   * geo: flat random graph 
   * hier: N-level hierarchial graph
   * ts: transit-stub graph
   * 
   * Fromat:
   * <method> [above] <# of graphs> <seed> [optional]
  */

  pref << "geo 1 " << seed << endl; // if needed can change this so it isnt flat

  /*
   * Graph Parameters:
   * n: number of nodes in graph
   * scale: one-sided dimension of space in which nodes are distributed
   * edgemethod:
   *   1) Waxman 1
   *   2) Waxman 2
   *   3) Pure Random 
   *   4) Door-Leslie
   *   5) Exponential 
   *   6) Locality
   * alpha: random graph parameter (0 =< alpha =< 1.0)
   * beta: " " (0 <= beta)
   * gamma: " " (0 <= gamma)
   * geo: <n> <scale> <edgemethod> <alpha> [<beta><gamma>]
  */
  char preferences[255];
  DEBUGE("verts: %d : edge: %f\n", i->get_vertices(), i->get_edgechance());
  sprintf(preferences, "%d %d %d %f\n", 
  i->get_vertices(), 10, 3, i->get_edgechance());
  
  pref << preferences;
  pref.close();

  /* Run itm on pref file created above. */
  fprintf(stderr,"Calling ../bin/itm pref\n");  
  syscheck = system("../bin/itm pref");  
  if (syscheck != 0){
    fprintf(stderr, "itm not found or failed to run!\n");
    exit(1);
  }

  /*convert gb file to readable alternate format*/
  fprintf(stderr,"Calling ../bin/sgb2alt pref-0.gb pref.alt\n");
  syscheck = system("../bin/sgb2alt pref-0.gb pref.alt");
  if (syscheck == 1){
    fprintf(stderr, "sgb2alt failed to run, not found, incompatable file!\n");
    exit(1);
  }

}

/*
 * Convert the graph from the alternative format to a dot format
 * that can easily be loaded into BGL as a graph.
 * This function will have some hard coding variables that may need to be
 * changed at a later date.
*/

Graph con_graph(UI in){
  ifstream alt;
  alt.open("pref.alt");
  if (!alt){
    fprintf(stderr, "Error: pref.alt does not Exist\n");
  }

  gsl_rng_env_setup(); //set up our random
  r = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(r, in->get_seed()); //use seed

  string line, v1, v2, weight, whitespace;
  int V1, V2;
  int num_verts = 1; //max of V1, V2.
  vector <pair< pair <int,int>, pair <int, long> > > edges;
  while(getline(alt,line) != NULL){
    if (line.find("EDGES") != npos){
      while(getline(alt,line) != NULL){
        istringstream stream(line);
        stream >> v1;
        stream >> v2;
        stream >> weight; //throw away itm weight, generate our own.
        V1 = atoi(v1.c_str());
        V2 = atoi(v2.c_str());
        /*
         * This code will determine the number of vertices that are used
         * in the graph, it will do this by checking if either number found
         * is larger than a previous one, this will find the number of
         * vertices under the assumption that gt-itm will generate fully
         * connected graphs, and therefore, the largest value will be the
         * the highest value seen.  It will also be offset by 1, because
         * gt-itm starts nodes at 0. This is compensated later in the code.
         */
        if (V1 > num_verts || V2 > num_verts){
          if (V1 > V2){
            num_verts = V1;
          } else {
            num_verts = V2;
          }
        }
        /* generate weights and capacities for this link*/
        long weight;
        long minw = in->get_minw();
        long maxw = in->get_maxw();
        long maxc = in->get_maxcap(); // Use maxcap() for core links.
        if (minw == maxw){
          weight = minw; //if they are set to same value, all links have
        } else {
          weight = UniformD(minw,maxw);  //otherwise, distributed
        }
        /* make them into pairs, then through int a vector.*/
        pair<int,int> edge (V1,V2);
        pair <int, long> w_c (weight,maxc);
        pair < pair <int, int> , pair <int, long> > edge_prop (edge,w_c);
        edges.push_back(edge_prop);
      }
    }
  }

  num_verts += 1;  //above code doesnt take into account gt-itm starts at 0
  Graph g(num_verts); //generate graph with num_verts nodes.
  //convert from alternative format to dot format for graphviz.
  for (int itor = 0; itor < num_verts; ++itor){
    //set the name for each node to its index
    g[itor].name = lexical_cast<string>(itor);
  }
  
  //add the edges to the graph now.
  for(vector< pair< pair < int, int>, pair <int, long> > >::iterator 
    it = edges.begin(); it != edges.end(); ++it) {
    //let ele = be ((u,v),(w,c)) for both edge and properties
    pair< pair < int, int>, pair <int, long> > ele = (*it);
    edge_info prop; //set the properties to add to the graph with the edge.
    prop.weight = ele.second.first;
    prop.capacity = ele.second.second;
    pair<edge_desc, bool> res; // make sure that it added the edge.
    res = add_edge(ele.first.first,ele.first.second, prop, g);
    if (res.second == false){
      DEBUGE("Failed to add edge to graph!\n");
      DEBUGE("Edge in question: %d-%ld\n",
              ele.first.first,ele.second.second);
      exit(3);
    }
  }

  /* now open up the dot file and write the graph to it */
  ofstream dot("graph.dot");
  write_graphviz(dot, g, 
    boost::make_label_writer(boost::get(&vert_info::name, g)),
    make_edge_writer(boost::get(&edge_info::weight,g), 
    boost::get(&edge_info::capacity,g)));

  DEBUGE("Converting dot file to pdf format -- RUN WAS A:  ");
  errno = system("dot -Tpdf graph.dot -o graph.pdf &> /dev/null");
  if (errno == 0){
    DEBUGE("SUCCESS!\n");
  } else {
    DEBUGE("FAILURE!\n");
  }

  return g;
}
