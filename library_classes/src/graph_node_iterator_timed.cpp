#include "graph_node_iterator_timed.hpp"

Graph_Node_Iterator_Timed::Graph_Node_Iterator_Timed(std::vector<std::unique_ptr<Node>>::iterator begin_input, std::vector<std::unique_ptr<Node>>::iterator end_input, size_t thread_count){
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
}

std::vector<std::unique_ptr<Node>>::iterator Graph_Node_Iterator_Timed::get_begin(){
    return begin;
}

std::vector<std::unique_ptr<Node>>::iterator Graph_Node_Iterator_Timed::get_end(){
    return end;
}

const std::vector<size_t>& Graph_Node_Iterator_Timed::get_thread_limits(){
    return thread_limits;
}

const std::vector<u_int64_t>& Graph_Node_Iterator_Timed::get_prime_numbers(){
    return prime_numbers;
}

std::vector<std::vector<std::pair<std::chrono::_V2::system_clock::time_point, std::chrono::_V2::system_clock::time_point>>>& Graph_Node_Iterator_Timed::get_times_vec(){
    return times_vec;
}

void for_each_time(Graph_Node_Iterator_Timed& g_it, FunctionTypeNode f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    g_it.get_times_vec().push_back(std::vector<std::pair<std::chrono::_V2::system_clock::time_point, std::chrono::_V2::system_clock::time_point>>(g_it.get_thread_limits().size()));
    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it,f, i](){
            auto start = std::chrono::high_resolution_clock::now();
            auto it_start = g_it.get_begin() + g_it.get_thread_limits()[i];
            auto it_end = g_it.get_begin() + g_it.get_thread_limits()[i+1];
       
            for(auto it = it_start; it != it_end; it++){
                f(it->get());
            }
            auto end = std::chrono::high_resolution_clock::now();
            g_it.get_times_vec().at(g_it.get_times_vec().size()-1).at(i) = std::make_pair(start, end);
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

void for_each_random_time(Graph_Node_Iterator_Timed& g_it, FunctionTypeNode f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    std::vector<std::pair<std::chrono::_V2::system_clock::time_point, std::chrono::_V2::system_clock::time_point>> vec(g_it.get_thread_limits().size());
    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it, &vec, f, i](){
            auto start_timer = std::chrono::high_resolution_clock::now();
            std::random_device rd;  
            std::mt19937 rng(rd()); 
            std::uniform_int_distribution<int> distrib(0,g_it.get_prime_numbers().size()-1);
            u_int64_t current_prime = g_it.get_prime_numbers()[distrib(rng)];
            auto start = g_it.get_thread_limits()[i];
            auto end = g_it.get_thread_limits()[i+1];
            auto iter = g_it.get_begin() + g_it.get_thread_limits()[i];
            u_int64_t offset = current_prime % (end-start);
            for(auto it = start; it < end; it++){
                f((iter+offset)->get());
                offset = (offset + current_prime) % (end-start);
            }
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