#include "node.hpp"
#include "relationship.hpp"

#include "graph_pool.hpp"

#include <queue>

#ifndef GRAPH_HPP
#define GRAPH_HPP

/*
    class implementing a graph
*/
class Graph{
    graph_db_ptr* graph;
    /*
        stores the Nodes
    */
    std::vector<std::unique_ptr<Node>> node_vec;

    /*
        stores the Relationships
    */
    std::vector<std::unique_ptr<Relationship>> rel_vec;

    std::unordered_map<size_t, Node*> node_index;

    std::unordered_map<size_t, Relationship*> rel_index;


    std::shared_mutex write_nodes;
    std::shared_mutex write_rel;

    public:
        Graph(graph_db_ptr& g);

        /*
            adds the Node with the given id
        */
        Node* add_node(node::id_t input);

        /*
            adds the Relationship between the given nodes.
        */
        Relationship* add_relationship(relationship::id_t input);

        /*
            deletes the given node.
        */
        void delete_node(std::vector<std::unique_ptr<Node>>::iterator it); 

        /*
            deletes the given Relationship
        */
        void delete_rel(std::vector<std::unique_ptr<Relationship>>::iterator it);

        /*
            get an iterator to the first node
        */
        std::vector<std::unique_ptr<Node>>::iterator get_node_iterator_begin();

        /*
            get an iterator to one element behind the last node
        */
        std::vector<std::unique_ptr<Node>>::iterator get_node_iterator_end();

        /*
            get an iterator to the first relationship
        */
        std::vector<std::unique_ptr<Relationship>>::iterator get_rel_iterator_begin();

        /*
            get an iterator to one element behind the last relationship
        */
        std::vector<std::unique_ptr<Relationship>>::iterator get_rel_iterator_end();

        Node* get_node(node::id_t input);

        Relationship* get_rel(relationship::id_t input);

        graph_db_ptr& get_graph();

        std::vector<std::unique_ptr<Node>>& get_node_vec();

        std::vector<std::unique_ptr<Relationship>>& get_rel_vec();
};

#endif