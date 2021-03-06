#include "graph.hpp"
#include <primesieve.hpp>

#include <future>
#include "thread_pool.hpp"
#include <random>

#ifndef GRAPH_REL_ITER_HPP
#define GRAPH_REL_ITER_HPP

/*
    class to use with for_each and for_each_random function.
*/
class Graph_Rel_Iterator{
    std::vector<std::unique_ptr<Relationship>>::iterator begin;
    std::vector<std::unique_ptr<Relationship>>::iterator end;

    /*
        contains the range of relationships each thread gets
    */
    std::vector<size_t> thread_limits;

    /*
        contains prime_numbers for random acess.
    */
    std::vector<u_int64_t> prime_numbers;

    public:
        Graph_Rel_Iterator(std::vector<std::unique_ptr<Relationship>>::iterator begin_input, std::vector<std::unique_ptr<Relationship>>::iterator end_input, size_t thread_count = std::thread::hardware_concurrency());

        std::vector<std::unique_ptr<Relationship>>::iterator get_begin();

        std::vector<std::unique_ptr<Relationship>>::iterator get_end();

        const std::vector<size_t>& get_thread_limits();

        const std::vector<u_int64_t>& get_prime_numbers();
};

using FunctionTypeRel = std::function<void(Relationship*)>;

/*
    use multithreading to apply the given function to all Relationships between the start and end iterator.
*/
void for_each(Graph_Rel_Iterator g_it, FunctionTypeRel f);

/*
    use multithreading to apply the given function to all Relationships between the start and end iterator. The order in which the elemnts are iterated over is random
*/
void for_each_random(Graph_Rel_Iterator g_it, FunctionTypeRel f);

#endif