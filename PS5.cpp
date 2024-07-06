#include "PS5.hpp"

namespace pad {

  namespace ps5 {

    void PS5PadData::handleCrossXData(int16_t val) {
      if (val > 0)
        button_event_ = {evnum::right, button_state::ON, ButtonAction::Push};

      else if (val < 0)
        button_event_ = {evnum::left, button_state::ON, ButtonAction::Push};

      else {
        if (buttons_.at(evnum::right)->isON())
          button_event_.number = evnum::right;
        else if (buttons_.at(evnum::left)->isON())
          button_event_.number = evnum::left;

        button_event_.state  = button_state::OFF;
        button_event_.action = ButtonAction::Release;  
      } 
    }

    void PS5PadData::handleCrossYData(int16_t val) {
      if (val > 0)
        button_event_ = {evnum::down, button_state::ON, ButtonAction::Push}; 

      else if (val < 0)
        button_event_ = {evnum::up, button_state::ON, ButtonAction::Push};

      else {
        if (buttons_.at(evnum::down)->isON())
          button_event_.number = evnum::down;
        else if (buttons_.at(evnum::up)->isON())
          button_event_.number = evnum::up;

        button_event_.state  = button_state::OFF;
        button_event_.action = ButtonAction::Release; 
      }
    } 

    void PS5PadData::handleButtonEvent(Event event) {
      button_event_.number = event.number;

      switch (event.value) {
        case 0: {
          button_event_.state  = button_state::OFF;
          button_event_.action = ButtonAction::Release;
          break;
        }
        case 1: {
          button_event_.state  = button_state::ON;
          button_event_.action = ButtonAction::Push;
          break;
        }
      }

      updateButton(button_event_);
    }

    void PS5PadData::handleAxisEvent(Event event) {
      int16_t val = event.value;

      switch (event.number) {
        case (evnum::crossX): {
          handleCrossXData(val);
          updateButton(button_event_);
          break;
        }
        case (evnum::crossY): {
          handleCrossYData(val);
          updateButton(button_event_);
          break;
        }
        case (evnum::leftY):
        case (evnum::rightY): {
          val *= -1;
        }
        default: {
          axis_event_.number = event.number;
          axis_event_.value  = val;
          cutDeadzone();
          updateAxis(axis_event_);
        }
      }
    }

    void PS5PadData::update(Event event) {
      resetButtonAction();

      switch (event.type) {
        case (EventType::EVENT_BUTTON): {
          handleButtonEvent(event);
          break;
        }
        case (EventType::EVENT_AXIS): {
          handleAxisEvent(event);
          break;
        }
        default: ;
      }
    }

    DualSense::DualSense(Connect mode) {
      std::string device_name;

      switch (mode) {
        case (Connect::USB): device_name = dev::usb; break;
        case (Connect::Bluetooth): device_name = dev::bluetooth; break;
      }

      bool connection = reader_.connect(device_name);
      this->is_connected_ = connection;

      std::vector<Button*> buttons = 
      {
        &Cross,
        &Circle,
        &Triangle,
        &Square,
        &L1,
        &R1,
        &L2,
        &R2,
        &Create,
        &Option,
        &PS,
        &L3,
        &R3,
        &Left,
        &Right,
        &Up,
        &Down
      };

      std::vector<Axis*> axes =
      {
        &Lstick.x,
        &Lstick.y,
        L2.getAxis(),
        &Rstick.x,
        &Rstick.y,
        R2.getAxis(),
      };

      data_.resister(buttons); 
      data_.resister(axes);

      for (int i = 0; i < dev::init_button_event_freq; i++) {
        if (is_connected_) update();
      }

      for (int i = 0; i < dev::init_axis_event_freq; i++) {
        if (is_connected_) update(); 
      }

      data_.clear();
    }

    DualSense::~DualSense() {
      reader_.disconnect();
    }

    bool DualSense::isConnected() { return this->is_connected_; }

    void DualSense::update() {
      bool is_connected = reader_.isConnected();
      bool is_updated   = reader_.readData();

      if (!is_connected || !is_updated) {
        this->is_connected_ = false;
        return;
      }

      this->is_connected_ = true;
      data_.update(reader_.getEvent());
    }
  }

}
    