#include "rules.hpp"
#include <algorithm>
#include <ranges>

bool CountRule::applyRule(char cellFrom, char& cellTo, const std::string& env, const std::string& alphabet) const {
    if (cellFrom != before)
        return false;
    
    // counting cells in environment
    std::map<char, size_t> countBoard;
    for (char c : alphabet)
        countBoard[c] = 0;
    for (char eCell : env)
        countBoard[eCell]++;

    // if all 'need' complied
    bool allComplied = std::ranges::all_of(m_need, [&countBoard](const std::pair<char, std::set<size_t>>& n) {
        if (!countBoard.contains(n.first))
            return false;
        return n.second.contains(countBoard[n.first]);
    });
    if (allComplied)
        cellTo = after;
    return allComplied;
}

void CountRule::addSubRule(char subRuleCh, const std::set<size_t>& subRuleCnt) {
    m_need[subRuleCh] = subRuleCnt;
}

bool PatternRule::applyRule(char cellFrom, char& cellTo, const std::string& env, const std::string& alphabet) const {
    (void)alphabet;
    if (comparePatterns(m_pattern, env.substr(0, 4) + cellFrom + env.substr(4))) {
        cellTo = after;
        return true;
    }
    return false;
}

bool PatternRule::comparePatterns(const std::string& setted, const std::string& getted) const {
    for (size_t i = 0; i < 9; i++) {
        if (setted[i] == '\0')
            continue;
        
        if (setted[i] != getted[i])
            return false;
    }
    return true;
}