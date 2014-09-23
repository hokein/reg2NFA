#include "parser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <queue>

namespace {
const std::string kDotLanguageBegin = "\
  digraph finite_state_machine { \n\
  rank=0;1;\n\
  rankdir = LR;\n\
  node [shape = doublecircle];0;\n\
";

const std::string kDotLanguageNodeSetting = "\
  node [shape = plaintext];\n\
  \"\" ->0 [label =\"start\"];\n\
  node [shape = circle];\n\
";

const std::string kDotLanguageEnd = "}";
}

namespace reg {
  
Parser::Parser(const std::string& reg):lexer_(reg), id_(0) {
  look_head_ = lexer_.NextToken();
}
  
Parser::~Parser() {
  Clear();
}

void Parser::GenerateNFA() {
  fsm_ = expression();
  ReorderStateId(fsm_.start_state);
}
  
std::string Parser::ToDotLanguage() {
  std::set<int> vis;
  std::ostringstream end_state_out, sout;
  TraversalAllEdges(fsm_.start_state, vis, sout);
  end_state_out << "  node [shape = doublecircle];"
                << fsm_.end_state->id << ";\n";
  return kDotLanguageBegin + end_state_out.str() +
      kDotLanguageNodeSetting + sout.str() + kDotLanguageEnd;
}

bool Parser::WriteDotToFile(const std::string& file_name) {
  std::ofstream fin(file_name);
  if (!fin) {
    std::cerr << "Cannot create " << file_name << std::endl;
    return false;
  }
  fin << ToDotLanguage();
  return true;
}

void Parser::TraversalAllEdges(FAState* state, std::set<int>& vis,
                               std::ostringstream& sout) {
  if (!state) return;
  vis.insert(state->id);
  for (int i = 0; i < state->next_states.size(); ++i) {
    int next_states_id = (((state->next_states)[i]).second)->id;
    std::string text = state->next_states[i].first.text;
    if (state->next_states[i].first.type != Token::kEmpty)
      sout << "  " << state->id << "->" << next_states_id << " [label = \""
           << text << "\"];\n";
    else
      sout << "  " << state->id << "->" << next_states_id
           << " [label = \"ε\"];\n";
    if (vis.find(next_states_id) == vis.end())
      TraversalAllEdges(state->next_states[i].second, vis, sout);
  }
}

void Parser::Clear() {
  std::set<int> vis;
  release(fsm_.start_state, vis);
  fsm_ = FSM(); // NULL
}
  
void Parser::release(reg::FAState *state, std::set<int> &vis) {
  if (!state)
    return;
  vis.insert(state->id);
  for (int i = 0; i < state->next_states.size(); ++i) {
    if (vis.find(state->id) == vis.end())
      release(state->next_states[i].second, vis);
  }
  delete state;
}
  
FSM Parser::expression() {
  if (reach_end())
    return FSM();
  FSM fsm = factor();
  if (look_head_.type == Token::kLetter
      || look_head_.type == Token::kLBrack) {
    FSM fsm2 = expression();
    fsm.end_state->is_end = false;
    *(fsm.end_state) = *(fsm2.start_state);
    delete fsm2.start_state;
    //fsm.end_state->AddStates(look_head_, fsm2.start_state);
    return FSM(fsm.start_state, fsm2.end_state);
  }
  return fsm;
}

FSM Parser::factor() {
  if (reach_end()) return FSM();
  FSM fsm = term();
  if (look_head_.type == Token::kPlus) { // case: +
    FSM new_fsm(new FAState(id_++, false), new FAState(id_++, true));
    fsm.end_state->is_end = false;
    
    new_fsm.start_state->AddStates(Token(Token::kEmpty), fsm.start_state);
    fsm.end_state->AddStates(Token(Token::kEmpty), new_fsm.end_state);
    fsm.end_state->AddStates(Token(Token::kEmpty), fsm.start_state);
    match(Token::kPlus);
    return new_fsm;
  } else if (look_head_.type == Token::kStar) { // case: *
    FSM new_fsm(new FAState(id_++, false), new FAState(id_++, true));
    fsm.end_state->is_end = false;
    
    new_fsm.start_state->AddStates(Token(Token::kEmpty), new_fsm.end_state);
    new_fsm.start_state->AddStates(Token(Token::kEmpty), fsm.start_state);
    fsm.end_state->AddStates(Token(Token::kEmpty), new_fsm.end_state);
    fsm.end_state->AddStates(Token(Token::kEmpty), fsm.start_state);
    
    match(Token::kStar);
    return new_fsm;
    
  } else if (look_head_.type == Token::kAlter) { // case: ?
    FSM new_fsm(new FAState(id_++, false), new FAState(id_++, true));
    fsm.end_state->is_end = false;
    
    new_fsm.start_state->AddStates(Token(Token::kEmpty), fsm.start_state);
    new_fsm.start_state->AddStates(Token(Token::kEmpty), new_fsm.end_state);
    fsm.end_state->AddStates(Token(Token::kEmpty), new_fsm.end_state);
    
    match(Token::kAlter);
    return new_fsm;
    
  } else if (look_head_.type == Token::kOr) { // case: |
    match(Token::kOr);
    
    FSM fsm2 = factor();
    FSM new_fsm(new FAState(id_++, false), new FAState(id_++, true));
    fsm.end_state->is_end = false;
    fsm2.end_state->is_end = false;
    new_fsm.start_state->AddStates(Token(Token::kEmpty), fsm.start_state);
    new_fsm.start_state->AddStates(Token(Token::kEmpty), fsm2.start_state);
    
    fsm.end_state->AddStates(Token(Token::kEmpty), new_fsm.end_state);
    fsm2.end_state->AddStates(Token(Token::kEmpty), new_fsm.end_state);
    
    return new_fsm;
  }
  return fsm;
}

FSM Parser::term() {
  if (look_head_.type == Token::kLetter) {
    FSM fsm(new FAState(id_++, false), new FAState(id_++, true));
    fsm.start_state->AddStates(look_head_, fsm.end_state);
    match(Token::kLetter);
    return fsm;
  } else if (look_head_.type == Token::kLBrack) {
    match(Token::kLBrack);
    FSM fsm = expression();
    match(Token::kRBrack);
    return fsm;
  } else {
    std::cerr << "Invalid term symbol: " << look_head_.text << std::endl;
  }
  return FSM();
}

void Parser::match(Token::Type type) {
  if (look_head_.type == type) consume();
  else
    std::cerr << "Can not match type" << std::endl;
}

void Parser::consume() {
  look_head_ = lexer_.NextToken();
}

bool Parser::reach_end() {
  return look_head_.type == Token::kEnd;
}

void Parser::ReorderStateId(FAState *state) {
  std::queue<FAState*> q;
  q.push(state);
  std::set<int> vis;
  id_ = 0;
  vis.insert(state->id);
  while (!q.empty()) {
    state = q.front();
    q.pop();
    state->id = id_++;
    for (int i = 0; i < state->next_states.size(); ++i) {
      int next_id = state->next_states[i].second->id;
      if (vis.find(next_id) == vis.end()) {
        q.push(state->next_states[i].second);
        vis.insert(next_id);
      }
    }
  }
}

}  // namespace reg
