#ifndef PS5_PAD_H
#define PS5_PAD_H

#include "ps5base.hpp"

namespace pad {

  namespace ps5 {

    class DualSense: public BasePad<PS5Handler> {
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


      DualSense(Connect connect_type);
    };

  }

}

#endif // XXX_H