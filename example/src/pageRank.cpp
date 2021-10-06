#include "graph.hpp"
#include "graph_node_iterator.hpp"
#include "graph_relationship_iterator.hpp"

#include <limits>

void init_nodes_in_graph(graph_db_ptr &graph, Graph &g)
{
    graph->begin_transaction();
    graph->nodes([&g](auto &n)
                 { g.add_node(n.id()); });
    graph->commit_transaction();
}

void init_ranks(Graph &g)
{
    Graph_Node_Iterator iter_node(g.get_node_iterator_begin(), g.get_node_iterator_end());
    
    g.get_graph()->begin_transaction();
    for_each(iter_node, [&g](Node *n)
             {
                 n->add_property("rank", 1.0);
                 n->add_property("new_rank", 1.0);
                 size_t count = 0;

                 g.get_graph()->foreach_from_relationship_of_node(g.get_graph()->node_by_id(n->get_id()), [&count](relationship &r)
                                                                  { count++; });
                 
        if(count == 0){
            std::cout << "here" << std::endl;
            g.get_graph()->nodes([&g,&n, &count](node& node){
                if(n->get_id() != node.id()){
                    g.get_graph()->add_relationship(n->get_id(), node.id(), "label_prop",{});
                    count++;
                }
            });
        }
        
                 n->add_property("neighbour_count", count);
             });
    g.get_graph()->commit_transaction();
}

void init_ranks_omp(Graph &g)
{
    Graph_Node_Iterator iter_node(g.get_node_iterator_begin(), g.get_node_iterator_end());
    
    g.get_graph()->begin_transaction();
    for_each_openmp(iter_node, [&g](Node *n)
             {
                 n->add_property("rank", 1.0);
                 n->add_property("new_rank", 1.0);
                 size_t count = 0;

                 g.get_graph()->foreach_from_relationship_of_node(g.get_graph()->node_by_id(n->get_id()), [&count](relationship &r)
                                                                  { count++; });
                 
        if(count == 0){
            std::cout << "here" << std::endl;
            g.get_graph()->nodes([&g,&n, &count](node& node){
                if(n->get_id() != node.id()){
                    g.get_graph()->add_relationship(n->get_id(), node.id(), "label_prop",{});
                    count++;
                }
            });
        }
        
                 n->add_property("neighbour_count", count);
             });
    g.get_graph()->commit_transaction();
}

void init_ranks_serial(Graph &g)
{
    g.get_graph()->begin_transaction();
    auto it = g.get_node_iterator_begin();
    g.get_graph()->nodes([&it, &g](node &n)
                         {
                             (*it)->add_property("rank", 1.0);
                             (*it)->add_property("new_rank", 1.0);
                             size_t count = 0;

                             g.get_graph()->foreach_from_relationship_of_node(g.get_graph()->node_by_id((*it)->get_id()), [&count](relationship &r)
                                                                              { count++; });
                             if (count == 0)
                             {
                                 std::cout << "here" << std::endl;
                                 g.get_graph()->nodes([&g, &it, &count](node &node)
                                                      {
                                                          if ((*it)->get_id() != node.id())
                                                          {
                                                              g.get_graph()->add_relationship((*it)->get_id(), node.id(), "label_prop", {});
                                                              count++;
                                                          }
                                                      });
                             }
                             (*it)->add_property("neighbour_count", count);
                             it++;
                         });
    g.get_graph()->commit_transaction();
}

void pageRank(Graph &g)
{
    Graph_Node_Iterator iter_node(g.get_node_iterator_begin(), g.get_node_iterator_end());
    Graph_Rel_Iterator iter_rel(g.get_rel_iterator_begin(), g.get_rel_iterator_end());

    bool is_smaller = false;
    double diff_limit = 0.001;

    size_t turns = 0;
    size_t max_turns = 100;
    while (!is_smaller && turns < max_turns)
    {
        is_smaller = true;
 
        g.get_graph()->begin_transaction();
        for_each(iter_node, [&is_smaller, &diff_limit, &g](Node *n)
                 {
                     double sum = 0;
                     //auto iter = g.get_node_iterator_begin();

                     g.get_graph()->foreach_to_relationship_of_node(g.get_graph()->node_by_id(n->get_id()), [&g, &sum](relationship &r)
                                                                    {
                                                                        auto neighbour = g.get_node(r.from_node_id());
                                                                        sum = sum + boost::any_cast<double>(neighbour->read_property("rank")) / boost::any_cast<size_t>(neighbour->read_property("neighbour_count"));
                                                                    });
                     double diff_before = boost::any_cast<double>(n->read_property("rank"));
                     n->change_property("new_rank", [&sum](boost::any &p)
                                        { p = 0.15 + 0.85 * sum; });
                     double diff_after = boost::any_cast<double>(n->read_property("new_rank"));
                     if (abs(diff_before - diff_after) > diff_limit)
                     {
                         is_smaller = false;
                     }
                 });

        for_each(iter_node, [](Node *n)
                 {
                     double d = boost::any_cast<double>(n->read_property("new_rank"));
                     n->change_property("rank", [&d](boost::any &p)
                                        { p = d; });
                 });

        g.get_graph()->commit_transaction();

        turns++;
    }
    std::cout << "Turns taken: " << turns << std::endl;
}

