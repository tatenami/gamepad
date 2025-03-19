#ifndef PAD_H
#define PAD_H

#include "./padbase.hpp"

namespace pad {

  template <class Editor>
  class GamePad: public BasePad<Editor> {
   public:
    GamePad(std::string device_name,
            int num_buttons = num_default_buttons, 
            int num_axes = num_default_axes):
      BasePad<Editor>(device_name, num_buttons, num_axes)
    {

    }

    void update() {
      this->buttons_.resetUpdateFlag();
      BasePad<Editor>::update();
    }

    inline bool press(ui_id id) {
      return this->buttons_.getState(id);
    }

    inline bool push(ui_id id) {
      if (!(this->buttons_.updated())) {
        return false;
      }

      bool  state = this->buttons_.getState(id);
      ui_id event_id = this->buttons_.getEventId();

      return (event_id == id) && (state == true);
    }

    inline bool release(ui_id id) {
      if (!(this->buttons_.updated())) {
        return false;
      }

      bool  state = this->buttons_.getState(id);
      ui_id event_id = this->buttons_.getEventId();

      return (event_id == id) && (state == false);
    }

    inline float axisValue(ui_id id) {
      return this->axes_.getValue(id);
    }
  };
}

#endif // PAD_H