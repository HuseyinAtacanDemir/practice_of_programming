#ifndef CSV_HPP
#define CSV_HPP

#include <iostream>
#include <string>
#include <vector>

class Csv {
public:
    // Constructor to initialize input stream and field separator
    explicit Csv(std::istream& fin = std::cin, const std::string& sep = ",") : fin(fin), fieldsep(sep) {}

    // Function to read a line and store fields
    int getline(std::string&);

    // Function to get a specific field
    std::string getfield(int n) const;

    // Overload the [] operator to access fields by index
    std::string operator[](int n) const { return getfield(n); }

    // Return the number of fields
    int getnfield() const { return nfield; }

    // Provide begin and end functions for STL-style iteration over fields
    std::vector<std::string>::iterator begin() { return field.begin(); }
    std::vector<std::string>::iterator end() { return field.end(); }
    std::vector<std::string>::const_iterator begin() const { return field.begin(); }
    std::vector<std::string>::const_iterator end() const { return field.end(); }

private:
    std::istream& fin;         // Input stream (file or stdin)
    std::string line;          // Current input line
    std::vector<std::string> field; // Parsed fields
    int nfield = 0;            // Number of fields
    std::string fieldsep;      // Field separator

    // Internal function to split the line into fields
    int split();

    // Internal function to detect end of line
    int endofline(char);

    // Helper functions to handle quoted and unquoted fields
    int advplain(const std::string& line, std::string& fld, int);
    int advquoted(const std::string& line, std::string& fld, int);
};

#endif

