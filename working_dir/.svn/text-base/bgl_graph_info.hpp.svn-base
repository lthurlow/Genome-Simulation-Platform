#ifndef _BGL_GRAPH_INFO_HPP_
#define _BGL_GRAPH_INFO_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream> //for ostream
#include <boost/graph/graphviz.hpp> //for boost related

/* Vert_Info is a data structure that is used to maintain information on 
 * vertices in the graph for BGL
 */
typedef struct vert_info{
  std::string name; //vertex label
}vert_info;

/* Same as vert_info, keeps track of data for edges in BGL */

typedef struct edge_info{
  long capacity; // capacity of the link
  int  weight;	 // weight for routing algorithms
}edge_info;

/* Required to sort the map of edge_info structures ='( */
inline bool operator<(const edge_info &lhs, const edge_info &rhs){
    return lhs.weight < rhs.weight;
}


typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, vert_info, edge_info> Graph;
typedef boost::graph_traits<Graph>::edge_descriptor edge_desc;
typedef boost::graph_traits<Graph>::edge_iterator edge_iter;
typedef boost::graph_traits <Graph>::vertex_descriptor vertex_descriptor;
typedef boost::property_map < Graph, boost::vertex_name_t >::type NameMap;
//unsigned integer type for num verts in graph
typedef boost::graph_traits<Graph>::vertices_size_type size_type;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_iterator;

/*  These two templates are required for printing out graph to dot format */
template <class WeightMap,class CapacityMap> class edge_writer {
  public:
    edge_writer(WeightMap w, CapacityMap c) : wm(w),cm(c) {}
    template <class Edge>
    void operator()(std::ostream &out, const Edge& e) const {
      out << "[label=\" W = " << wm[e] << "\\n C = " << cm[e] << "\"]";
  }
  private:
    WeightMap wm;
    CapacityMap cm;
};

template <class WeightMap, class CapacityMap>
inline edge_writer<WeightMap,CapacityMap> 
make_edge_writer(WeightMap w,CapacityMap c) {
  return edge_writer<WeightMap,CapacityMap>(w,c);
}

#endif
