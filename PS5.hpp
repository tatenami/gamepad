#ifndef PS5_H
#define PS5_H

#include <climits>
#include <memory>
#include "Pad.hpp"

namespace Pad {
  namespace PS5 {

    enum class Connect {
      USB, BlueTooth
    }; 

    enum class button {
      CROSS = 0,
      CIRCLE,
      TRIANGLE,
      SQUARE,
      L1,
      R1,
      L2,
      R2,
      CREATE,
      OPTION,
      HOME,
      L3,
      R3,
      LEFT,
      RIGHT,
      UP,
      DOWN
    };

    enum class axis {
      LEFT_X = 0,
      LEFT_Y,
      L2_DEPTH,
      RIGHT_X,
      RIGHT_Y,
      R2_DEPTH
    };

    namespace dev {
      extern const std::string wired_dev_name;
      extern const std::string wireless_dev_name;

      extern const int total_button_num;
      extern const int total_axis_num;

      extern const uint16_t default_deadzone;
    };

    class DualSense {
     private:
      std::unique_ptr<PadReader> reader_;
      PadData  pad_data_;
      PadEvent event_;
      uint16_t axis_deadzone_;

     private:
      void handleButtonEvent();
      void handleAxisEvent();

     public:
      DualSense(PS5::Connect connect_state);
      ~DualSense();

      void update();
      void setDeadzone(uint16_t deadzone);
      bool isConnected();
      bool isON(PS5::button button);
      bool isPushed(PS5::button button);
      bool isReleased(PS5::button button);
      int  getAxisValue(PS5::axis axis);
    };

  }
}

#endif // PS5_H