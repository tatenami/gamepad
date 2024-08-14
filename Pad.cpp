#include "Pad.hpp"

namespace pad {

  /**------------------------------------
   * [ PadReader class member function ]
   -------------------------------------*/
  void PadReader::findDeviceName(std::string devname, std::ifstream& read_stream) {
    bool is_found = false;
    std::string read_buf;

    while (std::getline(read_stream, read_buf)) {
      if (read_buf.find(devname) != std::string::npos) {
        is_found = true; 
        break;
      }
    }

    if (!is_found) throw std::string("Fail to find target device");
  }

  void PadReader::findDeviceHandler(std::ifstream& read_stream) {
    std::string read_buf;
    std::regex  regex_devfile(R"(js\d)");
    std::smatch smatch;

    while (std::getline(read_stream, read_buf)) {
      if (read_buf.find("Handlers=") != std::string::npos) break;
    }

    if (std::regex_search(read_buf, smatch, regex_devfile))
      devfile_.path =  common::devicefile_path + smatch[0].str();
    else 
      throw std::string("Fail to find device file");
  }

  void PadReader::openDeviceFile() {
    devfile_.fd = open(devfile_.path.c_str(), O_RDONLY | O_NONBLOCK);

    if (devfile_.fd == -1)
      throw std::string("Fail to open device file");
  }

  bool PadReader::connect(std::string devname) {
    std::ifstream read_stream(common::deviceinfo_path);
    bool is_readable = false;

    try {
      findDeviceName(devname, read_stream);
      findDeviceHandler(read_stream);
      openDeviceFile();
      is_readable = true; 
    }
    catch (std::string msg) {
      std::cout << "[ ERROR ] -> [ " << msg << " ]" << std::endl;
    }

    is_connected_ = is_readable;
    event_ = {EventType::NONE, 0, 0};

    read_stream.close();
    return is_readable;
  }


  PadReader::PadReader() {}

  PadReader::~PadReader() { disconnect(); }

  bool PadReader::readData() {
    if (read(devfile_.fd, &(raw_event_), sizeof(raw_event_)) > 0) {
      uint8_t event_type = raw_event_.type & common::event_type_filter;

      switch (event_type) {
        case JS_EVENT_AXIS:   event_.type = EventType::AXIS;   break;
        case JS_EVENT_BUTTON: event_.type = EventType::BUTTON; break;
      }

      event_.id    = raw_event_.number;
      event_.value = raw_event_.value; 

      return true;
    }
    else {
      if (errno != EAGAIN) {
        std::cout << "out" << std::endl;
        this->is_connected_ = false;
      }

      return false;
    }
  }

  void PadReader::disconnect() { close(devfile_.fd); }

  /**--------------------------------
   * [ Button class member function ]
   ----------------------------------*/

  Button::Button(const int event_number): id_{event_number} {}

  bool Button::pressed() { return this->state_; }

  bool Button::pushed() {
    if (this->action_ == ButtonAction::Push)
      return true;
    else 
      return false; 
  }

  bool Button::released() {
    if (this->action_ == ButtonAction::Release)
      return true;
    else 
      return false; 
  }

  /**------------------------------
   * [ Axis class member function ]  
   --------------------------------*/

  Axis::Axis(const int event_number): id_{event_number} {}
  
  int Axis::getValue() { return static_cast<int>(this->value_); }

  /**---------------------------------------
   * [ TriggerButton class member function ]
   -----------------------------------------*/

  TriggerButton::TriggerButton(int b_id, int a_id):
    Button(b_id), depth_(a_id) {}

  /**--------------------------------
   * [ Stick class member function ]
   ----------------------------------*/

  Stick::Stick(int x_id, int y_id):
    x(x_id), y(y_id) {} 

  /**----------------------------------
   * [ ButtonData class member function ]
   ------------------------------------*/

  ButtonData::ButtonData() {
    this->event_ = {0, bstate::OFF};
  }

  void ButtonData::resister(Button& button) {
    if (button.id_ >= list_.size()) 
      list_.resize(button.id_ + 1, nullptr);

    list_.at(button.id_) = &button; 
  }

  void ButtonData::update(ButtonEvent event) {
    this->event_ = event;
    ButtonAction action = ButtonAction::None;

    switch (event.state) {
      case (bstate::OFF): action = ButtonAction::Release; break;
      case (bstate::ON):  action = ButtonAction::Push;    break;
    }

    // this->list_[event_.id]->update(event_.state, action);

    list_[event_.id]->state_  = event_.state;
    list_[event_.id]->action_ = action; 
  }

  void ButtonData::resetAction() {
    list_[this->event_.id]->action_ = ButtonAction::None;
  }

  void ButtonData::clear() {
    for (auto b: this->list_) {
      b->action_ = ButtonAction::None;
    }
  }

  /**----------------------------------
   * [ AxisData class member function ]
   ------------------------------------*/

  AxisData::AxisData() {
    this->event_ = {0, 0};
  }

  void AxisData::resister(Axis& axis) {
    if (axis.id_ >= list_.size())
      list_.resize(axis.id_ + 1, nullptr);

    list_[axis.id_] = &axis;
  }

  void AxisData::update(AxisEvent event) {
    this->event_ = event;

    this->list_[event_.id]->value_ = event_.value;
  }

  void AxisData::clear() {
    for (auto a: this->list_) {
      a->value_ = 0;
    }
  }

  /**
   * [ PadData class member function ]
   */

  void PadData::update(EventHandler& handler, bool event_flag) {
    button_list_.resetAction();

    if (event_flag == false) return;

    switch (handler.getEventType()) {
      case (EventType::BUTTON): {
        button_list_.update(handler.getButtonEvent());
        break;
      }
      case (EventType::AXIS): {
        axis_list_.update(handler.getAxisEvent());
        break;
      }
      default: ;
    }
  }

  void PadData::resister(Button& button) {
    button_list_.resister(button);
  }

  void PadData::resister(Axis& axis) {
    axis_list_.resister(axis);
  }

  void PadData::resister(TriggerButton t_button) {
    button_list_.resister(t_button);
    axis_list_.resister(t_button.getAxis());
  }

  void PadData::clear() {
    button_list_.clear();
    axis_list_.clear();
  }

}