void pageRank_omp(Graph &g)
{
    Graph_Node_Iterator iter_node(g.get_node_iterator_begin(), g.get_node_iterator_end());
    Graph_Rel_Iterator iter_rel(g.get_rel_iterator_begin(), g.get_rel_iterator_end());

    bool is_smaller = false;
    double diff_limit = 0.001;

    size_t turns = 0;
    size_t max_turns = 100;
    while (!is_smaller && turns < max_turns)
    {
        is_smaller = true;
 
        g.get_graph()->begin_transaction();
        for_each_openmp(iter_node, [&is_smaller, &diff_limit, &g](Node *n)
                 {
                     double sum = 0;
                     //auto iter = g.get_node_iterator_begin();

                     g.get_graph()->foreach_to_relationship_of_node(g.get_graph()->node_by_id(n->get_id()), [&g, &sum](relationship &r)
                                                                    {
                                                                        auto neighbour = g.get_node(r.from_node_id());
                                                                        sum = sum + boost::any_cast<double>(neighbour->read_property("rank")) / boost::any_cast<size_t>(neighbour->read_property("neighbour_count"));
                                                                    });
                     double diff_before = boost::any_cast<double>(n->read_property("rank"));
                     n->change_property("new_rank", [&sum](boost::any &p)
                                        { p = 0.15 + 0.85 * sum; });
                     double diff_after = boost::any_cast<double>(n->read_property("new_rank"));
                     if (abs(diff_before - diff_after) > diff_limit)
                     {
                         is_smaller = false;
                     }
                 });

        for_each_openmp(iter_node, [](Node *n)
                 {
                     double d = boost::any_cast<double>(n->read_property("new_rank"));
                     n->change_property("rank", [&d](boost::any &p)
                                        { p = d; });
                 });

        g.get_graph()->commit_transaction();

        turns++;
    }
    std::cout << "Turns taken: " << turns << std::endl;
}

void pageRank_serial(Graph &g)
{
    Graph_Node_Iterator iter_node(g.get_node_iterator_begin(), g.get_node_iterator_end());
    Graph_Rel_Iterator iter_rel(g.get_rel_iterator_begin(), g.get_rel_iterator_end());

    bool is_smaller = false;
    double diff_limit = 0.001;

    size_t turns = 0;
    size_t max_turns = 100;

    while (!is_smaller && turns < max_turns)
    {
        is_smaller = true;
        g.get_graph()->begin_transaction();
        for (auto n = g.get_node_iterator_begin(); n != g.get_node_iterator_end(); n++)
        {
            double sum = 0;
            //auto iter = g.get_node_iterator_begin();

            g.get_graph()->foreach_to_relationship_of_node(g.get_graph()->node_by_id((*n)->get_id()), [&g,&sum](relationship &r)
                                                           {
                                                               auto neighbour = g.get_node(r.from_node_id());
                                                               sum = sum + boost::any_cast<double>(neighbour->read_property("rank")) / boost::any_cast<size_t>(neighbour->read_property("neighbour_count"));
                                                           });

            double diff_before = boost::any_cast<double>((*n)->read_property("rank"));
            (*n)->change_property("new_rank", [&sum](boost::any &p)
                                  { p = 0.15 + 0.85 * sum; });
            double diff_after = boost::any_cast<double>((*n)->read_property("new_rank"));
            if (abs(diff_before - diff_after) > diff_limit)
            {
                is_smaller = false;
            }
        }
        for (auto n = g.get_node_iterator_begin(); n != g.get_node_iterator_end(); n++)
        {
            double d = boost::any_cast<double>((*n)->read_property("new_rank"));
            (*n)->change_property("rank", [&d](boost::any &p)
                                  { p = d; });
        }
        g.get_graph()->commit_transaction();
        turns++;
    }
    std::cout << "Turns taken: " << turns << std::endl;
}

void cleanup(graph_db_ptr& graph){
    graph->begin_transaction();

    //cleanup
    graph->relationships_by_label("label_prop", [&graph](relationship &r)
                                  {
                                      //std::cout << "here" << std::endl;
                                      graph->delete_relationship(r.id());
                                  });
    graph->commit_transaction();

}

