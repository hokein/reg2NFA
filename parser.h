#ifndef PARSER_H_
#define PARSER_H_ 

#include "lexer.h"

#include <set>
#include <string>
#include <vector>

namespace reg {

// Represents each state in NFA.
struct FAState {
  typedef std::pair<Token, FAState*> Edge;
  FAState(int id, bool is_end):id(id), is_end(is_end) {}
  void AddStates(const Token& token, FAState* next) {
    next_states.push_back(Edge(token, next));
  }
  int id;
  bool is_end;
  std::vector<Edge> next_states;
};

struct FSM {
  FSM():start_state(NULL), end_state(NULL) {}
  FSM(FAState* start_state, FAState* end_state):
      start_state(start_state), end_state(end_state) {}
  FAState* start_state;
  FAState* end_state;
};

// A regular expression parser.
//
// Grammar:
//  expression = factor expression
//  factor = term* | term+ | term? | term'|'factor
//  term = a-z | 0-9 | (expression)
class Parser {
 public:
  Parser(const std::string& reg);
  ~Parser();
  void Clear();
  void GenerateNFA();
  std::string ToDotLanguage();
  bool WriteDotToFile(const std::string& file_name);

 private:
  void ReorderStateId(FAState* state);
  void TraversalAllEdges(FAState* state, std::set<int>& vis,  
                         std::ostringstream& sout);
  FSM expression();
  FSM factor();
  FSM term();
  bool reach_end();
  
  void match(Token::Type type);
  void consume();
  void release(FAState* state, std::set<int>& vis);
  
  FSM fsm_;
  int id_;
  Token look_head_;
  Lexer lexer_;
};

}  // namespace reg

#endif  // PARSER_H_
