#include "PS5.hpp"
#include <iostream>

using namespace std;
using namespace pad;
using namespace pad::ps5;

int main () {
  DualSense ps5(Connect::USB);

  Button* target = ps5.Left.get();
  // Axis* x = ps5.Lstick.x.get();
  // Axis* y = ps5.Lstick.y.get();

  while (ps5.isConnected()) {
    ps5.update();

    if (target->pushed()) {
      cout << "Pushed" << endl;
    }
    else if (target->released()) {
      cout << "Released" << endl;
    }

   // cout << "x: " << x->getValue() << " y: " << y->getValue() << endl;

    usleep(50);

    if (ps5.Option.pushed()) { break; }
  }

  return 0;
}