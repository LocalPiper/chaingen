#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

static const std::vector<std::string> operators = {"+", "-", "*",
                                                   "/", "&", "."};
static std::unordered_map<size_t, std::vector<std::pair<std::string, size_t>>>
    func_map;
const size_t OP_SIZE = operators.size();
size_t M;
size_t N;
size_t v_id = 0;
size_t m_id = 0;

using Chain = std::vector<std::string>;

inline std::string make_vector(const size_t id, const size_t len) {
  // vector id + vector len
  return "<v_" + std::to_string(id) + ", " + std::to_string(len) + ">";
}

inline std::string make_mask(const size_t id, const size_t len) {
  // mask id + number of set bits (len of resulting vector)
  return "<m_" + std::to_string(id) + ", " + std::to_string(len) + ">";
}

inline std::string make_function(const std::string &name, const size_t in,
                                 const size_t out) {
  // func name + input len + output len
  return "<" + name + ", " + std::to_string(in) + ", " + std::to_string(out) +
         ">";
}

inline void backtrack(Chain &chain, size_t current_vlen, size_t budget,
                      std::vector<Chain> &chains) {
  if (budget == 0) {
    chains.push_back(chain);
    return;
  }

  for (size_t i = 0; i < OP_SIZE; ++i) {
    chain.push_back(operators[i]);
    if (operators[i] == "&") {
      for (size_t mlen = 1; mlen <= current_vlen; ++mlen) {
        chain.push_back(make_mask(m_id++, mlen));
        backtrack(chain, mlen, budget - 1, chains);
        chain.pop_back();
        --m_id;
      }
    } else if (operators[i] == ".") {
      if (func_map.count(current_vlen)) {
        for (const auto &[func_name, func_out] : func_map.at(current_vlen)) {
          chain.push_back(make_function(func_name, current_vlen, func_out));
          backtrack(chain, func_out, budget - 1, chains);
          chain.pop_back();
        }
      }
    } else {
      chain.push_back(make_vector(v_id++, current_vlen));
      backtrack(chain, current_vlen, budget - 1, chains);
      chain.pop_back();
      --v_id;
    }
    chain.pop_back();
  }
}

inline void parse_functions(const std::string &filename) {
  std::ifstream file(filename);
  std::string line;
  while (std::getline(file, line)) {
    auto colon = line.find(':');
    auto arrow = line.find("->");
    if (colon == std::string::npos || arrow == std::string::npos)
      continue;

    std::string name = line.substr(0, colon);
    while (!name.empty() && std::isspace((unsigned char)name.back()))
      name.pop_back();

    size_t func_in = std::stoul(line.substr(colon + 1, arrow - colon - 1));
    size_t func_out = std::stoul(line.substr(arrow + 2));

    func_map[func_in].push_back({name, func_out});
  }
}

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cerr << "Usage: /chaingen <M> <N> <file.txt> [<output.txt>]\n";
    std::cerr << " where M - max vector length, N - number of operators\n";
    std::cerr << " file.txt contains function signatures of form:\n";
    std::cerr << " function_name: input_vlen -> output_vlen.\n";
    std::cerr
        << " output.txt - where to write output (stdout is used if omitted).\n";
    return 1;
  }
  M = std::stoi(argv[1]);
  N = std::stoi(argv[2]);
  parse_functions(argv[3]);

  std::vector<Chain> chains;
  for (size_t vlen = 1; vlen <= M; ++vlen) {
    v_id = 1;
    m_id = 1;
    Chain current_chain{make_vector(v_id++, vlen)};
    backtrack(current_chain, vlen, N, chains);
  }

  std::ofstream out_file;
  std::ostream *out = &std::cout;
  if (argc >= 5) {
    out_file.open(argv[4]);
    if (!out_file) {
      std::cerr << "Cannot open output file: " << argv[4] << "\n";
      return 1;
    }
    out = &out_file;
  }

  for (const Chain &chain : chains) {
    for (const auto &s : chain) {
      *out << s << " ";
    }
    *out << "\n";
  }

  return 0;
}