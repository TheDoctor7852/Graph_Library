#include "graph_node_iterator_workstealing_own.hpp"

Graph_Node_Iterator_Timed_Workstealing_Own::Graph_Node_Iterator_Timed_Workstealing_Own(std::vector<std::unique_ptr<Node>>::iterator begin_input, std::vector<std::unique_ptr<Node>>::iterator end_input, size_t thread_count) : distributer(begin_input, end_input, thread_count){
    begin = begin_input;
    end = end_input;
    size_t distance = std::distance(begin, end);

    thread_limits = {0};

    for(size_t i=0; i<thread_count; i++){
        if(i<(distance % thread_count)){
            thread_limits.push_back(thread_limits[i]+1+(distance/thread_count));
        } else{
            thread_limits.push_back(thread_limits[i]+(distance/thread_count));
        }
    }

    prime_numbers={};

    auto p = primesieve::iterator(thread_limits[1]-thread_limits[0]);

    for(int i=0; i<10; i++){
        prime_numbers.push_back(p.next_prime());
    }

    /*std::random_device rd;  
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> distrib(0,prime_numbers.size()-1);
    for(size_t i=0; i<thread_limits.size()-1; i++){
        u_int64_t current_prime = prime_numbers[distrib(rng)]; 
        auto start = thread_limits[i];
        auto end = thread_limits[i+1];
        u_int64_t offset = current_prime % (end-start);
        for(size_t s=start; s<end; s++){
            //std::cout << s << std::endl;
            queue_vec[i].push(start+offset);
            offset = (offset + current_prime) % (end-start);
        }
    }*/ 
}
//TODO: beachten, wenn Knoten gelöscht werden, dass diese ids nicht mehr da sind. -> da mit offstes gearbeitete wird, ist dies nicht notwendig

std::vector<std::unique_ptr<Node>>::iterator Graph_Node_Iterator_Timed_Workstealing_Own::get_begin(){
    return begin;
}

std::vector<std::unique_ptr<Node>>::iterator Graph_Node_Iterator_Timed_Workstealing_Own::get_end(){
    return end;
}

const std::vector<size_t>& Graph_Node_Iterator_Timed_Workstealing_Own::get_thread_limits(){
    return thread_limits;
}

const std::vector<u_int64_t>& Graph_Node_Iterator_Timed_Workstealing_Own::get_prime_numbers(){
    return prime_numbers;
}

std::vector<std::vector<std::pair<std::chrono::_V2::system_clock::time_point, std::chrono::_V2::system_clock::time_point>>>& Graph_Node_Iterator_Timed_Workstealing_Own::get_times_vec(){
    return times_vec;
}

Work_Distributer& Graph_Node_Iterator_Timed_Workstealing_Own::get_distributer(){
    return distributer;
}

//TODO: randomisiere Warteschlangenzugriff
void try_steal_work(Graph_Node_Iterator_Timed_Workstealing_Own& g_it, FunctionTypeNode f, const size_t& process_id, std::vector<size_t>::iterator& offset_pointer){
    while(g_it.get_distributer().steal(process_id)){
        //std::cout << "stolen: " << (*offset_pointer) << std::endl;
        f((g_it.get_begin()+(*offset_pointer))->get());
        try_steal_work(g_it, f, process_id, offset_pointer);
    }
}

//TODO: aufbau der queues parallelisieren (für den randomisierten zugriff) ansonsten die queue vielleicht auch kopieren
void for_each_time_workstealing(Graph_Node_Iterator_Timed_Workstealing_Own& g_it, FunctionTypeNode f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    g_it.get_times_vec().push_back(std::vector<std::pair<std::chrono::_V2::system_clock::time_point, std::chrono::_V2::system_clock::time_point>>(g_it.get_thread_limits().size()));
    g_it.get_distributer().reset_workcopy();
    g_it.get_distributer().reset_positions();
    //std::random_device rd;  
    //std::mt19937 rng(rd());
    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it,f, i](){
            auto start = std::chrono::high_resolution_clock::now();
            auto it = g_it.get_begin();
            auto own_offset_position = g_it.get_distributer().get_position_ptr(i);
            
            do{
                size_t offset = (*own_offset_position);
                //std::cout << offset << std::endl;
                f((it+offset)->get());
            }while(g_it.get_distributer().next(i));
            auto end = std::chrono::high_resolution_clock::now();
            g_it.get_times_vec().at(g_it.get_times_vec().size()-1).at(i) = std::make_pair(start, end);
            try_steal_work(g_it, f, i, own_offset_position);
          return true;
        }));
    }
    auto start = std::chrono::high_resolution_clock::now();
    for(auto it = futures.begin(); it != futures.end(); it++){
        it->get();
    }
    auto end = std::chrono::high_resolution_clock::now();
    g_it.get_times_vec().at(g_it.get_times_vec().size()-1).at(g_it.get_thread_limits().size()-1) = std::make_pair(start, end);
}

