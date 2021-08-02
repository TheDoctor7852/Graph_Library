#include "graph_relationship_iterator.hpp"

Graph_Rel_Iterator::Graph_Rel_Iterator(std::vector<std::unique_ptr<Relationship>>::iterator begin_input, std::vector<std::unique_ptr<Relationship>>::iterator end_input, size_t thread_count){
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

std::vector<std::unique_ptr<Relationship>>::iterator Graph_Rel_Iterator::get_begin(){
    return begin;
}

std::vector<std::unique_ptr<Relationship>>::iterator Graph_Rel_Iterator::get_end(){
    return end;
}

const std::vector<size_t>& Graph_Rel_Iterator::get_thread_limits(){
    return thread_limits;
}

const std::vector<u_int64_t>& Graph_Rel_Iterator::get_prime_numbers(){
    return prime_numbers;
}

void for_each(Graph_Rel_Iterator g_it, FunctionTypeRel f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it,f, i](){
            auto it_start = g_it.get_begin() + g_it.get_thread_limits()[i];
            auto it_end = g_it.get_begin() + g_it.get_thread_limits()[i+1];
       
            for(auto it = it_start; it != it_end; it++){
                f(it->get());
            }
          return true;
        }));
    }
    for(auto it = futures.begin(); it != futures.end(); it++){
        it->get();
    }
}

void for_each_random(Graph_Rel_Iterator g_it, FunctionTypeRel f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it,f, i](){
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
          return true;
        }));
    }
    for(auto it = futures.begin(); it != futures.end(); it++){
        it->get();
    }
}

void for_each_time(Graph_Rel_Iterator g_it, FunctionTypeRel f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it,f, i](){
            auto start = std::chrono::high_resolution_clock::now();
            auto it_start = g_it.get_begin() + g_it.get_thread_limits()[i];
            auto it_end = g_it.get_begin() + g_it.get_thread_limits()[i+1];
       
            for(auto it = it_start; it != it_end; it++){
                f(it->get());
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "Process " << g_it.get_thread_limits()[i] << " started: " << std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count() << "  ";
            std::cout << "Process " << g_it.get_thread_limits()[i] << " ended: " << std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); 
            std::cout << "Time taken by Process "  << g_it.get_thread_limits()[i] << ": "<< duration.count() << " microseconds" << std::endl;
            return true;
        }));
    }
    auto start = std::chrono::high_resolution_clock::now();
    for(auto it = futures.begin(); it != futures.end(); it++){ 
        it->get();   
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); 
    std::cout << "Time taken by waiting to finsish: "  <<  duration.count() << " microseconds" << std::endl;
    std::cout << "--------------------------------------------------------------------------------------------------------------------" << std::endl;
}

void for_each_random_time(Graph_Rel_Iterator g_it, FunctionTypeRel f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it,f, i](){
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
            std::cout << "Process " << g_it.get_thread_limits()[i] << " started: " << std::chrono::time_point_cast<std::chrono::microseconds>(start_timer).time_since_epoch().count() << "  ";
            std::cout << "Process " << g_it.get_thread_limits()[i] << " ended: " << std::chrono::time_point_cast<std::chrono::microseconds>(end_timer).time_since_epoch().count();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_timer - start_timer); 
            std::cout << "Time taken by Process "  << g_it.get_thread_limits()[i] << ": "<< duration.count() << " microseconds" << std::endl;
            return true;
        }));
    }
    auto start = std::chrono::high_resolution_clock::now();
    for(auto it = futures.begin(); it != futures.end(); it++){
        it->get();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); 
    std::cout << "Time taken by waiting to finsish: "  <<  duration.count() << " microseconds" << std::endl;
    std::cout << "--------------------------------------------------------------------------------------------------------------------" << std::endl;
}