int main()
{

    std::string path_Graphs = "../../../big_graphs/graph/";

    auto pool = graph_pool::open("./graph/PageRank_example_Test");
    auto graph = pool->open_graph("PageRank_example_Test");

    //auto pool = graph_pool::open(path_Graphs+"5000nodeGraph");
    //auto graph = pool->open_graph("5000nodeGraph");

    //auto pool = graph_pool::open(path_Graphs+"10000nodeGraph");
    //auto graph = pool->open_graph("10000nodeGraph");

    //auto pool = graph_pool::open(path_Graphs + "20000nodeGraph");
    //auto graph = pool->open_graph("20000nodeGraph");

    //auto pool = graph_pool::open(path_Graphs+"30000nodeGraph");
    //auto graph = pool->open_graph("30000nodeGraph");

    //auto pool = graph_pool::open(path_Graphs+"40000nodeGraph");
    //auto graph = pool->open_graph("40000nodeGraph");

    //auto pool = graph_pool::open(path_Graphs+"50000nodeGraph");
    //auto graph = pool->open_graph("50000nodeGraph");

    Graph g(graph);

    auto start_all = std::chrono::high_resolution_clock::now();

    auto start_init = std::chrono::high_resolution_clock::now();

    init_nodes_in_graph(graph, g);

    auto stop_init = std::chrono::high_resolution_clock::now();

    auto duration_init = std::chrono::duration_cast<std::chrono::microseconds>(stop_init - start_init);

    std::cout << "Time taken by init: "
              << duration_init.count() << " microseconds" << std::endl;

    auto start_rel = std::chrono::high_resolution_clock::now();

    //init_rel_with_function(graph, g);
    //init_rel_serial(graph, g);

    auto stop_rel = std::chrono::high_resolution_clock::now();

    auto duration_rel = std::chrono::duration_cast<std::chrono::microseconds>(stop_rel - start_rel);

    std::cout << "Time taken by rel: "
              << duration_rel.count() << " microseconds" << std::endl;

    auto start_init_label = std::chrono::high_resolution_clock::now();

    init_ranks(g); // auf Knoten und Kanten ausführen, sodass alle die Eigenschaft haben -> auch ausgehende Kanten zählen und dazuschreiben
    //init_ranks_serial(g);
    //init_ranks_omp(g);

    auto stop_init_label = std::chrono::high_resolution_clock::now();

    auto duration_init_label = std::chrono::duration_cast<std::chrono::microseconds>(stop_init_label - start_init_label);

    std::cout << "Time taken by init_rank: "
              << duration_init_label.count() << " microseconds" << std::endl;

    auto start_label_prop = std::chrono::high_resolution_clock::now();

    //pageRank(g);
    //pageRank_serial(g);
    pageRank_omp(g);

    auto stop_label_prop = std::chrono::high_resolution_clock::now();

    auto stop_all = std::chrono::high_resolution_clock::now();

    auto duration_label_prop = std::chrono::duration_cast<std::chrono::microseconds>(stop_label_prop - start_label_prop);

    std::cout << "Time taken by p-rank_prop: "
              << duration_label_prop.count() << " microseconds" << std::endl;

    auto duration_all = std::chrono::duration_cast<std::chrono::microseconds>(stop_all - start_all);

    std::cout << "Time taken by all: "
              << duration_all.count() << " microseconds" << std::endl;

    cleanup(graph);

    graph->begin_transaction();

    /*
    for(auto it=g.get_node_iterator_begin(); it != g.get_node_iterator_end();it++){
        std::cout << "Knoten " << graph->get_node_description((*it)->get_id()).properties.at("name") << " hat Label: " << (*it)->read_property("label") << std::endl;
    }
    */

    /* 
    for(auto it=g.get_node_iterator_begin(); it != g.get_node_iterator_end();it++){
        std::cout << graph->get_node_description((*it)->get_id()).properties.at("name") << "   :";
        for(int s = 0; s<(*it)->get_outgoing_rel().size(); s++){
            try{std::cout << graph->get_node_description((*it)->get_outgoing_rel()[s]->get_to_node()->get_id()).properties.at("name") << ",     ";}catch(...){std::cout << (*it)->get_outgoing_rel()[s]->get_id() << std::endl;}
        }
        std::cout << std::endl;
    }
    */

    for (auto it = g.get_node_iterator_begin(); it != g.get_node_iterator_end(); it++)
    {
        std::cout << graph->get_node_description((*it)->get_id()).properties.at("name") << " hat den Rank: " << (*it)->read_property("rank") << std::endl;
    }
    /*
    graph->relationships_by_label("label_prop", [](relationship& r){
        std::cout << r.from_node_id() << "             " << r.to_node_id() << std::endl;
    });
    graph->relationships_by_label("FOLLOWS", [&graph](relationship& r){
        std::cout << graph->get_node_description(r.from_node_id()).properties.at("name") << "             " << graph->get_node_description(r.to_node_id()).properties.at("name") << std::endl;
        //std::cout << r.from_node_id() << "             " << r.to_node_id() << std::endl;
    });
    std::cout << graph->get_node_description(2).properties.at("name") << std::endl;
    graph->foreach_from_relationship_of_node(graph->node_by_id(2),[&graph](relationship& r){
        std::cout << "Label:  " <<  graph->get_rship_description(r.id()).label << std::endl;
    });*/
    /*
    graph->nodes([&graph](node& n){
        std::cout << graph->get_node_description(n.id()).id << std::endl;
    });*/
    /*
    for(auto it = g.get_rel_iterator_begin(); it != g.get_rel_iterator_end(); it++){
        std::cout << graph->get_node_description((*it)->get_from_node()->get_id()).properties.at("name") << "  nach: " << graph->get_node_description((*it)->get_to_node()->get_id()).properties.at("name") << std::endl;
    }
*/
   graph->commit_transaction();
}