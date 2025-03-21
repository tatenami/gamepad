#include "padbase.hpp"

namespace pad {

  PadReader::~PadReader() { disconnect(); }

  void PadReader::findDeviceName(std::string devname, std::ifstream& stream) {
    bool is_found = false;
    std::string buf;

    // search target's device name in /proc/bus/input/devices
    while (std::getline(stream, buf)) {
      if (buf.find(devname) != std::string::npos) {
        is_found = true;
        break;
      }
    }

    if (!is_found) {
      throw std::runtime_error("Failed to find name of target device");
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

    // select device file [ eventX ]
    if (std::regex_search(buf, smatch, regex_devfile)) {
      this->dev_path_ = devfile_path + smatch[0].str();
    }
    else {
      throw std::runtime_error("Failed to find device file(handler)");
    }
  }

  void PadReader::openDeviceFile() {
    // 読み込みモード・ノンブロッキングモード
    this->fd_ = open(dev_path_.c_str(), O_RDONLY | O_NONBLOCK);

    if (this->fd_ == -1) {
      throw std::runtime_error("Failed to open device file");
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
  bool PadReader::connect(std::string device_name) {
    std::ifstream read_stream(devlist_path);
    bool is_readable = false;
    connection_ = false;

    try {
      findDeviceName(device_name, read_stream);
      findDeviceHandler(read_stream);
      openDeviceFile();
      is_readable = true; 
    }
    catch (std::runtime_error& e) {
      std::cout << e.what() << std::endl;
      return false;
    }

    read_stream.close();
    connection_ = is_readable;
    event_ = {0, 0, EventType::None};

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
      // 再読込エラーでなければ，ノンブロッキングread以外のエラーなので接続終了
      if (errno != EAGAIN) {
        connection_ = false;
        disconnect();
      }

      return false;
    }   
  }


  void PadEventEditor::addCodeIdEntry(uint event_code, ui_id id) {
    this->id_map_[event_code] = id;
  }

  void PadEventEditor::setDeadZone(float deadzone) {
    this->deadzone_ = deadzone;
  }

  void PadEventEditor::editEvent(PadReader& reader) {
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


  /* [ InputData member functions ] */

  ButtonData::ButtonData(uint total_input):
    InputData(total_input)
  {
    this->event_val_ = false;
    this->clear();
  }

  void ButtonData::clear() {
    for (auto e: this->list_) {
      e = false;
    }
  }

  void ButtonData::update(PadEventEditor& editor) {
    ButtonEvent event = editor.getButtonEvent();
    if (event.id >= this->size_) return;

    this->update_flag_ = true;
    this->event_id_  = event.id;
    this->event_val_ = event.state;
    list_[event_id_] = event_val_;
  }


  AxisData::AxisData(uint total_input):
    InputData(total_input) 
  {
    this->event_val_ = 0.0f;
    this->clear();
  }

  void AxisData::clear() {
    for (auto e: this->list_) {
      e = 0.0;
    }
  }

  void AxisData::update(PadEventEditor& editor) {
    AxisEvent event = editor.getAxisEvent();
    if (event.id >= this->size_) return;

    this->event_val_ = event.value;
    this->list_[event.id] = event_val_;
  }
}