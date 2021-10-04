#include "graph_pool.hpp"

void PageRank_example(){
  // Bsp aus Buch: Graph Algorithms by Amy E. Hodler and Mark Needham  (O'Reilly). ISBN 978-1-492-05781-9
  auto pool = graph_pool::create("./graph/PageRank_example_Test", 100000000);
  auto graph = pool->create_graph("PageRank_example_Test");

  graph->begin_transaction();

  auto James = graph->add_node("Person",{{"name", boost::any(std::string("James"))}});
  auto David = graph->add_node("Person",{{"name", boost::any(std::string("David"))}});
  auto Amy = graph->add_node("Person",{{"name", boost::any(std::string("Amy"))}});
  auto Charles = graph->add_node("Person",{{"name", boost::any(std::string("Charles"))}});
  auto Doug = graph->add_node("Person",{{"name", boost::any(std::string("Doug"))}});
  auto Mark = graph->add_node("Person",{{"name", boost::any(std::string("Mark"))}});
  auto Alice = graph->add_node("Person",{{"name", boost::any(std::string("Alice"))}});
  auto Bridget = graph->add_node("Person",{{"name", boost::any(std::string("Bridget"))}});
  auto Michael = graph->add_node("Person",{{"name", boost::any(std::string("Michael"))}});

  graph->add_relationship(James,David,"FOLLOWS",{});
  graph->add_relationship(David, Amy, "FOLLOWS",{});
  graph->add_relationship(Charles,Doug,"FOLLOWS",{});
  graph->add_relationship(Doug,Mark,"FOLLOWS",{});
  graph->add_relationship(Mark,Doug,"FOLLOWS",{});
  graph->add_relationship(Mark,Alice,"FOLLOWS",{});
  graph->add_relationship(Alice,Doug,"FOLLOWS",{});
  graph->add_relationship(Alice,Charles,"FOLLOWS",{});
  graph->add_relationship(Alice,Michael,"FOLLOWS",{});
  graph->add_relationship(Alice,Bridget,"FOLLOWS",{});
  graph->add_relationship(Bridget,Alice,"FOLLOWS",{});
  graph->add_relationship(Bridget,Doug,"FOLLOWS",{});
  graph->add_relationship(Bridget,Michael,"FOLLOWS",{});
  graph->add_relationship(Michael,Bridget,"FOLLOWS",{});
  graph->add_relationship(Michael,Alice,"FOLLOWS",{});
  graph->add_relationship(Michael,Doug,"FOLLOWS",{});

  graph->commit_transaction();
  pool->close();
}

void create_labelprop_testgraph(){
  auto pool = graph_pool::create("./graph/Label_Prop_Test", 100000000);
  auto graph = pool->create_graph("Label_Prop_Test");

  graph->begin_transaction();

  auto A = graph->add_node("Node",{{"name", boost::any(std::string("A"))}});
  auto B = graph->add_node("Node",{{"name", boost::any(std::string("B"))}});
  auto C = graph->add_node("Node",{{"name", boost::any(std::string("C"))}});
  auto D = graph->add_node("Node",{{"name", boost::any(std::string("D"))}});
  auto E = graph->add_node("Node",{{"name", boost::any(std::string("E"))}});
  auto F = graph->add_node("Node",{{"name", boost::any(std::string("F"))}});
  auto G = graph->add_node("Node",{{"name", boost::any(std::string("G"))}});
  auto H = graph->add_node("Node",{{"name", boost::any(std::string("H"))}});
  auto I = graph->add_node("Node",{{"name", boost::any(std::string("I"))}});
  auto J = graph->add_node("Node",{{"name", boost::any(std::string("J"))}});
  auto L = graph->add_node("Node",{{"name", boost::any(std::string("L"))}});
  auto M = graph->add_node("Node",{{"name", boost::any(std::string("M"))}});
  auto N = graph->add_node("Node",{{"name", boost::any(std::string("N"))}});
  auto O = graph->add_node("Node",{{"name", boost::any(std::string("O"))}});
  auto P = graph->add_node("Node",{{"name", boost::any(std::string("P"))}});
  auto Q = graph->add_node("Node",{{"name", boost::any(std::string("Q"))}});
  auto R = graph->add_node("Node",{{"name", boost::any(std::string("R"))}});
  auto S = graph->add_node("Node",{{"name", boost::any(std::string("S"))}});
  auto T = graph->add_node("Node",{{"name", boost::any(std::string("T"))}});
  auto K = graph->add_node("Node",{{"name", boost::any(std::string("K"))}});

  graph->add_relationship(A,B,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(A,D,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(B,C,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(C,A,"KNOW",{{"values", boost::any(5)}});
  graph->add_relationship(C,F,"KNOW",{{"values", boost::any(1)}});
  graph->add_relationship(D,C,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(D,E,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(E,A,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(E,C,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(F,G,"KNOW",{{"values", boost::any(1)}});
  graph->add_relationship(G,J,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(G,I,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(H,G,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(H,J,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(I,H,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(J,I,"KNOW",{{"values", boost::any(5)}});
  graph->add_relationship(L,N,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(L,O,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(M,L,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(M,N,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(N,O,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(N,P,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(O,P,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(O,F,"KNOW",{{"values", boost::any(1)}});
  graph->add_relationship(P,L,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(P,M,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(Q,R,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(Q,S,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(R,S,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(K,S,"KNOW",{});

  graph->commit_transaction();
  pool->close();
}

int main(){
  PageRank_example();
  create_labelprop_testgraph();
}