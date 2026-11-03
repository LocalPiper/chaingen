#include "bnf_processing.hpp"
#include "ggraph_dot.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  const std::string in_path = (argc > 1) ? argv[1] : "base_grammar.bnf";
  const std::string out_path = (argc > 2) ? argv[2] : "grammar.dot";

  std::ifstream bnf(in_path);
  if (!bnf) {
    std::cerr << "cannot open " << in_path << "\n";
    return 1;
  }

  Parser p;
  std::string line;
  while (std::getline(bnf, line)) {
    p.parse(line);
  }

  GGraph g = p.get_graph();

  std::ofstream dot(out_path);
  if (!dot) {
    std::cerr << "cannot write " << out_path << "\n";
    return 1;
  }
  write_dot(g, dot);
  return 0;
}
