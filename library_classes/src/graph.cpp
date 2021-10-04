#include "../header/graph.hpp"

#include <future>
#include "thread_pool.hpp"

Graph::Graph(graph_db_ptr& g): graph(&g){}

Node* Graph::add_node(node::id_t input){
    std::lock_guard<std::mutex> lock(write_nodes);
    node_vec.emplace_back(std::make_unique<Node>(Node(&(*graph)->node_by_id(input)))); 
    return node_vec.back().get();
}

Relationship* Graph::add_relationship(relationship::id_t input){
    std::lock_guard<std::mutex> lock_rel(write_rel);
    rel_vec.emplace_back(std::make_unique<Relationship>(Relationship(&(*graph)->rship_by_id(input)))); 
    return rel_vec.back().get();
}


void Graph::delete_node(std::vector<std::unique_ptr<Node>>::iterator it){
    std::lock_guard<std::mutex> lock(write_nodes);
        node_vec.erase(it);
}

void Graph::delete_rel(std::vector<std::unique_ptr<Relationship>>::iterator it){
    std::lock_guard<std::mutex> lock(write_rel);
    rel_vec.erase(it);
}

std::vector<std::unique_ptr<Node>>::iterator Graph::get_node_iterator_begin(){
    return node_vec.begin();
}

std::vector<std::unique_ptr<Node>>::iterator Graph::get_node_iterator_end(){
    return node_vec.end();
}

std::vector<std::unique_ptr<Relationship>>::iterator Graph::get_rel_iterator_begin(){
    return rel_vec.begin();
}

std::vector<std::unique_ptr<Relationship>>::iterator Graph::get_rel_iterator_end(){
    return rel_vec.end();
}

graph_db_ptr& Graph::get_graph(){
    return *graph;
}