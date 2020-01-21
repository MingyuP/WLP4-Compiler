#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <iomanip>
#include <utility>
#include <algorithm>
#include <cctype>
#include <set>
#include <array>
#include <climits>

using namespace std;

class Token {
  public:
    enum Kind {
        ID = 0,
        NUM,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        RETURN,
        IF,
        ELSE,
        WHILE,
        PRINTLN,
        WAIN,
        BECOMES,
        INT,
        EQ,
        NE,
        LT,
        GT,
        LE,
        GE,
        PLUS,
        MINUS,
        STAR,
        SLASH,
        PCT,
        COMMA,
        SEMI,
        NEW,
        DELETE,
        LBRACK,
        RBRACK,
        AMP,
        NUL,
        WHITESPACE,
        COMMENT
    };
    Kind kind;
    string lexeme;
    Token(Kind kind, string lexeme): kind(kind), lexeme(std::move(lexeme)) {}
    Kind getKind() const { return kind; }
    const string &getLexeme() const { return lexeme; }
    int64_t toLong() const {
        istringstream iss;
        int64_t result;
        iss.str(lexeme);
        iss >> result;
        return result;
    }
};

ostream& operator<<(ostream &out, const Token &tok) {
    switch (tok.getKind()) {
        case Token::ID:
            out << "ID";
            break;
        case Token::NUM:
            out << "NUM";
            break;
        case Token::LPAREN:
            out << "LPAREN";
            break;
        case Token::RPAREN:
            out << "RPAREN";
            break;
        case Token::LBRACE:
            out << "LBRACE";
            break;
        case Token::RBRACE:
            out << "RBRACE";
            break;
        case Token::RETURN:
            out << "RETURN";
            break;
        case Token::IF:
            out << "IF";
            break;
        case Token::ELSE:
            out << "ELSE";
            break;
        case Token::WHILE:
            out << "WHILE";
            break;
        case Token::PRINTLN:
            out << "PRINTLN";
            break;
        case Token::WAIN:
            out << "WAIN";
            break;
        case Token::BECOMES:
            out << "BECOMES";
            break;
        case Token::INT:
            out << "INT";
            break;
        case Token::EQ:
            out << "EQ";
            break;
        case Token::NE:
            out << "NE";
            break;
        case Token::LT:
            out << "LT";
            break;
        case Token::GT:
            out << "GT";
            break;
        case Token::LE:
            out << "LE";
            break;
        case Token::GE:
            out << "GE";
            break;
        case Token::PLUS:
            out << "PLUS";
            break;
        case Token::MINUS:
            out << "MINUS";
            break;
        case Token::STAR:
            out << "STAR";
            break;
        case Token::SLASH:
            out << "SLASH";
            break;
        case Token::PCT:
            out << "PCT";
            break;
        case Token::COMMA:
            out << "COMMA";
            break;
        case Token::SEMI:
            out << "SEMI";
            break;
        case Token::NEW:
            out << "NEW";
            break;
        case Token::DELETE:
            out << "DELETE";
            break;
        case Token::LBRACK:
            out << "LBRACK";
            break;
        case Token::RBRACK:
            out << "RBRACK";
            break;
        case Token::AMP:
            out << "AMP";
            break;
        case Token::NUL:
            out << "NULL";
            break;
        case Token::WHITESPACE:
            out << "WHITESPACE";
            break;
        case Token::COMMENT:
            out << "COMMENT";
            break;
    }
    out << " " << tok.getLexeme();
    return out;
}

class ScanningFailure {
    string message;
  public:
    ScanningFailure(string message): message{message} {}
    const string& what() const { return message; }
};

class Wlp4DFA {
  public:
    enum State {
        // States that are also kinds
        ID = 0,
        NUM,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        RETURN,
        IF,
        ELSE,
        WHILE,
        PRINTLN,
        WAIN,
        BECOMES,
        INT,
        EQ,
        NE,
        LT,
        GT,
        LE,
        GE,
        PLUS,
        MINUS,
        STAR,
        SLASH,
        PCT,
        COMMA,
        SEMI,
        NEW,
        DELETE,
        LBRACK,
        RBRACK,
        AMP,
        NUL,
        WHITESPACE,
        COMMENT,
        
