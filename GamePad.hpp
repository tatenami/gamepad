#ifndef GAMEPAD_H
#define GAMEPAD_H

/**-------------------
 *  Headers ( C-lang )
---------------------*/

#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <linux/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>

/**--------------------
 *  Headers ( C++ )
 * ------------------**/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <cmath>

namespace pad {

  extern const std::string dev_info_path;

  enum class button {
    Any,
    None
  };

  struct device_info {   // [ basic infomation of device's file and data ] 
    int         dev_fd;
    std::string dev_file_path;
    std::string dev_name;
  };

  /**--------------------
   *  [ GamePad clsss ]
   *  - this class read GamePad(joy-stick) device data.
   *  - then read device file, update pad state or datas.
   * ----------------**/
  class GamePad {
   private:
    void find_device_name(std::string device_name, std::ifstream& read_stream);
    void find_device_handler(std::ifstream& read_stream);
    void check_device_file();

   protected:
    device_info       dev_info_;
    bool              connection_state_;
    js_event          event_; 
    std::vector<bool> buttons_data_;
    std::vector<int16_t>  axes_data_;

    bool connect_device(std::string device_name);
    void update_pad_data();
    virtual void update_button_data() = 0;
    virtual void update_axis_data() = 0;
    void basic_button_process(int number, int value);
    void basic_axis_process(int number, int16_t value);

   public:
    GamePad(std::string device_name, int total_buttons, int total_axes);
    virtual ~GamePad();

    bool load_data();
    bool is_connect();
    bool is_ON(int number);
    bool is_ON(button state);
    bool is_Pushed(int number);
    bool is_Released(int number);
    int  get_axis_value(int number);
  };

  inline void GamePad::basic_button_process(int number, int value) {
    this->buttons_data_[number] = value;
  }

  inline void GamePad::basic_axis_process(int number, int16_t value) {
    this->axes_data_[number] = value;
  }

};

#endif // GAMEPAD_H