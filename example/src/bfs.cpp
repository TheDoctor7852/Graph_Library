#include "graph.hpp"
#include "graph_node_iterator.hpp"

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

int main(){
    auto pool = graph_pool::open("./graph/Label_Prop_Test");
    auto graph = pool->open_graph("Label_Prop_Test");

    Graph g(graph);
    bfs(g,0, true);

    graph->begin_transaction();

        for(auto iter=g.get_node_iterator_begin(); iter!=g.get_node_iterator_end(); iter++){
            try{
                auto r_ship_id = boost::any_cast<relationship::id_t>((*iter)->read_property("rel"));
            std::cout << graph->get_node_description(graph->rship_by_id(r_ship_id).from_node_id()).properties.at("name") << "  ---->  " << graph->get_node_description(graph->rship_by_id(r_ship_id).to_node_id()).properties.at("name")<< std::endl;
            }catch(std::out_of_range e){

            }   
        }

    graph->commit_transaction();
}