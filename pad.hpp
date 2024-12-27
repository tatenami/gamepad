#ifndef PAD_H
#define PAD_H

#include "padbase.hpp"

namespace pad {

  template <class Editor>
  class GamePad {
   protected:
    std::unique_ptr<PadEventEditor> editor_;
    PadReader   reader_;
    ButtonData  buttons_;
    AxisData    axes_;
    bool is_connected_{false};

   public:
    GamePad(uint total_button, uint total_axis):
      buttons_(total_button),
      axes_(total_axis)
    {
      this->editor_ = std::make_unique<Editor>();
    }

    ~GamePad() {
        reader_.disconnect();
    }

    bool isConnected() {
      return is_connected_;
    }
    
    void setDeadZone(float deadzone) {
      this->editor_->setDeadZone(deadzone);
    }

    void update() {
      buttons_.resetUpdateFlag();

      if (!reader_.isConnected()) {
        is_connected_ = false;
        buttons_.clear();
        axes_.clear();
        return;
      }

      if (reader_.readEvent()) {
        (*editor_).editEvent(reader_);

        buttons_.update(*editor_);
        axes_.update(*editor_);
      }
    }
  };


  class PadUI {
   protected:
    const uint8_t id_;

   public:  
    PadUI(const uint8_t id);
    uint8_t getID();
  };


  class Button: public PadUI {  
   protected:
    ButtonData *ad_;

   public:
    using PadUI::PadUI;
    void attach(ButtonData& ad);
    bool pressed();
    bool pushed();
    bool released();
  };

  class Axis: public PadUI {
   protected:
    AxisData *ad_;

   public:
    using PadUI::PadUI;
    void  attach(AxisData& ad);
    float getValue();
  };


  class Trigger: public Button {
   private:
    Axis depth_;

   public:
    Trigger(uint8_t button_id, uint8_t axis_id);
    float getDepth();
    Axis& getAxis();
  };


  class Stick {
   public:
    Axis x;
    Axis y;

    Stick(uint8_t x_id, uint8_t y_id);
    float angleDeg();
    float angleRad();
  };
}

#endif // PAD_H