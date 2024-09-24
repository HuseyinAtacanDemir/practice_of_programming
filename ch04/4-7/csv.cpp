#include "csv.hpp"

// getline: get one line, grow as needed
int Csv::getline(std::string& str)
{
    char c;
    line.clear();

    while (fin.get(c) && !endofline(c))
        line += c;

    split();
    str = line;
    return !fin.eof();
}

// endofline: check for and consume \r, \n, \r\n, or EOF
int Csv::endofline(char c)
{
    if (c == '\r' || c == '\n') {
        if (c == '\r' && fin.peek() == '\n') fin.get(); // consume \n if \r\n
        return 1;
    }
    return 0;
}

// split: split line into fields
int Csv::split()
{
    std::string fld;
    int i = 0, j;

    nfield = 0;
    if (line.empty())
        return 0;

    do {
        if (line[i] == '"') {
            j = advquoted(line, fld, ++i);  // skip quote
        } else {
            j = advplain(line, fld, i);
        }

        if (nfield >= field.size())
            field.push_back(fld);
        else
            field[nfield] = fld;

        nfield++;
        i = j + 1;
    } while (j < line.size());

    return nfield;
}

// advquoted: handle quoted fields
int Csv::advquoted(const std::string& s, std::string& fld, int i)
{
    fld.clear();
    for (int j = i; j < s.size(); ++j) {
        if (s[j] == '"' && s[++j] != '"') {
            int k = s.find_first_of(fieldsep, j);
            if (k > s.size()) k = s.size();
            fld += s.substr(i, j - i);
            return j;
        }
        fld += s[j];
    }
    return i;
}

// advplain: handle plain fields
int Csv::advplain(const std::string& s, std::string& fld, int i)
{
    int j = s.find_first_of(fieldsep, i);   // look for separator
    if (j > s.size()) j = s.size();
    fld = s.substr(i, j - i);
    return j;
}

// getfield: return n-th field
std::string Csv::getfield(int n) const
{
    if (n < 0 || n >= nfield)
        return "";
    return field[n];
}

