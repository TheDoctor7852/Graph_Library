#include "graph.hpp"
#include "graph_node_iterator_workstealing.hpp"

bool vergleiche_Werte(std::chrono::microseconds& a, std::chrono::microseconds&b){
    return (a.count() < b.count());
}

void generate_statistics(Graph_Node_Iterator_Timed_Workstealing& iter){
    std::vector<std::vector<std::chrono::microseconds>> process_and_wait_durations(iter.get_times_vec()[0].size());
    std::vector<std::chrono::microseconds> biggest_Process_diff = {};
    //werte sortieren
    for(size_t i=0; i<iter.get_times_vec().size();i++){
        auto latest = iter.get_times_vec()[i][0].first;
        auto earlyest = iter.get_times_vec()[i][0].first;
        for(size_t s=0; s<iter.get_times_vec()[i].size(); s++){
            process_and_wait_durations[s].push_back(std::chrono::duration_cast<std::chrono::microseconds>(iter.get_times_vec()[i][s].second - iter.get_times_vec()[i][s].first));
            if(latest < iter.get_times_vec()[i][s].first){
                latest = iter.get_times_vec()[i][s].first;
            }else if(earlyest > iter.get_times_vec()[i][s].first){
                earlyest = iter.get_times_vec()[i][s].first;
            }
        }
        biggest_Process_diff.push_back(std::chrono::duration_cast<std::chrono::microseconds>(latest - earlyest));
    }
    /*for(size_t i=0; i<process_and_wait_durations.size(); i++){
        std::cout << "Process " << i << "'s letzte gemesene Diff ist: " << process_and_wait_durations[i][process_and_wait_durations[i].size()-1].count() << std::endl;
    }
    std::cout << "Letzte gemesse diff war: " << biggest_Process_diff[biggest_Process_diff.size()-1].count() << std::endl;*/
    //Mittelwerte berechnen
    for(size_t i=0; i<process_and_wait_durations.size(); i++){
        int sum = 0;
        for(size_t s=0; s<process_and_wait_durations[i].size(); s++){
            sum += process_and_wait_durations[i][s].count();
        }
        if(i<process_and_wait_durations.size()-1){
            std::cout << "Average Process time of Process " << i << " is: " << (sum/process_and_wait_durations[i].size()) <<std::endl;
        }else{
            std::cout << "Average waiting time for all Processes to finish was: " << (sum/process_and_wait_durations[i].size()) <<std::endl;
        }
    }
    int sum =0;
    for(size_t i=0; i<biggest_Process_diff.size(); i++){
        sum += biggest_Process_diff[i].count();
    }
    std::cout << "Average \"first and last Process started\" delay was: "<< (sum/biggest_Process_diff.size()) <<std::endl;

    //Median finden
    for(size_t i=0; i<process_and_wait_durations.size(); i++){
        std::sort(process_and_wait_durations[i].begin(), process_and_wait_durations[i].end(), vergleiche_Werte);
        if(i<process_and_wait_durations.size()-1){
        std::cout << "Median-Laufzeit des Processes " << i << " dauerte: " << process_and_wait_durations[i][process_and_wait_durations[i].size()/2].count() <<std::endl;
        }else{
            std::cout << "Median-Wartezeit auf alle Prozesse ist: " << process_and_wait_durations[i][process_and_wait_durations[i].size()/2].count() <<std::endl;
        }
    }
    std::sort(biggest_Process_diff.begin(), biggest_Process_diff.end(), vergleiche_Werte);
    std::cout << "Median-\"first and last Process started\"-Wartezeit ist: " << biggest_Process_diff[biggest_Process_diff.size()/2].count() << std::endl;
}

void find_bigest_startdiff(Graph_Node_Iterator_Timed_Workstealing& iter, size_t pos){
    size_t latest_pos = 0;
    size_t earlyest_pos = 0;
    auto latest = iter.get_times_vec()[pos][0].first;
    auto earlyest = iter.get_times_vec()[pos][0].first;
    for(size_t i=1; i<iter.get_times_vec()[pos].size()-1; i++){
        if(latest < iter.get_times_vec()[pos][i].first){
            latest = iter.get_times_vec()[pos][i].first;
            latest_pos = i;
        }else if(earlyest > iter.get_times_vec()[pos][i].first){
            earlyest = iter.get_times_vec()[pos][i].first;
            earlyest_pos = i;
        }
    }
    std::cout << "Frühster Prozess war Nummer " << earlyest_pos << " und der späteste Start war Process: " << latest_pos << " mit einer Differenz von: " << std::chrono::duration_cast<std::chrono::microseconds>(latest - earlyest).count() << "microseconds." << std::endl;
}

