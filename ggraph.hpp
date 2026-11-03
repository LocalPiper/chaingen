#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct GGraphNode {
  bool is_non_term = false;
  std::string val;

  bool operator==(const GGraphNode &) const = default;
};

struct GGraphNodeHash {
  std::size_t operator()(const GGraphNode &g) const noexcept {
    std::size_t h1 = std::hash<bool>{}(g.is_non_term);
    std::size_t h2 = std::hash<std::string>{}(g.val);

    return h1 ^ (h2 << 1);
  }
};

struct GGraph {
  std::unordered_map<size_t, std::unordered_set<size_t>> g_map;
  std::vector<GGraphNode> ggn_arena;
};
