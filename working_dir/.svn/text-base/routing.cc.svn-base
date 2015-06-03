#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/lexical_cast.hpp>

#include "routing.hpp"

using namespace std;
using namespace boost;


/* build forwarding tables does what it sounds like it does.  It takes in
 * the BGL Graph + Routes and a Routing Algorithm, and returns a next-hop
 * list for every node to every other node based on the algorithm given.
*/
next_hop build_forwarding_tables(Graph G, int route, Routes *R){
  DEBUGE("Building Tables\n");
  switch(route){
    case 1:
      return ospf(G,R);
      break;
    case 2: //bellman-ford?
    case 3: //multipath?
      DEBUGE("Non Implemented Routing Algorithms");
      exit(1);
      break;
  }
  next_hop null(1);
  return null;
}


next_hop ospf(Graph G, Routes *R){
  print_graph(G);  //verify graph is correct

  graph_traits<Graph>::vertex_iterator vi, vend;
  int verts = num_vertices(G);
  next_hop H(verts, std::vector<vertex_descriptor>(verts));
  node_hop parent(verts);

  for (boost::tie(vi,vend) = vertices(G); vi != vend; ++vi){
    vertex_descriptor s = *vi;
    int node = boost::lexical_cast<int>(G[*vi].name);

  /*
  *  Run Dijkstra's, need to pass in the graph, the node to run dijkstra's
  *  from, and pass in the weight map that will get converted into a distance
  *  map.
  */
    Graph G_copy(num_vertices(G));

    //weight map and predecessor maps are define in BGL, use a unique version which
    //uses my structures and builds the next-hop data structure.
    boost::dijkstra_shortest_paths (G,s, 
    weight_map(get(&edge_info::weight,
               G)).distance_map(make_iterator_property_map((*R)[node].begin(),
               get(vertex_index, G))).
    predecessor_map(make_iterator_property_map((H)[node].begin(), 
               get(vertex_index, G), (((H)[node])[0]) )).
    visitor(make_dijkstra_visitor(copy_graph(G_copy, on_examine_edge()))));

  }

/* TURN ON TO VERIFY GRAPH

  for (int src=0; src < (int)num_vertices(G); src++){
    for (int dest=0; dest < (int)num_vertices(G); dest++){
      DEBUGE("Next hop from: %d to get to: %d is ::  %d\n", src, dest, (H)[src][dest]);
    }
  }
  DEBUGE("After Dij\n"); 
  // Verify that dijkstra is working, print distance from each node.
  //SHRT_MAX defn in limits.h = 32767
#ifdef PRINT_DEBUG
  for (int i = 0; i < (int)num_vertices(G); i++){ // if verts in graph > 32767, overflow negative
    DEBUGE("Node %d | Distance\n", i);
    vector<double> distances = (*R)[i];
    for (boost::tie(vi,vend) = vertices(G); vi != vend; ++vi){
      DEBUGE("distance(%s) = %f\n",G[*vi].name.c_str(),distances[*vi]);
    }
  }
#endif
*/

  return H;

}
