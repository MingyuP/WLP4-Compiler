#include <vector>
#include "node.h"

using namespace std;

Node::Node(string token, vector<string> toProduce, string rule): token{token}, toProduce{toProduce}, rule{rule} {}

void Node::addChild(string token, vector<string> toProduce,  string rule) {
    Node *n = new Node {token, toProduce, rule};
    children.emplace_back(n);
}

Node::~Node() {
    for (auto &node : children) { delete node; }
}

void Node::set_factor_procedure_paran(bool val) { factor_procedure_paran = val; }

