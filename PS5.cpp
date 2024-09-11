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

    void PS5Handler::editEvent(PadReader& reader) {
      this->event_ = reader.getPadEvent();

      switch (event_.type) {
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

      pad_.resister(Cross);
      pad_.resister(Circle);
      pad_.resister(Triangle);
      pad_.resister(Square);
      pad_.resister(L1);
      pad_.resister(R1);
      pad_.resister(L2);
      pad_.resister(R2);
      pad_.resister(Create);
      pad_.resister(Option);
      pad_.resister(PS);
      pad_.resister(L3);
      pad_.resister(R3);
      pad_.resister(Left);
      pad_.resister(Right);
      pad_.resister(Up);
      pad_.resister(Down);

      pad_.resister(Lstick.x);
      pad_.resister(Lstick.y);
      pad_.resister(Rstick.x);
      pad_.resister(Rstick.y);

      reader_.connect(device_name);

      // for (int i = 0; i < dev::init_button_event_freq; i++) {
      //   reader_.readData();
      // }

      std::cout << "Device is initialized" << std::endl;
    }

    // デストラクタ
    DualSense::~DualSense() {
      reader_.disconnect(); // PadReaderのデストラクタで呼ばれるけど念の為
    }

    void DualSense::update() {
      bool event_flag = reader_.readData();

      if (event_flag == true) handler_.editEvent(reader_);

      pad_.update(handler_, event_flag);
    }

    bool DualSense::isConnected() { return this->reader_.isConnected(); }

  }

}
    