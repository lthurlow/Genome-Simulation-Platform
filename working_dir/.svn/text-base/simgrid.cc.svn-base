#include<stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <stack>

#include <cassert> /*BRS*/

#include "bgl_graph_info.hpp"
#include "routing.hpp"
#include "randvars2.hpp"
#include "simgrid.hpp"

#include "msg/msg.h"
#include "surf/simgrid_dtd.h"
#include "xbt/log.h" //for a log channel
#include "xbt/asserts.h"

using namespace std;
using namespace boost;

/* task comp size is an MSG value, we dont use mainly for fact that cache to
 * local server should be neglabable in the scope of flow durations */
unsigned int task_comp_size = 0;
/* task comm size is the data object size, in our case objects*/
double task_comm_size = 300000000000.0;

int nb_hosts; /* All declared hosts */
std::vector<int> hostMap; // Map from sim to xbt host numbers
int MaxRouter(int numHosts)  {return((numHosts/2)-1);}
int MinSite(int numHosts)    {return((numHosts/2));}

gsl_rng *r;

typedef std::map <edge_desc, int> link_map; //mapping of edges to link values
std::map<long,long> gene_pulls; // # of times genome gets pulled
std::map<long,vector<long> > genome; //used to store where a genome is located
std::vector<std::vector<std::priority_queue<long> > > location; // used to pick the closest.

/* Function to write the top of the xml file */
void write_header(ofstream *f, int p){
  *f << "<?xml version='1.0'?>\n";
  *f << "\t<!DOCTYPE platform SYSTEM \"http://simgrid.gforge.inria.fr/simgrid.dtd\">\n";
  *f << "\t<!-- This File is needed for SimGrid, used by this Simulation-->\n";
  *f << "\t<!-- For modification of this file - review help menu-->\n";
  *f << "\t<platform version=\"4\">\n";
  if (p == 1)
    *f << "\t<AS  id=\"AS0\"  routing=\"Full\">\n\n";
}

/* Function to write the bottom of the xml file */
void write_closer(ofstream *f, int p){
  if (p == 1)
    *f << "\t</AS>\n";
  *f << "\t</platform>\n";
}

/* Writes the host section of the platform xml file */
void write_hosts(ofstream *f, int hosts){
  //ROUTERS
  for (int i=0; i< hosts; i++){
    // Host Power is meaningless unless caching the data takes time
    *f << "\t\t<host id=\"r" << i << "\" power=\"1000000\"/>\n";
  }

  *f << "\n";

  //SITES
  for (int i=0; i< hosts; i++){
    // Host Power is meaningless unless caching the data takes time
    *f << "\t\t<host id=\"s" << i << "\" power=\"1000000\"/>\n";
  }
  *f << "\n";
}

/* Writes out all the links with converted weight to latency */
void write_links(ofstream *f, Graph *G, link_map* M, std::pair<unsigned long,unsigned long> p, UI in){
  int i = 0;
  edge_iter ei,ei_end;
  if (p.first < 0 || p.second < 0) exit(3); //FIXME when i ran w/o I was getting negative b/w values?
  for (boost::tie(ei, ei_end) = boost::edges(*G); ei != ei_end; ++ei){
    *f << "\t\t<link id=\"l" <<  i << "\" bandwidth=\"" << p.second; //10000";
    edge_info prop = (*G)[*ei];
    double lat = prop.weight * .001; // convert the weight link to a latency
    *f << "\" latency=\"" << lat <<"\" />\n";
    M->insert( std::make_pair(*ei,i) );
    i++;
  }
  *f << "\n";
  //Add links to connect Site Notes to Router nodes
  for (int i = 0; i < (int)num_vertices(*G); i++){ 
    *f << "\t\t<link id=\"c" <<  i << "\" bandwidth=\"" << p.first;
    *f << "\" latency=\"" << ".003" <<"\" />\n";
  }
  *f << "\n";
}

