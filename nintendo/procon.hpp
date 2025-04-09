#ifndef PROCON_H
#define PROCON_H

#include "pad/padbase.hpp"

namespace pad {

  namespace nintendo {
    
    namespace dev {
      const std::string usb_name = "Nintendo Co., Ltd. Pro Controller";
      const std::string bluetooth_name = "Pro Controller";

      const int button_num = 18;
      const int axis_num = 4;
    }

    const float default_deadzone = 0.05;

    namespace ButtonID {
      const ui_id B = 0;
      const ui_id A = 1;
      const ui_id X = 2;
      const ui_id Y = 3;
      const ui_id L = 4;
      const ui_id R = 5;
      const ui_id ZL = 6;
      const ui_id ZR = 7;
      const ui_id minus = 8;
      const ui_id plus = 9;
      const ui_id home = 10;
      const ui_id cpature = 11;
      const ui_id Ls = 12;
      const ui_id Rs = 13;
      const ui_id left = 14;
      const ui_id right = 15;
      const ui_id up = 16;
      const ui_id down = 17;
    }

    namespace AxisID {
      const ui_id leftX = 0;
      const ui_id leftY = 1;
      const ui_id rightX = 2;
      const ui_id rightY = 3;
      const ui_id crossX = 4;
      const ui_id crossY = 5;
    }

    class ProconEditor: public PadEventEditor {
      private:
      int pre_crossXid_;
      int pre_crossYid_;
      unsigned int axis_max_;

      void editCrossXData(int32_t val);
      void editCrossYData(int32_t val);
      void editButtonEvent() override;
      void editAxisEvent() override;

      public:
      ProconEditor();
    };
  }
}

#endif // PROCON_H