#include <vector>
#include <map>
#include<shared_mutex>

#include "nodes.hpp"

class Relationship;
class Graph;
/*
    Idee: wenn man schneller Knoten löschen oder umhängen möchte, dann wie folgt ändern: 
            - statt Relationship* wird size_t gespeichert. Dies ist die Position der Relationship im Vektor. 
            - damit das funktioniert, dürfen gelöschte Relationen nicht direkt gelöscht werden sondern mit nullptr ersetzen. 
              zusätzlich speichere die ids in priortity queue um diese dann beim einfügen wieder neu zu vergeben.
            ---> man kann keine direkten Pointer auf relationships mehr benutzen, dafür muss man um einen Knoten zu löschen oder zu ersetzen nun nicht mehr alle Relationships durchgehen sondern nur noch an die positionen schauen.
                Nachteil: der vector kann nicht mehr verkleinert werden 
*/

#ifndef NODE_HPP
#define NODE_HPP
/*
  class implementing Nodes of a Graph
*/
class Node{
        node::id_t id;
        std::shared_mutex write_property;

        std::map<std::string, boost::any> propertys;

    public:
        Node(node::id_t input);
        Node(Node&& n);

        /*
          add an property with any value to the Node. The given string identfies the added Property. If the key already exists, nothing will happen.
        */
        void add_property(std::string key, boost::any value);

        /*
          removes the property identified by the given string. 
        */
        void remove_property(std::string key);

        /*
          changes the property identified by the given string. A function is required if for example an vector was stored and needs to be modified without overwriting. 
        */
        bool change_property(std::string key, boost::any value);

        /*
          read the property identified by the given string. If no value identified by the given string exists an exception will be thrown.
        */
        const boost::any read_property(std::string key);

        const node::id_t& get_id();
};

#endif