        // States that are not kinds
        START,
        FAIL,
        INT_1,
        INT_2,
        ELSE_1,
        ELSE_2,
        ELSE_3,
        WHILE_1,
        WHILE_2,
        WHILE_3,
        WHILE_4,
        RETURN_1,
        RETURN_2,
        RETURN_3,
        RETURN_4,
        RETURN_5,
        WAIN_2,
        WAIN_3,
        PRINTLN_1,
        PRINTLN_2,
        PRINTLN_3,
        PRINTLN_4,
        PRINTLN_5,
        PRINTLN_6,
        NULL_1,
        NULL_2,
        NULL_3,
        NEW_1,
        NEW_2,
        DELETE_1,
        DELETE_2,
        DELETE_3,
        DELETE_4,
        DELETE_5,
        EXCLAM_1,
        ZERO,

        LARGEST_STATE = ZERO
    };

  private:
    set<State> acceptingStates;

    array<array<State, 128>, LARGEST_STATE + 1> transitionFunction;

    Token::Kind stateToKind(State s) const {
      switch (s) {
        case ID:
            return Token::ID;
        case NUM:
            return Token::NUM;
        case LPAREN:
            return Token::LPAREN;
        case RPAREN:
            return Token::RPAREN;
        case LBRACE:
            return Token::LBRACE;
        case RBRACE:
            return Token::RBRACE;
        case RETURN:
            return Token::RETURN;
        case IF:
            return Token::IF;
        case ELSE:
            return Token::ELSE;
        case WHILE:
            return Token::WHILE;
        case PRINTLN:
            return Token::PRINTLN;
        case WAIN:
            return Token::WAIN;
        case BECOMES:
            return Token::BECOMES;
        case INT:
            return Token::INT;
        case EQ:
            return Token::EQ;
        case NE:
            return Token::NE;
        case LT:
            return Token::LT;
        case GT:
            return Token::GT;
        case LE:
            return Token::LE;
        case GE:
            return Token::GE;
        case PLUS:
            return Token::PLUS;
        case MINUS:
            return Token::MINUS;
        case STAR:
            return Token::STAR;
        case SLASH:
            return Token::SLASH;
        case PCT:
            return Token::PCT;
        case COMMA:
            return Token::COMMA;
        case SEMI:
            return Token::SEMI;
        case NEW:
            return Token::NEW;
        case DELETE:
            return Token::DELETE;
        case LBRACK:
            return Token::LBRACK;
        case RBRACK:
            return Token::RBRACK;
        case AMP:
            return Token::AMP;
        case NUL:
            return Token::NUL;
        case WHITESPACE:
            return Token::WHITESPACE;
        case COMMENT:
            return Token::COMMENT;
        case ZERO:
            return Token::NUM;
        case INT_1:
            return Token::ID;
        case INT_2:
            return Token::ID;
        case ELSE_1:
            return Token::ID;
        case ELSE_2:
            return Token::ID;
        case ELSE_3:
            return Token::ID;
        case WHILE_1:
            return Token::ID;
        case WHILE_2:
            return Token::ID;
        case WHILE_3:
            return Token::ID;
        case WHILE_4:
            return Token::ID;
        case RETURN_1:
            return Token::ID;
        case RETURN_2:
            return Token::ID;
        case RETURN_3:
            return Token::ID;
        case RETURN_4:
            return Token::ID;
        case RETURN_5:
            return Token::ID;
        case WAIN_2:
            return Token::ID;
        case WAIN_3:
            return Token::ID;
        case PRINTLN_1:
            return Token::ID;
        case PRINTLN_2:
            return Token::ID;
        case PRINTLN_3:
            return Token::ID;
        case PRINTLN_4:
            return Token::ID;
        case PRINTLN_5:
            return Token::ID;
        case PRINTLN_6:
            return Token::ID;
        case NULL_1:
            return Token::ID;
        case NULL_2:
            return Token::ID;
        case NULL_3:
            return Token::ID;
        case NEW_1:
            return Token::ID;
        case NEW_2:
            return Token::ID;
        case DELETE_1:
            return Token::ID;
        case DELETE_2:
            return Token::ID;
        case DELETE_3:
            return Token::ID;
        case DELETE_4:
            return Token::ID;
        case DELETE_5:
            return Token::ID;
        default: throw ScanningFailure{"ERROR: Cannot convert state to kind."};
      }
    }
    
