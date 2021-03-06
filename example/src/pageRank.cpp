#include "graph.hpp"
#include "graph_node_iterator.hpp"
#include "graph_relationship_iterator.hpp"

#include <limits>

void init_nodes_in_graph(graph_db_ptr& graph, Graph& g){
    graph->begin_transaction();
    graph->nodes([&g](auto& n){
       g.add_node(n.id());
    });
    graph->commit_transaction();
}

void init_rel_with_function(graph_db_ptr& graph, Graph& g){

    g.initialise_relationships(graph, [&graph, &g](std::vector<std::unique_ptr<Node>>::iterator start){
                    std::vector<node::id_t> ret = {};
                    node::id_t active_node = (*start)->get_id();
                    graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
                        ret.push_back(r.to_node_id());
                     });
                    if(ret.size()==0){
                        for(auto it = g.get_node_iterator_begin(); it != g.get_node_iterator_end(); it++){
                            if(it!=start){
                               ret.push_back((*it)->get_id()); 
                            }
                        }
                    }
                    (*start)->add_property("num_out_rels", ret.size());
                    return ret;
    });
    /*Graph_Rel_Iterator iter_rel(g.get_rel_iterator_begin(), g.get_rel_iterator_end());
    for_each(iter_rel,[](Relationship* rel){
        std::cout << rel->get_from_node()->get_id() << "  nach " << rel->get_to_node()->get_id()<<std::endl;
    }); */ 
}

void init_rel_serial(graph_db_ptr& graph, Graph& g){
    size_t count_all = 0;
auto tx = graph->begin_transaction();
    for(auto start = g.get_node_iterator_begin(); start != g.get_node_iterator_end(); start++){
                    std::vector<node::id_t> ret = {};
                    size_t count = 0;
                    node::id_t active_node = (*start)->get_id();
                    graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
                        ret.push_back(r.to_node_id());
                        count++;
                     });
                    if(ret.size()==0){
                        for(auto it = g.get_node_iterator_begin(); it != g.get_node_iterator_end(); it++){
                            if(it!=start){
                               ret.push_back((*it)->get_id()); 
                            }
                        }
                    }
                    (*start)->add_property("num_out_rels", ret.size());
                    count_all += count;
                    for(size_t i = 0; i<ret.size();i++){
                        g.add_relationship(start, g.get_node_iterator_begin()+ ret[i]);
                    }
    };
    graph->abort_transaction();
    std::cout << count_all << std::endl;
    /*Graph_Rel_Iterator iter_rel(g.get_rel_iterator_begin(), g.get_rel_iterator_end());
    for_each(iter_rel,[](Relationship* rel){
        std::cout << rel->get_from_node()->get_id() << "  nach " << rel->get_to_node()->get_id()<<std::endl;
    }); */ 
}

void init_ranks(Graph& g){
    Graph_Node_Iterator iter_node(g.get_node_iterator_begin(), g.get_node_iterator_end());
    Graph_Rel_Iterator iter_rel(g.get_rel_iterator_begin(), g.get_rel_iterator_end());

    for_each(iter_rel,[](Relationship* rel){
        rel->add_property("rank", 0.0);
    });

    for_each(iter_node, [](Node* n){
        n->add_property("rank", 1.0);    
    });
}

