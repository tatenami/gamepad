#ifndef PS5_BASE_H
#define PS5_BASE_H

#include "pad/pad.hpp"

namespace pad {

  namespace ps5 {
    namespace dev {
      const std::string usb_name = "\"Sony Interactive Entertainment DualSense Wireless Controller\"";
      const std::string bluetooth_name = "\"DualSense Wireless Controller\"";

      const int button_num = 17;
      const int axis_num = 6;
    }

    const float default_deadzone = 0.05; 

    namespace ButtonID {
      const int cross    = 0;
      const int circle   = 1;
      const int triangle = 2;
      const int square   = 3;
      const int L1       = 4;
      const int R1       = 5;
      const int L2       = 6;
      const int R2       = 7;
      const int create   = 8;
      const int option   = 9;
      const int ps       = 10;
      const int L3       = 11;
      const int R3       = 12;
      const int left     = 13;
      const int right    = 14;
      const int up       = 15;
      const int down     = 16;
    }

    namespace AxisID {
      const int leftX   = 0;
      const int leftY   = 1;
      const int L2depth = 2;
      const int rightX  = 3;
      const int rightY  = 4;
      const int R2depth = 5;
      const int crossX  = 6;
      const int crossY  = 7;
    }

    class PS5Editor: public PadEventEditor {
     private:
      int pre_crossXid_;
      int pre_crossYid_;
      uint32_t axis_max_;

      void editCrossXData(int32_t val);
      void editCrossYData(int32_t val);
      void editButtonEvent() override;
      void editAxisEvent() override;

     public:
      PS5Editor();
    };

    class DualSense: public GamePad<PS5Editor> {
     public:
      Button Cross    {ButtonID::cross};
      Button Circle   {ButtonID::circle};
      Button Triangle {ButtonID::triangle};
      Button Square   {ButtonID::square};
      Button L1       {ButtonID::L1};
      Button R1       {ButtonID::R1};
      Button Create   {ButtonID::create};
      Button Option   {ButtonID::option};
      Button Ps       {ButtonID::ps};
      Button L3       {ButtonID::L3};
      Button R3       {ButtonID::R3};
      Button Left     {ButtonID::left};
      Button Right    {ButtonID::right};
      Button Up       {ButtonID::up};
      Button Down     {ButtonID::down};

      Trigger L2 {ButtonID::L2, AxisID::L2depth};
      Trigger R2 {ButtonID::R2, AxisID::R2depth};

      Stick LStick {AxisID::leftX,  AxisID::leftY};
      Stick RStick {AxisID::rightX, AxisID::rightY};

      DualSense(std::string device_name);
    };
  }
}

#endif // XXX_H