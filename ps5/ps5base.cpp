#include "ps5base.hpp"

namespace pad {

  namespace ps5 {
    const uint16_t default_deadzone = 5;

    PS5Handler::PS5Handler() {
      addCodeIdEntry(BTN_SOUTH,  ButtonID::cross);
      addCodeIdEntry(BTN_EAST,   ButtonID::circle);
      addCodeIdEntry(BTN_NORTH,  ButtonID::triangle);
      addCodeIdEntry(BTN_WEST,   ButtonID::square);
      addCodeIdEntry(BTN_TL,     ButtonID::L1);
      addCodeIdEntry(BTN_TR,     ButtonID::R1);
      addCodeIdEntry(BTN_TL2,    ButtonID::L2);
      addCodeIdEntry(BTN_TR2,    ButtonID::R2);
      addCodeIdEntry(BTN_SELECT, ButtonID::create);
      addCodeIdEntry(BTN_START,  ButtonID::option);
      addCodeIdEntry(BTN_MODE,   ButtonID::ps);
      addCodeIdEntry(BTN_THUMBL, ButtonID::L3);
      addCodeIdEntry(BTN_THUMBR, ButtonID::R3);

      addCodeIdEntry(ABS_X,  AxisID::leftX);
      addCodeIdEntry(ABS_Y,  AxisID::leftY);
      addCodeIdEntry(ABS_RX, AxisID::rightX);
      addCodeIdEntry(ABS_RY, AxisID::rightY);
      addCodeIdEntry(ABS_Z,  AxisID::L2depth);
      addCodeIdEntry(ABS_RZ, AxisID::R2depth);
      addCodeIdEntry(ABS_HAT0X, AxisID::crossX);
      addCodeIdEntry(ABS_HAT0Y, AxisID::crossY);
    }

    void PS5Handler::editCrossXData(int32_t val) {
      if (val > 0) {
        this->pre_crossXid_ = ButtonID::right;
        button_event_ = {ButtonID::right, true};
      }
      else if (val < 0)  {
        this->pre_crossXid_ = ButtonID::left;
        button_event_ = {ButtonID::left, true};
      }
      else {
        switch (this->pre_crossXid_) {
          case (ButtonID::right): {
            button_event_.id = ButtonID::right; 
            break;
          }
          case (ButtonID::left): {
            button_event_.id = ButtonID::left;  
            break;
          }
        }
        button_event_.state = false;
      }
    }

    void PS5Handler::editCrossYData(int32_t val) {
      if (val > 0) {
        this->pre_crossXid_ = ButtonID::down;
        button_event_ = {ButtonID::down, true};
      }
      else if (val < 0)  {
        this->pre_crossXid_ = ButtonID::up;
        button_event_ = {ButtonID::up, true};
      }
      else {
        switch (this->pre_crossXid_) {
          case (ButtonID::down): {
            button_event_.id = ButtonID::down; 
            break;
          }
          case (ButtonID::up): {  
            button_event_.id = ButtonID::up;  
            break;
          }
        }
        button_event_.state = false;
      }
    }

    void PS5Handler::editAxisEvent() {
      int32_t val = event_.value;
      uint8_t id  = id_map_[event_.code]; 

      switch (id) {
        case (AxisID::crossX): {
          event_.type = EventType::Button;
          editCrossXData(val);
          return;
        }
        case (AxisID::crossY): {
          event_.type = EventType::Button;
          editCrossYData(val);
          return;
        }
      }

      if (id != AxisID::L2depth && id != AxisID::R2depth) {
        val *= 2;
        val -= axis_max_;
      }

      switch (id) {
        case (AxisID::leftY):
        case (AxisID::rightY): {
          val *= -1;
        }
        default: {
          if (abs(val) < deadzone_) val = 0;
          axis_event_.id = id;
          axis_event_.value = static_cast<float>(val) / axis_max_;
        }
      }
    }

    void PS5Handler::editButtonEvent() {
      button_event_.id = id_map_[event_.code];

      switch (event_.value) {
        case 0: {
          button_event_.state = false;
          break;
        }
        case 1: {
          button_event_.state = true;
          break;
        }
      }
    }
  }

}