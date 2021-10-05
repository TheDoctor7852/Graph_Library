#include "../header/graph.hpp"

#include <future>
#include "thread_pool.hpp"

Graph::Graph(graph_db_ptr& g): graph(&g){}

Node* Graph::add_node(node::id_t input){
    std::unique_lock<std::shared_mutex> lock(write_nodes);
    node_vec.emplace_back(std::make_unique<Node>(Node(input))); 
    node_index.insert(std::make_pair(input,node_vec.back().get()));
    return node_vec.back().get();
}

Relationship* Graph::add_relationship(relationship::id_t input){
    std::unique_lock<std::shared_mutex> lock_rel(write_rel);
    rel_vec.emplace_back(std::make_unique<Relationship>(Relationship((input)))); 
    rel_index.insert(std::make_pair(input,rel_vec.back().get()));
    return rel_vec.back().get();
}


void Graph::delete_node(std::vector<std::unique_ptr<Node>>::iterator it){
    std::unique_lock<std::shared_mutex> lock(write_nodes);
    node_index.erase((*it)->get_id());
        node_vec.erase(it);
}

void Graph::delete_rel(std::vector<std::unique_ptr<Relationship>>::iterator it){
    std::unique_lock<std::shared_mutex> lock(write_rel);
    rel_index.erase((*it)->get_id());
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

Node* Graph::get_node(node::id_t input){
    std::shared_lock<std::shared_mutex> lock(write_nodes);
    auto pos = node_index.find(input);
    if(pos != node_index.end()){
        return pos->second;
    }else{
        std::cout << "key Not found" << std::endl;
        return nullptr;
    }
}

Relationship* Graph::get_rel(relationship::id_t input){
    std::shared_lock<std::shared_mutex> lock(write_rel);
    auto pos = rel_index.find(input);
    if(pos != rel_index.end()){
        return pos->second;
    }else{
        std::cout << "key Not found" << std::endl;
        return nullptr;
    }
        
}

graph_db_ptr& Graph::get_graph(){
    return *graph;
}