  public:
    vector<Token> simplifiedMaximalMunch(const string &input) const {
        vector<Token> result;
        State state = start();
        string munchedInput;

        for (string::const_iterator inputPosn = input.begin();
             inputPosn != input.end();) {

            State oldState = state;
            state = transition(state, *inputPosn);

            if (!failed(state)) {
                munchedInput += *inputPosn;
                oldState = state;
                ++inputPosn;
            }

            if (inputPosn == input.end() || failed(state)) {
                if (accept(oldState)) {
                    result.push_back(Token(stateToKind(oldState), munchedInput));

                    munchedInput = "";
                    state = start();
                } else {
                    if (failed(state)) {
                        munchedInput += *inputPosn;
                    }
                    throw ScanningFailure{"ERROR: SMM failed on input: " + munchedInput};
                }
            }
        }
        return result;
    }

    Wlp4DFA() {
        acceptingStates = {ID, NUM, LPAREN, RPAREN, LBRACE, RBRACE, RETURN, IF, ELSE,
                             WHILE, PRINTLN, WAIN, BECOMES, INT, EQ, NE, LT, GT, LE,
                             GE, PLUS, MINUS, STAR, SLASH, PCT, COMMA, SEMI, NEW, DELETE,
                             LBRACK, RBRACK, AMP, NUL, ZERO, WHITESPACE, COMMENT, INT_1,
                             INT_2, ELSE_1, ELSE_2, ELSE_3, WHILE_1, WHILE_2, WHILE_3,
                             WHILE_4, RETURN_1, RETURN_2, RETURN_3, RETURN_4, RETURN_5,
                             WAIN_2, WAIN_3, PRINTLN_1, PRINTLN_2, PRINTLN_3, PRINTLN_4,
                             PRINTLN_5, PRINTLN_6, NULL_1, NULL_2, NULL_3, NEW_1, NEW_2,
                             DELETE_1, DELETE_2, DELETE_3, DELETE_4, DELETE_5};
        for (size_t i = 0; i < transitionFunction.size(); ++i) {
            for (size_t j = 0; j < transitionFunction[0].size(); ++j) {
                transitionFunction[i][j] = FAIL;
            }
        }
        registerTransition(START, isalpha, ID);
        registerTransition(START, "0", ZERO);
        registerTransition(START, "123456789", NUM);
        registerTransition(START, ";", SEMI);
        registerTransition(START, ",", COMMA);
        registerTransition(START, "(", LPAREN);
        registerTransition(START, ")", RPAREN);
        registerTransition(START, "(", LPAREN);
        registerTransition(START, "[", LBRACK);
        registerTransition(START, "]", RBRACK);         
        registerTransition(START, "{", LBRACE);                          
        registerTransition(START, "}", RBRACE);
        registerTransition(START, "<", LT);
        registerTransition(START, ">", GT);
        registerTransition(START, "=", BECOMES);
        registerTransition(START, "!", EXCLAM_1);
        registerTransition(START, "+", PLUS);
        registerTransition(START, "-", MINUS);
        registerTransition(START, "/", SLASH);
        registerTransition(START, "%", PCT);
        registerTransition(START, "&", AMP);
        registerTransition(START, "*", STAR);
        registerTransition(START, isspace, WHITESPACE);
        registerTransition(START, "i", INT_1);
        registerTransition(START, "e", ELSE_1);
        registerTransition(START, "w", WHILE_1);
        registerTransition(START, "r", RETURN_1);
        registerTransition(START, "p", PRINTLN_1);
        registerTransition(START, "N", NULL_1);
        registerTransition(START, "d", DELETE_1);
        registerTransition(START, "n", NEW_1);
        registerTransition(SLASH, "/", COMMENT);
        registerTransition(ID, isalnum, ID);
        registerTransition(NUM, isdigit, NUM);
        registerTransition(LT, "=", LE);
        registerTransition(GT, "=", GE);
        registerTransition(BECOMES, "=", EQ);
        registerTransition(EXCLAM_1, "=", NE);
        registerTransition(COMMENT, [](int c) -> int { return c != '\n'; }, COMMENT);
        registerTransition(WHITESPACE, isspace, WHITESPACE);
        registerTransition(INT_1, isalnum, ID);
        registerTransition(INT_2, isalnum, ID);
        registerTransition(ELSE_1, isalnum, ID);
        registerTransition(ELSE_2, isalnum, ID);
        registerTransition(ELSE_3, isalnum, ID);
        registerTransition(WHILE_1, isalnum, ID);
        registerTransition(WHILE_2, isalnum, ID);
        registerTransition(WHILE_3, isalnum, ID);
        registerTransition(WHILE_4, isalnum, ID);
        registerTransition(RETURN_1, isalnum, ID);
        registerTransition(RETURN_2, isalnum, ID);
        registerTransition(RETURN_3, isalnum, ID);
        registerTransition(RETURN_4, isalnum, ID);
        registerTransition(RETURN_5, isalnum, ID);
        registerTransition(WAIN_2, isalnum, ID);
        registerTransition(WAIN_3, isalnum, ID);
        registerTransition(PRINTLN_1, isalnum, ID);
        registerTransition(PRINTLN_2, isalnum, ID);       
        registerTransition(PRINTLN_3, isalnum, ID);
        registerTransition(PRINTLN_4, isalnum, ID);
        registerTransition(PRINTLN_5, isalnum, ID);       
        registerTransition(PRINTLN_6, isalnum, ID);
        registerTransition(NULL_1, isalnum, ID);
        registerTransition(NULL_2, isalnum, ID);        
        registerTransition(NULL_3, isalnum, ID);
        registerTransition(NEW_1, isalnum, ID);
        registerTransition(NEW_2, isalnum, ID);        
        registerTransition(DELETE_1, isalnum, ID);
        registerTransition(DELETE_2, isalnum, ID);       
        registerTransition(DELETE_3, isalnum, ID);
        registerTransition(DELETE_4, isalnum, ID);
        registerTransition(DELETE_5, isalnum, ID);
        registerTransition(INT_1, "n", INT_2);
        registerTransition(INT_2, "t", INT);
        registerTransition(INT_1, "f", IF);
        registerTransition(ELSE_1, "l", ELSE_2);
        registerTransition(ELSE_2, "s", ELSE_3);
        registerTransition(ELSE_3, "e", ELSE);
        registerTransition(WHILE_1, "h", WHILE_2);
        registerTransition(WHILE_2, "i", WHILE_3);
        registerTransition(WHILE_3, "l", WHILE_4);
        registerTransition(WHILE_4, "e", WHILE);
        registerTransition(RETURN_1, "e", RETURN_2);
        registerTransition(RETURN_2, "t", RETURN_3);
        registerTransition(RETURN_3, "u", RETURN_4);
        registerTransition(RETURN_4, "r", RETURN_5);
        registerTransition(RETURN_5, "n", RETURN);
        registerTransition(WHILE_1, "a", WAIN_2);
        registerTransition(WAIN_2, "i", WAIN_3);
        registerTransition(WAIN_3, "n", WAIN);
        registerTransition(PRINTLN_1, "r", PRINTLN_2);
        registerTransition(PRINTLN_2, "i", PRINTLN_3);
        registerTransition(PRINTLN_3, "n", PRINTLN_4);
        registerTransition(PRINTLN_4, "t", PRINTLN_5);
        registerTransition(PRINTLN_5, "l", PRINTLN_6);
        registerTransition(PRINTLN_6, "n", PRINTLN);
        registerTransition(NULL_1, "U", NULL_2);
        registerTransition(NULL_2, "L", NULL_3);
        registerTransition(NULL_3, "L", NUL);
        registerTransition(NEW_1, "e", NEW_2);
        registerTransition(NEW_2, "w", NEW);
        registerTransition(DELETE_1, "e", DELETE_2);
        registerTransition(DELETE_2, "l", DELETE_3);
        registerTransition(DELETE_3, "e", DELETE_4);
        registerTransition(DELETE_4, "t", DELETE_5);
        registerTransition(DELETE_5, "e", DELETE);
        registerTransition(RETURN, isalnum, ID);
        registerTransition(IF, isalnum, ID);
        registerTransition(ELSE, isalnum, ID);
        registerTransition(WHILE, isalnum, ID);
        registerTransition(PRINTLN, isalnum, ID);
        registerTransition(WAIN, isalnum, ID);
        registerTransition(NEW, isalnum, ID);
        registerTransition(DELETE, isalnum, ID);
        registerTransition(NUL, isalnum, ID);
        registerTransition(INT, isalnum, ID);
    }
    // Register a transition on all chars in chars
    void registerTransition(State oldState, const std::string &chars,
        State newState) {
      for (char c : chars) {
        transitionFunction[oldState][c] = newState;
      }
    }

