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
}