/* Write all routes to platform xml file, using Boost's next hop generated struct*/
void write_routes(ofstream *f, Graph *G, next_hop *H, link_map* M){
  //routing protocol dependent

  for (int i = 0; i < (int)num_vertices(*G); i++){ 
    *f << "\t\t<route src=\"r" << i;
    *f << "\" dst=\"s" << i << "\">\n";
    *f << "\t\t\t<link_ctn id=\"c" << i << "\"/>\n";
    *f << "\t\t</route>\n";
  }
  *f << "\n";

  std::set<pair<int, int> > edge_set; //make sure no dup edges in file
  std::pair<set< pair< int, int > >::iterator, bool > ret;
  int x,y;
  /* iterate through all src-dest edges */
  for (int i = 0; i < (int)num_vertices(*G); i++){ 
    for (int j = 0; j < (int)num_vertices(*G); j++){
      if (i > j){
        x = j;
        y = i;
      } else {
        x = i;
        y = j;
      }
      /*This edge set removes SURF PARSE multiple defn. of routes*/
      ret = edge_set.insert(std::make_pair(x,y));
      if (i != j && ret.second){ //if they are the same vert, dont add to route
        std::vector<int> links;
        std::stack<edge_desc> edge_stack; //use a stack to print links correctly

        /* xml formating for routes */
        *f << "\t\t<route src=\"r" << i;
        *f << "\" dst=\"r" << j << "\">\n";
        *f << "\t\t\t";
        
        int src = i;
        int dest = j;
        /* find all links used to go from src -> dest  */
        while ((int)(*H)[src][dest] != dest){
          edge_desc edge = boost::edge((*H)[src][dest],dest,*G).first;
          edge_stack.push(edge);
          dest = (int)(*H)[src][dest];
        }
        /* print all links used in the correct ordering */
        while (!edge_stack.empty()){
          edge_desc e = edge_stack.top();
	  map<edge_desc,int>::iterator link = M->find(e);
	  assert(link != M->end());
          *f << "<link_ctn id=\"l" << link->second << "\"/>";
          edge_stack.pop();
        }
        *f << "\n";
        *f << "\t\t</route>\n";
      }
    }
  }

  *f << "\n";

  std::set<pair<int, int> > edge_set4; //make sure no dup edges in file
  std::pair<set< pair< int, int > >::iterator, bool > ret4;

  for (int i = 0; i < (int)num_vertices(*G); i++){ 
    for (int j = 0; j < (int)num_vertices(*G); j++){
      if (i > j){
        x = j;
        y = i;
      } else {
        x = i;
        y = j;
      }
      /*This edge set removes SURF PARSE multiple defn. of routes*/
      ret4 = edge_set4.insert(std::make_pair(x,y));
      if (i != j && ret4.second){ //if they are the same vert, dont add to route
        std::vector<int> links;
        std::stack<edge_desc> edge_stack; //use a stack to print links correctly

        /* xml formating for routes */
        *f << "\t\t<route src=\"r" << i;
        *f << "\" dst=\"s" << j << "\">\n";
        *f << "\t\t\t";
        
        int src = i;
        int dest = j;
        /* find all links used to go from src -> dest  */
        while ((int)(*H)[src][dest] != dest){
          edge_desc edge = boost::edge((*H)[src][dest],dest,*G).first;
          edge_stack.push(edge);
          dest = (int)(*H)[src][dest];
        }
        /* print all links used in the correct ordering */
        while (!edge_stack.empty()){
          edge_desc e = edge_stack.top();
          map<edge_desc,int>::iterator link = M->find(e);
	  assert(link != M->end());
          *f << "<link_ctn id=\"l" << link->second << "\"/>";
          edge_stack.pop();
        }
        *f << "<link_ctn id=\"c" << j << "\"/>";
        *f << "\n";
        *f << "\t\t</route>\n";
      }
    }
  }


  *f << "\n";

  std::set<pair<int, int> > edge_set3; //make sure no dup edges in file
  std::pair<set< pair< int, int > >::iterator, bool > ret3;

  for (int i = 0; i < (int)num_vertices(*G); i++){ 
    for (int j = 0; j < (int)num_vertices(*G); j++){
      if (i > j){
        x = j;
        y = i;
      } else {
        x = i;
        y = j;
      }
      /*This edge set removes SURF PARSE multiple defn. of routes*/
      ret3 = edge_set3.insert(std::make_pair(x,y));
      if (i != j && ret3.second){ //if they are the same vert, dont add to route
        std::vector<int> links;
        std::stack<edge_desc> edge_stack; //use a stack to print links correctly

        /* xml formating for routes */
        *f << "\t\t<route src=\"s" << i;
        *f << "\" dst=\"r" << j << "\">\n";
        *f << "\t\t\t";
        
        int src = i;
        int dest = j;
        /* find all links used to go from src -> dest  */
        while ((int)(*H)[src][dest] != dest){
          edge_desc edge = boost::edge((*H)[src][dest],dest,*G).first;
          edge_stack.push(edge);
          dest = (int)(*H)[src][dest];
        }
        /* print all links used in the correct ordering */
        *f << "<link_ctn id=\"c" << i << "\"/>";
        while (!edge_stack.empty()){
          edge_desc e = edge_stack.top();
          map<edge_desc,int>::iterator link = M->find(e);
	  assert(link != M->end());
          *f << "<link_ctn id=\"l" << link->second << "\"/>";
          edge_stack.pop();
        }
        *f << "\n";
        *f << "\t\t</route>\n";
      }
    }
  }

  *f << "\n";

  std::set<pair<int, int> > edge_set2; //make sure no dup edges in file
  std::pair<set< pair< int, int > >::iterator, bool > ret2;

  for (int i = 0; i < (int)num_vertices(*G); i++){ 
    for (int j = 0; j < (int)num_vertices(*G); j++){
      if (i > j){
        x = j;
        y = i;
      } else {
        x = i;
        y = j;
      }
      /*This edge set removes SURF PARSE multiple defn. of routes*/
      ret2 = edge_set2.insert(std::make_pair(x,y));
      if (i != j && ret2.second){ //if they are the same vert, dont add to route
        std::vector<int> links;
        std::stack<edge_desc> edge_stack; //use a stack to print links correctly

        /* xml formating for routes */
        *f << "\t\t<route src=\"s" << i;
        *f << "\" dst=\"s" << j << "\">\n";
        *f << "\t\t\t";
        
        int src = i;
        int dest = j;
        /* find all links used to go from src -> dest  */
        while ((int)(*H)[src][dest] != dest){
          edge_desc edge = boost::edge((*H)[src][dest],dest,*G).first;
          edge_stack.push(edge);
          dest = (int)(*H)[src][dest];
        }
        /* print all links used in the correct ordering */
        *f << "<link_ctn id=\"c" << i << "\"/>";
        while (!edge_stack.empty()){
          edge_desc e = edge_stack.top();
          map<edge_desc,int>::iterator link = M->find(e);
	  assert(link != M->end());
          *f << "<link_ctn id=\"l" << link->second << "\"/>";
          edge_stack.pop();
        }
        *f << "<link_ctn id=\"c" << j << "\"/>";
        *f << "\n";
        *f << "\t\t</route>\n";
      }
    }
  }

}