    // Register a transition on all chars matching test
    // For some reason the cctype functions all use ints, hence the function
    // argument type.
    void registerTransition(State oldState, int (*test)(int), State newState) {

      for (int c = 0; c < 128; ++c) {
        if (test(c)) {
          transitionFunction[oldState][c] = newState;
        }
      }
    }

    /* Returns the state corresponding to following a transition
     * from the given starting state on the given character,
     * or a special fail state if the transition does not exist.
     */
    State transition(State state, char nextChar) const {
      return transitionFunction[state][nextChar];
    }

    /* Checks whether the state returned by transition
     * corresponds to failure to transition.
     */
    bool failed(State state) const { return state == FAIL; }

    /* Checks whether the state returned by transition
     * is an accepting state.
     */
    bool accept(State state) const {
      return acceptingStates.count(state) > 0;
    }

    /* Returns the starting state of the DFA
     */
    State start() const { return START; }
};

vector<Token> scan(const string &input) {
    static Wlp4DFA theDFA;
    vector<Token> tokens = theDFA.simplifiedMaximalMunch(input);

    vector<Token> newTokens;

    for (auto &token : tokens) {
        if (token.getKind() != Token::Kind::COMMENT) {
            newTokens.push_back(token);
        }
    }
    return newTokens;
}

