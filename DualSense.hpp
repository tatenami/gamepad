#ifndef DUAL_SENSE_H
#define DUAL_SENSE_H

#include <climits>
#include "GamePad.hpp"

namespace ps5 {

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
    extern const std::string wired_device_name;    
    extern const std::string wireless_device_name;

    extern const int total_buttons;
    extern const int total_axes;

    extern const unsigned int default_axis_modify_limit;
  };

  class DualSense: public pad::GamePad {
   private:
    unsigned int axis_modify_limit;

   private:
    void update_button_data() override;
    void update_axis_data() override;

   public:
    DualSense(std::string device_name);
    ~DualSense();

    bool is_ON(ps5::button button);
    bool is_Pushed(ps5::button button);
    bool is_Released(ps5::button button);
    int get_axis_value(ps5::axis axis);
    void set_axis_limit(unsigned int number);
  };

};

#endif // DUAL_SENSE_H