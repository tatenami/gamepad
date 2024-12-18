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
    ButtonData *ad_;

   public:
    using PadUI::PadUI;
    Button& get();
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
    Axis& get();
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
  
  template <class Handler>
  class BasePad {
   protected:
    std::unique_ptr<Handler> handler_;
    PadReader   reader_;
    ButtonData  buttons_;
    AxisData    axes_;
    bool is_connected_{false};

   public:
    BasePad(uint b_total, uint a_total):
      buttons_(b_total),
      axes_(a_total)
    {
      this->handler_ = std::make_unique<Handler>();
    }

    ~BasePad() {
        reader_.disconnect();
    }

    bool isConnected() {
      return is_connected_;
    }

    virtual void update() {
      buttons_.resetUpdateFlag();

      if (!reader_.isConnected()) {
        this->is_connected_ = false;
        buttons_.clear();
        axes_.clear();
        return;
      }

      if (this->reader_.readEvent()) {
        (*handler_).editEvent(reader_);

        EventType type = (*handler_).getEventType();
        switch (type) {
          case EventType::Button: {
            buttons_.update((*handler_).getButtonEvent());
            break;
          }
          case EventType::Axis: {
            axes_.update((*handler_).getAxisEvent());
            break;
          }
        }
      }

    }
  };
}

#endif // XXX_H