int main() {
    string line;
    bool seperate = 0;
    try {
        while (getline(cin, line)) {
            vector<Token> tokenLine = scan(line);
            for (auto &token : tokenLine) {
                if (seperate && token.getKind() != Token::WHITESPACE &&
                        (token.getKind() == Token::ID || token.getKind() == Token::NUM ||
                         token.getKind() == Token::RETURN || token.getKind() == Token::IF ||
                         token.getKind() == Token::ELSE || token.getKind() == Token::WHILE ||
                         token.getKind() == Token::PRINTLN || token.getKind() == Token::WAIN ||
                         token.getKind() == Token::INT || token.getKind() == Token::NEW ||
                         token.getKind() == Token::DELETE || token.getKind() == Token::EQ ||
                         token.getKind() == Token::NE || token.getKind() == Token::LT ||
                         token.getKind() == Token::LE || token.getKind() == Token::GT ||
                         token.getKind() == Token::GE || token.getKind() == Token::BECOMES)) {

                    throw ScanningFailure{"ERROR: Expecting seperation by whitespace, but receieved: " + token.getLexeme() + " instead"};
                }
                if (token.getKind() == Token::ID || token.getKind() == Token::NUM ||
                    token.getKind() == Token::RETURN || token.getKind() == Token::IF ||
                    token.getKind() == Token::ELSE || token.getKind() == Token::WHILE ||
                    token.getKind() == Token::PRINTLN || token.getKind() == Token::WAIN ||
                    token.getKind() == Token::INT || token.getKind() == Token::NEW ||
                    token.getKind() == Token::DELETE || token.getKind() == Token::EQ ||
                    token.getKind() == Token::NE || token.getKind() == Token::LT ||
                    token.getKind() == Token::LE || token.getKind() == Token::GT ||
                    token.getKind() == Token::GE || token.getKind() == Token::BECOMES) {
                    seperate = 1;
                } else {
                    seperate = 0;
                }
                if (token.getKind() == Token::NUM && token.toLong() > INT_MAX) {
                    throw ScanningFailure{"ERROR: Cannot initialize number bigger than 2^31 - 1, received: " + to_string(token.toLong())};
                }
                if (token.getKind() != Token::WHITESPACE) {
                    cout << token << endl;
                }
            }
        }
    } catch (ScanningFailure &f) {
            cerr << f.what() << endl;
            return 1;
    }
}
