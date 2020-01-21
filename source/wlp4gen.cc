#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include "node.h"

using namespace std;

const vector<string> TERMINAL {"AMP", "BECOMES", "BOF", "COMMA", "DELETE", "ELSE", "EOF", "EQ", "GE", "GT", "ID", "IF", "INT", "LBRACE", "LBRACK", "LE", "LPAREN", "LT", "MINUS", "NE", "NEW", "PCT", "PLUS", "PRINTLN", "RBRACE", "RETURN", "RPAREN", "SEMI", "SLASH", "STAR", "WAIN", "WHILE"};

const vector<string> NONTERMINAL {"start", "dcl", "dcls", "expr", "factor", "lvalue", "procedure", "procedures", "main", "params", "paramlist", "statement", "statements", "term", "test", "type", "arglist", "start"};

int ifcounter = 0;
int whilecounter = 0;
int deletecounter = 0;

class ParseFailure {
    string message;
  public:
    ParseFailure(string message): message{message} {}
    string what() { return message; }
};

// Reads in the production rule. Parse the output of the production rules into of strings
vector<string> parse(string input) {
    stringstream ss{input};
    vector<string> output;
    string temp;
    while (ss >> temp) {
        output.emplace_back(temp);
    }
    return output;
}

vector<Node*> search_for_given(Node *root, string given) {
    vector<Node*> queue;
    vector<Node*> nodes_match_with_given;
    queue.emplace_back(root);
    while (!queue.empty()) {
        Node* node = queue.front();
        if (given == "procedure") {
            if (node->token == given || node->token == "main") {
                nodes_match_with_given.emplace_back(node);
            }
        } else if (given == "statement_and_expr_procedure_calls") {
            if (node->token == "ID" && node->factor_procedure_paran) {
                nodes_match_with_given.emplace_back(node);
            }
        } else if (given == "statement_and_expr_ids") {
           if (node->token == "ID" && !node->factor_procedure_paran) {
               nodes_match_with_given.emplace_back(node);               
           }
        } else {
            if (node->token == given) {
                nodes_match_with_given.emplace_back(node);
            }
        }
        queue.erase(queue.begin());
        for (auto &it : node->children) {
            queue.emplace_back(it);
        }
    }
    return nodes_match_with_given;
}

vector<Node*> depth_search_for_given(Node *root, string given) {
    vector<Node*> stack;
    vector<Node*> nodes_match_with_given;
    stack.emplace_back(root);
    while (!stack.empty()) {
        Node* node = stack.back();
        if (given == "procedure") {
            if (node->token == given || node->token == "main") {
                nodes_match_with_given.emplace_back(node);
            }
        } else if (given == "statement_and_expr_procedure_calls") {
            if (node->token == "ID" && node->factor_procedure_paran) {
                nodes_match_with_given.emplace_back(node);
            }
        } else if (given == "statement_and_expr_ids") {
           if (node->token == "ID" && !node->factor_procedure_paran) {
               nodes_match_with_given.emplace_back(node);               
           }
        } else {
            if (node->token == given) {
                nodes_match_with_given.emplace_back(node);
            }
        }
        stack.pop_back();
        for (auto it = node->children.rbegin(); it != node->children.rend(); it++) {
            stack.emplace_back(*it);
        }
    }
    return nodes_match_with_given;
}


Node* initRoot() {
    string rule;
    getline(cin, rule);
    vector<string> toProduce = parse(rule);
    string token = toProduce[0];
    toProduce.erase(toProduce.begin());
    Node* root = new Node {token, toProduce, rule};
    return root;
}


void readRules(Node* node, map<string,bool> term_or_not) {
    for (auto &prod : node->toProduce) {
        string rule;
        getline(cin, rule);
        vector<string> toProduce = parse(rule);
        string token;
        if (!toProduce.empty()) {
            token = toProduce[0];
        }
        toProduce.erase(toProduce.begin());
        node->addChild(token, toProduce, rule);
        if (node->token == "factor" && node->rule == "factor ID LPAREN arglist RPAREN" && token == "ID") {
            node->children.back()->set_factor_procedure_paran(true);
        }
        if (term_or_not[token]) {
            readRules(node->children.back(), term_or_not);
        }
    }
}

