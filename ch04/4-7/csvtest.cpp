#include "csv.hpp"
#include <iostream>

int main() {
    std::string line;
    Csv csv;

    while (csv.getline(line) != 0) {
        std::cout << "line = '" << line << "'\n";

        // Iterate over the fields using STL iterators
        int i = 0;
        for (auto it = csv.begin(); it != csv.end(); ++it, ++i) {
            std::cout << "field[" << i << "] = '" << *it << "'\n";
        }
    }

    return 0;
}

