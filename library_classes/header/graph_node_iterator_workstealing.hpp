#include "graph.hpp"
#include <primesieve.hpp>

#include <future>
#include "thread_pool.hpp"
#include <random>

#include "wsq.hpp"

#ifndef GRAPH_NODE_ITER_WORKSTEAL_HPP
#define GRAPH_NODE_ITER_WORKSTEAL_HPP

/*
    class to use with for_each and for_each_random function.
*/

class Graph_Node_Iterator_Timed_Workstealing{
    std::vector<std::unique_ptr<Node>>::iterator begin;
    std::vector<std::unique_ptr<Node>>::iterator end;

    /*
        contains the range of nodes each thread gets
    */
    std::vector<size_t> thread_limits;
    
    /*
        contains prime_numbers for random acess.
    */
    std::vector<u_int64_t> prime_numbers;

    std::vector<std::vector<std::pair<std::chrono::_V2::system_clock::time_point, std::chrono::_V2::system_clock::time_point>>> times_vec;

    std::vector<WorkStealingQueue<size_t>> queue_vec; 

    public:
        Graph_Node_Iterator_Timed_Workstealing(std::vector<std::unique_ptr<Node>>::iterator begin_input, std::vector<std::unique_ptr<Node>>::iterator end_input, size_t thread_count = std::thread::hardware_concurrency());

        std::vector<std::unique_ptr<Node>>::iterator get_begin();

        std::vector<std::unique_ptr<Node>>::iterator get_end();

        const std::vector<size_t>& get_thread_limits();

        const std::vector<u_int64_t>& get_prime_numbers();

        std::vector<std::vector<std::pair<std::chrono::_V2::system_clock::time_point, std::chrono::_V2::system_clock::time_point>>>& get_times_vec();

        std::vector<WorkStealingQueue<size_t>>& get_queue();
};

using FunctionTypeNode = std::function<void(Node*)>;

void for_each_workstealing(Graph_Node_Iterator_Timed_Workstealing& g_it, FunctionTypeNode f);

void for_each_time_workstealing(Graph_Node_Iterator_Timed_Workstealing& g_it, FunctionTypeNode f);

void for_each_random_workstealing(Graph_Node_Iterator_Timed_Workstealing& g_it, FunctionTypeNode f);

void for_each_random_time_workstealing(Graph_Node_Iterator_Timed_Workstealing& g_it, FunctionTypeNode f);

#endif