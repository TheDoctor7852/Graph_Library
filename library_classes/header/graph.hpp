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
    /*
        stores the Nodes
    */
    std::vector<std::unique_ptr<Node>> node_vec;

    /*
        stores the Relationships
    */
    std::vector<std::unique_ptr<Relationship>> rel_vec;


    std::shared_mutex write_nodes;
    std::shared_mutex write_rel;

    //void add_incomming_relationship(Node* n, Relationship* r);

    //void add_outgoing_relationship(Node* n, Relationship* r);

    public:
        Graph() = default;
        //Graph(graph_db_ptr& graph, std::function<void()> f_nodes, std::function<std::vector<node::id_t>(std::vector<std::unique_ptr<Node>>::iterator)> f_rels, size_t thread_count = std::thread::hardware_concurrency());
        ~Graph();

        /*
            use multithreading to overwrite the existing Relationships. The Function is given the currently viewed Node and is meant to return the ids of all nodes a Relationship should be formed to.
            TODO: dont read the nodes off of the position, make an Array like in other examples with pointers and graph->nodas_as_vec.capacity
        */
        void initialise_relationships(graph_db_ptr& graph, std::function<std::vector<node::id_t>(std::vector<std::unique_ptr<Node>>::iterator)> f_rels, size_t thread_count = std::thread::hardware_concurrency());

        /*
            adds the Node with the given id
        */
        Node* add_node(node::id_t input);

        /*
            adds the Relationship between the given nodes.
            TODO: change the way the id is generated or given by the user
        */
        Relationship* add_relationship(std::vector<std::unique_ptr<Node>>::iterator from_node, std::vector<std::unique_ptr<Node>>::iterator to_node);

        /*
            adds the Relationship between the given nodes.
        */
        Relationship* add_relationship(Node* from_node, Node* to_node);

        /*
            deletes the given node. Note that all Relationships leaving and pointing to this node must be deleted before
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
};

#endif