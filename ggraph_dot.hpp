#pragma once
#include "ggraph.hpp"
#include <ostream>
#include <string>

inline void write_dot(const GGraph &g, std::ostream &out,
                      const std::string &name = "GGraph") {
  out << "digraph " << name << "  {\n"
      << "    rankdir=LR;\n"
      << "    node [shape=box, style=\"rounded,filled\", "
         "fontname=\"monospace\"];\n\n";

  for (size_t i = 0; i < g.ggn_arena.size(); ++i) {
    const auto &n = g.ggn_arena[i];

    std::string color = !i              ? "gold"
                        : n.is_non_term ? "lightblue"
                                        : "lightgreen";

    std::string label;
    for (char c : n.val) {
      if (c == '"' || c == '\\')
        label += '\\';
      label += c;
    }

    out << "    n" << i << " [label=\"" << label << "\", fillcolor=\"" << color
        << "\"];\n";
  }

  out << '\n';
  for (const auto &[from, tos] : g.g_map) {
    for (size_t to : tos) {
      out << "    n" << from << " -> n" << to << ";\n";
    }
  }

  out << "}\n";
}
