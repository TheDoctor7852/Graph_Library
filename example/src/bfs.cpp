#include "graph.hpp"
#include "graph_node_iterator.hpp"
#include "graph_node_iterator_offset.hpp"
#include "graph_relationship_iterator.hpp"
#include "graph_rel_iterator_offset.hpp"

//bfs, das eine Komponente durchsucht -> Startknoten muss in übergebenen Graph enthalten sein.
void bfs(Graph& g, node::id_t id, bool undirected = false){
    //auto start = g.get_graph()->get_nodes()->as_vec().begin();
    Node* start_node = g.add_node(id);
    size_t old_end_offset = std::distance(g.get_node_iterator_begin(),g.get_node_iterator_end());
    g.get_graph()->begin_transaction();

        g.get_graph()->foreach_from_relationship_of_node(g.get_graph()->node_by_id(start_node->get_id()), [&g](relationship& r){
            auto did_work = g.add_node(r.to_node_id());
            if(did_work != nullptr){
                did_work->add_property("rel", r.id());
            }
        });

    g.get_graph()->commit_transaction();
    size_t new_end_offset = std::distance(g.get_node_iterator_begin(),g.get_node_iterator_end());
    
    while(old_end_offset != new_end_offset){
        g.get_graph()->begin_transaction();
        for(size_t offset = old_end_offset; offset < new_end_offset; offset++){
            g.get_graph()->foreach_from_relationship_of_node(g.get_graph()->node_by_id( (*(g.get_node_iterator_begin()+offset))->get_id()), [&g](relationship& r){
                auto did_work = g.add_node(r.to_node_id());
                if(did_work != nullptr){
                    did_work->add_property("rel", r.id());
                }
            });
            if(undirected){
                g.get_graph()->foreach_to_relationship_of_node(g.get_graph()->node_by_id( (*(g.get_node_iterator_begin()+offset))->get_id()), [&g](relationship& r){
                auto did_work = g.add_node(r.from_node_id());
                if(did_work != nullptr){
                    did_work->add_property("rel", r.id());
                }
            });
            }
        }
        old_end_offset = new_end_offset;
        new_end_offset = std::distance(g.get_node_iterator_begin(),g.get_node_iterator_end());
        g.get_graph()->commit_transaction();
    }
}

void bfs_thread_pool(Graph& g, node::id_t id, bool undirected = false){
    Node* start_node = g.add_node(id);
    size_t old_end_offset = std::distance(g.get_node_iterator_begin(),g.get_node_iterator_end());
    g.get_graph()->begin_transaction();

        g.get_graph()->foreach_from_relationship_of_node(g.get_graph()->node_by_id(start_node->get_id()), [&g](relationship& r){
            auto did_work = g.add_node(r.to_node_id());
            if(did_work != nullptr){
                did_work->add_property("rel", r.id());
            }
        });

    g.get_graph()->commit_transaction();
    size_t new_end_offset = std::distance(g.get_node_iterator_begin(),g.get_node_iterator_end());
    
    while(old_end_offset != new_end_offset){
        g.get_graph()->begin_transaction();
        Graph_Node_Iterator_Offset it(g.get_node_vec(), old_end_offset, new_end_offset);
        for_each(it, [&undirected,&g](Node* n){
            g.get_graph()->foreach_from_relationship_of_node(g.get_graph()->node_by_id(n->get_id()), [&g](relationship& r){
                auto did_work = g.add_node(r.to_node_id());
                if(did_work != nullptr){
                    did_work->add_property("rel", r.id());
                }
            });
            //std::cout << "here" << std::endl;
            if(undirected){
                g.get_graph()->foreach_to_relationship_of_node(g.get_graph()->node_by_id(n->get_id()), [&g](relationship& r){
                auto did_work = g.add_node(r.from_node_id());
                if(did_work != nullptr){
                    did_work->add_property("rel", r.id());
                }
            });
            }
        });
        old_end_offset = new_end_offset;
        new_end_offset = std::distance(g.get_node_iterator_begin(),g.get_node_iterator_end());
        g.get_graph()->commit_transaction();
    }
}