struct RelationshipWeight{
    relationship::id_t id;
    double weight;

    RelationshipWeight(relationship::id_t input_id, double input_weight){
        id = input_id;
        weight = input_weight;
    }
};

double convertBoostAnyToDouble(boost::any input){
    try{
        return boost::any_cast<double>(input);
    } catch (boost::bad_any_cast &e){ //sollte boost::any vom Typ int sein, schlägt die obere Umwandlung mit boost::bad_any_cast fehl.
        return static_cast<double>(boost::any_cast<int>(input)); // boost::any wird erst in einen integer umgewandelt um dann in eine double-Typ überführt zu werden. 
    }
}

std::vector<node::id_t> determin_max_values_ids(std::vector<RelationshipWeight>& from_node){
    //initialisiere die benötigten Variablen
    std::vector<node::id_t> result = {};
    bool found_max_last_round = true;
    RelationshipWeight max = RelationshipWeight(0,std::numeric_limits<double>::min());
    int position_of_max_ele = 0;

    //solange wie in der letzten Runde ein Element >= dem maximalen Element gefunden wurde, wird nach einem weiteren maximalen Element gesucht. -> es werden alle maximale Elemente gefunden. 
    while ((found_max_last_round) && (!from_node.empty())){
        found_max_last_round = false;
        auto max_entry = std::max_element(from_node.begin(),from_node.end(),[] (const RelationshipWeight &a, const RelationshipWeight &b) {
            return a.weight < b.weight;
        });
        position_of_max_ele = std::distance(from_node.begin(), max_entry);
        if(max.weight <= from_node[position_of_max_ele].weight){
            max = from_node[position_of_max_ele];
            result.push_back(max.id);
            found_max_last_round = true;
            from_node.erase(from_node.begin() + position_of_max_ele);
        }
    }
    return result;
};

void init_nodes_in_graph(graph_db_ptr& graph, Graph& g){
    graph->begin_transaction();
    graph->nodes([&g](auto& n){
       g.add_node(n.id());
    });
    graph->commit_transaction();
}

void init_rel_with_function(graph_db_ptr& graph, Graph& g){

    std::string property = "values";
    double default_value = 1.0;

    g.initialise_relationships(graph, [&graph, &property, &default_value](std::vector<std::unique_ptr<Node>>::iterator start){
        std::vector<RelationshipWeight> from_node = {};
        std::vector<node::id_t> ret = {};
                    node::id_t active_node = (*start)->get_id();
                    graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
                        if(graph->get_rship_description(r.id()).has_property(property)){
                            from_node.push_back(RelationshipWeight(r.to_node_id(),convertBoostAnyToDouble(graph->get_rship_description(r.id()).properties.at(property))));
                        } else{
                            // sollte die Kante nicht die Property haben, wird der übergebene default_value verwendet 
                            from_node.push_back(RelationshipWeight(r.to_node_id(),default_value)); 
                        }
                     });
        
                    ret = determin_max_values_ids(from_node);
                    return ret;
    });
}

void init_rel_serial(graph_db_ptr& graph, Graph& g){

    std::string property = "values";
    double default_value = 1.0;

    auto it_begin = g.get_node_iterator_begin();

    graph->begin_transaction();

   for(auto start = g.get_node_iterator_begin(); start != g.get_node_iterator_end(); start++){
        std::vector<RelationshipWeight> from_node = {};
        std::vector<node::id_t> ret = {};
                    node::id_t active_node = (*start)->get_id();
                    graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
                        if(graph->get_rship_description(r.id()).has_property(property)){
                            from_node.push_back(RelationshipWeight(r.to_node_id(),convertBoostAnyToDouble(graph->get_rship_description(r.id()).properties.at(property))));
                        } else{
                            // sollte die Kante nicht die Property haben, wird der übergebene default_value verwendet 
                            from_node.push_back(RelationshipWeight(r.to_node_id(),default_value)); 
                        }
                     });
        
                    ret = determin_max_values_ids(from_node);
                    for(size_t i= 0; i<ret.size(); i++){
                        g.add_relationship(start, it_begin + ret[i]);
                    }
    }

    graph->commit_transaction();
}

void init_labels(Graph& g){
    Graph_Node_Iterator_Timed_Workstealing iter(g.get_node_iterator_begin(), g.get_node_iterator_end());
    for_each_time_workstealing(iter, [](Node* n){
        n->add_property("label", n->get_id());
    });
}

void init_labels_serial(Graph& g){
    for(auto it = g.get_node_iterator_begin(); it != g.get_node_iterator_end(); it++){
        (*it)->add_property("label", (*it)->get_id());
    }
}

