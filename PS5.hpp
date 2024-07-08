#pragma once

#include "Pad.hpp"

namespace pad {
  namespace ps5 {

    enum class Connect {
      USB, Bluetooth
    };


    namespace dev {
      const std::string usb = "\"Sony Interactive Entertainment DualSense Wireless Controller\"";
      const std::string bluetooth = "\"DualSense Wireless Controller\"";

      const int init_button_event_freq = 12;
      const int init_axis_event_freq = 8;
      const int total_button_num = 17;
      const int total_axis_num = 6;
    }

    namespace id {
      // Button event number
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

      // Axis event number
      const int leftX   = 0;
      const int leftY   = 1;
      const int L2depth = 2;
      const int rightX  = 3;
      const int rightY  = 4;
      const int R2depth = 5;
      const int crossX  = 6;
      const int crossY  = 7;
    }

    class PS5Handler: public EventHandler {
     private:
      int16_t deadzone_{common::default_deadzone};
      int pre_crossXid_;
      int pre_crossYid_;

      void handleCrossXData(int16_t val);
      void handleCrossYData(int16_t val);
      void handleButtonEvent() override;
      void handleAxisEvent() override;

     public:
      void editEvent(PadEvent event) override;
    };

    class DualSense {
     private:
      PadReader  reader_;
      PS5Handler handler_;
      ButtonData buttons_;
      AxisData   axes_;

     public:
      Button Cross     {id::cross};
      Button Circle    {id::circle};
      Button Triangle  {id::triangle};
      Button Square    {id::square};
      Button L1        {id::L1};
      Button R1        {id::R1};
      TriggerButton L2 {id::L2, id::L2depth};
      TriggerButton R2 {id::R2, id::R2depth};
      Button Create    {id::create};
      Button Option    {id::option};
      Button PS        {id::ps};
      Button L3        {id::L3};
      Button R3        {id::R3};
      Button Left      {id::left};
      Button Right     {id::right};
      Button Up        {id::up};
      Button Down      {id::down};

      Stick Lstick {id::leftX, id::leftY};
      Stick Rstick {id::rightX, id::rightY};

      DualSense(Connect mode);
      ~DualSense();

      void update();
      bool isConnected();
    };

  }
}