/* Need to write the platform file for SimGrid simulation
   Graph G will be used to iterate over each vertex and create a host for each
   next_hop H will be used to create the links between each host
*/

void write_platform(Graph *G, next_hop *Hop, UI in){
  //link map keeps edge (src,dest) -> link # information
  link_map* link_name = new link_map();
  
  ofstream plat;
  // Name the output file platform.xml
  plat.open("platform.xml");
  //get the number of hosts to create from the # of verts in graph
  int hosts = boost::num_vertices(*G);

  //Write all of the xml header information
  write_header(&plat,1);

  // each host will have a power and name, name will be index
  // <host id="h0" power="100000000"/>
  write_hosts(&plat, hosts);

  std::pair<long, long> link_pair(in->get_mincap(), in->get_maxcap());

  // links will have b/w and some latency proportional to their weight.
  // <link id="l0" bandwidth="X" latency="X" />
  // Higher Weight --> Greater Latency -> 0-100 --> 0ms -> 20ms --> divide by 5
  // convert to float * 10e-3 W*.0002
  write_links(&plat, G, link_name, link_pair, in);

  //routes - transverse the next_hop for each host, and iterate each link
  // <route src="h0" dst="h1">
  // \t<link_ctn id ="l0"/><link_ctn id="l1"/>
  //</route>
  write_routes(&plat, G, Hop, link_name);
  
  //End the xml format with </AS> \n </platform>
  write_closer(&plat,1);
}

