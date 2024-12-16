#include "pad.hpp"

namespace pad {

  /* [ PadReader member functions ] */

  /**
   * @brief Destroy the Pad Reader:: Pad Reader object
   * 
   */
  PadReader::~PadReader() { disconnect(); }

  void PadReader::findDeviceName(std::string devname, std::ifstream& stream) {
    bool is_found = false;
    std::string buf;

    // serch target device name in /proc/bus/input/devices
    while (std::getline(stream, buf)) {
      if (buf.find(devname) != std::string::npos) {
        is_found = true;
        break;
      }
    }

    if (!is_found) {
      throw std::string("Fail to find target device");
    }
  }

  void PadReader::findDeviceHandler(std::ifstream& stream) {
    std::string buf;
    std::regex  regex_devfile(R"(event(\d+))");
    std::smatch smatch;

    // search device file name
    while (std::getline(stream, buf)) {
      if (buf.find("Handlers=") != std::string::npos) break;
    }

    // select device file [ event? ]
    if (std::regex_search(buf, smatch, regex_devfile)) {
      this->path += smatch[0].str();
    }
    else {
      throw std::string("Fail to find device file");
    }
  }

  void PadReader::openDeviceFile() {
    // read only, non blocking mode 
    this->fd_ = open(path.c_str(), O_RDONLY | O_NONBLOCK);

    if (this->fd_ == -1) {
      throw std::string("Fail to open device file");
    }
  }

  void PadReader::disconnect() { close(this->fd_); }

  /**
   * @brief prepare read device file of "devname"
   * 
   * @param devname target device name in /proc/bus/input/devices
   * @return true   
   * @return false 
   */
  bool PadReader::connect(std::string devname) {
    std::ifstream read_stream(devlist_path);
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

    connection_ = is_readable;
    event_ = {0, 0, EventType::None};

    read_stream.close();
    return is_readable;
  }

  bool PadReader::readEvent() {
    if (read(this->fd_, &(raw_event_), sizeof(raw_event_)) > 0) {

      if (raw_event_.type == EV_SYN) {
        return false;
      }

      switch (raw_event_.type) {
        case EV_ABS: event_.type = EventType::Axis;   break;
        case EV_KEY: event_.type = EventType::Button; break;
      }

      event_.code  = raw_event_.code;
      event_.value = raw_event_.value; 

      return true;
    }
    else {
      if (errno != EAGAIN)
        connection_ = false;

      return false;
    }   
  }

  /* [ PadReader member functions ] */

  PadEventHandler::PadEventHandler(uint32_t axis_max) {
    this->axis_max_ = axis_max;
  }

  void PadEventHandler::setDeadzone(float deadzone) {
    this->deadzone_ = deadzone;
  }
  
  void PadEventHandler::editEvent(PadReader& reader) {
    this->event_ = reader.getPadEvent();

    switch (event_.type) {
      case (EventType::Button): {
        editButtonEvent();
        break;
      }
      case (EventType::Axis): {
        editAxisEvent();
        break;
      }
    }
  }

  void PadEventHandler::addCodeIdEntry(uint event_code, uint8_t ui_id) {
    this->id_map_[event_code] = ui_id;
  }

  /* [ PadUI member functions ] */

  PadUI::PadUI(const uint8_t id): id_{id} {}

  uint8_t PadUI::getID() { return this->id_; }
  
  /* [ InputData member functions ] */

  template <typename T>
  InputData<T>::InputData(uint total_input) {
    this->list_.resize(total_input);
  }

  template <typename T>
  const std::vector<T> InputData<T>::getVector() {
    return this->list_;
  }


  ButtonData::ButtonData(uint total_input):
    InputData(total_input)
  {
    this->type_ = EventType::Button;
  }

  void ButtonData::update(ButtonEvent event) {
    update_flag_ = true;
    event_ = event;
    list_.at(event_.id) = event_.state;
  }

  void ButtonData::clear() {
    for (auto e: this->list_) {
      e = false;
    }
  }



  AxisData::AxisData(uint total_input):
    InputData(total_input) 
  {
    this->type_ = EventType::Axis;
  }

  void AxisData::update(AxisEvent event) {
    event_ = event;
    list_.at(event_.id) = event_.value;
  }

  void AxisData::clear() {
    for (auto e: this->list_) {
      e = 0.0;
    }
  }


  Button& Button::get() {
    return *this;
  }

  void Button::attach(ButtonData& ad) {
    this->ad_ = &ad;
  }

  bool Button::pressed() {
    return ad_->getState(this->id_);
  }

  bool Button::pushed() {
    bool updated = ad_->isUpdated();

    if (!updated) return false;
    
    ad_->resetUpdateFlag();

    if (ad_->getState(this->id_) == true) {
      return true;
    }
    else {
      return false;
    }
  }

  bool Button::released() {
    bool updated = ad_->isUpdated();

    if (!updated) return false;

    ad_->resetUpdateFlag();
    
    if (ad_->getState(this->id_) == false) {
      return true;
    }
    else {
      return false;
    }
  }


  Axis& Axis::get() {
    return *this;
  }

  void Axis::attach(AxisData& ad) {
    this->ad_ = &ad;
  }

  float Axis::getValue() {
    return ad_->getValue(this->id_);
  }


  Trigger::Trigger(uint8_t b_event_num, uint8_t a_event_num): 
    Button(b_event_num),
    depth_{a_event_num}
  {

  }

  float Trigger::getDepth() { return this->depth_.getValue(); }

  Axis& Trigger::getAxis() { return this->depth_.get(); }


  Stick::Stick(uint8_t x_event_num, uint8_t y_event_num):
    x(x_event_num),
    y(y_event_num)
  {

  }

  float Stick::angleDeg() {
    float rad = atan2f(y.getValue(), x.getValue());
    return rad * (180 / M_PI);
  }

  float Stick::angleRad() {
    return atan2f(y.getValue(), x.getValue());
  }

  template <class Handler>
  BasePad<Handler>::BasePad(uint b_total, uint a_total) {
    this->handler_ = std::make_unique<Handler>();
  }

  template <class Handler>
  void BasePad<Handler>::update() {

    if (!reader_.isConnected()) {
      buttons_.clear();
      axes_.clear();
      return;
    }

    if (this->reader_.readEvent()) {
      (*handler_).editEvent(reader_);

      EventType type = (*handler_).getEventType();
      switch (type) {
        case EventType::Button: {
          buttons_.update((*handler_).getButtonEvent());
          break;
        }
        case EventType::Axis: {
          axes_.update((*handler_).getAxisEvent());
          break;
        }
      }
    }

  }
}