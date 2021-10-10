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
void test_insert_multiple_nodes_and_rel(){
    auto pool = graph_pool::open("./graph/PageRank_example_Test");
    auto graph = pool->open_graph("PageRank_example_Test");

    Graph g(graph);

    Node* n_1 = g.add_node(0);
    Node* n_2 = g.add_node(0);
    Node* n_3 = g.add_node(1);
    Node* n_4 = g.add_node(2);
    g.delete_node(g.get_node_iterator_begin());
    Node* n_5 = g.add_node(0);

    if(n_1 != nullptr) std::cout << "true" << std::endl;
    if(n_2 == nullptr) std::cout << "true" << std::endl;
    if(n_3 != nullptr) std::cout << "true" << std::endl;
    if(n_4 != nullptr) std::cout << "true" << std::endl;
    if(n_5 != nullptr) std::cout << "true" << std::endl;

    for(auto it = g.get_node_iterator_begin(); it != g.get_node_iterator_end(); it++){std::cout << (*it)->get_id() << std::endl;}

    Relationship* r_1 = g.add_relationship(0);
    Relationship* r_2 = g.add_relationship(0);
    Relationship* r_3 = g.add_relationship(1);
    Relationship* r_4 = g.add_relationship(2);
    g.delete_rel(g.get_rel_iterator_begin());
    Relationship* r_5 = g.add_relationship(0);

    if(r_1 != nullptr) std::cout << "true" << std::endl;
    if(r_2 == nullptr) std::cout << "true" << std::endl;
    if(r_3 != nullptr) std::cout << "true" << std::endl;
    if(r_4 != nullptr) std::cout << "true" << std::endl;
    if(r_5 != nullptr) std::cout << "true" << std::endl;

    for(auto it = g.get_rel_iterator_begin(); it != g.get_rel_iterator_end(); it++){std::cout << (*it)->get_id() << std::endl;}


}

void test_smaller_iterators(){
    auto pool = graph_pool::open("./graph/PageRank_example_Test");
    auto graph = pool->open_graph("PageRank_example_Test");

    Graph g(graph);

    graph->begin_transaction();

    graph->nodes([&g](node& n){
        g.add_node(n.id());
    });

    graph->commit_transaction();

    Graph_Node_Iterator it(g.get_node_iterator_begin(), g.get_node_iterator_begin()+3);

    auto limits = it.get_thread_limits();
    auto primes = it.get_prime_numbers();

    for(int i=0; i<limits.size(); i++){
        std::cout << limits[i] << std::endl; 
    }
    std::cout << "<------------------------------------------------------------------->" << std::endl;
    for(int i=0; i<primes.size(); i++){
        std::cout << primes[i] << std::endl; 
    }

}

void test_change_prop(){
    auto pool = graph_pool::open("./graph/PageRank_example_Test");
    auto graph = pool->open_graph("PageRank_example_Test");

    Graph g(graph);

    graph->begin_transaction();

    graph->nodes([&g](node& n){
        auto node = g.add_node(n.id());
        node->add_property("number", n.id());
    });

    graph->commit_transaction();

    Graph_Node_Iterator it(g.get_node_iterator_begin(), g.get_node_iterator_end());

    for(auto i = it.get_begin(); i!= it.get_end(); i++){
        std::cout << (*i)->get_id() << " hat number: " << (*i)->read_property("number") << std::endl;
    }

    std::cout << "-------------------------------------------------------" << std::endl;

    for_each(it, [](Node* n){
        size_t new_v = n->get_id() +1;
        n->change_property("number", new_v);
    });

    for(auto i = it.get_begin(); i!= it.get_end(); i++){
        std::cout << (*i)->get_id() << " hat number: " << (*i)->read_property("number") << std::endl;
    }

    std::cout << "-------------------------------------------------------" << std::endl;

    for(auto i = it.get_begin(); i!= it.get_end(); i++){
        std::cout << (*i)->get_id() << " hat number: " << (*i)->read_property("number") << std::endl;
    }
}

int main(){
    //test_node_insert_and_index();
    //test_rel_insert_and_index();
    //test_insert_multiple_nodes_and_rel();
    //test_smaller_iterators();
    test_change_prop();
}