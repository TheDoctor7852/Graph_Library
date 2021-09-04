CXX_FLAGS := -Wno-unused-local-typedefs -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -Wno-unused -pthread -fopenmp -Wall -Wno-deprecated -O3 -Wsign-compare   -std=c++17
CXX_DEFINES := -DBOOST_ALL_NO_LIB -DBOOST_DATE_TIME_DYN_LINK -DHAS_BOOST -DUSE_LOGGING -DUSE_PMDK -DUSE_TX
OBJDIR := obj
SRCDIR := src
PATH_TO_POSEIDON_CORE := /home/jan/Dokumente/poseidon_core#hier der totale oder abstratkte Pfad zu Poseidon
POS_COR_SRC := $(PATH_TO_POSEIDON_CORE)/src#falls das src-Verzischniss umbenannt wurde auch hier umbenennen
POS_COR_BUILD := $(PATH_TO_POSEIDON_CORE)/build#falls das build-Verzischniss umbenannt(bzw. anders benannt) wurde auch hier umbenennen
INC := -I$(POS_COR_SRC) -I$(POS_COR_SRC)/btree -I$(POS_COR_SRC)/dict -I$(POS_COR_SRC)/ldbc -I$(POS_COR_SRC)/log -I$(POS_COR_SRC)/pool -I$(POS_COR_SRC)/py -I$(POS_COR_SRC)/qop -I$(POS_COR_SRC)/storage -I$(POS_COR_SRC)/tx -I$(POS_COR_SRC)/graph_analytics/analytics -I$(POS_COR_SRC)/graph_analytics/utils -I$(POS_COR_BUILD)/_deps/spdlog-src/include -I$(POS_COR_BUILD)/_deps/csv-src -I$(POS_COR_BUILD)/_deps/catch-src/single_include/catch2 -I$(POS_COR_BUILD)/generated
LINKER_COMMANDS := -Wl,-rpath,$(POS_COR_BUILD) -L$(POS_COR_BUILD) -lposeidon_core -lpmemobj /usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.71.0 -lpthread

example: $(OBJDIR)/example.o
	g++ -g $(CXX_FLAGS) $^ -o $@ $(LINKER_COMMANDS) -Wl,-rpath,../library_classes/build -L../library_classes/build -lgraph_components -lprimesieve

$(OBJDIR)/example.o: $(SRCDIR)/example.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../library_classes/header  -o $@ -c $^

example_timed: $(OBJDIR)/example_timed.o
	g++ -g $(CXX_FLAGS) $^ -o $@ $(LINKER_COMMANDS) -Wl,-rpath,../library_classes/build -L../library_classes/build -lgraph_components -lprimesieve

$(OBJDIR)/example_timed.o: $(SRCDIR)/example_timed.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../library_classes/header  -o $@ -c $^

example_workstealing: $(OBJDIR)/example_workstealing.o
	g++ -g $(CXX_FLAGS) $^ -o $@ $(LINKER_COMMANDS) -Wl,-rpath,../library_classes/build -L../library_classes/build -lgraph_components -lprimesieve

$(OBJDIR)/example_workstealing.o: $(SRCDIR)/example_workstealing.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../library_classes/header -I/home/jan/Dokumente/Bachelorarbeit/Parallel_Librarys/work-stealing-queue -o $@ -c $^

example_gperf: $(OBJDIR)/example_gperf.o
	g++ -g $(CXX_FLAGS) $^ -o $@ $(LINKER_COMMANDS) -Wl,-rpath,../library_classes/build -L../library_classes/build -lgraph_components -lprimesieve -lprofiler

$(OBJDIR)/example_gperf.o: $(SRCDIR)/example_gperf.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../library_classes/header  -o $@ -c $^

example_ownWorkstealing: $(OBJDIR)/example_ownWorkstealing.o
	g++ -g $(CXX_FLAGS) $^ -o $@ $(LINKER_COMMANDS) -Wl,-rpath,../library_classes/build -L../library_classes/build -lgraph_components -lprimesieve -lprofiler

$(OBJDIR)/example_ownWorkstealing.o: $(SRCDIR)/example_ownWorkstealing.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../library_classes/header  -o $@ -c $^

PageRank: $(OBJDIR)/pageRank.o
	g++ -g $(CXX_FLAGS) $^ -o $@ $(LINKER_COMMANDS) -Wl,-rpath,../library_classes/build -L../library_classes/build -lgraph_components -lprimesieve

$(OBJDIR)/pageRank.o: $(SRCDIR)/pageRank.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../library_classes/header  -o $@ -c $^