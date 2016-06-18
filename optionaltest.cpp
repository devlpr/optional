#include "optional.h"
#include <string>
#include <iostream>

/**
 * Examples of use for Optional class
 */

int
main(int argc, const char* argv[])
{
  std::cerr << "Iteration using old style iterators" << std::endl;
  std::string s("24");
  Optional<std::string*> opt(&s);
  for (auto it = opt.begin(); it != opt.end(); ++it)  {
    std::cerr << "Item: " << **it << std::endl;
  }

  std::cerr << "Range Based for()" << std::endl;
  std::string s2("42");
  Optional<std::string> opt2(s2);
  for (auto& d : opt2)  {
    std::cerr << "Data: " << d << std::endl;
  }

  std::cerr << "Emplace semantics" << std::endl;
  const char* n = "35";
  Optional<std::string> opt3(n); // Pass args to type T constructor, emplace construction
  for (auto& d : opt3)  {
    std::cerr << "Emplaced Data: " << d << std::endl;
  }

  Optional<std::string> opt4("90"); // Emplace construction
  std::cerr << "Get: " << opt4.get() << std::endl;

  Optional<std::string> opt5;
  if (opt5.defined()) {
    std::cerr << "Get: " << opt5.get() << std::endl;
  }
  else if (opt5.empty()) {
    std::cerr << "Optional is empty" << std::endl;
    std::cerr << "Test throws during empty access." << std::endl;
    try {
      std::cerr << "Get: " << opt5.get() << std::endl;
    }
    catch (const std::runtime_error& exc) {
      std::cerr << "Successfully caught the get() exception\n\t"
                << exc.what()
                << std::endl;
    }
  }

  return 0;
}
