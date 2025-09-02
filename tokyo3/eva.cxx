#include <iostream>

int main(int argc, char* argv[]) {
  std::cout << "NERV READY" << '\n';
  std::cout << "INITIALIZING APP" << '\n';
  std::cout << "STARTED.";

  for (int i = 0; i < argc; i++) {
    std::cout << "Argument " << i << ":" << argv[i] << '\n';
  }
  return 0;
}