string whatType(Node *node, map<string,string> varSymbTable) {
    if (node->token == "dcl") {
        return whatType(node->children[1], varSymbTable);
    } else if (node->token == "NUM") {
        return "int";
    } else if (node->token == "NULL") {
        return "int*";
    } else if (node->token == "ID") {
        return varSymbTable[node->toProduce[0]];
    } else if (node->token == "term") {
        if (node->rule == "term factor") {
            return whatType(node->children[0], varSymbTable);
        } else {
            return "int";
        }
    } else if (node->token == "factor") {
        if (node->toProduce.size() == 1) {
            return whatType(node->children[0], varSymbTable);
        } else if (node->rule == "factor LPAREN expr RPAREN") {
            return whatType(node->children[1], varSymbTable);
        } else if (node->rule == "factor AMP lvalue" || 
                   node->rule == "factor NEW INT LBRACK expr RBRACK") {
            return "int*";
        } else if (node->rule == "factor STAR factor" ||
                   node->rule == "factor ID LPAREN RPAREN" ||
                   node->rule == "factor ID LPAREN arglist RPAREN") {
            return "int";
        }
    } else if (node->token == "lvalue") {
        if (node->rule == "lvalue ID") {
            return whatType(node->children[0], varSymbTable);
        } else if (node->rule == "lvalue LPAREN lvalue RPAREN") {
            return whatType(node->children[1], varSymbTable);
        } else if (node->rule == "lvalue STAR factor") {
            return "int";
        }
    } else if (node->token == "expr") {
        if (node->rule == "expr term") {
            return whatType(node->children[0], varSymbTable);
        } else if (node->rule == "expr expr PLUS term") {
            string derivedExpr = whatType(node->children[0], varSymbTable);
            string derivedTerm = whatType(node->children[2], varSymbTable);
            if (derivedExpr == "int" && derivedTerm == "int") return "int";
            else if (derivedExpr == "int*" && derivedTerm == "int") return "int*";
            else if (derivedExpr == "int" && derivedTerm == "int*") return "int*";
        } else if (node->rule == "expr expr MINUS term") {
            string derivedExpr = whatType(node->children[0], varSymbTable);
            string derivedTerm = whatType(node->children[2], varSymbTable);
            if (derivedExpr == "int" && derivedTerm == "int") return "int";
            else if (derivedExpr == "int*" && derivedTerm == "int") return "int*";
            else if (derivedExpr == "int*" && derivedTerm == "int*") return "int";
        }
    }
}

void typeCheck(Node* node, map<string,string> varSymbTable, map<string, vector<string>> proc_param_table) {
    // cout << "checking node " << node->rule << endl;
    if (node->token == "dcls") {
        if (node->rule == "dcls dcls dcl BECOMES NUM SEMI") {
            if (!(whatType(node->children[1], varSymbTable) == "int")) {
                throw ParseFailure{"ERROR: Expecting int for variable " + node->children[1]->toProduce[0]};
            }
        } else if (node->rule == "dcls dcls dcl BECOMES NULL SEMI") {
            if (!(whatType(node->children[1], varSymbTable) == "int*")) {
                throw ParseFailure{"ERROR: Expecting int* for variable " + node->children[1]->toProduce[0]};
            }
        }
    } else if (node->token == "factor") {
        if (node->rule == "factor AMP lvalue") {
            if (!(whatType(node->children[1], varSymbTable) == "int")) {
                throw ParseFailure{"ERROR: Expecting int for lvalue"};
            }
        } else if (node->rule == "factor STAR factor") {
            if (!(whatType(node->children[1], varSymbTable) == "int*")) {
                throw ParseFailure{"ERROR: Expecting int* for factor"};
            }
        } else if (node->rule == "factor NEW INT LBRACK expr RBRACK") {
            if (!(whatType(node->children[3], varSymbTable) == "int")) {
                throw ParseFailure{"ERROR: Expecting int for expr"};
            }
        } else if (node->rule == "factor ID LPAREN RPAREN") {
            if (!(proc_param_table[node->children[0]->toProduce[0]].empty())) {
                throw ParseFailure{"ERROR: Expecting empty parameter for procedure " + node->children[0]->toProduce[0]};
            }
        } else if (node->rule == "factor ID LPAREN arglist RPAREN") {
            vector<Node*> exprNodes = search_for_given(node->children[2], "expr");
            if (varSymbTable.count(node->children[0]->toProduce[0])) {
                throw ParseFailure{"ERROR: Trying to use variable already defined as procedure"};
            }
            bool lengthMatch = exprNodes.size() == proc_param_table[node->children[0]->toProduce[0]].size();
            bool typeMatch = 1;
            for (int i = 0; i < exprNodes.size(); ++i) {
                if (whatType(exprNodes[i], varSymbTable) != proc_param_table[node->children[0]->toProduce[0]][i]) {
                    typeMatch = 0;
                    break;
                }
            }
            if (!lengthMatch) {
                throw ParseFailure{"ERROR: Parameter length mismatch for procedure " + node->children[0]->toProduce[0]};
            }
            if (!typeMatch) {
                throw ParseFailure{"ERROR: Parameter type mismatch for procedure " + node->children[0]->toProduce[0]};
            }
        }
    } else if (node->token == "term") {
        if (node->rule != "term factor") {
            if (!(whatType(node->children[0], varSymbTable) == "int" &&
                   whatType(node->children[2], varSymbTable) == "int")) {
                throw ParseFailure{"ERROR: Expecting int for arithmetic operation"};
            }
        }
    } else if (node->token == "procedure") {
        if (!(whatType(node->children[9], varSymbTable) == "int")) {
            throw ParseFailure{"ERROR: Return value must be int for procedure " + node->children[1]->toProduce[0]};
        }
    } else if (node->token == "main") {
        if (!(whatType(node->children[5], varSymbTable) == "int")) {
            throw ParseFailure{"ERROR: Expecting int for second parameter for wain"};
        }
        if (!(whatType(node->children[11], varSymbTable) == "int")) {
            throw ParseFailure{"ERROR: Return value must be int for wain"};
        }
    } else if (node->token == "statement") {
        if (node->rule == "statement lvalue BECOMES expr SEMI") {
            typeCheck(node->children[2], varSymbTable, proc_param_table);
            if (!(whatType(node->children[0], varSymbTable) == whatType(node->children[2], varSymbTable))) {
                throw ParseFailure{"ERROR: Type mismatch between lvalue and expr"};
            }
        } else if (node->rule == "statement PRINTLN LPAREN expr RPAREN SEMI") {
            typeCheck(node->children[2], varSymbTable, proc_param_table);
            if (!(whatType(node->children[2], varSymbTable) == "int")) {
                throw ParseFailure{"ERROR: PRINTLN must be used with int"};
            }
        } else if (node->rule == "statement DELETE LBRACK RBRACK expr SEMI") {
            typeCheck(node->children[4], varSymbTable, proc_param_table);
            if(!(whatType(node->children[3], varSymbTable) == "int*")) {
                throw ParseFailure{"ERROR: Delete must be used with int*"};
            }
        }
    } else if (node->token == "test") {
        if(!(whatType(node->children[0], varSymbTable) == whatType(node->children[2], varSymbTable))) {
            throw ParseFailure{"ERROR: Type mismatch in test clause"};
        }
    } else if (node->token == "expr") {
        if (node->rule == "expr expr PLUS term") {
            if (whatType(node->children[0], varSymbTable) == "int*" &&
                whatType(node->children[2], varSymbTable) == "int*") {
                throw ParseFailure{"ERROR: Incompatible types for operand +"};
            }
        } else if (node->rule == "expr expr MINUS term") {
            if (whatType(node->children[0], varSymbTable) == "int" &&
                whatType(node->children[2], varSymbTable) == "int*") {
                throw ParseFailure{"ERROR: Incompatible types for operand -"};
            }
        }
    }
}

