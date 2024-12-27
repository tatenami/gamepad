#ifndef PAD_ROS_H
#define PAD_ROS_H

#include "padbase.hpp"

namespace pad {

  namespace ros {

    template <class Editor>
    class RosPad {
     protected:
      std::unique_ptr<PadEventEditor> editor_;
      PadReader   reader_;
      ButtonData  buttons_;
      AxisData    axes_;
      bool is_connected_{false};

     public:
      RosPad(std::string device_name, uint total_button, uint total_axis):
        buttons_(total_button),
        axes_(total_axis)
      {
        this->editor_ = std::make_unique<Editor>();
        this->is_connected_ = reader_.connect(device_name);
      }

      ~RosPad() {
        reader_.disconnect();
      }

      bool isConnected() {
        return is_connected_;
      }

      void copyRawData(std::vector<bool>& button_data, std::vector<float>& axis_data) {
        button_data = buttons_.getVector();
        axis_data   = axes_.getVector();
      }

      void update() {
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

  }

}

#endif // PAD_ROS_H