void pageRank(Graph& g){
    Graph_Node_Iterator iter_node(g.get_node_iterator_begin(), g.get_node_iterator_end());
    Graph_Rel_Iterator iter_rel(g.get_rel_iterator_begin(), g.get_rel_iterator_end());

    bool is_smaller = false;
    double diff_limit = 0.001;

    size_t turns = 0;
    size_t max_turns = 100;

    while(!is_smaller && turns < max_turns){
        is_smaller = true;
        for_each(iter_rel,[](Relationship* rel){
            Node* n = rel->get_from_node();
            rel->change_property("rank", [&n](boost::any& p){
                p = boost::any_cast<double>(n->read_property("rank"))/boost::any_cast<size_t>(n->read_property("num_out_rels"));
                //std::cout << p << std::endl;
            }); 
        });
        for_each(iter_node, [&is_smaller, &diff_limit](Node* n){
                double sum = 0;
                const std::vector<Relationship*> vec = n->get_incomming_rel();
                for(size_t i=0; i< vec.size(); i++){ //hier hat mit zeigern nicht funktioniert, da das array als Kopie gesendet wird und somit die zeiger ins nichts zeigen solange es nicht zwischengespeichert wird
                     sum += boost::any_cast<double>(vec[i]->read_property("rank")); //da ich hier immer eine neue Kopie abrufe k??nnte das hier erheblich mehr zeit kosten
                }
                double diff_before = boost::any_cast<double>(n->read_property("rank"));
                n->change_property("rank", [&sum](boost::any& p){
                    p = 0.15+0.85*sum;
                });
                double diff_after = boost::any_cast<double>(n->read_property("rank"));
                if(abs(diff_before-diff_after)>diff_limit){
                    is_smaller = false;
                }
        });
        turns++;
    }
    std::cout << "Turns taken: " << turns << std::endl;
}

void pageRank_serial(Graph& g){
    Graph_Node_Iterator iter_node(g.get_node_iterator_begin(), g.get_node_iterator_end());
    Graph_Rel_Iterator iter_rel(g.get_rel_iterator_begin(), g.get_rel_iterator_end());

    bool is_smaller = false;
    double diff_limit = 0.001;

    size_t turns = 0;
    size_t max_turns = 100;

    while(!is_smaller && turns < max_turns){
        is_smaller = true;
        for(auto rel = g.get_rel_iterator_begin(); rel != g.get_rel_iterator_end(); rel++){
            Node* n = (*rel)->get_from_node();
            (*rel)->change_property("rank", [&n](boost::any& p){
                p = boost::any_cast<double>(n->read_property("rank"))/boost::any_cast<size_t>(n->read_property("num_out_rels"));
                //std::cout << p << std::endl;
            }); 
        }
        for(auto n = g.get_node_iterator_begin(); n != g.get_node_iterator_end(); n++){
                double sum = 0;
                const std::vector<Relationship*> vec = (*n)->get_incomming_rel();
                for(size_t i=0; i< vec.size(); i++){ //hier hat mit zeigern nicht funktioniert, da das array als Kopie gesendet wird und somit die zeiger ins nichts zeigen solange es nicht zwischengespeichert wird
                     sum += boost::any_cast<double>(vec[i]->read_property("rank")); //da ich hier immer eine neue Kopie abrufe k??nnte das hier erheblich mehr zeit kosten
                }
                double diff_before = boost::any_cast<double>((*n)->read_property("rank"));
                (*n)->change_property("rank", [&sum](boost::any& p){
                    p = 0.15+0.85*sum;
                });
                double diff_after = boost::any_cast<double>((*n)->read_property("rank"));
                if(abs(diff_before-diff_after)>diff_limit){
                    is_smaller = false;
                }
        }
        turns++;
    }
    std::cout << "Turns taken: " << turns << std::endl;
}

