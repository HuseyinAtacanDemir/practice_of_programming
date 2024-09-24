#include "csv.hpp"

// Csvtest main: test Csv class
int main(void)
{
    string line;
    Csv csv;

    while (csv.getline(line) != 0) {
        cout << "line = '" << line << "'\n";
        for (int i = 0; i < csv.getnfield(); i++)
            cout << "field[" << i << "] = '" << csv[i] << "'\n";
    }
    return 0;
} 