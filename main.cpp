#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

static const std::vector<std::string> operators = {"+", "-", "*",
                                                   "/", "&", "."};
static std::unordered_map<size_t, std::vector<std::pair<std::string, size_t>>>
    func_map;
static const size_t OP_SIZE = operators.size();
size_t M;
size_t N;

static std::vector<std::string> v_str;
static std::vector<std::string> m_str;

using Chain = std::vector<std::string>;

static std::string out_buf;
static constexpr size_t OUT_FLUSH_THRESHOLD = 1u << 20;
static std::ostream *out_stream = nullptr;

inline void flush_buf() {
  if (!out_buf.empty()) {
    out_stream->write(out_buf.data(),
                      static_cast<std::streamsize>(out_buf.size()));
    out_buf.clear();
  }
}

inline void emit_chain(const Chain &chain) {
  for (const auto &s : chain) {
    out_buf += s;
    out_buf += ' ';
  }
  out_buf += '\n';
  if (out_buf.size() >= OUT_FLUSH_THRESHOLD)
    flush_buf();
}

inline void precompute_strings() {
  const size_t max_vid = N + 2;
  const size_t max_mid = N + 2;
  const size_t max_len = M;
  const size_t stride = M + 1;

  v_str.assign((max_vid + 1) * stride, std::string{});
  m_str.assign((max_mid + 1) * stride, std::string{});

  char buf[64];
  for (size_t id = 1; id <= max_vid; ++id) {
    for (size_t len = 1; len <= max_len; ++len) {
      int n = std::snprintf(buf, sizeof buf, "<v_%zu, %zu>", id, len);
      v_str[id * stride + len].assign(buf, n);
    }
  }
  for (size_t id = 1; id <= max_mid; ++id) {
    for (size_t len = 1; len <= max_len; ++len) {
      int n = std::snprintf(buf, sizeof buf, "<m_%zu, %zu>", id, len);
      m_str[id * stride + len].assign(buf, n);
    }
  }
}

inline std::string make_function(const std::string &name, const size_t in,
                                 const size_t out) {
  std::string s;
  s.reserve(name.size() + 24);
  s += '<';
  s += name;
  s += ", ";
  s += std::to_string(in);
  s += ", ";
  s += std::to_string(out);
  s += '>';
  return s;
}

inline void backtrack(Chain &chain, size_t current_vlen, size_t budget,
                      size_t v_id, size_t m_id) {
  if (budget == 0) {
    emit_chain(chain);
    return;
  }

  const size_t stride = M + 1;

  for (size_t i = 0; i < OP_SIZE; ++i) {
    chain.push_back(operators[i]);
    if (operators[i] == "&") {
      for (size_t mlen = 1; mlen <= current_vlen; ++mlen) {
        chain.push_back(m_str[m_id * stride + mlen]);
        backtrack(chain, mlen, budget - 1, v_id, m_id + 1);
        chain.pop_back();
      }
    } else if (operators[i] == ".") {
      auto it = func_map.find(current_vlen);
      if (it != func_map.end()) {
        for (const auto &[func_name, func_out] : it->second) {
          chain.push_back(make_function(func_name, current_vlen, func_out));
          backtrack(chain, func_out, budget - 1, v_id, m_id);
          chain.pop_back();
        }
      }
    } else {
      chain.push_back(v_str[v_id * stride + current_vlen]);
      backtrack(chain, current_vlen, budget - 1, v_id + 1, m_id);
      chain.pop_back();
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

  precompute_strings();

  std::ofstream out_file;
  out_stream = &std::cout;
  if (argc >= 5) {
    out_file.open(argv[4], std::ios::binary);
    if (!out_file) {
      std::cerr << "Cannot open output file: " << argv[4] << "\n";
      return 1;
    }
    out_stream = &out_file;
  }
  out_buf.reserve(OUT_FLUSH_THRESHOLD + 256);

  Chain chain;
  chain.reserve(N + 1);

  for (size_t vlen = 1; vlen <= M; ++vlen) {
    chain.clear();
    chain.push_back(v_str[1 * (M + 1) + vlen]);
    backtrack(chain, vlen, N, 2, 1);
  }

  flush_buf();
  if (out_file.is_open())
    out_file.flush();
  return 0;
}