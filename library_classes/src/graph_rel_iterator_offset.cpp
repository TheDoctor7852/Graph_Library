#include "graph_rel_iterator_offset.hpp"

Graph_Rel_Iterator_Offset::Graph_Rel_Iterator_Offset(std::vector<std::unique_ptr<Relationship>>& vec_input, size_t begin_input, size_t end_input, size_t thread_count){
    begin = begin_input;
    end = end_input;
    size_t distance = end-begin;

    vec = &vec_input;

    if(distance < thread_count){
        thread_count = distance;
    }

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

size_t Graph_Rel_Iterator_Offset::get_begin(){
    return begin;
}

size_t Graph_Rel_Iterator_Offset::get_end(){
    return end;
}

const std::vector<size_t>& Graph_Rel_Iterator_Offset::get_thread_limits(){
    return thread_limits;
}

const std::vector<u_int64_t>& Graph_Rel_Iterator_Offset::get_prime_numbers(){
    return prime_numbers;
}

std::vector<std::unique_ptr<Relationship>>& Graph_Rel_Iterator_Offset::get_vec(){
    return *vec;
}

void for_each(Graph_Rel_Iterator_Offset g_it, FunctionTypeRel f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    auto tx = current_transaction();
    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it,f, i, &tx](){
            current_transaction_ = tx;
            auto it_start = g_it.get_begin() + g_it.get_thread_limits()[i];
            auto it_end = g_it.get_begin() + g_it.get_thread_limits()[i+1];
       
            for(auto it = it_start; it != it_end; it++){
                f(g_it.get_vec()[it].get());
            }
          return true;
        }));
    }
    for(auto it = futures.begin(); it != futures.end(); it++){
        it->get();
    }
}

void for_each_random(Graph_Rel_Iterator_Offset g_it, FunctionTypeRel f){
    thread_pool pool;
    std::vector<std::future<bool>> futures;
    auto tx = current_transaction();
    for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
        futures.emplace_back(pool.submit([&g_it,f, i, &tx](){
            current_transaction_ = tx;
            std::random_device rd;  
            std::mt19937 rng(rd()); 
            std::uniform_int_distribution<int> distrib(0,g_it.get_prime_numbers().size()-1);
            u_int64_t current_prime = g_it.get_prime_numbers()[distrib(rng)];
            auto start = g_it.get_thread_limits()[i];
            auto end = g_it.get_thread_limits()[i+1];
            auto iter = g_it.get_begin() + g_it.get_thread_limits()[i];
            u_int64_t offset = current_prime % (end-start);
            for(auto it = start; it < end; it++){
                f(g_it.get_vec()[iter+offset].get());
                offset = (offset + current_prime) % (end-start);
            }
          return true;
        }));
    }
    for(auto it = futures.begin(); it != futures.end(); it++){
        it->get();
    }
}

void for_each_openmp(Graph_Rel_Iterator_Offset g_it, FunctionTypeRel f){
    auto tx = current_transaction();
    #pragma omp parallel for
        for(auto it=g_it.get_begin(); it!=g_it.get_end(); it++){
            current_transaction_ = tx;
            f(g_it.get_vec()[it].get());
        }
}

void for_each_random_openmp(Graph_Rel_Iterator_Offset g_it, FunctionTypeRel f){
    auto tx = current_transaction();
    #pragma omp parallel for
        for(size_t i=0;i< g_it.get_thread_limits().size()-1;i++){
            current_transaction_ = tx;
            std::random_device rd;  
            std::mt19937 rng(rd()); 
            std::uniform_int_distribution<int> distrib(0,g_it.get_prime_numbers().size()-1);
            u_int64_t current_prime = g_it.get_prime_numbers()[distrib(rng)];
            auto start = g_it.get_thread_limits()[i];
            auto end = g_it.get_thread_limits()[i+1];
            auto iter = g_it.get_begin() + g_it.get_thread_limits()[i];
            u_int64_t offset = current_prime % (end-start);
            for(auto it = start; it < end; it++){
                f(g_it.get_vec()[iter+offset].get());
                offset = (offset + current_prime) % (end-start);
            }
        }
}