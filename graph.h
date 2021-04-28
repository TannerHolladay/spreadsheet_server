//
// Created by Abbey on 4/28/2021.
//

#ifndef SPREADSHEET_SERVER_DEPENDENCY_H
#define SPREADSHEET_SERVER_DEPENDENCY_H

#include <map>
#include <vector>
#include <unordered_set>

class graph {
public:
    graph();

    std::unordered_set<std::string> getDependees(std::string s);

    std::unordered_set<std::string> getDependents(std::string s);

    void addDependency(std::string s, std:: string t);

    void removeDependency(std::string s, std:: string t);

    void replaceDependents(std::string s, std::vector<std::string> newDependents);

    void replaceDependees(std::string s, std::vector<std::string> newDependees);

    bool hasDependents(std::string s);

    bool hasDependees(std::string s);

    int size;


private:
    // The set of strings t that depend on s if (s, t) is an ordered pair
    // What cells depend on a given cell
    std::map<std::string, std::unordered_set<std::string>> dependents;

    // The set of strings t such that (t, s) is an ordered pair
    // What a given cell depends on
    std::map<std::string, std::unordered_set<std::string>> dependees;

    std::map<std::string, std::unordered_set<std::string>> iterator;

    bool containsKey(std::map<std::string, std::unordered_set<std::string>> map, std::string s);
};

#endif //SPREADSHEET_SERVER_DEPENDENCY_H
