#include "relationships.hpp"
#include "node.hpp"

#ifndef REL_HPP
#define REL_HPP
class Relationship{
        relationship::id_t id;
        std::shared_mutex write;

        Node* from;
        Node* to;

        std::map<std::string, boost::any> propertys;
    public:
        Relationship(relationship::id_t input, Node* from_node, Node* to_node);
        Relationship(Relationship&& r);
        //~Relationship() = default;

        void add_property(std::string key, boost::any value);
        void remove_property(std::string key);
        bool change_property(std::string key, std::function<void(boost::any&)> f);
        const boost::any read_property(std::string key);

        relationship::id_t get_id();
        Node* get_from_node();
        Node* get_to_node();
};

#endif