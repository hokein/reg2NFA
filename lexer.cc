#include "lexer.h"

namespace reg {

Token Lexer::NextToken() {
  while (hasNext()) {
    switch(data_[pos_]) {
      case ' ':
        consume();
        continue;
      case '*':
        consume();
        return Token(Token::kStar, "*");
      case '+':
        consume();
        return Token(Token::kPlus, "+");
      case '|':
        consume();
        return Token(Token::kOr, "|");
      case '?':
        consume();
        return Token(Token::kAlter, "?");
      case '(':
        consume();
        return Token(Token::kLBrack, "(");
      case ')':
        consume();
        return Token(Token::kRBrack, ")");
      default:
        if (isLetterOrDigit()) {
          return Token(Token::kLetter, std::string(1, data_[pos_++]));
        }
    }
  }
  return Token(Token::kEnd);
}

bool Lexer::hasNext() {
  return pos_ < data_.size();
}

void Lexer::consume() {
  ++pos_;
}

bool Lexer::isLetterOrDigit() {
  return (data_[pos_] >= 'a' && data_[pos_] <= 'z') ||
         (data_[pos_] >= 'A' && data_[pos_] <= 'Z') ||
         (data_[pos_] >= '1' && data_[pos_] <= '9');
}

}
