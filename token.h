#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

namespace reg {

struct Token {
  enum Type {
    kLBrack, // (
    kRBrack, // )
    kStar, // *
    kPlus, // +
    kOr, // |
    kAlter, // ?
    kEmpty, //
    kLetter, // [a-z0-9]
    kUnknown, // unknown symbol
    kEnd, // EOF
  };
  Token():type(Token::kUnknown) {}
  Token(Type type):type(type) {}
  Token(Type type, std::string text): type(type), text(text) {}
  
  Type type;
  std::string text;
};

}  // namespace reg

#endif  // TOKEN_H_