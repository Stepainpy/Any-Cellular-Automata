#pragma once
#include <utility>
#include <string>
#include <set>
#include <map>

class Rule {
public:
    Rule(char be, char af) : before(be), after(af) {}
    virtual ~Rule() {}
    virtual bool applyRule(char cellFrom, char& cellTo, const std::string& env, const std::string& alphabet) = 0;

protected:
    char before, after;
};

class CountRule : public Rule {
public:
    CountRule(char be, char af, const std::map<char, std::set<size_t>>& need = {})
        : Rule(be, af), m_need(std::move(need)) {}

    bool applyRule(char cellFrom, char& cellTo, const std::string& env, const std::string& alphabet) override;
    void addSubRule(char subRuleCh, const std::set<size_t>& subRuleCnt);

private:
    std::map<char, std::set<size_t>> m_need;
};