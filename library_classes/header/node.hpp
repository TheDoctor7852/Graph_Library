#include <vector>
#include <map>

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
class Node{
        node::id_t id;
        std::shared_mutex write_property;
        std::shared_mutex write_inc;
        std::shared_mutex write_out;

        std::vector<Relationship*> incomming_rel;
        std::vector<Relationship*> outgoing_rel;

        std::map<std::string, boost::any> propertys;

        friend Relationship;
        friend Graph;

        void add_incomming_rel(Relationship* input);
        void remove_incomming_rel(Relationship* input);

        void add_outgoing_rel(Relationship* input);
        void remove_outgoing_rel(Relationship* input);//die vier waren vorher private

    public:
        Node(node::id_t input);
        Node(Node&& n); //wird wegen mutex benötigt

        void add_property(std::string key, boost::any value);
        void remove_property(std::string key);
        bool change_property(std::string key, std::function<void(boost::any&)> f);
        const boost::any read_property(std::string key); //hier mal die rückgabe referenz rausgenommen, um zu verhindern, dass man unfertigen zustand liest
 
        const std::vector<Relationship*> get_incomming_rel();

        const std::vector<Relationship*> get_outgoing_rel();
 
        node::id_t get_id();
};

#endif