void print(map<string, vector<string>> proc_param_table, map<string, map<string, string>> proc_var_table) {
    for (auto &param : proc_param_table) {
        cerr << param.first;
        for (auto & p : param.second) {
            cerr << " " << p;
        }
        cerr << endl;
        for (auto &var : proc_var_table[param.first]) {
            cerr << var.first << " " << var.second << endl;
        }
        cerr << endl;
    }
}

map<string, int> buildOffsetTable(Node* node, map<string, int> &table_to_build, map<string, bool> term_or_not, int &offset) {
    if (node->rule == "dcl type ID") {
        string name = node->children[1]->toProduce[0];
        table_to_build[name] = offset;
        offset -= 4;
    }
    if (term_or_not[node->token]) {
        for (auto &ch : node->children) {
            table_to_build = buildOffsetTable(ch, table_to_build, term_or_not, offset);
        }
    }
    return table_to_build;
}

void depthCheck(Node *root, map<string,string> varSymbTable, map<string, vector<string>> proc_param_table) {
    vector<Node*> stack;
    stack.emplace_back(root);
    while (!stack.empty()) {
        Node* node = stack.back();
        typeCheck(node, varSymbTable, proc_param_table);
        stack.pop_back();
        for (auto it = node->children.rbegin(); it != node->children.rend(); ++it) {
            stack.emplace_back(*it);
        }
    }
}
void breadthPrint(Node *root) {
    vector<Node*> queue;
    queue.emplace_back(root);
    while (!queue.empty()) {
        Node* node = queue.front();
        cout << node->rule << endl;
        queue.erase(queue.begin());
        for (auto &it : node->children) {
            queue.emplace_back(it);
        }
    }
}

void loadWord(string label, map<string, int> varOffsetTable) {
    cout << "lw $3, " << varOffsetTable[label] << "($29)" << endl;
}

void push(int reg) {
    cout << "sw $" << reg << ", -4($30)" << endl;
    cout << "sub $30, $30, $4" << endl;
}

