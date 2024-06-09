#include "PS5.hpp"

namespace Pad {
  namespace PS5 {

    static const int  leftY_evnum{1};
    static const int rightY_evnum{4};
    static const int crossX_evnum{6};
    static const int crossY_evnum{7};
    static const int left_bnum{static_cast<int>(button::LEFT)};
    static const int right_bnum{static_cast<int>(button::RIGHT)};
    static const int up_bnum{static_cast<int>(button::UP)};
    static const int down_bnum{static_cast<int>(button::DOWN)};

    namespace dev {
      const std::string wireless_dev_name = "\"DualSense Wireless Controller\"";
      const std::string wired_dev_name =    "\"Sony Interactive Entertainment DualSense Wireless Controller\"";

      const int total_button_num = 17;
      const int total_axis_num = 6;

      const uint16_t default_deadzone = 1500;
    }

    DualSense::DualSense(Connect connect_state):
      pad_data_(dev::total_button_num, dev::total_axis_num),
      axis_deadzone_(dev::default_deadzone)
    {
      std::string dev_name;

      switch (connect_state) {
        case Connect::USB: {
          dev_name = dev::wired_dev_name;
          break;  
        }
        case Connect::BlueTooth: {
          dev_name = dev::wireless_dev_name;
          break;
        }
      }

      reader_ = std::make_unique<PadReader>(PadReader(dev_name));
    }

    DualSense::~DualSense() {
      (*reader_).closeDeviceFile();
      if (reader_) reader_.reset();
    }

    void DualSense::handleButtonEvent() {
      pad_data_.updateOldButton();
      pad_data_.setButtonState(event_.number, event_.value);
    }

    void DualSense::handleAxisEvent() {
      int16_t val = event_.value;
      int event_num = event_.number;

      switch (event_num) {
        case crossX_evnum: {
          pad_data_.updateOldButton();
          event_.type = EventType::EVENT_BUTTON;

          if (val > 0) {
            pad_data_.setButtonState(right_bnum, true);
            pad_data_.setButtonState(left_bnum, false);
          }
          else if (val < 0) {
            pad_data_.setButtonState(left_bnum, true);
            pad_data_.setButtonState(right_bnum, false);
          }
          else {
            pad_data_.setButtonState(left_bnum, false);
            pad_data_.setButtonState(right_bnum, false);
          }
          break;
        }
        case crossY_evnum: {
          pad_data_.updateOldButton();
          event_.type = EventType::EVENT_BUTTON;

          if (val > 0) {
            pad_data_.setButtonState(down_bnum, true);
            pad_data_.setButtonState(up_bnum, false);
          }
          else if (val < 0) {
            pad_data_.setButtonState(up_bnum, true);
            pad_data_.setButtonState(down_bnum, false);
          }
          else {
            pad_data_.setButtonState(up_bnum, false);
            pad_data_.setButtonState(down_bnum, false);
          }
          break;
        }
        case leftY_evnum:
        case rightY_evnum: {
          val *= -1;
        }
        default: {
          if (abs(val) < axis_deadzone_) val = 0;
          pad_data_.setAxisData(event_num, val);
        }
      }
    }
    
    void DualSense::update() {
      bool is_connected = (*reader_).isConnected();
      bool is_updated = (*reader_).update();
      
      if (!is_connected || !is_updated) return;

      event_ = (*reader_).getEvent();

      switch (event_.type) {
        case EventType::EVENT_BUTTON: {
          handleButtonEvent();
          break;
        }
        case EventType::EVENT_AXIS: {
          handleAxisEvent();
          break;
        }
        default: break;
      }
    }

    void DualSense::setDeadzone(uint16_t deadzone) { axis_deadzone_ = deadzone; }

    bool DualSense::isConnected() { return (*reader_).isConnected(); }

    bool DualSense::isON(PS5::button button) {
      return pad_data_.getButtonState(static_cast<int>(button));
    }

    bool DualSense::isPushed(PS5::button button) {
      if (event_.type != EventType::EVENT_BUTTON) return false;

      button_pair on = pad_data_.getOldNewButtonPair(static_cast<int>(button));
      bool old_state = std::get<OLD>(on);
      bool new_state = std::get<NEW>(on);
      return (old_state == false) && (new_state == true);
    }

    bool DualSense::isReleased(PS5::button button) {
      if (event_.type != EventType::EVENT_BUTTON) return false;

      button_pair on = pad_data_.getOldNewButtonPair(static_cast<int>(button));
      bool old_state = std::get<OLD>(on);
      bool new_state = std::get<NEW>(on);
      return (old_state == true) && (new_state == false);
    }

    int DualSense::getAxisValue(PS5::axis axis) {
      int16_t val = pad_data_.getAxisValue(static_cast<int>(axis));
      return static_cast<int>(val);
    }

  }
}