/* Copyright (c) 2008, 2009, 2010. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

XBT_LOG_NEW_DEFAULT_CATEGORY(CLOUD,"Cloud based Simulation in SimGrid");

long label = 0; // used to increment the label on pushes
double global_time = 0.0; //used for the pushing requests at given time
double start_time = 0.0; //start time to begin pulling
long sim_pushes = 0; // how many pushes have been done in simulation
long sim_pulls = 0; // how many pulls have been done
float avg_trans = 0.0; // the average time per transfer
long debug_id = 0; //used for finding matching push/pull requests - DEBUG

int recv(int argc, char *argv[]){
  /* get name of process -> returns current process -> convert to int */
  int host_number = atoi(MSG_process_get_name(MSG_process_self()));

  /* Mailbox is the talking point between agents*/
  char mailbox[256];

  /* set task to send to NULL (otherwise a previous task may partially reside*/
  msg_task_t task = NULL;
  
  /* Some unused variable MACRO in misc.h */
  _XBT_GNUC_UNUSED int res;

  task = NULL;
  sprintf(mailbox,"%d", host_number);
  res = MSG_task_receive(&(task), MSG_process_get_name(MSG_process_self()));
  xbt_assert(res == MSG_OK, "MSG_task_get failed");
  MSG_task_execute(task); // Run task
  /* Get all the task data that was passed in */
  task_info *tsk = ((task_info*)(MSG_task_get_data(task)));
  long task_lab = tsk->get_label();
  char pop[256];
  /* Determine if this was a push or pull in the network*/ 
  if (tsk->get_pop() == 1){ 
    sprintf(pop,"%s","pull");
    gene_pulls[task_lab]++; //increment the genome that was finished
  } else {
    sprintf(pop,"%s","push");
    sim_pushes++;
  }
  /* Determine the total time the request was outstanding */
  avg_trans += (MSG_get_clock() - tsk->get_start());

  UI tsk_in = tsk->get_UI();
  long task_id = tsk->get_id()+1; // add the debug information to the string
  //XBT_INFO("FINISH %s of Genome #%ld -> ID: %ld",pop,task_lab,task_id);
  MSG_task_destroy(task); //destory the task

  //make sure only pushes execute this code
  while (sim_pulls < tsk_in->get_pulls() && tsk->get_pop() == 0){ 
    char* name_host;
    xbt_dynar_t hosts = MSG_hosts_as_dynar(); 
    //set destination to the last of the routers
    proc_info *dat = new proc_info(MaxRouter(nb_hosts),300000000000.0,0,1);
    dat->set_UI(tsk_in);
    //needs to take into account tht nb_hosts/2 -> nb_hosts are all sites
    name_host = bprintf("%d",UniformD(MinSite(nb_hosts),nb_hosts));
    dat->set_label(task_lab);
    MSG_process_sleep(abs(Exponential(tsk_in->get_pullrate())));
    MSG_process_create( name_host, send, dat, xbt_dynar_get_as(hosts,hostMap[atoi(name_host)],msg_host_t) );
    //needs to take into account tht nb_hosts/2 -> nb_hosts are all sites
    //destination needs to be a valid router
    name_host = bprintf("%d",MaxRouter(nb_hosts));
    MSG_process_create( name_host, recv, NULL, xbt_dynar_get_as(hosts,hostMap[MaxRouter(nb_hosts)],msg_host_t) );
    sim_pulls++;
  }
  return 0;
}

int send(int argc, char *argv[]){
  /* get name of process -> returns current process -> convert to int */
  int host_number = atoi(MSG_process_get_name(MSG_process_self()));

  /* Mailbox is the talking point between agents*/
  char mailbox[256];

  /* set task to send to NULL (otherwise a previous task may partially reside*/
  msg_task_t task = NULL;
  
  /* Some unused variable MACRO in misc.h */
  _XBT_GNUC_UNUSED int res;

  /* Pull Requests will ENTER HERE */
  /* Push Requests will ENTER HERE */
  //create debug string for debug 
  char debug[256];
  
  //Get information passed in from the process to find destinantion, size, etc.
  proc_info *dat  = (proc_info*)MSG_process_get_data(MSG_process_self());

  //convert host_number to a string for mailbox 
  sprintf(mailbox,"%d",dat->get_recv());

  //Set the comm size to that of what was passed in
  task_comm_size = dat->get_size();

  //Update the time for which this process will be injected into network

  char pop[256];
  task_info *t;
  
  if ( dat->get_pop() == 0){ //push the data
    global_time += abs(Exponential(dat->get_time())); //add to global time, sleep time
    MSG_process_sleep(global_time); // camt go above MSG_task_create
    t = new task_info(label++,0,(debug_id++)); // create a new task with label info
    sprintf(pop,"%s","push");
    //XBT_INFO("START %s: Genome #%ld from %d to %d -> ID: %ld",
    //          pop,t->get_label(),host_number,dat->get_recv(), debug_id);
  } else { //pull the data
    sprintf(pop,"%s","pull");

    long ele = dat->get_label();

    t = new task_info(ele,1,(debug_id++)); //create the task for pulling
    //XBT_INFO("START %s: Genome #%ld from %d to %d -> ID: %ld",
    //          pop,t->get_label(),dat->get_recv(),host_number,debug_id);
  }
  t->set_start(MSG_get_clock()); //set the start time for the process
  t->set_UI(dat->get_UI());

  /* Ship off the task to simulator */
  task = MSG_task_create(debug, task_comp_size, task_comm_size, t);
  MSG_task_send(task, mailbox);

  return 0;
}

