#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>

static const std::vector<std::string> operators = {"+", "-", "*", "/"};
const size_t OP_SIZE = 4;
size_t M;
size_t N;

using Chain = std::vector<std::string>;

inline std::string make_vector(const size_t id, const size_t len) {
    return "<v_" + std::to_string(id) + ", " + std::to_string(len) + ">";
}

inline void backtrack(Chain &chain, size_t current_vlen, size_t budget, std::vector<Chain> &chains) {
    if (budget == 0) {
        chains.push_back(chain);
        return;
    }

    for (size_t i = 0; i < OP_SIZE; ++i) {
        chain.push_back(operators[i]);
        chain.push_back(make_vector(N - (budget - 2), current_vlen));
        backtrack(chain, current_vlen, budget - 1, chains);
        chain.pop_back();
        chain.pop_back();
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: /chaingen <M> <N>\n";
        std::cerr << " where M - max vector length, N - number of operators";
        return 1;
    }
    M = std::stoi(argv[1]);
    N = std::stoi(argv[2]);
    std::vector<Chain> chains;
    for (size_t vlen = 1; vlen < M; ++vlen) {
        Chain current_chain{make_vector(1, vlen)};
        backtrack(current_chain, vlen, N, chains);
    }

    for (const Chain &chain : chains) {
        for (const auto &s : chain) {
            std::cout << s << " ";
        }
        std::cout << "\n";
    }

    return 0;
}