void pop(int reg) {
    cout << "add $30, $30, $4" << endl;
    cout << "lw $" << reg << ", -4($30)" << endl;
}

void code(Node* node, map<string, int> &var_offset_table, map<string, string> var_type_table) {
    if (node->token == "main") {
    	cout << ".import print" << endl;
    	cout << ".import init" << endl;
    	cout << ".import new" << endl;
    	cout << ".import delete" << endl;
    	cout << "lis $4" << endl << ".word 4" << endl;
    	cout << "lis $10" << endl << ".word print" << endl;
    	cout << "lis $11" << endl << ".word 1" << endl;
        cout << "sub $29, $30, $4" << endl;
        push(2);
        if (whatType(node->children[3], var_type_table) == "int") {
            cout << "; int found making $2 0" << endl;
            cout << "add $2, $0, $0" << endl;
        }
        push(31);
        cout << "lis $5" << endl;
        cout << ".word init" << endl;
        cout << "jalr $5" << endl;
        pop(31);
        pop(2);
        push(1);
        push(2);
        code(node->children[8], var_offset_table, var_type_table); // dcls
        code(node->children[9], var_offset_table, var_type_table); // statements
        code(node->children[11], var_offset_table, var_type_table); // return expression
        cout << "add $30, $29, $4" << endl;
    	cout << "jr $31" << endl;
    } else if (node->token == "procedures") {
    	if (!node->children.empty()) {
    		code(node->children[0], var_offset_table, var_type_table);
    		code(node->children[1], var_offset_table, var_type_table);
    	}
    } else if (node->token == "procedure") {
        cout << "F" << node->children[1]->toProduce[0] << ":" << endl;
        vector<Node*> paramDCLNode = search_for_given(node->children[3], "dcl");
        for (auto &each : var_offset_table) {
            each.second += paramDCLNode.size() * 4;
        }
    	cout << "sub $29, $30, $4" << endl;
    	code(node->children[6], var_offset_table, var_type_table);
        push(1);
        push(2);
        push(5);
        push(6);
        push(7);
    	push(30);
    	code(node->children[7], var_offset_table, var_type_table);
        code(node->children[9], var_offset_table, var_type_table);
    	pop(30);
        pop(7);
        pop(6);
        pop(5);
        pop(2);
        pop(1);
    	cout << "add $30, $29, $4" << endl;
    	cout << "jr $31" << endl;
        for (auto &each : var_offset_table) {
            each.second -= paramDCLNode.size() * 4;
        }
    } else if (node->token == "dcls") {
        if(!node->children.empty()) {
            code(node->children[0], var_offset_table, var_type_table);
            if (whatType(node->children[1], var_type_table) == "int") {
                cout << "lis $5" << endl << ".word " << node->children[3]->toProduce[0] << endl;
            } else {
                cout << "lis $5" << endl << ".word " << 1 << endl;
            }
            push(5);
        }
    } else if (node->token == "statements") {
        if (!node->children.empty()) {
            code(node->children[0], var_offset_table, var_type_table);
            code(node->children[1], var_offset_table, var_type_table);
        }
    } else if (node->token == "statement") {
        if (node->rule == "statement lvalue BECOMES expr SEMI") {
            if (node->children[0]->rule == "lvalue ID") {
                code(node->children[2], var_offset_table, var_type_table);
                cout << "sw $3, " << var_offset_table[node->children[0]->children[0]->toProduce[0]] << "($29)" << endl;
            } else if (node->children[0]->rule == "lvalue STAR factor") {
                code(node->children[2], var_offset_table, var_type_table);
                push(3);
                code(node->children[0]->children[1], var_offset_table, var_type_table);
                pop(5);
                cout << "sw $5, 0($3)" << endl;
            }
        } else if (node->rule == "statement PRINTLN LPAREN expr RPAREN SEMI") {
            cout << "; printing stuff" << endl;
            push(1);
            code(node->children[2], var_offset_table, var_type_table);
            cout << "add $1, $3, $0" << endl;
            push(31);
            cout << "lis $5" << endl << ".word print" << endl;
            cout << "jalr $5" << endl;
            pop(31);
            pop(1);
        } else if (node->rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
            int curCounter = ifcounter;
            ifcounter++;
            code(node->children[2], var_offset_table, var_type_table);
            cout << "beq $3, $0, else" << curCounter  << endl;
            code(node->children[5], var_offset_table, var_type_table);
            cout << "beq $0, $0, endif" << curCounter << endl;
            cout << "else" << curCounter << ": ";
            code(node->children[9], var_offset_table, var_type_table);
            cout << "endif" << curCounter << ": " << endl;
        } else if (node->rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
            int curCounter = whilecounter;
            whilecounter++;
            cout << "loop" << curCounter << ": " << endl;
            code(node->children[2], var_offset_table, var_type_table);
            cout << "beq $3, $0, endWhile" << curCounter << endl;
            code(node->children[5], var_offset_table, var_type_table);
            cout << "beq $0, $0, loop" << curCounter << endl;
            cout << "endWhile" << curCounter << ": " << endl;
        } else if (node->rule == "statement DELETE LBRACK RBRACK expr SEMI") {
        	deletecounter++;
        	code(node->children[3], var_offset_table, var_type_table);
        	cout << "beq $3, $11, skipDelete" << deletecounter << endl;
        	cout << "add $1, $3, $0" << endl;
            push(31);
        	cout << "lis $5" << endl;
        	cout << ".word delete" << endl;
        	cout << "jalr $5" << endl;
            pop(31);
        	cout << "skipDelete" << deletecounter << ":" << endl;
        }
    } else if (node->token == "lvalue") {
        if (node->rule == "lvalue ID") {
            cout << "sw $3, " << var_offset_table[node->children[0]->toProduce[0]] << "($29)" << endl;
        } else if (node->rule == "lvalue LPAREN lvalue RPAREN") {
            code(node->children[1], var_offset_table, var_type_table);
        }
    } else if (node->token == "expr") {
        if (node->rule == "expr term") {
            code(node->children[0], var_offset_table, var_type_table);
        } else if (node->rule == "expr expr PLUS term") {
        	string exprtype = whatType(node->children[0], var_type_table);
        	string termtype = whatType(node->children[2], var_type_table);
        	if (exprtype == "int*" && termtype == "int") {
                cout << "; int* + int" << endl;
        		code(node->children[0], var_offset_table, var_type_table);
        		push(3);
        		code(node->children[2], var_offset_table, var_type_table);
        		cout << "mult $3, $4" << endl;
        		cout << "mflo $3" << endl;
        		pop(5);
        		cout << "add $3, $5, $3" << endl;
        	} else if (exprtype == "int" && termtype == "int*") {
                cout << "; int + int*" << endl;
        		code(node->children[0], var_offset_table, var_type_table);
        		cout << "mult $3, $4" << endl;
        		cout << "mflo $3" << endl;
        		push(3);
        		code(node->children[2], var_offset_table, var_type_table);        		
        		pop(5);
        		cout << "add $3, $5, $3" << endl;
        	} else {
                cout << "; int + int" << endl;
        		code(node->children[0], var_offset_table, var_type_table); // for expr
            	push(3);
            	code(node->children[2], var_offset_table, var_type_table); // for term
            	pop(5);
            	cout << "add $3, $5, $3" << endl;
        	}
        } else if (node->rule == "expr expr MINUS term") {
        	string exprtype = whatType(node->children[0], var_type_table);
        	string termtype = whatType(node->children[2], var_type_table);
        	if (exprtype == "int*" && termtype == "int") {
                cout << "; int* - int" << endl;
        		code(node->children[0], var_offset_table, var_type_table);
        		push(3);
        		code(node->children[2], var_offset_table, var_type_table);
        		cout << "mult $3, $4" << endl;
        		cout << "mflo $3" << endl;
        		pop(5);
        		cout << "sub $3, $5, $3" << endl;
        	} else if (exprtype == "int*" && termtype == "int*") {
                cout << "; int* - int*" << endl;
        		code(node->children[0], var_offset_table, var_type_table);
        		push(3);
        		code(node->children[2], var_offset_table, var_type_table);
        		pop(5);
        		cout << "sub $3, $5, $3" << endl;
        		cout << "div $3, $4" << endl;
        		cout << "mflo $3" << endl;
        	} else {
                cout << "; int - int" << endl;
        		code(node->children[0], var_offset_table, var_type_table); // for expr
            	push(3);
            	code(node->children[2], var_offset_table, var_type_table); // for term
            	pop(5);
            	cout << "sub $3, $5, $3" << endl;
        	}            
        }
    } else if (node->token == "factor") {
        if (node->rule == "factor ID") {
            loadWord(node->children[0]->toProduce[0], var_offset_table);
        } else if (node->rule == "factor LPAREN expr RPAREN") {
            code(node->children[1], var_offset_table, var_type_table);
        } else if (node->rule == "factor NUM") {
            cout << "lis $3" << endl;
            cout << ".word " << node->children[0]->toProduce[0] << endl;
        } else if (node->rule == "factor NULL") {
            cout << "add $3, $0, $11" << endl;
        } else if (node->rule == "factor STAR factor") {
            code(node->children[1], var_offset_table, var_type_table);
            cout << "lw $3, 0($3)" << endl;
        } else if (node->rule == "factor AMP lvalue") {
            if (node->children[1]->rule == "lvalue ID") {
                cout << "lis $3" << endl;
                cout << ".word " << var_offset_table[node->children[1]->children[0]->toProduce[0]] << endl;
                cout << "add $3, $3, $29" << endl;
            } else if (node->children[1]->rule == "lvalue STAR factor") {
                code(node->children[1]->children[1], var_offset_table, var_type_table);
            }
        } else if (node->rule == "factor NEW INT LBRACK expr RBRACK") {
        	code(node->children[3], var_offset_table, var_type_table);
        	cout << "add $1, $3, $0" << endl;
            push(31);
        	cout << "lis $5" << endl;
        	cout << ".word new" << endl;
        	cout << "jalr $5" << endl;
            pop(31);
        	cout << "bne $3, $0, 1" << endl;
        	cout << "add $3, $11, $0" << endl;
        } else if (node->rule == "factor ID LPAREN arglist RPAREN") {
        	push(29);
        	push(31);
        	vector<Node*> exprNodes = search_for_given(node->children[2], "expr");
        	for (auto &each : exprNodes) {
        		code(each, var_offset_table, var_type_table);
        		push(3);
        	}
        	cout << "lis $5" << endl;
        	cout << ".word F" << node->children[0]->toProduce[0] << endl;
        	cout << "jalr $5" << endl;
        	for (auto &each : exprNodes) {
        		cout << "add $30, $30, $4" << endl;
        	}
        	pop(31);
        	pop(29);
         } else if (node->rule == "factor ID LPAREN RPAREN") {
             push(29);
             push(31);
             cout << "lis $5" << endl;
             cout << ".word F" << node->children[0]->toProduce[0] << endl;
             cout << "jalr $5" << endl;
             pop(31);
             pop(29);
         }
    } else if (node->token == "term") {
        if (node->rule == "term factor") {
            code(node->children[0], var_offset_table, var_type_table);
        } else if (node->rule == "term term STAR factor") {
            code(node->children[0], var_offset_table, var_type_table);
            push(3);
            code(node->children[2], var_offset_table, var_type_table);
            pop(5);
            cout << "mult $5, $3" << endl << "mflo $3" << endl;
        } else if (node->rule == "term term SLASH factor") {
            code(node->children[0], var_offset_table, var_type_table);
            push(3);
            code(node->children[2], var_offset_table, var_type_table);
            pop(5);
            cout << "div $5, $3" << endl << "mflo $3" << endl;
        } else if (node->rule == "term term PCT factor") {
            code(node->children[0], var_offset_table, var_type_table);
            push(3);
            code(node->children[2], var_offset_table, var_type_table);
            pop(5);
            cout << "div $5, $3" << endl << "mfhi $3" << endl;
        }
    } else if (node->token == "test") {
        code(node->children[0], var_offset_table, var_type_table);
        push(3);
        code(node->children[2], var_offset_table, var_type_table);
        pop(5);
        string comp = whatType(node->children[0], var_type_table);
        if (comp == "int") {
        	comp = "slt";
        } else if (comp == "int*") {
        	comp = "sltu";
        }
        if (node->rule == "test expr LT expr") {
            cout << comp << " $3, $5, $3" << endl;  
        } else if (node->rule == "test expr GT expr") {
            cout << comp << " $3, $3, $5" << endl;
        } else if (node->rule == "test expr NE expr") {
            push(6);
            push(7);
            cout << comp << " $6, $3, $5" << endl;
            cout << comp << " $7, $5, $3" << endl;
            cout << "add $3, $6, $7" << endl;
            pop(7);
            pop(6);
        } else if (node->rule == "test expr EQ expr") {
            push(6);
            push(7);
            cout << comp << " $6, $3, $5" << endl;
            cout << comp << " $7, $5, $3" << endl;
            cout << "add $3, $6, $7" << endl;
            cout << "sub $3, $11, $3" << endl;
            pop(7);
            pop(6);
        } else if (node->rule == "test expr LE expr") {
            cout << comp << " $3, $3, $5" << endl;
            cout << "sub $3, $11, $3" << endl;
        } else if (node->rule == "test expr GE expr") {
            cout << comp << " $3, $5, $3" << endl;
            cout << "sub $3, $11, $3" << endl;
        }
    }
}



