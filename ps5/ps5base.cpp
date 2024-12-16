#include "ps5base.hpp"

namespace pad {

  namespace ps5 {
    const uint16_t default_deadzone = 5;

    PS5Handler::PS5Handler(uint32_t axis_max = std::numeric_limits<uint8_t>::max()):
      PadEventHandler(axis_max)
    {

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