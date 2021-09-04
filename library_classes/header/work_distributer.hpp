#include "graph.hpp"
# include <vector>
#include<mutex>
#include <random>

#ifndef GRAPH_NODE_ITER_TIMED_HPP
#define GRAPH_NODE_ITER_TIMED_HPP
//bei Next() muss aufpassen, entweder muss in do...while() Schleife verwenden oder mann muss sonderfall für 0 einbauen (weil wenn in while(next()){...} verwendet, wird immer vor der Schleife hochgezählt->würde die Startwerte verpassen -> kann einen niedriger starten -> probleme bei 0 weil in size_t)
class Work_Distributer{
    std::vector<size_t> thread_limits;
    std::vector<size_t> thread_limits_work_copy;
    std::vector<std::mutex> locks;
    std::vector<size_t> thread_positions;
    //std::random_device rd;  
    //std::mt19937 rng; 

    public:
        Work_Distributer(std::vector<std::unique_ptr<Node>>::iterator begin,std::vector<std::unique_ptr<Node>>::iterator end,size_t thread_count = std::thread::hardware_concurrency());
        bool next(size_t process_id);
        bool steal(size_t process_id);
        std::vector<size_t>::iterator get_position_ptr(size_t process_id);
        void reset_workcopy();
        void reset_positions();
};

#endif