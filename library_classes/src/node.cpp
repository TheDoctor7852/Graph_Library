#include <node.hpp>

Node::Node(node* input){
    graph_reference = input;
    propertys = std::map<std::string, boost::any>();
}

Node::Node(Node&& n){
    std::unique_lock<std::shared_mutex> lock_prop(n.write_property);
    graph_reference = std::move(n.graph_reference);
    propertys = std::move(n.propertys);
}

void Node::add_property(std::string key, boost::any value){
    std::unique_lock<std::shared_mutex> lock(write_property);
    propertys.insert(std::make_pair(key, value));
}

void Node::remove_property(std::string key){
    std::unique_lock<std::shared_mutex> lock(write_property);
    propertys.erase(key);
}

const boost::any Node::read_property(std::string key){
    std::shared_lock<std::shared_mutex> lock(write_property);
    return propertys.at(key);
}

bool Node::change_property(std::string key, std::function<void(boost::any&)> f){ //besser prüft mit find ob key vorhanden
    std::unique_lock<std::shared_mutex> lock(write_property);
        try{
            f(propertys.at(key));
            return true;
        }catch(...){
            return false;
        }
}

const node* Node::get_reference(){
    return graph_reference;
}