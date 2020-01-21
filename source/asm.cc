#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <map>
#include "scanner.h"

/*
 * C++ Starter code for CS241 A3
 * All code requires C++14, so if you're getting compile errors make sure to
 * use -std=c++14.
 *
 * This file contains the main function of your program. By default, it just
 * prints the scanned list of tokens back to standard output.
 */

using namespace std;

const int BYTE = 4;

void print_instr(int instr) {
    char c = instr >> 24;
    cout << c;
    c = instr >> 16;
    cout << c;
    c = instr >> 8;
    cout << c;
    c = instr;
    cout << c;
}

void print_uint_instr(unsigned int instr) {
    char c = instr >> 24;
    cout << c;
    c = instr >> 16;
    cout << c;
    c = instr >> 8;
    cout << c;
    c = instr;
    cout << c;
}

int main() {
    string line;
    int instr = 0;
    map<string,int> table;
    int ctr = 0;
    vector<vector<Token>> tokenTable;
    bool recordLabel = false;
    try {
        while (getline(cin ,line)) {
            int i = 0;
            vector<Token> tokenLine = scan(line);
            if (!tokenLine.empty() && tokenLine[0].getKind() == Token::Kind::INT) {
                throw ScanningFailure{"ERROR: Starts with INT"};
            }
            if (!tokenLine.empty()) {
                if (recordLabel) {
                    recordLabel = false;
                }
                for (auto &token : tokenLine) {

                    if (token.getKind() == Token::Kind::LABEL) {
                        recordLabel = true;
                        string label = token.getLexeme();
                        // cout << "Got Label" << endl;
                        label.pop_back();
                        if (table.count(label)) {
                            throw ScanningFailure{"ERROR: Duplicate Label"};
                        }
                        table[label] = ctr * BYTE;
                        ++i;
                        // tokenLine.erase(tokenLine.begin());
                    } else {
                        recordLabel = false;
                        break;
                    }

                 }
                 if (i) {
                     tokenLine.erase(tokenLine.begin(), tokenLine.begin() + i);
                 }
                 /*
                 int eraser = 0;
                 vector<Token>tokens_of_word;
                 bool no_more_label = false;
                 for (auto &token : tokenLine) {
                     if (tokens_of_word.empty() || token.getKind() == Token::Kind::LABEL) {
                         tokenLine.erase(tokenLine.begin() + eraser);
                     } else if (token.getKind() == Token::Kind::WORD) {
                         tokens_of_word.emplace_back(token);
                     }
                     ++eraser;
                 }
                 if (no_more_label) throw ScanningFailure{"ERROR: Expecting end, got labels"};
                 */
                 if (!recordLabel) ctr++;
                 tokenTable.emplace_back(tokenLine);
            }
      }
      for (auto &it : table) {
          cerr << it.first << " " << it.second << endl;
      }
      int line_ctr = 0;
      for (auto &tokenLine : tokenTable) {
        if (!tokenLine.empty()) {
            ++line_ctr;
            Token tok1 = tokenLine[0];
            string exp1 = tok1.getLexeme();
            int len = tokenLine.size();
            if (len < 2) {
                throw ScanningFailure{"ERROR: Expecting operand for the command " + exp1 + ", but recieved none"};
            }
            Token tok2 = tokenLine[1];
            string exp2 = tok2.getLexeme();
            if (tok1.getKind() == Token::Kind::WORD) {

                if (tok2.getKind() == Token::Kind::INT || tok2.getKind() == Token::Kind::HEXINT) {
                    int64_t operand = tok2.toLong();
                    if (operand > UINT32_MAX || operand < INT32_MIN) {
                        throw ScanningFailure{"ERROR: Operand for .word out of range"};
                    }
                    if (operand > INT32_MAX) {
                        unsigned int uint = operand;
                        print_uint_instr(uint);
                    } else {
                        instr = operand;
                        print_instr(operand);
                    }
                } else if (tok2.getKind() == Token::Kind::ID) {
                    if (!table.count(exp2)) {
                        throw ScanningFailure{"ERROR: No such label: " + exp2};
                        instr = table[exp2];
                        print_instr(instr);
                    } else {
                        instr = table[exp2];
                        print_instr(instr);
                    }
                } else {
                    throw ScanningFailure{"ERROR: Invalid operand for .word"};
                }

            } else if (tok1.getKind() == Token::Kind::ID) {

                if (exp1 == "jr" || exp1 == "jalr") {
                    
                    if (len > 2) throw ScanningFailure{"ERROR: Too many stuffs"};

                    if (tok2.getKind() != Token::Kind::REG) {
                        throw ScanningFailure{"ERROR: Invalid Operand for the command " + exp1};
                    }

                    int64_t reg_s = tok2.toLong();
                    if (reg_s > 31 || reg_s < 0) {
                        throw ScanningFailure{"ERROR: Register number invalid"};
                    }

                    if (exp1 == "jr") {
                        instr = reg_s << 21 | 8;
                    } else {
                        instr = reg_s << 21 | 9;
                    }
                    print_instr(instr);

                } else if (exp1 == "add" || exp1 == "slt" || exp1 == "sltu" || exp1 == "sub") {
                    
                    if (len != 6) throw ScanningFailure{"ERROR: Arguments number incorrect must be 6"};

                    Token tok3 = tokenLine[2];
                    Token tok4 = tokenLine[3];
                    Token tok5 = tokenLine[4];
                    Token tok6 = tokenLine[5];

                    if (tok2.getKind() != Token::Kind::REG || tok3.getKind() != Token::Kind::COMMA ||
                            tok4.getKind() != Token::Kind::REG || tok5.getKind() != Token::Kind::COMMA ||
                            tok6.getKind() != Token::Kind::REG) {
                        throw ScanningFailure{"ERROR: Invalid Operand for the command " + exp1};
                    }

                    int64_t reg_d = tok2.toLong();
                    int64_t reg_s = tok4.toLong();
                    int64_t reg_t = tok6.toLong();
                    if (reg_d > 31 || reg_d < 0 || reg_s > 31 || reg_s < 0 || reg_t > 31 || reg_t < 0) {
                        throw ScanningFailure{"ERROR: Register number invalid"};
                    }
                    instr = reg_s << 21 | reg_t << 16 | reg_d << 11;
                    if (exp1 == "add") {
                        instr = instr | 32;
                    } else if (exp1 == "sub") {
                        instr = instr | 34;
                    } else if (exp1 == "slt") {
                        instr = instr | 42;
                    } else if (exp1 == "sltu") {
                        instr = instr | 43;
                    }
                    print_instr(instr);
                } else if (exp1 == "beq" || exp1 == "bne") {                
                    if (len != 6) throw ScanningFailure{"ERROR: Arguments number is not 6"};

                    Token tok3 = tokenLine[2];
                    Token tok4 = tokenLine[3];
                    Token tok5 = tokenLine[4];
                    Token tok6 = tokenLine[5];

                    if (tok2.getKind() != Token::Kind::REG || tok3.getKind() != Token::Kind::COMMA ||
                            tok4.getKind() != Token::Kind::REG || tok5.getKind() != Token::Kind::COMMA ||
                            (tok6.getKind() != Token::Kind::INT && tok6.getKind() != Token::Kind::HEXINT &&
                            tok6.getKind() != Token::Kind::ID)) {
                        throw ScanningFailure{"ERROR Invalid Operand for the command " + exp1};
                    }

                    auto reg_s = tok2.toLong();
                    auto reg_t = tok4.toLong();
                    int64_t offset;
                    if (tok6.getKind() == Token::Kind::ID) {
                        if (!table.count(tok6.getLexeme())) {
                            throw ScanningFailure{"ERROR: No label found for " + exp1};
                        } else {
                            offset = (table[tok6.getLexeme()] - line_ctr * BYTE)/4;
                        }
                    } else {
                        offset = tok6.toLong();
                    }
                    if (tok6.getKind() == Token::Kind::INT && (offset > 32767 || offset < -32768)) {
                        throw ScanningFailure{"ERROR: Offset too big or too small for " + exp1};
                    } else if (tok6.getKind() == Token::Kind::HEXINT && offset > 0xffff) {
                        throw ScanningFailure{"ERROR: Hex Offset too big for " + exp1};
                    }
                    if (reg_s > 31 || reg_s < 0 || reg_t > 31 || reg_t < 0) {
                        throw ScanningFailure{"ERROR: Invalid Registry number"};
                    }
                    if (exp1 == "beq") instr = 4 << 26 | reg_s << 21 | reg_t << 16;
                    else if (exp1 == "bne") instr = 5 << 26 | reg_s << 21 | reg_t << 16;
                    if (offset < 0) instr = instr | (offset & 0xffff);
                    else instr = instr | offset;
                    print_instr(instr);
                } else if (exp1 == "lis" || exp1 == "mfhi" || exp1 == "mflo") {
                   
                    if (len != 2) throw ScanningFailure{"ERROR: Argument number is not 2 for " + exp1};

                    if (tok2.getKind() != Token::Kind::REG) throw ScanningFailure{"ERROR: No reg found for " + exp1};

                    auto reg_d = tok2.toLong();
                    
                    if (reg_d > 31 || reg_d < 0) throw ScanningFailure{"ERROR: Reg num weird for " + exp1};

                    if (exp1 == "lis") instr = reg_d << 11 | 20;
                    else if (exp1 == "mfhi") instr = reg_d << 11 | 16;
                    else instr = reg_d << 11 | 18;

                    print_instr(instr);

                } else if (exp1 == "div" || exp1 == "mult" || exp1 == "divu" || exp1 == "multu") {
                    
                    if (len != 4) throw ScanningFailure{"ERROR: Argument number is not 4 for " + exp1};
                    
                    Token tok3 = tokenLine[2];
                    Token tok4 = tokenLine[3];

                    if (tok2.getKind() != Token::Kind::REG || tok3.getKind() != Token::Kind::COMMA || 
                            tok4.getKind() != Token::Kind::REG) {
                        throw ScanningFailure {"ERROR: No reg found for " + exp1};
                    }
                    auto reg_s = tok2.toLong();
                    auto reg_t = tok4.toLong();
                    
                    if (reg_s > 31 || reg_s < 0 || reg_t > 31 || reg_t < 0) throw ScanningFailure{"ERROR: Reg num weird for " + exp1};

                    if (exp1 == "div") instr = reg_s << 21 | reg_t << 16 | 26;
                    else if (exp1 == "mult") instr = reg_s << 21 | reg_t << 16 | 24;
                    else if (exp1 == "divu") instr = reg_s << 21 | reg_t << 16 | 27;
                    else if (exp1 == "multu") instr = reg_s << 21 | reg_t << 16 | 25;

                    print_instr(instr);

                } else if (exp1 == "lw" || exp1 == "sw") {
                    
                    if (len != 7) throw ScanningFailure{"ERROR: Arg Num must be 7 for " + exp1};

                    Token tok3 = tokenLine[2];
                    Token tok4 = tokenLine[3];
                    Token tok5 = tokenLine[4];
                    Token tok6 = tokenLine[5];
                    Token tok7 = tokenLine[6];

                    if (tok2.getKind() != Token::Kind::REG || tok3.getKind() != Token::Kind::COMMA ||
                            (tok4.getKind() != Token::Kind::INT && tok4.getKind() != Token::Kind::HEXINT) ||
                            tok5.getKind() != Token::Kind::LPAREN || tok6.getKind() != Token::Kind::REG ||
                            tok7.getKind() != Token::Kind::RPAREN) {
                        throw ScanningFailure{"ERROR: not valid arguments for " + exp1};
                    }

                    auto reg_t = tok2.toLong();
                    auto reg_s = tok6.toLong();
                    auto offset = tok4.toLong();

                    if (reg_t > 31 || reg_t < 0 || reg_s > 31 || reg_s < 0) {
                        throw ScanningFailure{"ERROR: Reg num weird for " + exp1};
                    }

                    if (tok6.getKind() == Token::Kind::INT && (offset > 32767 | offset < -32768)) {
                        throw ScanningFailure{"ERROR: Offset too big or small for " + exp1};
                    } else if (tok6.getKind() == Token::Kind::HEXINT && offset > 0xffff) {
                        throw ScanningFailure{"ERROR: Hex Offset too big for " + exp1};
                    }

                    if (offset < 0) offset = offset & 0xffff;

                    if (exp1 == "lw") instr = 35 << 26 | reg_s << 21 | reg_t << 16 | offset;
                    else if (exp1 == "sw") instr = 43 << 26 | reg_s << 21 | reg_t << 16 | offset;

                    print_instr(instr);

                } else {
                    throw ScanningFailure{"ERROR: Expecting MIPS command, received " + exp1 + " instead"};
                }
                
            } else {
                throw ScanningFailure{"ERROR: Expecting MIPS command, received " + exp1 + " instead"};
            }
        }
      }
    } catch (ScanningFailure &f) {
      cerr << f.what() << std::endl;
      return 1;
    }
    return 0;
}
