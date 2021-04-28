//
// Created by Abbey on 4/28/2021.
//

#include "graph.h"
#include <map>
#include <vector>
#include <unordered_set>


graph::graph() {
    dependents = std::map<std::string, std::unordered_set<std::string>>();
    dependees = std::map<std::string, std::unordered_set<std::string>>();
    size = 0;
}

std::unordered_set<std::string> graph::getDependees(std::string s) {
    std::unordered_set<std::string> resultDependees = std::unordered_set<std::string>();
    if (containsKey(dependees, s))
    {
        for(std::string dependee : dependees[s])
        {
            resultDependees.insert(dependee);
        }
        return resultDependees;
    }
    else
        return resultDependees;
}

std::unordered_set<std::string> graph::getDependents(std::string s) {
    std::unordered_set<std::string> resultDependents = std::unordered_set<std::string>();
    if (containsKey(dependents, s))
    {
        for(std::string dependent : dependents[s])
        {
            resultDependents.insert(dependent);
        }
        return resultDependents;
    }
    else
        return resultDependents;
}

void graph::addDependency(std::string s, std::string t) {
    // Create a new HashSet for DependentValues and DependeeValues, as we will need to change both of them after adding a pair.
    std::unordered_set<std::string> newDependentValues = std::unordered_set<std::string>();
    std::unordered_set<std::string> newDependeeValues = std::unordered_set<std::string>();
    // If s is a key in the dependents dictionary and (s, t) is not already an ordered pair, add the pair (s, t).
    // Otherwise (s, t) is already a pair, so do nothing.
    if (containsKey(dependents, s))
    {
        if (dependents[s].find(t) != dependents[s].end())
        {
            dependents[s].insert(t);
        }
        else
        {
            return;
        }
    }
        // Otherwise s is not a key in the dependents dictionary. Add t to the empty DependentValues, then add (s, t) to the dependents dicitonary.
    else
    {
        newDependentValues.insert(t);
        dependents.insert(std::pair<std::string, std::unordered_set<std::string>>(s, newDependentValues));
    }
    // Create the backwards edge, so s points to t in the dependency dictionary and t points to s in the dependee dictionary.
    if (!containsKey(dependees, t))
    {
        newDependeeValues.insert(s);
        dependees.insert(std::pair<std::string, std::unordered_set<std::string>>(t, newDependeeValues));
    }
    else
    {
        dependees[t].insert(s);
    }
    // Increment the size of the DependencyGraph.
    size++;

}

void graph::removeDependency(std::string s, std::string t) {
    // If s is not a key in the dependents dictionary, do nothing.
    if (!containsKey(dependents,s))
    {
        return;
    }
        // Otherwise, remove the pair (s,t) only if it exists. Decrement the size of the DependencyGraph.
    else
    {
        if (dependents[s].find(t) == dependents[s].end())
        {
            dependents[s].erase(t);
            dependees[t].erase(s);
            size--;
        }
        return;
    }
}

void graph::replaceDependents(std::string s, std::vector<std::string> newDependents) {
    // If s is a key in the dependents dictionary, remove all pairs (s, r) from s's dependents.
    if (containsKey(dependents,s))
    {
        for (std::string value : getDependents(s))
        {
            removeDependency(s, value);
        }
    }
    // Adds a pair in the form of (s,t)
    for (std::string newValue : newDependents)
    {
        addDependency(s, newValue);
    }
}

void graph::replaceDependees(std::string s, std::vector<std::string> newDependees) {
    // If s is a key in the dependees dictionary, remove all pairs (r, s) from s's dependees.
    if (containsKey(dependees,s))
    {
        for (std::string key : getDependees(s))
        {
            removeDependency(key, s);
        }
    }
    // Adds a pair in the form of (t, s)
    for (std::string newKey : newDependees)
    {
        addDependency(newKey, s);
    }
}
}

bool graph::hasDependents(std::string s) {
    return(containsKey(dependents, s));
}

bool graph::hasDependees(std::string s) {
    return(containsKey(dependees, s));
}

bool graph::containsKey(std::map<std::string, std::unordered_set<std::string>> map, std::string s) {
    auto it = map.find(s);
    if (it != map.end())
    {
        return true;
    }
    return false;
}