void single_server(UI in){
  DEBUGE("In simGrid \n");
  int res;
  /* Initialie "some MSG" internal data */
  /* int *argc, char **argv */
  int num_args = 7;
  char* arguments[7];
  arguments[0] = (char*)"platform.xml"; /* Does something... */
  /* This log line is a pain in the ass, without it, it will crash occasionally 
   * The string is set up in a certain format such that parsing can be done.
  */
  arguments[1] = (char*)"--log=root.fmt:[%h:%P:(%i)%e%r]%e[%c/%p]%e%m%n";
  arguments[2] = (char *)"--cfg=tracing:1";
  arguments[3] = (char *)"--cfg=tracing/uncategorized:1";
  arguments[4] = (char *)"--cfg=tracing/filename:SIM.trace";
  arguments[5] = (char *)"--cfg=triva/uncategorized:uncat.plist";
  char winSizeArg[256];
  sprintf(winSizeArg, "--cfg=network/TCP_gamma:%u", in->get_winsize());
  arguments[6] = winSizeArg; /*default 20000*/

  MSG_init(&num_args, arguments); /* set up enviroment with platform.xml file*/
  MSG_create_environment("platform.xml"); /* nb_hosts is set to a dynamic array set */

  TRACE_platform_graph_export_graphviz("SIMtopo.dot");
  system("dot -Tpdf SIMtopo.dot -o SIMtopo.pdf &> /dev/null");

  xbt_dynar_t hosts = MSG_hosts_as_dynar(); 
  /* get the length of the dynamic array = # of hosts */
  nb_hosts =  xbt_dynar_length(hosts); 
  // Setup map from sim host numbers to xbt host numbers.
  hostMap.resize(nb_hosts);
  char		*bstr;
  unsigned int   xbtNum;
  xbt_dynar_foreach(hosts, xbtNum, bstr) {
    const char *hStr;
    int		simNum;
    hStr   = MSG_host_get_name(xbt_dynar_get_as(hosts,xbtNum,msg_host_t));
    simNum = atoi(&hStr[1]) + ((hStr[0] == 'r') ? 0 : MinSite(nb_hosts));
    hostMap[simNum] = xbtNum;
  }

  /* Let SimGrid know that hosts use the host function */
  MSG_function_register("send", send);
  MSG_function_register("recv", recv);
  /* Log this event at INFO priority */
  //XBT_INFO("Number of host '%d'",nb_hosts);
  //XBT_INFO("Reciever %d", MaxRouter(nb_hosts));

  //initialize gene_pull counter for each object
  for(long itor = 0; itor < in->get_objects(); itor++){
    gene_pulls[itor] = 0;
  }
  
  char* name_host;  

  //push these objects objects will spawn pulls
  for (int i=0; i< in->get_objects(); i++){
    //needs to account for the destination.
    proc_info *dat = new proc_info(MaxRouter(nb_hosts),300000000000.0,in->get_pushrate(),0);
    dat->set_UI(in);

    //needs to take into account tht nb_hosts/2 -> nb_hosts are all sites
    name_host = bprintf("%d",UniformD(MinSite(nb_hosts),nb_hosts));
    MSG_process_create( name_host, send, dat, xbt_dynar_get_as(hosts,hostMap[atoi(name_host)],msg_host_t) );
    //needs to take into account tht nb_hosts/2 -> nb_hosts are all sites
    //destination needs to be a valid router
    name_host = bprintf("%d",MaxRouter(nb_hosts));
    MSG_process_create( name_host, recv, NULL, xbt_dynar_get_as(hosts,hostMap[MaxRouter(nb_hosts)],msg_host_t) );
    /* This sleep is required to make sure that the simulation is not stuck to just 1 process*/
  }

/*BRS
for (int i = 0; i < nb_hosts; i++) {
  printf("@@@@ hostMap[%d] = %d -> %s\n", i, hostMap[i], MSG_host_get_name(xbt_dynar_get_as(hosts,hostMap[i],msg_host_t)));
}
*/

  /* Free the dynamic array that holds the host list*/
  xbt_dynar_free(&hosts);

  /* LAUNCH MSG SIMULATOR */
  res = MSG_main();
  
  /* Log event at INFO priority */
  DEBUGO("Simulation time %0.1f\n", MSG_get_clock());


  DEBUGO("Pushes: %ld - total Pulls: %ld\n", sim_pushes, sim_pulls);
  DEBUGO("average time: %f\n",avg_trans/(sim_pushes+sim_pulls));
  //DEBUGO("Genome Pulls:\n");
  std::map<long, long>::const_iterator it;
  for (it= gene_pulls.begin(); it != gene_pulls.end(); ++it){
    //DEBUGO("Genome [%ld] = %ld pulls\n",it->first, it->second);
  }
  
  /* Did the Simulator return properly - or was an error encountered. */
  if (res != MSG_OK){
    exit_fail((char*)__FILE__,__LINE__,6, (char*)"MSG SimGrid Failed to Run!");
  }
}

std::vector<std::set<int> > locations; //where is the genome located
pair<set<int>::iterator,bool> ret; //set return type

