UNAME := $(shell uname)
OUT := $(shell echo "Building for $(UNAME)\n")

INCLUDE = -I/home/lthurlow/Genome/include/ #-I/../SimGrid-3.7.1/include/
SOURCE = main.cc  file_format.cc util.cc randvars2.cc \
         routing.cc simgrid.cc #single_server.cc #simulation.cc generate_request.cc
OBJ =    main.o   file_format.o  util.o  randvars2.o  \
         routing.o  simgrid.o  #single_server.o #simulation.o generate_request.o 
LIB = -lgsl -lgslcblas -lm -lsimgrid -rdynamic /opt/simgrid/lib/libsimgrid.so.3.8.1
HEADERS = bgl_graph_info.hpp types.hpp randvars2.hpp routing.hpp \
          simgrid.hpp #simulation.hpp
EXE = sim
CFLAGS = -g -ansi -Wall -lrt -L/opt/simgrid/lib
GENFILES = pref pref-0.gb pref.alt graph.dot graph.pdf



ALL: DEBUG $(EXE)

DEBUG:
	@echo "------> Building for $(UNAME) <------"

# Source and Target have same name, this will compile that for us.
# $< = first dependency
%.o: %.cc $(HEADERS)
	g++ $(CFLAGS) $(INCLUDE) -c -o $@ $<

# $@ = left hand side
# $^ = right hand side
$(EXE): $(OBJ)
	g++ $(CFLAGS) $(INCLUDE) -o $@ $^ $(LIB) 

#executables - kill errs
#objects - print errors
#files - kill errs
#touch before remove, removes rm error, updates timestamp for make.
clean :
	touch sim
	touch main.o generate_request.o file_format.o util.o randvars2.o routing.o simulation.o
	touch simgrid.o
	touch pref pref-0.gb pref.alt graph.dot graph.pdf
	touch platform.xml deployment.xml
	touch sim.err sim.trace sim.plist
	rm sim 
	rm main.o generate_request.o file_format.o util.o randvars2.o routing.o simulation.o
	rm simgrid.o
	rm pref pref-0.gb pref.alt graph.dot graph.pdf
	rm platform.xml deployment.xml
	rm sim.err sim.trace sim.plist
