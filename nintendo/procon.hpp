#ifndef PROCON_H
#define PROCON_H

#include "pad/pad.hpp"

namespace pad {

  namespace nintendo {
    
    namespace dev {
      const std::string usb_name = "Nintendo Co., Ltd. Pro Controller";

      const int button_num = 18;
      const int axis_num = 4;
    }

    const float default_deadzone = 0.05;

    namespace ButtonID {
      const int B = 0;
      const int A = 1;
      const int X = 2;
      const int Y = 3;
      const int L = 4;
      const int R = 5;
      const int ZL = 6;
      const int ZR = 7;
      const int minus = 8;
      const int plus = 9;
      const int home = 10;
      const int cpature = 11;
      const int Ls = 12;
      const int Rs = 13;
      const int left = 14;
      const int right = 15;
      const int up = 16;
      const int down = 17;
    }

    namespace AxisID {
      const int leftX = 0;
      const int leftY = 1;
      const int rightX = 2;
      const int rightY = 3;
      const int crossX = 4;
      const int crossY = 5;
    }

    class ProconEditor: public PadEventEditor {
      private:
      int pre_crossXid_;
      int pre_crossYid_;
      uint32_t axis_max_;

      void editCrossXData(int32_t val);
      void editCrossYData(int32_t val);
      void editButtonEvent() override;
      void editAxisEvent() override;

      public:
      ProconEditor();
    };

    class Procon: public GamePad<ProconEditor> {
     public:
      Button B  {ButtonID::B};
      Button A  {ButtonID::A};
      Button X  {ButtonID::X};
      Button Y  {ButtonID::Y};
      Button L  {ButtonID::L};
      Button R  {ButtonID::R};
      Button ZL {ButtonID::ZL};
      Button ZR {ButtonID::ZR};
      Button Minus {ButtonID::minus};
      Button Plus  {ButtonID::plus};
      Button Home  {ButtonID::home};
      Button Capture {ButtonID::cpature};
      Button LS    {ButtonID::Ls};
      Button RS    {ButtonID::Rs};
      Button Left  {ButtonID::left};
      Button Right {ButtonID::right};
      Button Up    {ButtonID::up};
      Button Down  {ButtonID::down};

      Stick LStick {AxisID::leftX,  AxisID::leftY};
      Stick RStick {AxisID::rightX, AxisID::rightY};

      Procon(std::string device_name);
    };
  }
}

#endif // PROCON_H