int ssc_recv(int argc, char *argv[]){
  /* get name of process -> returns current process -> convert to int */
  int host_number = atoi(MSG_process_get_name(MSG_process_self()));

  /* Mailbox is the talking point between agents*/
  char mailbox[256];

  /* set task to send to NULL (otherwise a previous task may partially reside*/
  msg_task_t task = NULL;
  
  /* Some unused variable MACRO in misc.h */
  _XBT_GNUC_UNUSED int res;

  task = NULL;
  sprintf(mailbox,"%d", host_number);
  res = MSG_task_receive(&(task), MSG_process_get_name(MSG_process_self()));
  xbt_assert(res == MSG_OK, "MSG_task_get failed");
  MSG_task_execute(task); // Run task
  /* Get all the task data that was passed in */
  task_info *tsk = ((task_info*)(MSG_task_get_data(task)));
  long task_lab = tsk->get_label();
  char pop[256];
  /* Determine if this was a push or pull in the network*/ 
  if (tsk->get_pop() == 1){ 
    sprintf(pop,"%s","pull");
    gene_pulls[task_lab]++; //increment the genome that was finished
  } else {
    sprintf(pop,"%s","push");
    sim_pushes++;
  }
  /* Determine the total time the request was outstanding */
  avg_trans += (MSG_get_clock() - tsk->get_start());

  
  UI tsk_in = tsk->get_UI();
  long task_id = tsk->get_id()+1; // add the debug information to the string

  //If we have a push comming through, add genome
  if (tsk->get_pop() == 0){
    locations[task_lab].insert(tsk->get_dest()); //add the destination to genome list
    DEBUGE("%ld - %d\n",task_lab,*(locations[(int)task_lab].begin()));
  } 
  //If we have a pull request finishing, add the genome to the path of routers crossed
  else {
    int src = (tsk->get_src() - (nb_hosts/2)); //want the router version
    int dest = tsk->get_dest();
    next_hop *H = tsk->get_nh();
    std::vector<int> visited;
    //DEBUGE("SRC %d DEST %d\n", src,dest);
    while ((int)(*H)[src][dest] != dest){
      dest = (int)(*H)[src][dest];
      visited.push_back(dest);
    }
    for (std::vector<int>::iterator i = visited.begin(); i != visited.end(); ++i){
      ret = locations[task_lab].insert(*i); // Add each of the Routers
      if (ret.second){
        DEBUGE("Router %d added for Genome %ld\n",*i,task_lab);
      }
    }
  }

  Routes *R = tsk->get_route();
  next_hop *H = tsk->get_nh();

  //XBT_INFO("FINISH %s of Genome #%ld -> ID: %ld",pop,task_lab,task_id);
  MSG_task_destroy(task); //destory the task

  //make sure only pushes execute this code
  while (sim_pulls < tsk_in->get_pulls() && tsk->get_pop() == 0){ 
    char* name_host;
    xbt_dynar_t hosts = MSG_hosts_as_dynar(); 
    //Destination field here will Have to change from cached router
    proc_info *dat = new proc_info(MaxRouter(nb_hosts),300000000000.0,0,1);
    dat->set_route(R); //used to find short latency
    dat->set_nh(H); //used to find all routers Genome crosses
    dat->set_UI(tsk_in);
    dat->set_label(task_lab);
    name_host = bprintf("%d",UniformD(MinSite(nb_hosts),nb_hosts)); //pick any site
    MSG_process_sleep(abs(Exponential(tsk_in->get_pullrate())));
    MSG_process_create( name_host, ssc_send, dat, xbt_dynar_get_as(hosts,hostMap[atoi(name_host)],msg_host_t) );

    // ONLY SHOULD SPAWN PROCESS HERE, NEED TO CATCH ELSEWHERE

    sim_pulls++;
  }
  return 0;
}

