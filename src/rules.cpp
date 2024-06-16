#include "rules.hpp"
#include <algorithm>
#include <ranges>

bool CountRule::applyRule(char cellFrom, char& cellTo, const std::string& env, const std::string& alphabet) {
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