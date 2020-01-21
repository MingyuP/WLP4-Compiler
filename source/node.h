#include <vector>
#include <string>

class Node {
  public:
    std::string token;
    std::vector<std::string> toProduce;
    std::string rule;
    std::vector<Node*> children;
    bool factor_procedure_paran = false;
    Node(std::string token, std::vector<std::string> toProduce, std::string rule);
    void addChild(std::string token, std::vector<std::string> toProduce, std::string rule);
    void set_factor_procedure_paran(bool val);
    ~Node();
};
