#ifndef PAD_H
#define PAD_H

#include "padbase.hpp"

namespace pad {


  class PadUI {
   protected:
    const uint8_t id_;

   public:  
    PadUI(const uint8_t id);
    uint8_t getID();
  };


  class Button: public PadUI {  
   protected:
    ButtonData *ref_data_;

   public:
    using PadUI::PadUI;
    void attach(ButtonData& ad);
    bool pressed();
    bool pushed();
    bool released();
  };

  class Axis: public PadUI {
   protected:
    AxisData *ref_data_;

   public:
    using PadUI::PadUI;
    void  attach(AxisData& ad);
    float getValue();
  };


  class Trigger: public Button {
   public:
    Axis depth;
    Trigger(uint8_t button_id, uint8_t axis_id);
    float getDepth();
  };


  class Stick {
   public:
    Axis x;
    Axis y;

    Stick(uint8_t x_id, uint8_t y_id);
    float angleDeg();
    float angleRad();
  };

  template <class Editor>
  class GamePad: public BasePad<Editor> {
   public:
    GamePad(std::string device_name,
            int button_num = default_button_num, 
            int axis_num = default_axis_num):
      BasePad<Editor>(device_name, button_num, axis_num)
    {
      this->editor_ = std::make_unique<Editor>();
    }

    void attachUI(Button& button) {
      if (button.getID() < this->buttons_.getSize())
        button.attach(this->buttons_);
    }

    void attachUI(Axis& axis) {
      if (axis.getID() < this->axes_.getSize()) 
        axis.attach(this->axes_);
    }

    void attachUI(Trigger& trigger) {
      Button& button = trigger;
      attachUI(button);
      attachUI(trigger.depth);
    }

    void attachUI(std::vector<Button*>& buttons) {
      int size = this->buttons_.getSize();
      for (Button* button: buttons) {  
        if (button->getID() > size) continue;

        button->attach(this->buttons_);
      }
    }

    void attachUI(std::vector<Axis*>& axes) {
      int size = this->axes_.getSize();
      for (Axis* axis: axes) {
        if (axis->getID() > size) continue;

        axis->attach(this->axes_);
      }
    }

    void update() {
      this->buttons_.resetUpdateFlag();
      BasePad<Editor>::update();
    }
  };
}

#endif // PAD_H