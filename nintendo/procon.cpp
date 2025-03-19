#include "procon.hpp"

namespace pad {
  
  namespace nintendo {

    ProconEditor::ProconEditor() {
      id_map_ = code_id_map {
        {BTN_SOUTH,  ButtonID::B},
        {BTN_EAST,   ButtonID::A},
        {BTN_NORTH,  ButtonID::X},
        {BTN_WEST,   ButtonID::Y},
        {BTN_TL,     ButtonID::L},
        {BTN_TR,     ButtonID::R},
        {BTN_TL2,    ButtonID::ZL},
        {BTN_TR2,    ButtonID::ZR},
        {BTN_SELECT, ButtonID::minus},
        {BTN_START,  ButtonID::plus},
        {BTN_MODE,   ButtonID::home},
        {BTN_Z,      ButtonID::cpature},
        {BTN_THUMBL, ButtonID::Ls},
        {BTN_THUMBR, ButtonID::Rs},

        {ABS_X,   AxisID::leftX},
        {ABS_Y,   AxisID::leftY},
        {ABS_RX,  AxisID::rightX},
        {ABS_RY,  AxisID::rightY},
        {ABS_HAT0X,  AxisID::crossX},
        {ABS_HAT0Y,  AxisID::crossY},
      };

      this->axis_max_ = std::numeric_limits<int16_t>::max();
      this->deadzone_ = default_deadzone;
    }

    void ProconEditor::editCrossXData(int32_t val) {
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

    void ProconEditor::editCrossYData(int32_t val) {
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

    void ProconEditor::editAxisEvent() {
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

      switch (id) {
        case (AxisID::leftY):
        case (AxisID::rightY): {
          val *= -1;
        }
        default: {
          axis_event_.id = id;
          float fval = static_cast<float>(val) / axis_max_;
          if (fabs(fval) < deadzone_) fval = 0.0;
          axis_event_.value = fval;
        }
      }
    }

    void ProconEditor::editButtonEvent() {
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