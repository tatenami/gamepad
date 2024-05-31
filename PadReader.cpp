#include "PadReader.hpp"

namespace Pad {

  void PadReader::findDeviceName(std::string devname, std::ifstream& read_stream) {
    bool is_found = false;
    std::string read_buf;

    while (std::getline(read_stream, read_buf)) {
      if (read_buf.find(devname) != std::string::npos) {
        is_found = true;
        break;
      }
    }

    if (!is_found) { throw std::string("Fail to find target device"); }
  }

  void PadReader::findDeviceHandler(std::ifstream& read_stream) {
    std::string read_buf;
    std::regex  regex_devfile(R"(js\d)");
    std::smatch smatch;

    while (std::getline(read_stream, read_buf)) {
      if (read_buf.find("Handlers=") != std::string::npos) break;
    }

    if (std::regex_search(read_buf, smatch, regex_devfile)) {
      devfile_.path = Common::devfile_path + smatch[0].str();
    }
    else {
      throw std::string("Fail to find device file");
    }
  }

  void PadReader::openDeviceFile() {
    int fd = open(devfile_.path.c_str(), O_RDONLY);

    if (fd == -1) {
      throw std::string("Fail to open device file");
    }

    devfile_.fd = fd;
  }

  bool PadReader::connect(std::string devname, ReadMode mode) {
    std::ifstream read_stream(Common::devinfo_path);
    bool is_connected = false;

    try {
      findDeviceName(devname, read_stream);
      findDeviceHandler(read_stream);
      openDeviceFile();
      if (mode == ReadMode::NON_BLOKING) fcntl(devfile_.fd, F_SETFL, O_NONBLOCK);
      is_connected = true;
    } 
    catch (std::string msg) {
      std::cout << "[ ERROR ]: " << msg << std::endl;
    } 

    read_stream.close();
    return is_connected;
  }

  PadReader::PadReader(std::string devname, ReadMode mode) {
    is_connected_ = connect(devname, mode);
    padinfo_.pad_name = devname;

    event_ = {EventType::NONE, 0, 0};
  }

  PadReader::~PadReader() {
    close(devfile_.fd);
  }

  void PadReader::update() {
    uint8_t event_type;
    int size = read(devfile_.fd, &(read_event_), sizeof(js_event));

    if (size != sizeof(js_event)) {
      event_ = {EventType::NONE, 0, 0};
    } 

    event_type = read_event_.type & Common::event_type_filter;

    switch (event_type) {
      case JS_EVENT_AXIS:   event_.type = EventType::EVENT_AXIS;
      case JS_EVENT_BUTTON: event_.type = EventType::EVENT_BUTTON;
    }

    event_.number = read_event_.number;
    event_.value = read_event_.value;
  }

  PadEvent PadReader::getEvent() { return event_; }

  PadInfo PadReader::getPadInfo() { return padinfo_; }

  bool PadReader::isConnected() { return is_connected_; }

}