#include "bnf_processing.hpp"
#include <cassert>
#include <cctype>
#include <vector>

inline std::string_view trim(std::string_view s) {
  while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
    s.remove_prefix(1);
  while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
    s.remove_suffix(1);
  return s;
}

inline std::vector<std::string_view> split_whitespace(std::string_view s) {
  std::vector<std::string_view> result;

  while (!s.empty()) {
    auto start = s.find_first_not_of(" \t\n\r\f\v");

    if (start == std::string_view::npos)
      break;

    s.remove_prefix(start);

    auto end = s.find_first_of(" \t\n\r\f\v");

    result.push_back(s.substr(0, end));

    if (end == std::string_view::npos)
      break;

    s.remove_prefix(end);
  }

  return result;
}

inline bool is_non_term(std::string_view s) {
  return !s.empty() && s.front() == '<' && s.back() == '>';
}

std::optional<Split> Parser::split(std::string_view line,
                                   std::string_view delimeter) {
  auto pos = line.find(delimeter);

  if (pos == std::string_view::npos) {
    return std::nullopt;
  }

  return Split{line.substr(0, pos), line.substr(pos + delimeter.size())};
}

size_t Parser::intern(std::string_view s) {
  if (auto it = tokens.find(s); it != tokens.end()) {
    return it->second;
  }
  graph.ggn_arena.push_back(GGraphNode{is_non_term(s), std::string(s)});
  size_t id = graph.ggn_arena.size() - 1;
  tokens.emplace(std::string(s), id);
  return id;
}

void Parser::parse(std::string_view line) {
  if (auto rule = split(line, "::=")) {
    current_rule = static_cast<int>(intern(trim(rule->first)));
    parse(rule->second);
  } else if (auto alt = split(line, "|")) {
    parse(alt->first);
    parse(alt->second);
  } else {
    std::vector<std::string_view> parts = split_whitespace(line);
    if (parts.empty())
      return;
    assert(current_rule != -1);

    for (auto p : parts) {
      graph.g_map[current_rule].insert(intern(p));
    }
  }
}
