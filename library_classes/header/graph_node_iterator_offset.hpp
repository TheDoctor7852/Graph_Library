#include "graph.hpp"
#include <primesieve.hpp>

#include <future>
#include "thread_pool.hpp"
#include <random>

#include<omp.h>

#ifndef GRAPH_NODE_ITER_OFFSET_HPP
#define GRAPH_NODE_ITER_OFFSET_HPP

/*
    class to use with for_each and for_each_random function.
*/
class Graph_Node_Iterator_Offset{
    std::vector<std::unique_ptr<Node>>* vec;
    size_t begin;
    size_t end;

    /*
        contains the range of nodes each thread gets
    */
    std::vector<size_t> thread_limits;
    
    /*
        contains prime_numbers for random acess.
    */
    std::vector<u_int64_t> prime_numbers;

    public:
        Graph_Node_Iterator_Offset(std::vector<std::unique_ptr<Node>>& vec_input, size_t begin_input, size_t end_input, size_t thread_count = std::thread::hardware_concurrency());

        size_t get_begin();

        size_t get_end();

        const std::vector<size_t>& get_thread_limits();

        const std::vector<u_int64_t>& get_prime_numbers();

        std::vector<std::unique_ptr<Node>>& get_vec();
};

using FunctionTypeNode = std::function<void(Node*)>;

/*
    use multithreading to apply the given function to all Relationships between the start and end iterator.
*/
void for_each(Graph_Node_Iterator_Offset g_it, FunctionTypeNode f);

/*
    use multithreading to apply the given function to all Relationships between the start and end iterator. The order in which the elemnts are iterated over is random
*/
void for_each_random(Graph_Node_Iterator_Offset g_it, FunctionTypeNode f);


void for_each_openmp(Graph_Node_Iterator_Offset g_it, FunctionTypeNode f);

void for_each_random_openmp(Graph_Node_Iterator_Offset g_it, FunctionTypeNode f);

#endif