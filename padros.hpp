#ifndef PAD_ROS_H
#define PAD_ROS_H

#include "padbase.hpp"

namespace pad {

  namespace ros {

    template <class Handler>
    class RosPad {
     protected:
      std::unique_ptr<Handler> handler_;
      PadReader  reader_;
      ButtonData buttons_;
      AxisData   axes_;
      bool is_connected_{false};

     public:
      RosPad(uint b_total, uint a_total):
        buttons_(b_total),
        axes_(a_total)
      {
        this->handler_ = std::make_unique<Handler>();
      }

      ~RosPad() {
        reader_.disconnect();
      }

      void copyRawData(std::vector<bool>& button_data, std::vector<float>& axis_data) {
        button_data = buttons_.getVector();
        axis_data   = axes_.getVector();
      }

      virtual void update() {
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

}

#endif // PAD_ROS_H