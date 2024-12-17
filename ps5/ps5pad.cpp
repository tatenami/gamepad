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

      Cross.attach(buttons_);
      Circle.attach(buttons_);
      Triangle.attach(buttons_);
      Square.attach(buttons_);
      L1.attach(buttons_);
      R1.attach(buttons_);
      Create.attach(buttons_);
      Option.attach(buttons_);
      Ps.attach(buttons_);
      L3.attach(buttons_);
      R3.attach(buttons_);
      Left.attach(buttons_);
      Right.attach(buttons_);
      Up.attach(buttons_);
      Down.attach(buttons_);

      L2.attach(buttons_);
      L2.getAxis().attach(axes_);
      R2.attach(buttons_);
      R2.getAxis().attach(axes_);

      LStick.x.attach(axes_);
      LStick.y.attach(axes_);
      RStick.x.attach(axes_);
      RStick.y.attach(axes_);
    }

  }

}