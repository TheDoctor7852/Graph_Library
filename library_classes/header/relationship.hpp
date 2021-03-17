#include "relationships.hpp"
#include "node.hpp"

#ifndef REL_HPP
#define REL_HPP

/*
  class implementing Relationships of a Graph
*/
class Relationship{
        relationship::id_t id;
        std::shared_mutex write;
        
        Node* from;
        Node* to;

        /*
          maintain Propertys added to this Node
        */
        std::map<std::string, boost::any> propertys;

    public:

        Relationship(relationship::id_t input, Node* from_node, Node* to_node);
        Relationship(Relationship&& r);

        /*
          add an property with any value to the Relationship. The given string identfies the added Property. If the key already exists, nothing will happen.
        */
        void add_property(std::string key, boost::any value);

        /*
          removes the property identified by the given string. 
        */
        void remove_property(std::string key);

        /*
          changes the property identified by the given string. A function is required if for example an vector was stored and needs to be modified without overwriting. 
        */
        bool change_property(std::string key, std::function<void(boost::any&)> f);

        /*
          read the property identified by the given string. If no value identified by the given string exists an exception will be thrown.
        */
        const boost::any read_property(std::string key);

        /*
          get the Relationship id
        */
        relationship::id_t get_id();

        /*
            returns a pointer to the node the relationship originates from.
        */
        Node* get_from_node();

        /*
            returns a pointer to the node the relationship points to.
        */
        Node* get_to_node();
};

#endif