#include "padbase.hpp"

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
      throw ErrorCode::FindDeviceName;
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
      throw ErrorCode::FindDeviceFile;
    }
  }

  void PadReader::openDeviceFile() {
    // read only, non blocking mode 
    this->fd_ = open(path.c_str(), O_RDONLY | O_NONBLOCK);

    if (this->fd_ == -1) {
      throw ErrorCode::OpenDeviceFile;
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
      code_ = ErrorCode::None;
    }
    catch (ErrorCode code) {
      this->code_ = code;
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


  void PadEventEditor::addCodeIdEntry(uint event_code, uint8_t ui_id) {
    this->id_map_[event_code] = ui_id;
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
    this->type_ = EventType::Button;
    this->clear();
  }

  void ButtonData::clear() {
    for (auto e: this->list_) {
      e = false;
    }
  }

  void ButtonData::update(PadEventEditor& editor) {
    if (editor.getEventType() == EventType::Button) {
      update_flag_ = true;
      event_ = editor.getButtonEvent();
      if (event_.id <= this->size_)
        list_[event_.id] = event_.state;
    }
  }


  AxisData::AxisData(uint total_input):
    InputData(total_input) 
  {
    this->type_ = EventType::Axis;
    this->clear();
  }

  void AxisData::clear() {
    for (auto e: this->list_) {
      e = 0.0;
    }
  }

  void AxisData::update(PadEventEditor& editor) {
    if (editor.getEventType() == EventType::Axis) {
      event_ = editor.getAxisEvent();
      if (event_.id <= this->size_)
      list_[event_.id] = event_.value;
    }
  }
}