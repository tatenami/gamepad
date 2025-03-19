#ifndef PS5_BASE_H
#define PS5_BASE_H

#include "../padbase.hpp"

namespace pad {

  namespace ps5 {
    namespace dev {
      const std::string usb_name = "Sony Interactive Entertainment DualSense Wireless Controller";
      const std::string bluetooth_name = "DualSense Wireless Controller";

      const int button_num = 17;
      const int axis_num = 6;
    }

    const float default_deadzone = 0.05; 

    namespace ButtonID {
      const ui_id cross    = 0;
      const ui_id circle   = 1;
      const ui_id triangle = 2;
      const ui_id square   = 3;
      const ui_id L1       = 4;
      const ui_id R1       = 5;
      const ui_id L2       = 6;
      const ui_id R2       = 7;
      const ui_id create   = 8;
      const ui_id option   = 9;
      const ui_id ps       = 10;
      const ui_id L3       = 11;
      const ui_id R3       = 12;
      const ui_id left     = 13;
      const ui_id right    = 14;
      const ui_id up       = 15;
      const ui_id down     = 16;
    }

    namespace AxisID {
      const ui_id leftX   = 0;
      const ui_id leftY   = 1;
      const ui_id L2depth = 2;
      const ui_id rightX  = 3;
      const ui_id rightY  = 4;
      const ui_id R2depth = 5;
      const ui_id crossX  = 6;
      const ui_id crossY  = 7;
    }

    class PS5Editor: public PadEventEditor {
     private:
      int pre_crossXid_;
      int pre_crossYid_;
      unsigned int axis_max_;

      void editCrossXData(int32_t val);
      void editCrossYData(int32_t val);
      void editButtonEvent() override;
      void editAxisEvent() override;

     public:
      PS5Editor();
    };
  }
}

#endif // XXX_H