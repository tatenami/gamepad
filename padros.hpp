#ifndef PAD_ROS_H
#define PAD_ROS_H

#include "padbase.hpp"

namespace pad {

  namespace ros {

    template <class Editor>
    class RosPad: public BasePad<Editor> {
     public:
      using BasePad<Editor>::BasePad;

      void copyRawData(std::vector<bool>& button_data, std::vector<float>& axis_data) {
        button_data = this->buttons_.getVector();
        axis_data   = this->axes_.getVector();
      }
    };

  }

}

#endif // PAD_ROS_H