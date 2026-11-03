#pragma once
#include "ggraph.hpp"
#include <optional>
#include <string_view>
#include <unordered_map>
#include <utility>

using Split = std::pair<std::string_view, std::string_view>;

struct StringHash {
  using is_transparent = void;

  std::size_t operator()(std::string_view s) const noexcept {
    return std::hash<std::string_view>{}(s);
  }
};

struct StringEqual {
  using is_transparent = void;

  bool operator()(std::string_view a, std::string_view b) const noexcept {
    return a == b;
  }
};

class Parser {
  std::unordered_map<std::string, size_t, StringHash, StringEqual> tokens;
  GGraph graph;
  int current_rule = -1;

  std::optional<Split> split(std::string_view line, std::string_view delimeter);
  size_t intern(std::string_view);

public:
  void parse(std::string_view line);
  GGraph &&get_graph() { return std::move(graph); }
};
