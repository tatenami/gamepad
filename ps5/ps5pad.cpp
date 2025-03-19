#include "ps5pad.hpp"

namespace pad {

  namespace ps5 {
    PS5Editor::PS5Editor() {

      id_map_ = code_id_map {
        {BTN_SOUTH,  ButtonID::cross},
        {BTN_EAST,   ButtonID::circle},
        {BTN_NORTH,  ButtonID::triangle},
        {BTN_WEST,   ButtonID::square},
        {BTN_TL,     ButtonID::L1},
        {BTN_TR,     ButtonID::R1},
        {BTN_TL2,    ButtonID::L2},
        {BTN_TR2,    ButtonID::R2},
        {BTN_SELECT, ButtonID::create},
        {BTN_START,  ButtonID::option},
        {BTN_MODE,   ButtonID::ps},
        {BTN_THUMBL, ButtonID::L3},
        {BTN_THUMBR, ButtonID::R3},

        {ABS_X,  AxisID::leftX},
        {ABS_Y,  AxisID::leftY},
        {ABS_RX, AxisID::rightX},
        {ABS_RY, AxisID::rightY},
        {ABS_Z,  AxisID::L2depth},
        {ABS_RZ, AxisID::R2depth},
        {ABS_HAT0X,  AxisID::crossX},
        {ABS_HAT0Y,  AxisID::crossY},
      };

      this->axis_max_ = std::numeric_limits<unsigned char>::max();
      this->deadzone_ = default_deadzone;
    }

    void PS5Editor::editCrossXData(int32_t val) {
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

    void PS5Editor::editCrossYData(int32_t val) {
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

    void PS5Editor::editAxisEvent() {
      int32_t val = event_.value;
      ui_id   id  = id_map_[event_.code]; 

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

      // 値の範囲を-axis_max <--> axis_max に拡張
      if (id != AxisID::L2depth && id != AxisID::R2depth) {
        val = (val * 2) - axis_max_;
      }

      switch (id) {
        // Y軸の上側が+になるよう反転
        case (AxisID::leftY):
        case (AxisID::rightY): {
          val *= -1;
        }
        default: {
          // axis値を-1.0 <--> 1.0 に
          axis_event_.id = id;
          float fval = static_cast<float>(val) / axis_max_;
          if (fabs(fval) < deadzone_) { 
            fval = 0.0;
          }
          axis_event_.value = fval;
        }
      }
    }

    void PS5Editor::editButtonEvent() {
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