void label_prop(Graph& g){

    Graph_Node_Iterator_Timed_Workstealing iter(g.get_node_iterator_begin(), g.get_node_iterator_end());
    bool did_change = true;
    size_t turns = 0;
    size_t max_turns = 4000;

    std::random_device rd;  
    std::mt19937 rng(rd());
    
    while(did_change && turns < max_turns){
        did_change = false;
        for_each_random_workstealing(iter, [&did_change, &rng](Node* n){
            if(n->get_outgoing_rel().size() > 1){
                std::uniform_int_distribution<int> distrib(0,n->get_outgoing_rel().size()-1);
                size_t pos = distrib(rng);
                if(boost::any_cast<node::id_t>(n->read_property("label")) != boost::any_cast<node::id_t>(n->get_outgoing_rel()[pos]->get_to_node()->read_property("label"))){
                    n->change_property("label", [&n, &pos](boost::any& a){
                        a = n->get_outgoing_rel()[pos]->get_to_node()->read_property("label");
                    });
                    did_change = true;
                }
            }else if(n->get_outgoing_rel().size() == 1){
                if(boost::any_cast<node::id_t>(n->read_property("label")) != boost::any_cast<node::id_t>(n->get_outgoing_rel()[0]->get_to_node()->read_property("label"))){
                    n->change_property("label", [&n](boost::any& a){
                        a = n->get_outgoing_rel()[0]->get_to_node()->read_property("label");
                    });
                    did_change = true;
                }
            }
        });
        turns++;
    }
    std::cout << "Turns taken: " << turns << std::endl;
}

void label_prop_time(Graph& g){

    Graph_Node_Iterator_Timed_Workstealing iter(g.get_node_iterator_begin(), g.get_node_iterator_end());
    bool did_change = true;
    size_t turns = 0;
    size_t max_turns = 4000;

    std::random_device rd;  
    std::mt19937 rng(rd());
    
    while(did_change && turns < max_turns){
        did_change = false;
        for_each_random_time_workstealing(iter, [&did_change, &rng](Node* n){
            if(n->get_outgoing_rel().size() > 1){
                std::uniform_int_distribution<int> distrib(0,n->get_outgoing_rel().size()-1);
                size_t pos = distrib(rng);
                if(boost::any_cast<node::id_t>(n->read_property("label")) != boost::any_cast<node::id_t>(n->get_outgoing_rel()[pos]->get_to_node()->read_property("label"))){
                    n->change_property("label", [&n, &pos](boost::any& a){
                        a = n->get_outgoing_rel()[pos]->get_to_node()->read_property("label");
                    });
                    did_change = true;
                }
            }else if(n->get_outgoing_rel().size() == 1){
                if(boost::any_cast<node::id_t>(n->read_property("label")) != boost::any_cast<node::id_t>(n->get_outgoing_rel()[0]->get_to_node()->read_property("label"))){
                    n->change_property("label", [&n](boost::any& a){
                        a = n->get_outgoing_rel()[0]->get_to_node()->read_property("label");
                    });
                    did_change = true;
                }
            }
        });
        turns++;
    }
    for (size_t i=0; i<iter.get_times_vec().size(); i++){
        for(size_t s=0; s<iter.get_times_vec()[i].size()-1; s++){
            std::cout << "Process " << s << " started: " << std::chrono::time_point_cast<std::chrono::microseconds>(iter.get_times_vec()[i][s].first).time_since_epoch().count() << "  ";
            std::cout << "Process " << s << " ended: " << std::chrono::time_point_cast<std::chrono::microseconds>(iter.get_times_vec()[i][s].second).time_since_epoch().count();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(iter.get_times_vec()[i][s].second - iter.get_times_vec()[i][s].first); 
            std::cout << "Time taken by Process "  << s << ": "<< duration.count() << " microseconds" << std::endl;
        }
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(iter.get_times_vec()[i][iter.get_times_vec()[i].size()-1].second - iter.get_times_vec()[i][iter.get_times_vec()[i].size()-1].first); 
        std::cout << "Time taken by waiting to finsish: "  <<  duration.count() << " microseconds" << std::endl;
        find_bigest_startdiff(iter, i);
        std::cout << "--------------------------------------------------------------------------------------------------------------------" << std::endl;
    }
    generate_statistics(iter);
    std::cout << "Turns taken: " << turns << std::endl;
}

