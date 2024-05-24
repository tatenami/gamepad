#include "DualSense.hpp"

namespace ps5{

  const std::string dev::wired_device_name = "\"Sony Interactive Entertainment DualSense Wireless Controller\"";
  const std::string dev::wireless_device_name = "\"DualSense Wireless Controller\"";

  static const int left_y_num = 1;
  static const int right_y_num = 4;
  static const int cross_x_axis = 6;
  static const int cross_y_axis = 7;
  static const int left_num = 13;
  static const int right_num = 14;
  static const int up_num = 15;
  static const int down_num = 16;

  const int dev::total_buttons = 17;
  const int dev::total_axes = 6;

  const unsigned int dev::default_axis_modify_limit = 1000;


  ps5::DualSense::DualSense(std::string device_name): 
    GamePad(device_name, dev::total_buttons, dev::total_axes),
    axis_modify_limit{dev::default_axis_modify_limit}
  {
    std::string state;

    if (device_name == dev::wired_device_name) state = "Wired";
    else if (device_name == dev::wireless_device_name) state = "Wireless";

    std::cout << "using DualSense [ " << state << " ] state" << std::endl; 
  }

  ps5::DualSense::~DualSense() {

  }

  void ps5::DualSense::update_button_data() {
    basic_button_process(event_.number, event_.value);
  }

  void ps5::DualSense::update_axis_data() {
    int16_t axis_val = event_.value;
    int event_num = event_.number;

    if (event_num == cross_x_axis) {
      if (axis_val > 0) {
        buttons_data_[right_num] = true;
        buttons_data_[left_num] = false;
      }
      else if (axis_val < 0) {
        buttons_data_[left_num] = true;
        buttons_data_[right_num] = false;
      }
      else {
        buttons_data_[left_num] = false;
        buttons_data_[right_num] = false;
      }
    }
    else if (event_num == cross_y_axis) {
      if (axis_val > 0) {
        buttons_data_[down_num] = true;
        buttons_data_[up_num] = false;
      }
      else if (axis_val < 0) {
        buttons_data_[up_num] = true;
        buttons_data_[down_num] = false;
      }
      else {
        buttons_data_[up_num] = false;
        buttons_data_[down_num] = false;
      }
    }
    else if (event_num == left_y_num ||
            event_num == right_y_num) 
    { 
      axis_val *= -1;
    }

    if (abs(axis_val) < axis_modify_limit) {
      axis_val = 0;
    }

    basic_axis_process(event_num, axis_val);
  }

  bool DualSense::is_ON(button button) {
    return buttons_data_[static_cast<int>(button)];
  }

  bool DualSense::is_Pushed(button button) {
    return GamePad::is_Pushed(static_cast<int>(button));
  }

  bool DualSense::is_Released(button button) {
    return GamePad::is_Released(static_cast<int>(button));
  }

  int DualSense::get_axis_value(axis axis) {
    return GamePad::get_axis_value(static_cast<int>(axis));
  }

  void DualSense::set_axis_limit(unsigned int limit_val) {
    if (limit_val < 0 || limit_val > SHRT_MAX) {
      return;
    }
    axis_modify_limit = limit_val;
  }
}