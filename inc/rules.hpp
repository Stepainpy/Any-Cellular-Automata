#pragma once
#include <utility>
#include <string>
#include <set>
#include <map>

using namespace std::string_literals;

class Rule {
public:
    Rule(char be, char af)
        : before(be), after(af) {}
    virtual ~Rule() {}
    virtual bool applyRule(
        char cellFrom, char& cellTo,
        const std::string& env, const std::string& alphabet) const = 0;

protected:
    char before, after;
};

class CountRule : public Rule {
public:
    CountRule(char be, char af, const std::map<char, std::set<size_t>>& need = {})
        : Rule(be, af), m_need(std::move(need)) {}

    bool applyRule(
        char cellFrom, char& cellTo,
        const std::string& env, const std::string& alphabet) const override;
    void addSubRule(char subRuleCh, const std::set<size_t>& subRuleCnt);

private:
    std::map<char, std::set<size_t>> m_need;
};

class PatternRule : public Rule {
public:
    PatternRule(char be, char af, const std::string& pat = "\0\0\0\0\0\0\0\0\0"s)
        : Rule(be, af), m_pattern(pat) {}
    
    bool applyRule(
        char cellFrom, char& cellTo,
        const std::string& env, const std::string& alphabet) const override;

private:
    bool comparePatterns(const std::string& setted, const std::string& getted) const;

private:
    std::string m_pattern;
};