void label_prop_serial(Graph& g){

    Graph_Node_Iterator_Timed_Workstealing iter(g.get_node_iterator_begin(), g.get_node_iterator_end());
    bool did_change = true;
    size_t turns = 0;
    size_t max_turns = 4000;
    size_t distance = std::distance(g.get_node_iterator_begin(), g.get_node_iterator_end());

    std::random_device rd;  
    std::mt19937 rng(rd()); 

    std::vector<u_int64_t> prime_numbers={};

    auto p = primesieve::iterator(distance);

    for(int i=0; i<10; i++){
        prime_numbers.push_back(p.next_prime());
    }

    auto it_start = g.get_node_iterator_begin();

    while(did_change && turns < max_turns){
        did_change = false;
        std::uniform_int_distribution<int> distrib(0,prime_numbers.size()-1);
        u_int64_t current_prime = prime_numbers[distrib(rng)];
        u_int64_t offset = current_prime % distance;
        for(auto it = g.get_node_iterator_begin(); it != g.get_node_iterator_end(); it++){
            auto n = (it_start+offset)->get();
            if(n->get_outgoing_rel().size() > 1){
                std::uniform_int_distribution<int> distrib(0,n->get_outgoing_rel().size()-1);
                size_t pos = distrib(rng);
                if(boost::any_cast<node::id_t>(n->read_property("label")) != boost::any_cast<node::id_t>(n->get_outgoing_rel()[pos]->get_to_node()->read_property("label"))){
                    n->change_property("label", [&n, &pos](boost::any& a){
                        a = n->get_outgoing_rel()[pos]->get_to_node()->read_property("label");
                    });
                    did_change = true;
                }
            }else if(n->get_outgoing_rel().size() == 1){
                if(boost::any_cast<node::id_t>(n->read_property("label")) != boost::any_cast<node::id_t>(n->get_outgoing_rel()[0]->get_to_node()->read_property("label"))){
                    n->change_property("label", [&n](boost::any& a){
                        a = n->get_outgoing_rel()[0]->get_to_node()->read_property("label");
                    });
                    did_change = true;
                }
            }
            offset = (offset + current_prime) % distance;
        }
        turns++;
    }
    std::cout << "Turns taken: " << turns << std::endl;
}

int main(){

    /*Graph g;

    for(size_t i=0; i<20; i++){
        g.add_node(i);
    }

    Graph_Node_Iterator_Timed_Workstealing iter(g.get_node_iterator_begin(), g.get_node_iterator_end());

    for(size_t i = 0; i<iter.get_queue().size(); i++){
        while(!iter.get_queue()[i].empty()){
            std::cout << iter.get_queue()[i].pop().value() << std::endl;
        }
        std::cout << "------------------------------" << std::endl;
    }*/
    Graph g;

    std::string path_Graphs = "../../../C++_Programme/Poseidon_GraphAnalytics/test/graph/";

    auto pool = graph_pool::open(path_Graphs + "20000nodeGraph");
    auto graph = pool->open_graph("20000nodeGraph");

    auto start_init = std::chrono::high_resolution_clock::now();

    init_nodes_in_graph(graph, g);

    auto stop_init = std::chrono::high_resolution_clock::now();

    auto duration_init = std::chrono::duration_cast<std::chrono::microseconds>(stop_init - start_init); 
  
    std::cout << "Time taken by init: " 
      << duration_init.count() << " microseconds" << std::endl;

    auto start_rel = std::chrono::high_resolution_clock::now();

    init_rel_with_function(graph, g);

    auto stop_rel = std::chrono::high_resolution_clock::now();

    auto duration_rel = std::chrono::duration_cast<std::chrono::microseconds>(stop_rel - start_rel); 
  
    std::cout << "Time taken by rel: " 
      << duration_rel.count() << " microseconds" << std::endl;

    auto start_init_label= std::chrono::high_resolution_clock::now();

    init_labels(g);

    auto stop_init_label = std::chrono::high_resolution_clock::now();

    auto duration_init_label = std::chrono::duration_cast<std::chrono::microseconds>(stop_init_label - start_init_label); 
  
    std::cout << "Time taken by init_label: " 
      << duration_init_label.count() << " microseconds" << std::endl;

    auto start_label_prop= std::chrono::high_resolution_clock::now();

    label_prop(g);
    //label_prop_time(g);
    //label_prop_serial(g);

    auto stop_label_prop = std::chrono::high_resolution_clock::now();

    auto duration_label_prop = std::chrono::duration_cast<std::chrono::microseconds>(stop_label_prop - start_label_prop); 
  
    std::cout << "Time taken by label_prop: " 
      << duration_label_prop.count() << " microseconds" << std::endl;

    //graph->begin_transaction();
    
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
    for(auto it = g.get_rel_iterator_begin(); it != g.get_rel_iterator_end(); it++){
        std::cout << graph->get_node_description((*it)->get_from_node()->get_id()).properties.at("name") << "  nach: " << graph->get_node_description((*it)->get_to_node()->get_id()).properties.at("name") << std::endl;
    }*/

    //graph->commit_transaction();
}