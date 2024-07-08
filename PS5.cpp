#include "PS5.hpp"

namespace pad {

  namespace ps5 {

    void PS5Handler::handleCrossXData(int16_t val) {
      if (val > 0) {
        this->pre_crossXid_ = id::right;
        button_event_ = {id::right, bstate::ON};
      }
      else if (val < 0) {
        this->pre_crossXid_ = id::left;
        button_event_ = {id::left, bstate::ON};
      }
      else {
        switch (this->pre_crossXid_) {
          case (id::right): button_event_.id = id::right; break;
          case (id::left) : button_event_.id = id::left; break;
        }
        button_event_.state = bstate::OFF;
      }
    }

    void PS5Handler::handleCrossYData(int16_t val) {
      if (val > 0) {
        this->pre_crossYid_ = id::down;
        button_event_ = {id::down, bstate::ON};
      }
      else if (val < 0) {
        this->pre_crossYid_ = id::up;
        button_event_ = {id::up, bstate::ON};
      }
      else {
        switch (this->pre_crossYid_) {
          case (id::down): button_event_.id = id::down; break;
          case (id::up) : button_event_.id = id::up; break;
        }
        button_event_.state = bstate::OFF;
      }
    }

    void PS5Handler::handleAxisEvent() {
      int16_t val = event_.value;

      switch (event_.id) {
        case (id::crossX): {
          event_.type = EventType::BUTTON;
          handleCrossXData(val);
          break;
        }
        case (id::crossY): {
          event_.type = EventType::BUTTON;
          handleCrossYData(val);
          break;
        }
        case (id::leftY):
        case (id::rightY): {
          val *= -1;
        }
        default: {
          if (abs(val) < deadzone_) val = 0;
          axis_event_.id = event_.id;
          axis_event_.value = val;
        }
      }
    }

    void PS5Handler::handleButtonEvent() {
      button_event_.id = event_.id;

      switch (event_.value) {
        case 0: {
          button_event_.state  = bstate::OFF;
          break;
        }
        case 1: {
          button_event_.state  = bstate::ON;
          break;
        }
      }
    } 

    void PS5Handler::editEvent(PadEvent event) {
      this->event_ = event;

      switch (event.type) {
        case (EventType::BUTTON): {
          handleButtonEvent();
          break;
        }
        case (EventType::AXIS): {
          handleAxisEvent();
          break;
        }
        default: ;
      }

    } 

    // コンストラクタ [ 初期化処理全部詰め ]
    DualSense::DualSense(Connect mode) {
      std::string device_name;

      switch (mode) {
        case (Connect::USB): device_name = dev::usb; break;
        case (Connect::Bluetooth): device_name = dev::bluetooth; break;
      }

      std::vector<Button*> buttons = {
        &Cross, &Circle, &Triangle, &Square,
        &L1, &R1, &L2, &R2, 
        &Create, &Option, &PS, &L3, &R3,
        &Left, &Right, &Up, &Down       
      };

      std::vector<Axis*> axes = {
        &Lstick.x, &Lstick.y, L2.getAxis(), &Rstick.x, &Rstick.y, R2.getAxis()       
      };

      reader_.connect(device_name);
      buttons_.resister(buttons);
      axes_.resister(axes);

      for (int i = 0; i < dev::init_button_event_freq; i++) {
        reader_.readData();
      }

      buttons_.clear();
      axes_.clear();

      std::cout << "Device is initialized" << std::endl;
    }

    // デストラクタ
    DualSense::~DualSense() {
      reader_.disconnect(); // PadReaderのデストラクタで呼ばれるけど念の為
    }

    void DualSense::update() {
      if (reader_.readData()) {   // イベントが発生

        handler_.editEvent(reader_.getPadEvent()); // Handlerでイベントの編集
        buttons_.resetAction();

        switch (handler_.getEventType()) {  // イベントの種類でボタン・軸の処理分け
          case (EventType::BUTTON): {
            buttons_.update(handler_.getButtonEvent());
            break;
          }
          case (EventType::AXIS): {
            axes_.update(handler_.getAxisEvent());
            break;
          }
          default: ;
        }
      }
      else {
        buttons_.resetAction(); // push, releaseを正しく動作させるために必ずリセット
      }
    }

    bool DualSense::isConnected() { return this->reader_.isConnected(); }

  }

}
    