#include "../header/graph.hpp"

#include <future>
#include "thread_pool.hpp"

Graph::~Graph(){
    //rel_vec.clear();
    //node_vec.clear();
}
/*
//statt das in constructor zu machen, könnte stadessen auch eine extra function für relationships machen -> sowas wie init_rel_ships -> sollte lieber erstmal diese funktion machen und zum laufen bekommen
Graph::Graph(graph_db_ptr& graph, std::function<void()> f_nodes, std::function<std::vector<node::id_t>(std::vector<std::unique_ptr<Node>>::iterator)> f_rels, size_t thread_count){
    auto tx = graph->begin_transaction();
        f_nodes();
    graph->commit_transaction();

    std::vector<bool> futures = {};
    thread_pool pool = thread_pool(thread_count);
    std::vector<size_t> thread_startAndEnd = {0};

    for(size_t i=0; i<thread_count; i++){
      if(i<(node_vec.size() % thread_count)){
        thread_startAndEnd.push_back(thread_startAndEnd[i]+1+(node_vec.size()/thread_count));
      } else{
        thread_startAndEnd.push_back(thread_startAndEnd[i]+(node_vec.size()/thread_count));
      }
    }
    
    auto it = node_vec.begin();

    for(size_t i=0;i<thread_startAndEnd.size()-1;i++){
          std::future<bool> f = pool.submit([it, i, &graph, &thread_startAndEnd, &f_rels, this](){
              auto it_start = it + thread_startAndEnd[i];
              auto it_end = it + thread_startAndEnd[i+1];
              graph->begin_transaction();
              std::vector<std::unique_ptr<Relationship>> adding_neighbours;
              std::vector<node::id_t> ret = {};
                for(auto start = it_start; start != it_end; start++){
                    ret = f_rels(start);
                    for(size_t i= 0; i<ret.size(); i++){
                        adding_neighbours.emplace_back(std::make_unique<Relationship>(Relationship(0, start->get(), node_vec[ret[i]].get())));
                    }
                }
                graph->commit_transaction();
                std::unique_lock<std::shared_mutex> lock(write_rel);
                rel_vec.insert(rel_vec.end(), std::make_move_iterator(adding_neighbours.begin()), std::make_move_iterator(adding_neighbours.end()));
              return true;
          });
    }
}
*/

void Graph::initialise_relationships(graph_db_ptr& graph, std::function<std::vector<node::id_t>(std::vector<std::unique_ptr<Node>>::iterator)> f_rels, size_t thread_count){
    std::vector<std::future<bool>> futures = {};
    thread_pool pool = thread_pool(thread_count);
    std::vector<size_t> thread_startAndEnd = {0};

    for(size_t i=0; i<thread_count; i++){
      if(i<(node_vec.size() % thread_count)){
        thread_startAndEnd.push_back(thread_startAndEnd[i]+1+(node_vec.size()/thread_count));
      } else{
        thread_startAndEnd.push_back(thread_startAndEnd[i]+(node_vec.size()/thread_count));
      }
    }

    std::shared_lock<std::shared_mutex> l(write_nodes);

    auto it = node_vec.begin();

    for(size_t i=0;i<thread_startAndEnd.size()-1;i++){
          std::future<bool> f = pool.submit([it, i, &graph, &thread_startAndEnd, &f_rels, this](){
              auto it_start = it + thread_startAndEnd[i];
              auto it_end = it + thread_startAndEnd[i+1];
              graph->begin_transaction();
              std::vector<std::unique_ptr<Relationship>> adding_neighbours;
              std::vector<node::id_t> ret = {};
                for(auto start = it_start; start != it_end; start++){
                    ret = f_rels(start);
                    for(size_t i= 0; i<ret.size(); i++){
                        auto h = adding_neighbours.emplace_back(std::make_unique<Relationship>(Relationship(0, start->get(), node_vec[ret[i]].get()))).get();
                        h->get_from_node()->add_outgoing_rel(h);
                        h->get_to_node()->add_incomming_rel(h);
                    }
                }
                graph->commit_transaction();
                std::unique_lock<std::shared_mutex> lock(write_rel);
                rel_vec.insert(rel_vec.end(), std::make_move_iterator(adding_neighbours.begin()), std::make_move_iterator(adding_neighbours.end()));
              return true;
          });
        futures.push_back(std::move(f));
    }

    for(size_t i = 0; i<futures.size(); i++){
        futures[i].get();
    }
}

Node* Graph::add_node(node::id_t input){
    std::unique_lock<std::shared_mutex> lock(write_nodes);
    node_vec.emplace_back(std::make_unique<Node>(Node(input)));
    return node_vec.back().get();
}

Relationship* Graph::add_relationship(std::vector<std::unique_ptr<Node>>::iterator from_node, std::vector<std::unique_ptr<Node>>::iterator to_node){
    std::shared_lock<std::shared_mutex> lock_node(write_nodes);
    std::unique_lock<std::shared_mutex> lock_rel(write_rel);
    try{
        //if(rel_id_q.empty()){
        auto h = rel_vec.emplace_back(std::make_unique<Relationship>(Relationship(rel_vec.size(), from_node->get(), to_node->get()))).get();
        //lock_rel.unlock();
        (*from_node)->add_outgoing_rel(h);
        (*to_node)->add_incomming_rel(h);
        return h;
        /*}else{
            size_t id = rel_id_q.top();
            rel_id_q.pop();
            rel_vec[id] =  std::move(std::make_unique<Relationship>(Relationship(id, from_node->get(), to_node->get())));
            //lock_rel.unlock();
            (*from_node)->add_outgoing_rel(rel_vec.back().get());
            (*to_node)->add_incomming_rel(rel_vec.back().get());
            return rel_vec[id].get();
        }   */
    }catch(...){
        return nullptr;
    }
}

Relationship* Graph::add_relationship(Node* from_node, Node* to_node){
    std::shared_lock<std::shared_mutex> lock_node(write_nodes);
    std::unique_lock<std::shared_mutex> lock_rel(write_rel);
        //if(rel_id_q.empty()){
        auto h = rel_vec.emplace_back(std::make_unique<Relationship>(Relationship(rel_vec.size(), from_node, to_node))).get();
        //lock_rel.unlock();
        from_node->add_outgoing_rel(h);
        to_node->add_incomming_rel(h);
        return h;
        /*}else{
            size_t id = rel_id_q.top();
            rel_id_q.pop();
            rel_vec[id] =  std::move(std::make_unique<Relationship>(Relationship(id, from_node, to_node)));
            //lock_rel.unlock();
            from_node->add_outgoing_rel(rel_vec.back().get());
            to_node->add_incomming_rel(rel_vec.back().get());
            return rel_vec[id].get();
        }   */
}

void Graph::delete_node(std::vector<std::unique_ptr<Node>>::iterator it){
    std::unique_lock<std::shared_mutex> lock(write_nodes);
    if((*it)->get_incomming_rel().empty() && (*it)->get_outgoing_rel().empty()){
        node_vec.erase(it);
    }
}

void Graph::delete_rel(std::vector<std::unique_ptr<Relationship>>::iterator it){
    std::unique_lock<std::shared_mutex> lock(write_rel);
    (*it)->get_from_node()->remove_outgoing_rel(it->get());
    (*it)->get_to_node()->remove_incomming_rel(it->get());
    //rel_id_q.push((*it)->get_id());
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
/*
void Graph::add_incomming_relationship(Node* n, Relationship* r){
    n->add_incomming_rel(r);
}

void Graph::add_outgoing_relationship(Node* n, Relationship* r){
    n->add_outgoing_rel(r);
}*/