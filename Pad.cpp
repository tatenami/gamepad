#include "Pad.hpp"

namespace pad {

  /**
   * [ Button class member function ]
   */
  void Button::update(bool state, ButtonAction action) {
    this->state_  = state;
    this->action_ = action;
  }

  void Button::update(ButtonAction action) {
    this->action_ = action;
  }

  Button::Button(int event_number): number_(event_number) {}

  bool Button::isON() { return this->state_; }

  bool Button::isPushed() {
    if (this->action_ == ButtonAction::Push)
      return true;
    else 
      return false; 
  }

  bool Button::isReleased() {
    if (this->action_ == ButtonAction::Release)
      return true;
    else 
      return false; 
  }

  /**
   * [ Axis class member function ]  
   */

  void Axis::update(int16_t value) {
    this->value_ = value;
  }
  
  Axis::Axis(int event_number): number_(event_number) {}

  int Axis::getValue() { return static_cast<int>(this->value_); }

  /**
   * [ TriggerButton class member function ]
   */

  TriggerButton::TriggerButton(int b_evnum, int a_evnum):
    Button(b_evnum), depth_(a_evnum) 
  {

  }

  /**
   * [ Stick class member function ]
   */

  Stick::Stick(int x_evnum, int y_evnum):
    x(x_evnum), y(y_evnum)
  {
    
  } 

  /**
   * [ ButtonData class member function ] 
   */

  // ButtonData::ButtonData(int numof_button): total_num_(numof_button) {
  //   this->event_ = {0, button_state::OFF, ButtonAction::None};
  //   this->list_.resize(this->total_num_, nullptr);
  // }

  // void ButtonData::resister(std::vector<Button>& buttons) {
  //   for (auto b: buttons) {
  //     this->list_[b.getNumber()] = &b; 
  //   }

  //   for (auto bp: this->list_) {
  //     if (bp == nullptr) 
  //       std::cout << "button number is out of range";
  //   }
  // }

  // void ButtonData::update(ButtonEvent event) {
  //   this->event_ = event;
  //   this->list_[event.number]->update(event.state, event.action);
  // }

  // void ButtonData::resetAction() {
  //   this->list_[event_.number]->update(ButtonAction::None);
  // }

  // Button* ButtonData::getData(int num) {
  //   return this->list_.at(num);
  // }

  // /**
  //  * [ AxisData class member function ]
  //  */

  // AxisData::AxisData(int numof_axis): total_num_(numof_axis) {
  //   this->event_ = {0, 0};
  //   this->list_.resize(this->total_num_, nullptr);
  // }

  // void AxisData::resister(std::vector<Axis>& axes) {
  //   for (auto a: axes) {
  //     this->list_[a.getNumber()] = &a;
  //   }

  //   for (auto ap: this->list_) {
  //     if (ap == nullptr) 
  //       std::cout << "axis number is out of range";
  //   }
  // }

  // void AxisData::update(AxisEvent event) {
  //   this->event_ = event;
  //   this->list_[event.number]->update(event.value);
  // } 

  /**----------------------------------
   * [ PadData class member function ]
   ------------------------------------*/

  // PadData::PadData(int numof_buttons, int numof_axes):
  //   numof_buttons_(numof_buttons), numof_axes_(numof_axes)
  // {
  //   this->buttons_.resize(numof_buttons_, nullptr);
  //   this->axes_.resize(numof_axes_, nullptr);
  // }

  PadData::PadData() {}

  void PadData::resetButtonAction() {
    this->button_event_.action = ButtonAction::None;
    updateButton(button_event_);
  }

  void PadData::updateButton(ButtonEvent event) {
    this->buttons_.at(event.number)->update(event.state, event.action);
  }

  void PadData::updateAxis(AxisEvent event) {
    this->axes_.at(event.number)->update(event.value);
  }

  void PadData::cutDeadzone() {
    if (abs(axis_event_.value) < this->deadzone_) {
      axis_event_.value = 0;
    }
  }

  void PadData::resister(std::vector<Button*> buttons) {
    this->buttons_.resize(buttons.size(), nullptr);

    for (auto b: buttons) {
      this->buttons_.at(b->getNumber()) = b;
    }
  }

  void PadData::resister(std::vector<Axis*> axes) {
    this->axes_.resize(axes.size(), nullptr);

    for (auto a: axes) {
      this->axes_.at(a->getNumber()) = a;
    }
  }

  void PadData::clear() {
    for (auto button: this->buttons_) {
      button->update(button_state::OFF, ButtonAction::None);
    }

    for (auto axis: this->axes_)   {
      axis->update(0);
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
    devfile_.fd = open(devfile_.path.c_str(), O_RDONLY);

    if (devfile_.fd == -1)
      throw std::string("Fail to open device file");

    if (read(devfile_.fd, &(raw_event_), sizeof(raw_event_)) == -1)
      throw std::string("Fail to read data");

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
    event_ = {EventType::EVENT_NONE, 0, 0};

    read_stream.close();
    return is_readable;
  }


  PadReader::PadReader() {}

  PadReader::~PadReader() { disconnect(); }

  bool PadReader::readData() {
    uint8_t event_type;
    int size = read(devfile_.fd, &(raw_event_), sizeof(raw_event_));

    if (size != sizeof(raw_event_)) {
      event_ = {EventType::EVENT_NONE, 0, 0};
      is_connected_ = false;
      return false;
    }

    event_type = raw_event_.type & common::event_type_filter;

    switch (event_type) {
      case JS_EVENT_AXIS:   event_.type = EventType::EVENT_AXIS; break;
      case JS_EVENT_BUTTON: event_.type = EventType::EVENT_BUTTON; break;
    }

    event_.number = raw_event_.number;
    event_.value = raw_event_.value;
    return true;
  }

  void PadReader::disconnect() { close(devfile_.fd); }

}