int ssc_send(int argc, char *argv[]){
  /* get name of process -> returns current process -> convert to int */
  int host_number = atoi(MSG_process_get_name(MSG_process_self()));

  /* Mailbox is the talking point between agents*/
  char mailbox[256];

  /* set task to send to NULL (otherwise a previous task may partially reside*/
  msg_task_t task = NULL;
  
  /* Some unused variable MACRO in misc.h */
  _XBT_GNUC_UNUSED int res;

  /* Pull Requests will ENTER HERE */
  //create debug string for debug 
  char debug[256];
  
  //Get information passed in from the process to find destinantion, size, etc.
  proc_info *dat  = (proc_info*)MSG_process_get_data(MSG_process_self());

  //Set the comm size to that of what was passed in
  task_comm_size = dat->get_size();

  //convert host_number to a string for mailbox 
  sprintf(mailbox,"%d",dat->get_recv());

  //Update the time for which this process will be injected into network

  char pop[256];
  task_info *t;
  
  if ( dat->get_pop() == 0){ //push the data
    global_time += abs(Exponential(dat->get_time())); //add to global time, sleep time
    MSG_process_sleep(global_time); // camt go above MSG_task_create
    t = new task_info(label++,0,(debug_id++)); // create a new task with label info
    sprintf(pop,"%s","push");
    //XBT_INFO("START %s: Genome #%ld from %d to %d -> ID: %ld",
    //          pop,t->get_label(),host_number,dat->get_recv(), debug_id);
  } else { //pull the data
    sprintf(pop,"%s","pull");

    long ele = dat->get_label();

    // For the element picked, find the nearest holder of the Genome.
    int closest = 9;
    int short_dist = 32000;

    if (!locations[ele].empty()){ // if it isnt empty (shouldnt ever)
      std::set<int>::iterator loc_i;
      loc_i = locations[ele].begin();
      //DEBUGE("SRC %d\n",host_number);
      int src = host_number-(nb_hosts/2); //HOST CONVERSION TO SITE
      Routes *R = dat->get_route();
      /* Here we need to find the closest located Genome, so go through all
         Genome locations, find the distance from source to location, store
         only the closest genome location, use that for the pull request instead
         of nb_hosts-1 (default value - is always contained in locations
      */
      while (loc_i != locations[ele].end()){ 
        int dist = (*R)[src][(*loc_i)];
        //DEBUGE("SRC %d DEST %d DIST %d \n",src,*loc_i,dist);
        if (dist < short_dist){
          short_dist = dist;
          closest = *loc_i;
        }
        ++loc_i;
      }
    }

    //Create the task for this pull
    t = new task_info(ele,1,(debug_id++)); //create the task for pulling
    
    //Change the location from nb_hosts-1 to closest if applicable
    dat->set_recv(closest);
    sprintf(mailbox,"%d",dat->get_recv());
    char* name_host = bprintf("%d",dat->get_recv());
    xbt_dynar_t hosts = MSG_hosts_as_dynar(); 
    //create process to catch this in the end. Had to be done dynamically, at
    //time of process creation not clear where destination would be
    MSG_process_create( name_host, ssc_recv, NULL, xbt_dynar_get_as(hosts,hostMap[atoi(name_host)],msg_host_t) );

    //XBT_INFO("START %s: Genome #%ld from %d to %d -> ID: %ld",
    //          pop,t->get_label(),dat->get_recv(),host_number,debug_id);
  }
  t->set_route(dat->get_route()); //set new route
  t->set_nh(dat->get_nh()); //set new next_hop info
  t->set_start(MSG_get_clock()); //set the start time for the process
  t->set_UI(dat->get_UI()); //add UI information
  t->set_src(host_number); //set new source
  t->set_dest(dat->get_recv()); //set intended reciever

  /* Ship off the task to simulator */
  task = MSG_task_create(debug, task_comp_size, task_comm_size, t);
  MSG_task_send(task, mailbox);

  return 0;
}

