#include "parser.h"

#include <iostream>
#include <vector>


int main() {
  std::string s;
  while (std::cin >> s) {
    reg::Parser parser(s);
    parser.GenerateNFA();
    std::cout << parser.ToDotLanguage() << std::endl;
    parser.WriteDotToFile("result.dot");
  }
  return 0;
}
