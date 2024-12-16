#include "ps5pad.hpp"

namespace pad {

  namespace ps5 {

    DualSense::DualSense(Connect connect_type):
      BasePad(dev::button_num, dev::axis_num)
    {
      std::string dev_name;

      switch (connect_type) {
        case (Connect::USB): {
          dev_name = dev::usb_name; 
          break;
        }
        case (Connect::Bluetooth): {
          dev_name = dev::bluetooth_name;
          break;
        }
      }

      bool connection = reader_.connect(dev_name);
      this->is_connected_ = connection;
    }

  }

}