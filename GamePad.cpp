#include "GamePad.hpp"

namespace pad {

  static const std::string dev_file_dir = "/dev/input/";
  static const std::string device_info_file = "/proc/bus/input/devices";
  static const uint8_t     event_type_filter = ~(static_cast<int8_t>(JS_EVENT_INIT));

  void GamePad::find_device_name(std::string device_name, std::ifstream& read_stream) {
    bool          device_connection = false;
    std::string   read_buf, msg;

    while (std::getline(read_stream, read_buf)) {
      if (read_buf.find(device_name) != std::string::npos) {
        device_connection = true;
        break;
      }
    }

    if (!device_connection) {
      msg = "Fail to find target device";
      throw msg;
    }
  }

  void GamePad::find_device_handler(std::ifstream& read_stream) {
    std::string msg, read_buf;
    std::regex  regex_dev_file(R"(js\d)");
    std::smatch smatch;

    while (std::getline(read_stream, read_buf)) {
        if (read_buf.find("Handlers=") != std::string::npos) break;
    }

    if (std::regex_search(read_buf, smatch, regex_dev_file  )) {
      dev_info_.dev_file_path = dev_file_dir + smatch[0].str();
    }
    else {
      msg = "Fail to find device file";
      throw msg;
    }
  }

  void GamePad::check_device_file() {
    std::string msg;
    dev_info_.dev_fd = open(dev_info_.dev_file_path.c_str(), O_RDONLY);

    if (dev_info_.dev_fd == -1) {
      msg = "Fail to open device file";
      throw msg;
    }

    if (read(dev_info_.dev_fd, &(event_), sizeof(event_)) == -1) {
      close(dev_info_.dev_fd);
      msg = "Fail to read data";
      throw msg;
    }
  }

  bool GamePad::connect_device(std::string device_name) {
    std::ifstream read_stream(device_info_file);

    try {
      find_device_name(device_name, read_stream);
      find_device_handler(read_stream);
      read_stream.close();
      check_device_file();
    }
    catch (std::string msg) {
      std::cout << "[ ERROR ]: " << msg << std::endl;
      return false;
    }

    return true;
  }

  GamePad::GamePad(std::string device_name, int total_buttons, int total_axes) {
    connection_state_ = connect_device(device_name);
    dev_info_.dev_name = device_name;

    buttons_data_ = std::vector<bool>(total_buttons);
    axes_data_ = std::vector<int16_t>(total_axes);
  }

  GamePad::~GamePad() {
    close(dev_info_.dev_fd);
  }

  void GamePad::update_pad_data() {
    switch (event_.type & event_type_filter) {
      case JS_EVENT_BUTTON: {
        update_button_data();
        break;
      }
      case JS_EVENT_AXIS: {
        update_axis_data();
        break;
      }
    }
  }

  bool GamePad::load_data() {
    if (connection_state_) {
      int size = read(dev_info_.dev_fd, &(event_), sizeof(event_));
      
      if (size == sizeof(event_)) update_pad_data();
      else connection_state_ = false;
    } 
    else {
      close(dev_info_.dev_fd);
      std::cout << "Close device file" << std::endl;
    }

    return connection_state_;
  }

  bool GamePad::is_connect() {
    return connection_state_;
  }

  bool GamePad::is_ON(int number) {
    return buttons_data_[number];
  }

  bool GamePad::is_ON(button state) {
    bool is_any;
    for (auto e: buttons_data_) {
      if (e) is_any = true;
    }

    if (state == button::None) is_any = ~is_any;

    return is_any;
  }

  bool GamePad::is_Pushed(int number) {
    if (event_.type != JS_EVENT_BUTTON) return false;

    if (event_.number == number && (event_.value & true)) return true;
    else return false;
  }

  bool GamePad::is_Released(int number) {
    if (event_.type != JS_EVENT_BUTTON) return false;

    if (event_.number == number && ~(event_.value & true)) return true;
    else return false;
  }

  int GamePad::get_axis_value(int number) {
    return axes_data_[number];
  }
};