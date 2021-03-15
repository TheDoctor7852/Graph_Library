#include <node.hpp>

Node::Node(node::id_t input){
    id = input;
    propertys = std::map<std::string, boost::any>();
}

Node::Node(Node&& n){
    std::unique_lock<std::shared_mutex> lock_prop(n.write_property);
    std::unique_lock<std::shared_mutex> lock_inc(n.write_inc);
    std::unique_lock<std::shared_mutex> lock_out(n.write_out);
    id = std::move(n.id);
    incomming_rel = std::move(n.incomming_rel);
    outgoing_rel = std::move(n.outgoing_rel);
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

bool Node::change_property(std::string key, std::function<void(boost::any&)> f){ //idee, übergebe eine Funktion die verändert und return true, wenn fertig
    std::unique_lock<std::shared_mutex> lock(write_property);
        try{
            f(propertys.at(key));
            return true;
        }catch(...){
            return false;
        }
}

void Node::add_incomming_rel(Relationship* input){
    std::unique_lock<std::shared_mutex> lock(write_inc);
    incomming_rel.push_back(input);
}

void Node::remove_incomming_rel(Relationship* input){
    std::unique_lock<std::shared_mutex> lock(write_inc);
    for(auto start=incomming_rel.begin(); start != incomming_rel.end(); start++){
        if((*start)==input){
            incomming_rel.erase(start);
            break;
        }
    }
}

const std::vector<Relationship*> Node::get_incomming_rel(){
    std::shared_lock<std::shared_mutex> lock(write_inc);
    return incomming_rel;
}

void Node::add_outgoing_rel(Relationship* input){
    std::unique_lock<std::shared_mutex> lock(write_out);
    outgoing_rel.push_back(input);
}

void Node::remove_outgoing_rel(Relationship* input){
    std::unique_lock<std::shared_mutex> lock(write_out);
    for(auto start=outgoing_rel.begin(); start != outgoing_rel.end(); start++){
        if((*start)==input){
            outgoing_rel.erase(start);
            break;
        }
    }
}

const std::vector<Relationship*> Node::get_outgoing_rel(){
    std::shared_lock<std::shared_mutex> lock(write_out);
    return outgoing_rel;
}

node::id_t Node::get_id(){
    return id;
}