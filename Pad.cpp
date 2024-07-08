#include "Pad.hpp"

namespace pad {

  /**--------------------------------
   * [ Button class member function ]
   ----------------------------------*/

  Button::Button(int id): id_{id} {}

  void Button::update(bool state, ButtonAction action) {
    this->state_  = state;
    this->action_ = action;
  }

  void Button::update(ButtonAction action) {
    this->action_ = action;
  }

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

  Axis::Axis(int id): id_{id} {}

  void Axis::update(int16_t value) {
    this->value_ = value;
  }
  
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

  void ButtonData::resister(std::vector<Button*> buttons) {
    this->list_.resize(buttons.size(), nullptr);

    for (auto b: buttons) {
      this->list_.at(b->getID()) = b;
    }
  }

  void ButtonData::update(ButtonEvent event) {
    this->event_ = event;
    ButtonAction action = ButtonAction::None;

    switch (event.state) {
      case (bstate::OFF): action = ButtonAction::Release; break;
      case (bstate::ON):  action = ButtonAction::Push;    break;
    }

    this->list_[event_.id]->update(event_.state, action);
  }

  void ButtonData::resetAction() {
    this->list_[this->event_.id]->update(ButtonAction::None);
  }

  void ButtonData::clear() {
    for (auto b: this->list_) {
      b->update(ButtonAction::None);
    }
  }

  /**----------------------------------
   * [ AxisData class member function ]
   ------------------------------------*/

  AxisData::AxisData() {
    this->event_ = {0, 0};
  }

  void AxisData::resister(std::vector<Axis*> axes) {
    this->list_.resize(axes.size(), nullptr);

    for (auto a: axes) {
      this->list_.at(a->getID()) = a;
    }
  }

  void AxisData::update(AxisEvent event) {
    this->event_ = event;

    this->list_[event_.id]->update(event_.value);
  }

  void AxisData::clear() {
    for (auto a: this->list_) {
      a->update(0);
    }
  }


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

}