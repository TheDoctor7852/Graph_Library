#include <node.hpp>

Node::Node(node::id_t input){
    id = input;
    propertys = std::map<std::string, boost::any>();
}

Node::Node(Node&& n){
    std::unique_lock<std::shared_mutex> lock_prop(n.write_property);
    id = std::move(n.id);
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

const node::id_t& Node::get_id(){
    return id;
}