void single_server_cache(UI in, Routes *R, next_hop *H){
  DEBUGE("In simGrid \n");
  int res;
  /* Initialie "some MSG" internal data */
  /* int *argc, char **argv */
  int num_args = 7;
  char* arguments[7];
  arguments[0] = (char*)"platform.xml"; /* Does something... */
  /* This log line is a pain in the ass, without it, it will crash occasionally 
   * The string is set up in a certain format such that parsing can be done.
  */
  arguments[1] = (char*)"--log=root.fmt:[%h:%P:(%i)%e%r]%e[%c/%p]%e%m%n";
  arguments[2] = (char *)"--cfg=tracing:1";
  arguments[3] = (char *)"--cfg=tracing/uncategorized:1";
  arguments[4] = (char *)"--cfg=tracing/filename:SIM.trace";
  arguments[5] = (char *)"--cfg=triva/uncategorized:uncat.plist";
  char winSizeArg[256];
  sprintf(winSizeArg, "--cfg=network/TCP_gamma:%u", in->get_winsize());
  arguments[6] = winSizeArg; /*default 20000*/

  MSG_init(&num_args, arguments); /* set up enviroment with platform.xml file*/
  MSG_create_environment("platform.xml"); /* nb_hosts is set to a dynamic array set */

  TRACE_platform_graph_export_graphviz("SIMtopo.dot");
  system("dot -Tpdf SIMtopo.dot -o SIMtopo.pdf &> /dev/null");

  xbt_dynar_t hosts = MSG_hosts_as_dynar(); 
  /* get the length of the dynamic array = # of hosts */
  nb_hosts =  xbt_dynar_length(hosts); 
  // Setup map from sim host numbers to xbt host numbers.
  hostMap.resize(nb_hosts);
  char		*bstr;
  unsigned int   xbtNum;
  xbt_dynar_foreach(hosts, xbtNum, bstr) {
    const char *hStr;
    int		simNum;
    hStr   = MSG_host_get_name(xbt_dynar_get_as(hosts,xbtNum,msg_host_t));
    simNum = atoi(&hStr[1]) + ((hStr[0] == 'r') ? 0 : MinSite(nb_hosts));
    hostMap[simNum] = xbtNum;
  }

  /* Let SimGrid know that hosts use the host function */
  MSG_function_register("ssc_send", ssc_send);
  MSG_function_register("ssc_recv", ssc_recv);
  /* Log this event at INFO priority */
  //XBT_INFO("Number of host '%d'",nb_hosts);


  for(long itor = 0; itor < in->get_objects(); itor++){
    gene_pulls[itor] = 0;
    std::set<int> s;
    locations.push_back(s);
  }
  
  char* name_host;  

  //push these objects
  for (int i=0; i< in->get_objects(); i++){
    proc_info *dat = new proc_info(MaxRouter(nb_hosts),300000000000.0,in->get_pushrate(),0);
    dat->set_UI(in);
    dat->set_route(R); //used to find short latency
    dat->set_nh(H); //used to find all routers Genome crosses
    name_host = bprintf("%d",UniformD(MinSite(nb_hosts),nb_hosts));
    dat->set_send(atol(name_host));
    MSG_process_create( name_host, ssc_send, dat, xbt_dynar_get_as(hosts,hostMap[atoi(name_host)],msg_host_t) );
    name_host = bprintf("%d",MaxRouter(nb_hosts));
    MSG_process_create( name_host, ssc_recv, NULL, xbt_dynar_get_as(hosts,hostMap[MaxRouter(nb_hosts)],msg_host_t) );
  }

  /* Free the dynamic array that holds the host list*/
  xbt_dynar_free(&hosts);

  /* LAUNCH MSG SIMULATOR */
  res = MSG_main();
  
  /* Log event at INFO priority */
  DEBUGO("Simulation time %0.1f\n", MSG_get_clock());

  /* Clean up after Simulator */

  DEBUGO("Pushes: %ld - total Pulls: %ld\n", sim_pushes, sim_pulls);
  DEBUGO("average time: %f\n",avg_trans/(sim_pushes+sim_pulls));
  //DEBUGO("Genome Pulls:\n");
  std::map<long, long>::const_iterator it;
  for (it= gene_pulls.begin(); it != gene_pulls.end(); ++it){
      //DEBUGO("Genome [%ld] = %ld pulls\n",it->first, it->second);
  }

/* SOMETHING TO ADD - WHEN IT DOESNT SEG FOR NO REASON
  DEBUGE("Genome Locations:\n");
  std::vector<std::set<int> >::iterator itor;
  int cnt = 0;

  for (itor = locations.begin(); itor != locations.end(); ++itor){
    DEBUGE("SIZE: %d\n",locations[5].size());
    DEBUGE("Genome [%d] located @ ", cnt);
    std::set<int>::iterator itt;
    for(itt = itor[cnt].begin(); itt != itor[cnt].end(); ++itt){
      fprintf(stderr,"%d ",*itt);
    }
    fprintf(stderr,"\n");
    cnt++;
  }
*/
  
  /* Did the Simulator return properly - or was an error encountered. */
  if (res != MSG_OK){
    exit_fail((char*)__FILE__,__LINE__,6, (char*)"MSG SimGrid Failed to Run!");
  }
}


void sim(UI input, Policy P, Routes *R, next_hop *H){
  DEBUGE("-------SIMULATION CORE--------\n");
  /* Set up PSNG */
  gsl_rng_env_setup();
  r = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(r, input->get_seed());
  task_comm_size = input->get_size();
  switch(P->get_policy()){
    case 0: // Undefined.
      fprintf(stderr, "Data Policy was not set correctly, exiting\n");
      exit(2);
      break;
    //SINGLE SERVER NO CACHE
    case 1: // Single Server
      DEBUGO("SINGLE SERVER NO CACHING ENABLED\n");
      /********************* POLICY *****************/
      //last node will be destination node. Read Single Server notes.
      // We also subtract another 1 BECUASE OFF BY 1, VERTS START @ 0 NOT 1
      input->set_vertices(input->get_vertices()-2);  
      //make sure that given this policy, more than 1 node
      if (input->get_vertices() == 1){ 
        fprintf(stderr, "Single Server Policy not usable in 1 vertex graph.\n");
        exit(3);
      }
      /***************** END POLICY *****************/
      
      // This will allow the request generator naively choose sources
      // While still allowing there to be a global destination.
      single_server(input);
      break;
    //SINGLE SERVER CACHING
    case 2:
      DEBUGO("SINGLE SERVER CACHING ENABLED\n");
      input->set_vertices(input->get_vertices()-2);  
      //make sure that given this policy, more than 1 node
      if (input->get_vertices() == 1){ 
        fprintf(stderr, "Single Server Policy not usable in 1 vertex graph.\n");
        exit(3);
      }
      single_server_cache(input, R, H);
      break;
    case 3:
      break;
    //....
    default:
      break;
  }
  gsl_rng_free(r);
}
