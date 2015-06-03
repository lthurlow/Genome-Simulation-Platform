#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>

#include "types.hpp"
#include "randvars2.hpp"
#include "bgl_graph_info.hpp"
#include "routing.hpp"
#ifndef __SIMGRID_HPP__ //may not be needed with other file guards in place
#include "simgrid.hpp"
#endif

using namespace std;

unsigned RandomSeed()
{
    FILE *fd = fopen("/dev/random", "r");
    if (fd == NULL)
    {
      fprintf(stderr, "Couldn't open /dev/random!!!\n");
      exit(1);
    }

    unsigned seed;
    fread((char *) &seed, sizeof(unsigned), 1, fd);
    // Following clears the last bit... something in mcarlo2 doesn't like
    // full 32 bit random numbers.
    return(seed & ~(1<<((8*sizeof(unsigned))-1)));
}

/*
 * Take in arguments:
 * Size of graph (# of verticies, # of edges)
 *  -- In future, graph stats, (avg b/w, avg link weight, etc)
 * value to seed for generator.
*/
int main(int argc, char* argv[]){
  bool flag_to_print = false; //print simulation variable values
  int vert = 10; //number of nodes in network
  float ce = 0.5; //chance of edge [0,1]
  int degree = 5;
  unsigned seed = RandomSeed(); // seed for random generator
  int	   network_route = 1;

  unsigned win_size = 175000000;

  //BYTES
  double obj_size   = 300000000000;
  // IMPORTANT NOTE!! min and max_capacity are being used for tail and core
  // circuit capacities, respecitively. I know it's lazy, but I don't to go
  // through all the code and change min/max to tail/core...
  unsigned long min_capacity = 125000000;
  unsigned long max_capacity = 125000000; // 10Gb/s default

  //SECONDS
  float pushPerDay = 556.0; // Total pushes/day
  float pullPerDay = 8.33;  // Per genome pulls/day
  int   maxPulls   = 250;   // Maximum total pulls per genome

  //double time_delta = 20.0; // time will use an exponentional function

  long min_weight = 5;	// These are translated to link latency
  long max_weight = 25;	// equal => uniform latency

  int data_policy = 1;	// Single Server, No Cache Default

  float days = 1.0; // Length of simulation

  int c; // character from input of user

  // get options and parse.
  
  while ((c = getopt(argc, argv, "b:c:C:d:D:hHl:m:p:P:r:s:v:w:W:")) != -1){
    switch (c){
      case 'b': // passed in a TCP window size ("b" for buffer).
        win_size = atoi(optarg)*1000; // Command line is in KB
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Could not convert window size to valid integer." );
        }
        DEBUGO("Window Size: %u (expects int)\n",win_size);
        break;
      case 'c':
        min_capacity = atol(optarg)*125000000; //tail circuit capacity (in Gb/s)
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Could not convert minimum capacity to valid integer.");
        }
        break;
      case 'C':
        max_capacity = atol(optarg)*125000000; //core circuit capacity (in Gb/s)
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Could not convert maximum capacity to valid integer.");
        }
        break;
        break;
      case 'd': //degree for graph
        degree = atoi(optarg);
        if (degree <= 0){
          exit_fail((char*)__FILE__,__LINE__,1, (char*) "Not a valid degree value, deg > 0.");
        }
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Could not convert degree to valid integer.");
        }
        break;
      case 'D': //how the simulation distributes genomes
        data_policy = atoi(optarg);
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Could not convert maximum capacity to valid integer.");
        }
        break;
      case 'l': // length of simulation (days)
        days = atof(optarg); // Command line is in KB
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Could not convert simulation length to falid float." );
        }
        DEBUGO("Length of simulation: %f (expects float)\n",days);
        break;
      case 'm': // maximum number of pulls per genome
        maxPulls = atoi(optarg); // Command line is in KB
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Could not convert maximum pulls/genome to valid integer." );
        }
        DEBUGO("Max pulls/genome: %u (expects int)\n",maxPulls);
        break;
      case 'r': //routing policy
        network_route = atoi(optarg);
        if (network_route <= 0){
          exit_fail((char*)__FILE__,__LINE__,1, (char*) "Not a valid routing value.");
        }
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Could not convert routing value to valid integer.");
        }
        break;
      case 's': // passed in a seed.
        seed = atoi(optarg);
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Could not convert seed to valid integer." );
        }
        DEBUGO("SEED: %d (expects int)\n",seed);
        break;
      case 'v': //vertices in graph
        //err = strtol(optarg,NULL,vert);
        vert = atol(optarg);
        if (vert < 3){
          exit_fail((char*)__FILE__,__LINE__,1, (char*) "Please choose a more dense topology.");
        }
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*) "Could not convert vertices to valid integer.");
        }
        break;
      case 'w':
        min_weight = atof(optarg); // minimum weight for link
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Could not convert minimum weight to valid integer.");
        }
        break;
      case 'W':
        max_weight = atof(optarg); //maximum link weight
        if (errno == ERANGE){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Could not convert maximum weight to valid integer.");
        }
        break;
      case 'p': //arrival rate for pushes
        pushPerDay = atof(optarg);
        if (pushPerDay <= 0){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Invalid arrival rate, must be above 0 or a valid float");
        }
        break;
      case 'P': //arival rate for pulls
        pullPerDay = atof(optarg);
        if (pullPerDay <= 0){
          exit_fail((char*)__FILE__,__LINE__,1, (char*)"Invalid arrival rate, must be above 0 or a valid float");
        }
        break;
      case 'H':
        flag_to_print = true;
        break;
      case 'h': // help menu
      default:
        print_help();
        exit_fail((char*)__FILE__,__LINE__,0, (char*)"Unknown Value Entered.");
        break;
    }
  }

  const int SEC_PER_DAY = 24 * 60 * 60;
  float  push_rate = SEC_PER_DAY/pushPerDay; // inter-arrival rate for pushes
  double pull_rate = SEC_PER_DAY/pullPerDay; // inter-arrival rate for pulls

  long objects = days * pushPerDay; // How many pushes will be done
  long pulls   = days * pullPerDay; // How many pulls will be done per object
  pulls	       = (pulls > maxPulls) ? maxPulls : pulls;
  
  if (min_capacity > max_capacity){
    max_capacity = min_capacity;
  }
  // Need to make sure here to apply 1,2,3 rule to what the user entered
  // if they did not also change pulls & objects.
  
  //convert degree into a percentage for gt-itm to calculate chance of edge.
  ce = degree/(float)(vert-1);
  if ( ce > 1 || ce <= 0){
    exit_fail((char*)__FILE__,__LINE__,2, (char*) "Degree is not valid for this graph.");
  }

  /* Set passed in parameters into class to pass to functions */
  UI input = new user_input;
  input->set_vertices(vert); //verts in graph
  input->set_edgechance(ce); //degree
  input->set_pushrate(push_rate); //push rate for genome
  input->set_pullrate(pull_rate); //pull rate for genome
  input->set_pulls(pulls*objects); //total pulls in simulation
  input->set_objects(objects); //objects in simulation
  input->set_size(obj_size); //object size in simulation
  input->set_mincap(min_capacity); //tail link capacity
  input->set_maxcap(max_capacity); //core link capacity
  input->set_minw(min_weight); //min weight i.e latency
  input->set_maxw(max_weight); //max weight i.e latency
  input->set_seed(seed); //seed for simulation
  input->set_route(network_route); //network routing policy
  input->set_winsize(win_size); // TCP window size
  input->set_pushPerDay(pushPerDay);
  input->set_pullPerDay(pullPerDay);
  input->set_days(days);
  input->set_maxpulls(maxPulls);

  /* Set policy to be used to distribute/cache data */
  Policy P = new policy;
  P->set_policy(data_policy);

  if (flag_to_print){ // Print user values
    print_input(input, P);
  }


  //Lol, this needs to be before I aritificially adjust the lengths.
  //Otherwise, gnarly errors, mainly because it will try accessing an
  //element that doesnt exist.  
  Routes R(vert,std::vector<double>(vert));

  /* Verify the policy is legitamate.*/
  if (P->get_policy() == 0 || P->get_policy() > MAX_POLICIES){
    exit_fail((char*)__FILE__,__LINE__,3, (char*) "Invalid Policy entered.");
  }

  /* Generate the initial gt-itm graph based on user-input */
  gen_graph(input, seed);

  /* Convert the sgb file to alternate formate and then to dot format */
  Graph G = con_graph(input);
  
  /* Create a structure for keeping next hop from src to dest*/ 
  next_hop hop;
  /* Build the next hop table based on Routing Policy*/
  hop = build_forwarding_tables(G, network_route, &R); //takes in some int based on how to route.
  
  if (hop.size() <= 1){
    exit_fail((char*)__FILE__,__LINE__,3, (char*) "Error: Routing was not done. Verify correct Route value.");
  }

  /* Write out the MSG platform file for MSG simulation*/
  write_platform(&G, &hop, input);
  
  //DEBUGE("Before SIMGRID\n");
  sim(input, P, &R, &hop);
  //DEBUGE("After SIMGRID\n");

}
