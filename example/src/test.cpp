#include "graph.hpp"
#include "graph_node_iterator.hpp"
#include "graph_relationship_iterator.hpp"

#include <limits>

void test_node_insert_and_index(){
    auto pool = graph_pool::open("./graph/PageRank_example_Test");
    auto graph = pool->open_graph("PageRank_example_Test");

    graph->begin_transaction();

    Graph g(graph);
    graph->nodes([&](node& n){
        g.add_node(n.id());
    });

    graph->commit_transaction();

    for(auto it = g.get_node_iterator_begin(); it != g.get_node_iterator_end(); it++){std::cout << (*it)->get_id() << std::endl;}

    std::cout << g.get_node(0)->get_id() << std::endl;
    std::cout << g.get_node(7)->get_id() << std::endl;
    std::cout << g.get_node(4)->get_id() << std::endl;
    std::cout << (g.get_node(9)==nullptr) << std::endl;

    auto it_begin = g.get_node_iterator_begin();

    g.delete_node(it_begin+4);

    for(auto it = g.get_node_iterator_begin(); it != g.get_node_iterator_end(); it++){std::cout << (*it)->get_id() << std::endl;}

    std::cout << g.get_node(0)->get_id() << std::endl;
    std::cout << g.get_node(7)->get_id() << std::endl;
    std::cout << (g.get_node(4)==nullptr) << std::endl;
    std::cout << (g.get_node(9)==nullptr) << std::endl;
}

void test_rel_insert_and_index(){
    auto pool = graph_pool::open("./graph/PageRank_example_Test");
    auto graph = pool->open_graph("PageRank_example_Test");

    Graph g(graph);

    for(size_t i=0; i<9; i++){g.add_relationship(i);}

    for(auto it = g.get_rel_iterator_begin(); it != g.get_rel_iterator_end(); it++){std::cout << (*it)->get_id() << std::endl;}

    std::cout << g.get_rel(0)->get_id() << std::endl;
    std::cout << g.get_rel(7)->get_id() << std::endl;
    std::cout << g.get_rel(4)->get_id() << std::endl;
    std::cout << (g.get_rel(9)==nullptr) << std::endl;

    auto it_begin = g.get_rel_iterator_begin();

    g.delete_rel(it_begin+4);

    for(auto it = g.get_rel_iterator_begin(); it != g.get_rel_iterator_end(); it++){std::cout << (*it)->get_id() << std::endl;}

    std::cout << g.get_rel(0)->get_id() << std::endl;
    std::cout << g.get_rel(7)->get_id() << std::endl;
    std::cout << (g.get_rel(4)==nullptr) << std::endl;
    std::cout << (g.get_rel(9)==nullptr) << std::endl;
}

int main(){
    test_node_insert_and_index();
    //test_rel_insert_and_index();
}