#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <utility>

using namespace std;

int tokenNum = 1;

class LRFailure {
  public:
    string what() { return "ERROR at " + to_string(tokenNum); }
};

class Rule {
  public:
    string given;
    vector<string> toProduce;
    Rule(string given, vector<string> toProduce): given{given}, toProduce{toProduce} {}
};

class State {
  public:
    int stateNum;
    map<string, bool> action; // false means reduce, true means shift
    map<string, int> num_to_jump;
    State(int num): stateNum{num} {}
    pair<bool, int> operator() (string top) {
        pair<bool, int> what;
        if (!num_to_jump.count(top) && !action.count(top)) {
            throw LRFailure{};
        }
        what.first = action[top];
        what.second = num_to_jump[top];
        return what;
    }
    void setAction(string symb, bool b) { action[symb] = b; }
    void setJump(string symb, int n) { num_to_jump[symb] = n; }
};

int main() {
    vector<string> terminals;
    vector<string> nonterminals;
    string start_symbol;
    vector<Rule> productionRules;
    vector<State> states;
    int terminalNum;
    cin >> terminalNum;
    for (int i = 0; i < terminalNum; ++i) {
        string term;
        cin >> term;
        terminals.emplace_back(term);
    }
    int nonterminalNum;
    cin >> nonterminalNum;
    for (int i = 0; i < nonterminalNum; ++i) {
        string nonterm;
        cin >> nonterm;
        nonterminals.emplace_back(nonterm);
    }
    cin >> start_symbol;
    int producRuleNum;
    cin >> producRuleNum;
    for (int i = 0; i < producRuleNum; ++i) {
        string given;
        cin >> given;
        vector<string> toProduce;
        string line;
        getline(cin, line);
        string s;
        istringstream iss{line};
        while(iss >> s) {
            toProduce.emplace_back(s);
        }
        Rule r{given, toProduce};
        productionRules.emplace_back(r);
    }
    int stateNums;
    cin >> stateNums;
    for (int i = 0; i < stateNums; ++i) {
        State st{i};
        states.emplace_back(st);
    }
    int actionNum;
    cin >> actionNum;
    for (int i = 0; i < actionNum; ++i) {
        int stateNum;
        string symb;
        string what;
        int num;
        cin >> stateNum;
        cin >> symb;
        cin >> what;
        cin >> num;
        if (what == "reduce") {
            states[stateNum].setAction(symb, false); // false means reduce
        } else if (what == "shift") {
            states[stateNum].setAction(symb, true); // true means shift
        }
        states[stateNum].setJump(symb, num);
    }
    vector<string> symStack;
    vector<int> stateStack;
    string sym;
    cin >> sym;
    try {
        symStack.emplace_back(sym);
        stateStack.emplace_back(states[0](sym).second);
        while (cin >> sym) {
            ++tokenNum;
            while (!states[stateStack.back()](sym).first) { // Need to reduce
                int ruleNum = states[stateStack.back()](sym).second;
                cout << productionRules[ruleNum].given << " ";
                for (int i = 0; i < productionRules[ruleNum].toProduce.size(); ++i) {
                    symStack.pop_back();
                    stateStack.pop_back();
                    cout << productionRules[ruleNum].toProduce[i] << " ";
                }
                cout << endl;
                symStack.emplace_back(productionRules[ruleNum].given);
                stateStack.emplace_back(states[stateStack.back()](symStack.back()).second);

            }
            symStack.emplace_back(sym);
            stateStack.emplace_back(states[stateStack.back()](sym).second);
        }
    } catch (LRFailure &f) {
        cerr << f.what() << endl;
        return 1;
    }
    try {
        if (symStack[0] != "BOF" || symStack[2] != "EOF" || symStack.size() != 3) {
            throw LRFailure{};
        } else {
            bool is_it_terminal = 1;
            for (auto &no : nonterminals) {
                if (symStack[1] == no) {
                    is_it_terminal = 0;
                    break;
                }
            }
            if (is_it_terminal) {
                throw LRFailure{};
            }
        }
    } catch (LRFailure &f) {
        cerr << f.what() << endl;
        return 1;
    }
    cout << start_symbol << " ";
    for (auto &sym : symStack) {
        cout << sym << " ";
    }
    cout << endl;

}