void bfs_omp(Graph& g, node::id_t id, bool undirected = false){
    Node* start_node = g.add_node(id);
    size_t old_end_offset = std::distance(g.get_node_iterator_begin(),g.get_node_iterator_end());
    g.get_graph()->begin_transaction();

        g.get_graph()->foreach_from_relationship_of_node(g.get_graph()->node_by_id(start_node->get_id()), [&g](relationship& r){
            auto did_work = g.add_node(r.to_node_id());
            if(did_work != nullptr){
                did_work->add_property("rel", r.id());
            }
        });

    g.get_graph()->commit_transaction();
    size_t new_end_offset = std::distance(g.get_node_iterator_begin(),g.get_node_iterator_end());
    
    while(old_end_offset != new_end_offset){
        g.get_graph()->begin_transaction();
        Graph_Node_Iterator_Offset it(g.get_node_vec(), old_end_offset, new_end_offset);
        for_each_openmp(it, [&undirected,&g](Node* n){
            g.get_graph()->foreach_from_relationship_of_node(g.get_graph()->node_by_id(n->get_id()), [&g](relationship& r){
                auto did_work = g.add_node(r.to_node_id());
                if(did_work != nullptr){
                    did_work->add_property("rel", r.id());
                }
            });
            if(undirected){
                g.get_graph()->foreach_to_relationship_of_node(g.get_graph()->node_by_id(n->get_id()), [&g](relationship& r){
                    auto did_work = g.add_node(r.from_node_id());
                    if(did_work != nullptr){
                        did_work->add_property("rel", r.id());
                    }
                });
            }
        });
        old_end_offset = new_end_offset;
        new_end_offset = std::distance(g.get_node_iterator_begin(),g.get_node_iterator_end());
        g.get_graph()->commit_transaction();
    }
}

struct Node_Rel{
    node::id_t node_id;
    relationship::id_t rel_id;

    Node_Rel(node::id_t input_node, relationship::id_t input_rel){
        node_id = input_node;
        rel_id = input_rel;
    }
};

void init_nodes(Graph& g){
    g.get_graph()->begin_transaction();
    g.get_graph()->nodes([&g](node& n){
        auto p = g.add_node(n.id());
        p->add_property("found", false);
    });
    g.get_graph()->commit_transaction();
}

void bfs_thread_pool_two(Graph& g, node::id_t id, bool undirected = false){
    g.get_graph()->begin_transaction();

    size_t old_end_offset = 0;

    g.get_node(id)->change_property("found", [](boost::any& p){
                    if(boost::any_cast<bool>(p)==false){
                        p = true;
                    }
                });

        g.get_graph()->foreach_from_relationship_of_node(g.get_graph()->node_by_id(id), [&g,id](relationship& r){
            if(!(boost::any_cast<bool>(g.get_node(r.to_node_id())->read_property("found")))){
                g.get_node(r.to_node_id())->change_property("found", [&g, &r](boost::any& p){
                    if(boost::any_cast<bool>(p)==false){
                        p = true;
                        g.add_relationship(r.id());
                    }
                });
            }
        });

    g.get_graph()->commit_transaction();

    size_t new_end_offset = std::distance(g.get_rel_iterator_begin(),g.get_rel_iterator_end());

    while(old_end_offset != new_end_offset){
        g.get_graph()->begin_transaction();
        Graph_Rel_Iterator_Offset it(g.get_rel_vec() , old_end_offset, new_end_offset);
        for_each(it, [&undirected,&g](Relationship* r){
            auto n = g.get_node(g.get_graph()->rship_by_id(r->get_id()).to_node_id());
            //std::cout << "here" << std::endl;
            g.get_graph()->foreach_from_relationship_of_node(g.get_graph()->node_by_id(n->get_id()), [&g](relationship& r){
               if(!(boost::any_cast<bool>(g.get_node(r.to_node_id())->read_property("found")))){
                g.get_node(r.to_node_id())->change_property("found", [&g, &r](boost::any& p){
                    if(boost::any_cast<bool>(p)==false){
                        p = true;
                        g.add_relationship(r.id());
                    }
                });
               }
            });
            if(undirected){
                g.get_graph()->foreach_to_relationship_of_node(g.get_graph()->node_by_id(n->get_id()), [&g](relationship& r){
                    if(!(boost::any_cast<bool>(g.get_node(r.to_node_id())->read_property("found")))){
                    g.get_node(r.to_node_id())->change_property("found", [&g, &r](boost::any& p){
                    if(boost::any_cast<bool>(p)==false){
                        p = true;
                        g.add_relationship(r.id());
                    }
                });
               }
                });
            }
        });
        old_end_offset = new_end_offset;
        new_end_offset = std::distance(g.get_rel_iterator_begin(),g.get_rel_iterator_end());
        g.get_graph()->commit_transaction();
    }

}

