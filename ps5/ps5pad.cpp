#include "ps5pad.hpp"

namespace pad {

  namespace ps5 {
    PS5Editor::PS5Editor() {
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

      this->axis_max_ = std::numeric_limits<uint8_t>::max();
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
          axis_event_.id = id;
          float fval = static_cast<float>(val) / axis_max_;
          if (fabs(fval) < deadzone_) fval = 0.0;
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


    DualSense::DualSense(std::string device_name):
      GamePad(device_name, dev::button_num, dev::axis_num)
    {
      std::vector<Button*> buttons = {
        &Cross, &Circle, &Triangle, &Square,
        &L1, &R1, &L2, &R2,
        &Create, &Option, &Ps,
        &L3, &R3,
        &Left, &Right, &Up, &Down
      };

      std::vector<Axis*> axes = {
        &(LStick.x),
        &(LStick.y),
        &(L2.depth),
        &(RStick.x),
        &(RStick.y),
        &(R2.depth)
      };

      this->attachUI(buttons);
      this->attachUI(axes);
    }
  }
}