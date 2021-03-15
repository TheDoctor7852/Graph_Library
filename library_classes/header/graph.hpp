#include "node.hpp"
#include "relationship.hpp"

#include "graph_pool.hpp"

#include <queue>

#ifndef GRAPH_HPP
#define GRAPH_HPP

class Graph{
    std::vector<std::unique_ptr<Node>> node_vec;
    std::vector<std::unique_ptr<Relationship>> rel_vec;

    //std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>> rel_id_q; //ist überflüssig solange nicht benötigt, und sollte dann noch rausgenommen werden

    std::shared_mutex write_nodes;
    std::shared_mutex write_rel;

    //void add_incomming_relationship(Node* n, Relationship* r);

    //void add_outgoing_relationship(Node* n, Relationship* r);

    public:
        Graph() = default;
        //Graph(graph_db_ptr& graph, std::function<void()> f_nodes, std::function<std::vector<node::id_t>(std::vector<std::unique_ptr<Node>>::iterator)> f_rels, size_t thread_count = std::thread::hardware_concurrency());
        ~Graph();
        void initialise_relationships(graph_db_ptr& graph, std::function<std::vector<node::id_t>(std::vector<std::unique_ptr<Node>>::iterator)> f_rels, size_t thread_count = std::thread::hardware_concurrency());
        Node* add_node(node::id_t input);
        Relationship* add_relationship(std::vector<std::unique_ptr<Node>>::iterator from_node, std::vector<std::unique_ptr<Node>>::iterator to_node);
        Relationship* add_relationship(Node* from_node, Node* to_node);
        void delete_node(std::vector<std::unique_ptr<Node>>::iterator it); //um einen Knoten zu löschen, darf keine Kante mehr auf ihn zeigen oder von ihm weggehen
        void delete_rel(std::vector<std::unique_ptr<Relationship>>::iterator it);

        std::vector<std::unique_ptr<Node>>::iterator get_node_iterator_begin();
        std::vector<std::unique_ptr<Node>>::iterator get_node_iterator_end();

        std::vector<std::unique_ptr<Relationship>>::iterator get_rel_iterator_begin();
        std::vector<std::unique_ptr<Relationship>>::iterator get_rel_iterator_end();
};

#endif