//TODO: undirected ist teilweise verbuggt -> gibt manchmal Speicherzugriffsfehler bei parallelen Ausführungen ==> tritt immer bei größeren Graphen auf
//-> hängt mit dem parallelen einfügen von Knoten zusammen -> die übergebenen Iteratoren werden üngültig durch die vielen eingesetzten Knoten!!!!!

int main(){

    std::string path_Graphs = "../../../big_graphs/graph/";

    //auto pool = graph_pool::open("./graph/Label_Prop_Test");
    //auto graph = pool->open_graph("Label_Prop_Test");

    //auto pool = graph_pool::open(path_Graphs+"5000nodeGraph");
    //auto graph = pool->open_graph("5000nodeGraph");

    //auto pool = graph_pool::open(path_Graphs+"10000nodeGraph");
    //auto graph = pool->open_graph("10000nodeGraph");

    //auto pool = graph_pool::open(path_Graphs + "20000nodeGraph");
    //auto graph = pool->open_graph("20000nodeGraph");

    auto pool = graph_pool::open(path_Graphs+"30000nodeGraph");
    auto graph = pool->open_graph("30000nodeGraph");

    //auto pool = graph_pool::open(path_Graphs+"40000nodeGraph");
    //auto graph = pool->open_graph("40000nodeGraph");

    //auto pool = graph_pool::open(path_Graphs+"50000nodeGraph");
    //auto graph = pool->open_graph("50000nodeGraph");

    Graph g(graph);

    init_nodes(g);

    auto start_label_prop = std::chrono::high_resolution_clock::now();

    //bfs(g,0);
    //bfs_thread_pool(g,0);
    //bfs_omp(g,0);
    bfs_thread_pool_two(g,0);

    auto stop_label_prop = std::chrono::high_resolution_clock::now();


    auto duration_label_prop = std::chrono::duration_cast<std::chrono::microseconds>(stop_label_prop - start_label_prop);

    std::cout << "Time taken by bfs: "
              << duration_label_prop.count() << " microseconds" << std::endl;

 //   graph->begin_transaction();
    /*
        for(auto iter=g.get_node_iterator_begin(); iter!=g.get_node_iterator_end(); iter++){
            try{
                auto r_ship_id = boost::any_cast<relationship::id_t>((*iter)->read_property("rel"));
            std::cout << graph->get_node_description(graph->rship_by_id(r_ship_id).from_node_id()).properties.at("name") << "  ---->  " << graph->get_node_description(graph->rship_by_id(r_ship_id).to_node_id()).properties.at("name")<< std::endl;
            }catch(std::out_of_range e){

            }   
        }
    *//*
   for(auto iter=g.get_rel_iterator_begin(); iter!=g.get_rel_iterator_end(); iter++){
            auto r_ship_id = (*iter)->get_id();
            std::cout << graph->get_node_description(graph->rship_by_id(r_ship_id).from_node_id()).properties.at("name") << "  ---->  " << graph->get_node_description(graph->rship_by_id(r_ship_id).to_node_id()).properties.at("name")<< std::endl; 
        }

    graph->commit_transaction();*/
}