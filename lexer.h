#ifndef LEXER_H_
#define LEXER_H_

#include "token.h"

#include <string>

namespace reg {

class Lexer {
 public:
  Lexer(const std::string& data): pos_(0), data_(data) {}
  Token NextToken();

 private:
  bool hasNext();
  void consume();
  bool isLetterOrDigit();

  int pos_;
  std::string data_;
};

}

#endif  // LEXER_H_