int main() {
    map<string, bool> term_or_not; // 0 means terminal 1 means nonterminal
    map<string, vector<string>> proc_param_table;
    map<string, map<string, string>> proc_var_table;
    map<string, map<string, int>> proc_offset_table;
    for (auto &s : TERMINAL) {
        term_or_not[s] = 0;
    }
    for (auto &s : NONTERMINAL) {
        term_or_not[s] = 1;
    }
    Node* root = initRoot();
    bool main_found = 0;
    readRules(root, term_or_not);
    vector<Node*> procedures = search_for_given(root, "procedure");
    for (auto &each : procedures) { // for each procedure
        string curProc;
        if (each->token == "main") {
            curProc = "wain";
            try {
                if (proc_param_table.count(curProc)) throw ParseFailure{"ERROR: Duplicate main function found"};
            } catch (ParseFailure &f) {
                cerr << f.what() << endl;
                delete root;
                return 1;
            }
            main_found = 1;
            int offset = 0;
            vector<string> types;
            map<string, string> varSymbTable;
            vector<string> paramVariables;
            map<string, int> varOffsetTable;
            Node* dclNode1 = each->children[3];
            Node* dclNode2 = each->children[5];
            if (dclNode1->children[0]->children.size() == 1) {
                types.emplace_back("int");
            } else if (dclNode1->children.size() == 2) {
                types.emplace_back("int*");
            }
            paramVariables.emplace_back(dclNode1->children[1]->toProduce[0]);
            if (dclNode2->children[0]->children.size() == 1) {
                types.emplace_back("int");
            } else if (dclNode2->children.size() == 2) {
                types.emplace_back("int*");
            }
            paramVariables.emplace_back(dclNode2->children[1]->toProduce[0]);
            vector<Node*> dclNodes = search_for_given(each->children[8], "dcl"); // dcls node
            dclNodes.emplace_back(dclNode1);
            dclNodes.emplace_back(dclNode2);
            for (auto &no : dclNodes) {
                string varType;
                Node* varTypeNode = no->children[0];
                Node* IDNode = no->children[1];
                if (varTypeNode->children.size() == 1) {
                    varType = "int";
                } else if (varTypeNode->children.size() == 2) {
                    varType = "int*";
                }
                try {
                    if (varSymbTable.count(IDNode->toProduce[0])) {
                        throw ParseFailure{"ERROR: duplicate definition of variable " + IDNode->toProduce[0] + " in parameter of procedure " + curProc};
                    }
                } catch(ParseFailure &f) {
                    cerr << f.what() << endl;
                    delete root;
                    return 1;
                }
                varSymbTable[IDNode->toProduce[0]] = varType;
            }
            proc_var_table[curProc] = varSymbTable;
            proc_param_table[curProc] = types;
            buildOffsetTable(each, varOffsetTable, term_or_not, offset);
            proc_offset_table[curProc] = varOffsetTable;
           /* vector<Node*> statementIDNodes = search_for_given(each->children[9], "statement_and_expr_ids"); // ID nodes from expression
            vector<Node*> IDNodes = search_for_given(each->children[11], "statement_and_expr_ids");
            for (auto &ex : statementIDNodes) {
                IDNodes.emplace_back(ex);
            }
            for (auto &idn : IDNodes) {
                try {
                    if (!varSymbTable.count(idn->toProduce[0])) {
                        throw ParseFailure{"ERROR: Use of undefined variable " + idn->toProduce[0] + " in procedure " + curProc};
                    }
                } catch(ParseFailure &f) {
                    cerr << f.what() << endl;
                    delete root;
                    return 1;
                }
            }
            vector<Node*> statementProcedureCallNodes = search_for_given(each->children[9], "statement_and_expr_procedure_calls");
            vector<Node*> procedureCallNodes = search_for_given(each->children[11], "statement_and_expr_procedure_calls");
            for (auto &pr : statementProcedureCallNodes) {
                procedureCallNodes.emplace_back(pr);
            }
            for (auto &pr : procedureCallNodes) {
                try {
                    if (!varSymbTable.count(pr->toProduce[0]) && !proc_param_table.count(pr->toProduce[0])) {
                        throw ParseFailure{"ERROR: Call of undefined procedure " + pr->toProduce[0] + " in procedure " + curProc};
                    }
                    if (pr->toProduce[0] == "wain") {
                        throw ParseFailure{"ERROR: Recursive call of wain is not allwoed"};
                    }
                } catch(ParseFailure &f) {
                    cerr << f.what() << endl;
                    delete root;
                    return 1;
                }
            }*/

        } else if (each->token == "procedure") {
            vector<string> types;
            curProc = each->children[1]->toProduce[0]; // ID of the procedure
            try {
                if (proc_param_table.count(curProc)) throw ParseFailure{"ERROR: Duplicate function definition of : " + curProc};
            } catch (ParseFailure &f) {
                cerr << f.what() << endl;
                delete root;
                return 1;
            }
            int offset = 0;
            map<string, string> varSymbTable;
            map<string, int> varOffsetTable;
            vector<string> paramVariables;
            vector<Node*> paramNodes_dcl = search_for_given(each->children[3], "dcl");
            vector<Node*> dclNodes = search_for_given(each->children[6], "dcl");
            for (auto &param_dcl : paramNodes_dcl) {
                if (param_dcl->children[0]->children.size() == 1) {
                    types.emplace_back("int");
                } else if (param_dcl->children[0]->children.size() == 2) {
                    types.emplace_back("int*");
                }
                dclNodes.emplace_back(param_dcl);
                paramVariables.emplace_back(param_dcl->children[1]->toProduce[0]);
            }
            for (auto &no : dclNodes) {
                string varType;
                Node* varTypeNode = no->children[0];
                Node* IDNode = no->children[1];
                if (varTypeNode->children.size() == 1) {
                    varType = "int";
                } else if (varTypeNode->children.size() == 2) {
                    varType = "int*";
                }
                try {
                    if (varSymbTable.count(IDNode->toProduce[0])) {
                        throw ParseFailure{"ERROR: duplicate definition of variable " + IDNode->toProduce[0] + " in parameter of procedure " + curProc};
                    }
                } catch(ParseFailure &f) {
                    cerr << f.what() << endl;
                    delete root;
                    return 1;
                }
                varSymbTable[IDNode->toProduce[0]] = varType;
            }
            proc_var_table[curProc] = varSymbTable;
            proc_param_table[curProc] = types;
            buildOffsetTable(each, varOffsetTable, term_or_not, offset);
            proc_offset_table[curProc] = varOffsetTable;
            /*vector<Node*> statementIDNodes = search_for_given(each->children[7], "statement_and_expr_ids"); // ID nodes from expression
            vector<Node*> IDNodes = search_for_given(each->children[9], "statement_and_expr_ids");
            for (auto &ex : statementIDNodes) {
                IDNodes.emplace_back(ex);
            }
            for (auto &idn : IDNodes) {
                try {
                    if (!varSymbTable.count(idn->toProduce[0])) {
                        throw ParseFailure{"ERROR: Use of undefined variable " + idn->toProduce[0] + " in procedure " + curProc};
                    }
                } catch(ParseFailure &f) {
                    cerr << f.what() << endl;
                    delete root;
                    return 1;
                }
            }
            vector<Node*> statementProcedureCallNodes = search_for_given(each->children[7], "statement_and_expr_procedure_calls");
            vector<Node*> procedureCallNodes = search_for_given(each->children[9], "statement_and_expr_procedure_calls");
            for (auto &pr : statementProcedureCallNodes) {
                procedureCallNodes.emplace_back(pr);
            }
            for (auto &pr : procedureCallNodes) {
                try {
                    if (!varSymbTable.count(pr->toProduce[0]) && !proc_param_table.count(pr->toProduce[0])) {
                        throw ParseFailure{"ERROR: Call of undefined procedure " + pr->toProduce[0] + " in procedure " + curProc};
                    }
                } catch(ParseFailure &f) {
                    cerr << f.what() << endl;
                    delete root;
                    return 1;
                }
            }*/
            /*vector<Node*> factorNodes = search_for_given(each, "factor");
            for (auto &fac : factorNodes) {
                vector<Node*> idNodes = search_for_given(
                if (!fac->rule != "factor ID LPAREN RPAREN" || !fac->rule != "factor ID LPAREN arglist RPAREN") {
                    vector<Node*> idNodes = search_for_given(fac, "ID");
                    for (auto &id : idNodes) {
                        try {
                            if (!varSymbTable.count(id->toProduce[0])) {
                                throw ParseFailure{"ERROR: Use of undefined variable " + id->toProduce[0] + " in procedure " + curProc};
                            }
                        } catch(ParseFailure &f) {
                            cerr << f.what() << endl;
                            delete root;
                            return 1;
                        }
                    }
                } else if (fac->rule == "factor ID LPAREN arglist RPAREN")
            }*/
        }
        try {
            depthCheck(each, proc_var_table[curProc], proc_param_table);
        } catch (ParseFailure &f) {
            cerr << f.what() << endl;
            delete root;
            return 1;
        }
    }
    for (auto &each : procedures) {
        string curProc;
        if (each->token == "main") {
            curProc = "wain";
            code(each, proc_offset_table[curProc], proc_var_table[curProc]);
        }
    }
    for (auto &each : procedures) {
        string curProc;
        if (each->token != "main") {
            curProc = each->children[1]->toProduce[0];
            cout << "; procedure name: " << curProc << endl;
            for (auto &each : proc_offset_table[curProc]) {
                cout << "; var name: " << each.first << " offset: " << each.second << endl;
            }
            code(each, proc_offset_table[curProc], proc_var_table[curProc]);
        }
    }
    try {
        if (!main_found) throw ParseFailure{"ERROR: No main function found"};
    } catch (ParseFailure &f) {
        cerr << f.what() << endl;
        delete root;
        return 1;
    }
    delete root;
}
