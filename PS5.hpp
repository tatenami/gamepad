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

    namespace evnum {
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

    class PS5PadData: public PadData {
     private:
      void handleCrossXData(int16_t val);
      void handleCrossYData(int16_t val);
      void handleButtonEvent(Event event) override;
      void handleAxisEvent(Event event) override;

     public:
      using PadData::PadData;
      void update(Event event) override;
    };

    class DualSense {
     private:
      bool is_connected_ = false; 
      PadReader  reader_;
      PS5PadData data_;

     public:
      Button Cross     {evnum::cross};
      Button Circle    {evnum::circle};
      Button Triangle  {evnum::triangle};
      Button Square    {evnum::square};
      Button L1        {evnum::L1};
      Button R1        {evnum::R1};
      TriggerButton L2 {evnum::L2, evnum::L2depth};
      TriggerButton R2 {evnum::R2, evnum::R2depth};
      Button Create    {evnum::create};
      Button Option    {evnum::option};
      Button PS        {evnum::ps};
      Button L3        {evnum::L3};
      Button R3        {evnum::R3};
      Button Left      {evnum::left};
      Button Right     {evnum::right};
      Button Up        {evnum::up};
      Button Down      {evnum::down};

      Stick Lstick {evnum::leftX, evnum::leftY};
      Stick Rstick {evnum::rightX, evnum::rightY};

      DualSense(Connect mode = Connect::USB);
      ~DualSense();

      void init();
      void update();
      bool isConnected();
    }; 

  }
}