int main(){
    Graph g;

    std::string path_Graphs = "../../../C++_Programme/Poseidon_GraphAnalytics/test/graph/";

    //auto pool = graph_pool::open("./graph/Label_Prop_Test"); 
    //auto graph = pool->open_graph("Label_Prop_Test");

    //auto pool = graph_pool::open("./graph/PageRank_small_Test");
    //auto graph = pool->open_graph("PageRank_small_Test");

    //auto pool = graph_pool::open("./graph/PageRank_example_Test");
    //auto graph = pool->open_graph("PageRank_example_Test");

    //auto pool = graph_pool::open("../../Poseidon_GraphAnalytics/test/graph/5000nodeGraph");
    //auto graph = pool->open_graph("5000nodeGraph");

    //auto pool = graph_pool::open("../../Poseidon_GraphAnalytics/test/graph/10000nodeGraph");
    //auto graph = pool->open_graph("10000nodeGraph");

    auto pool = graph_pool::open(path_Graphs + "20000nodeGraph");
    auto graph = pool->open_graph("20000nodeGraph");

    //auto pool = graph_pool::open("../../Poseidon_GraphAnalytics/test/graph/30000nodeGraph");
    //auto graph = pool->open_graph("30000nodeGraph");

    //auto pool = graph_pool::open("../../Poseidon_GraphAnalytics/test/graph/40000nodeGraph");
    //auto graph = pool->open_graph("40000nodeGraph");

    //auto pool = graph_pool::open("../../Poseidon_GraphAnalytics/test/graph/50000nodeGraph");
    //auto graph = pool->open_graph("50000nodeGraph");

    auto start_all = std::chrono::high_resolution_clock::now();

    auto start_init = std::chrono::high_resolution_clock::now();

    init_nodes_in_graph(graph, g);

    auto stop_init = std::chrono::high_resolution_clock::now();

    auto duration_init = std::chrono::duration_cast<std::chrono::microseconds>(stop_init - start_init); 
  
    std::cout << "Time taken by init: "
      << duration_init.count() << " microseconds" << std::endl;

    auto start_rel = std::chrono::high_resolution_clock::now();

    init_rel_with_function(graph, g);
    //init_rel_serial(graph, g);

    auto stop_rel = std::chrono::high_resolution_clock::now();

    auto duration_rel = std::chrono::duration_cast<std::chrono::microseconds>(stop_rel - start_rel); 
  
    std::cout << "Time taken by rel: " 
      << duration_rel.count() << " microseconds" << std::endl;

    auto start_init_label= std::chrono::high_resolution_clock::now();

    init_ranks(g); // auf Knoten und Kanten ausf??hren, sodass alle die Eigenschaft haben -> auch ausgehende Kanten z??hlen und dazuschreiben
    //init_labels_serial(g);

    auto stop_init_label = std::chrono::high_resolution_clock::now();

    auto duration_init_label = std::chrono::duration_cast<std::chrono::microseconds>(stop_init_label - start_init_label); 
  
    std::cout << "Time taken by init_rank: " 
      << duration_init_label.count() << " microseconds" << std::endl;

    auto start_label_prop= std::chrono::high_resolution_clock::now();

    pageRank(g);
    //pageRank_serial(g);

    auto stop_label_prop = std::chrono::high_resolution_clock::now();

    auto stop_all = std::chrono::high_resolution_clock::now();

    auto duration_label_prop = std::chrono::duration_cast<std::chrono::microseconds>(stop_label_prop - start_label_prop); 
  
    std::cout << "Time taken by p-rank_prop: "
      << duration_label_prop.count() << " microseconds" << std::endl;

     auto duration_all = std::chrono::duration_cast<std::chrono::microseconds>(stop_all - start_all); 
  
    std::cout << "Time taken by all: " 
      << duration_all.count() << " microseconds" << std::endl;

    std::cout << std::distance(g.get_node_iterator_begin(), g.get_node_iterator_end()) << std::endl;
    std::cout << std::distance(g.get_rel_iterator_begin(), g.get_rel_iterator_end()) << std::endl;

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
   /*
    for(auto it=g.get_node_iterator_begin(); it != g.get_node_iterator_end();it++){
        std::cout << graph->get_node_description((*it)->get_id()).properties.at("name") << " hat den Rank: " << (*it)->read_property("rank") << std::endl; 
    }
    */
    /*
    for(auto it = g.get_rel_iterator_begin(); it != g.get_rel_iterator_end(); it++){
        std::cout << graph->get_node_description((*it)->get_from_node()->get_id()).properties.at("name") << "  nach: " << graph->get_node_description((*it)->get_to_node()->get_id()).properties.at("name") << std::endl;
    }*/

    graph->commit_transaction();
}