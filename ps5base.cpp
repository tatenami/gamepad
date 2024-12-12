#include "ps5base.hpp"

namespace pad {

  namespace ps5 {
    const uint16_t default_deadzone = 5;

    PS5Handler::PS5Handler() {

    }

    void PS5Handler::editCrossXData(int32_t val) {
      if (val > 0) {
        this->pre_crossXid_ = id::right;
        button_event_ = {id::right, true};
      }
      else if (val < 0)  {
        this->pre_crossXid_ = id::left;
        button_event_ = {id::left, true};
      }
      else {
        switch (this->pre_crossXid_) {
          case (id::right): button_event_.id = id::right; break;
          case (id::left):  button_event_.id = id::left;  break;
        }
        button_event_.state = false;
      }
    }

    void PS5Handler::editCrossYData(int32_t val) {
      if (val > 0) {
        this->pre_crossXid_ = id::down;
        button_event_ = {id::down, true};
      }
      else if (val < 0)  {
        this->pre_crossXid_ = id::up;
        button_event_ = {id::up, true};
      }
      else {
        switch (this->pre_crossXid_) {
          case (id::down): button_event_.id = id::down; break;
          case (id::up):   button_event_.id = id::up;  break;
        }
        button_event_.state = false;
      }
    }

    void PS5Handler::editAxisEvent() {
      int32_t val = event_.value;
      uint8_t id  = id_map_[event_.code]; 

      switch (id) {
        case (id::crossX): {
          event_.type = EventType::Button;
          editCrossXData(val);
          return;
        }
        case (id::crossY): {
          event_.type = EventType::Button;
          editCrossYData(val);
          return;
        }
      }

      if (id != id::L2depth && id != id::R2depth) {
        val *= 2;
        val -= axis_range_;
      }

      switch (id) {
        case (id::leftY):
        case (id::rightY): {
          val *= -1;
        }
        default: {
          if (abs(val) < deadzone_) val = 0;
          axis_event_.id = id;
          axis_event_.value = static_cast<float>(val) / axis_range_;
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