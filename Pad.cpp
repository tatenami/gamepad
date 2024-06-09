#include "Pad.hpp"

namespace Pad {

  /**------------------------------------
   * [ PadReader class member function ]
   *-------------------------------------*/

  /* 認識されているデバイス一覧からコントローラーのデバイス名を探索 */
  void PadReader::findDeviceName(std::string devname, std::ifstream& read_stream) {
    bool is_found = false;
    std::string read_buf;

    while (std::getline(read_stream, read_buf)) {
      if (read_buf.find(devname) != std::string::npos) {
        is_found = true;
        break;
      }
    }

    if (!is_found) { 
      throw std::string("Fail to find target device"); 
    }
  }

  /* コントローラーに対応するデバイスファイルを探索 */
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

  /* デバイスファイルをOpen, 読み取りの確認 */
  void PadReader::openDeviceFile() {
    devfile_.fd = open(devfile_.path.c_str(), O_RDONLY);

    if (devfile_.fd == -1) {
      throw std::string("Fail to open device file");
    }

    if (read(devfile_.fd, &(read_event_), sizeof(read_event_)) == -1) {
      throw std::string("Fail to read data");
    }
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
    event_ = {EventType::NONE, 0, 0};
  }

  PadReader::~PadReader() {}

  /* デバイスファイルからのイベントの取得 */
  bool PadReader::update() {
    uint8_t event_type;
    int size = read(devfile_.fd, &(read_event_), sizeof(read_event_));
    
    if (size != sizeof(read_event_)) {
      event_ = {EventType::NONE, 0, 0};
      return false;
    } 

    event_type = read_event_.type & Common::event_type_filter;

    switch (event_type) {
      case JS_EVENT_AXIS:   event_.type = EventType::EVENT_AXIS; break;
      case JS_EVENT_BUTTON: event_.type = EventType::EVENT_BUTTON; break;
    }

    event_.number = read_event_.number;
    event_.value = read_event_.value;
    return true;
  }

  void PadReader::closeDeviceFile() { close(devfile_.fd); }

  PadEvent PadReader::getEvent() { return event_; }

  bool PadReader::isConnected() { return is_connected_; }


  /**------------------------------------
   * [ PadData class member function ]
   *-------------------------------------*/

  PadData::PadData(int button_num, int axis_num) {
    button_data_.resize(button_num);
    old_button_data_.resize(button_num);
    axis_data_.resize(axis_num);
  }

  PadData::~PadData() {}
}