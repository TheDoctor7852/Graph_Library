#include "relationship.hpp"

Relationship::Relationship(relationship* input){
    graph_reference = input;
    propertys = std::map<std::string, boost::any>();
}

Relationship::Relationship(Relationship&& r){
    std::unique_lock<std::shared_mutex> lock(r.write);
    graph_reference = std::move(r.graph_reference);
    propertys = std::move(r.propertys);
}

/*Relationship::~Relationship(){ //wird durch den Graphen gelöscht
    //from->remove_outgoing_rel(this);
    //to->remove_incomming_rel(this);
}*/

void Relationship::add_property(std::string key, boost::any value){
    std::unique_lock<std::shared_mutex> lock(write);
    propertys.insert(std::make_pair(key, value));
}

void Relationship::remove_property(std::string key){
    std::unique_lock<std::shared_mutex> lock(write);
    propertys.erase(key);
}

bool Relationship::change_property(std::string key, std::function<void(boost::any&)> f){
    std::unique_lock<std::shared_mutex> lock(write);
        try{
            f(propertys.at(key));
            return true;
        }catch(...){
            return false;
        }
}

const boost::any Relationship::read_property(std::string key){
     std::shared_lock<std::shared_mutex> lock(write);
    return propertys.at(key);
}

const relationship* Relationship::get_reference(){
    return graph_reference;
}