void for_each_workstealing(Graph_Node_Iterator_Timed_Workstealing_Own& g_it, FunctionTypeNode f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    g_it.get_distributer().reset_workcopy();
    g_it.get_distributer().reset_positions();

    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it,f, i](){
            auto start = std::chrono::high_resolution_clock::now();
            auto it = g_it.get_begin();
            auto own_offset_position = g_it.get_distributer().get_position_ptr(i);
       
            do{
                size_t offset = (*own_offset_position);
                f((it+offset)->get());
            }while(g_it.get_distributer().next(i));
            try_steal_work(g_it, f, i,own_offset_position);
          return true;
        }));
    }
    for(auto it = futures.begin(); it != futures.end(); it++){
        it->get();
    }
}
/*
void for_each_random_time_workstealing(Graph_Node_Iterator_Timed_Workstealing_Own& g_it, FunctionTypeNode f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    std::vector<std::pair<std::chrono::_V2::system_clock::time_point, std::chrono::_V2::system_clock::time_point>> vec(g_it.get_thread_limits().size());
    std::random_device rd;  
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> distrib(0,g_it.get_prime_numbers().size()-1);
    for(size_t i=0; i<g_it.get_thread_limits().size()-1; i++){
        u_int64_t current_prime = g_it.get_prime_numbers()[distrib(rng)]; 
        auto start = g_it.get_thread_limits()[i];
        auto end = g_it.get_thread_limits()[i+1];
        u_int64_t offset = current_prime % (end-start);
        for(size_t s=start; s<end; s++){
            //std::cout << s << std::endl;
            g_it.get_queue()[i].push(start+offset);
            offset = (offset + current_prime) % (end-start);
        }
    } 
    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it, &vec, f, i](){
            auto start_timer = std::chrono::high_resolution_clock::now();
            auto iter = g_it.get_begin();
            auto& own_queue = g_it.get_queue()[i];
            while(!own_queue.empty()){
                auto offset = own_queue.pop();
                if(offset){
                    f((iter+(*offset))->get());
                }
            }
            try_steal_work(g_it, f);
            auto end_timer = std::chrono::high_resolution_clock::now();
            vec[i]= std::make_pair(start_timer, end_timer);
          return true;
        }));
    }
    auto start = std::chrono::high_resolution_clock::now();
    for(auto it = futures.begin(); it != futures.end(); it++){ 
        it->get();      
    }
    auto end = std::chrono::high_resolution_clock::now();
    vec[g_it.get_thread_limits().size()-1] = std::make_pair(start, end);
    g_it.get_times_vec().push_back(vec);
}

void for_each_random_workstealing(Graph_Node_Iterator_Timed_Workstealing_Own& g_it, FunctionTypeNode f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    std::vector<std::pair<std::chrono::_V2::system_clock::time_point, std::chrono::_V2::system_clock::time_point>> vec(g_it.get_thread_limits().size());
    std::random_device rd;  
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> distrib(0,g_it.get_prime_numbers().size()-1);
    for(size_t i=0; i<g_it.get_thread_limits().size()-1; i++){
        u_int64_t current_prime = g_it.get_prime_numbers()[distrib(rng)]; 
        auto start = g_it.get_thread_limits()[i];
        auto end = g_it.get_thread_limits()[i+1];
        u_int64_t offset = current_prime % (end-start);
        for(size_t s=start; s<end; s++){
            //std::cout << s << std::endl;
            g_it.get_queue()[i].push(start+offset);
            offset = (offset + current_prime) % (end-start);
        }
    } 
    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it, &vec, f, i](){
            auto iter = g_it.get_begin();
            auto& own_queue = g_it.get_queue()[i];
            while(!own_queue.empty()){
                auto offset = own_queue.pop();
                if(offset){
                    f((iter+(*offset))->get());
                }
            }
            try_steal_work(g_it, f);
          return true;
        }));
    }
    for(auto it = futures.begin(); it != futures.end(); it++){ 
        it->get();      
    }
}*/