#ifndef PAD_ROS_H
#define PAD_ROS_H

#include "padbase.hpp"

namespace pad {

  namespace ros {

    template <class Editor>
    class RosPad: public BasePad {
     public:
      RosPad(std::string device_name, uint total_button, uint total_axis):
        BasePad(device_name, total_button, total_axis)
      {
        this->editor_ = std::make_unique<Editor>();
      }

      void copyRawData(std::vector<bool>& button_data, std::vector<float>& axis_data) {
        button_data = buttons_.getVector();
        axis_data   = axes_.getVector();
      }
    };

  }

}

#endif // PAD_ROS_H