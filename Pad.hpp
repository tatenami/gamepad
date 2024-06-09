#ifndef PAD_H
#define PAD_H

#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include <tuple>

namespace Pad {

  /* ボタンの Push, Release の判定に使用する変数 */
  using button_pair = std::pair<bool, bool>; 
  const int OLD = 0, NEW = 1;

  /* デバイスファイルの読み取りに関連するデータ */
  namespace Common {
    const std::string devfile_path = "/dev/input/";
    const std::string devinfo_path = "/proc/bus/input/devices";
    const int8_t event_type_filter = ~(static_cast<int8_t>(JS_EVENT_INIT));
  } 

  /* 読み取られるイベントの種類 */
  enum class EventType {
    NONE, EVENT_BUTTON, EVENT_AXIS
  };

  /* デバイスファイルの読み取りモード */
  enum class ReadMode {
    BLOKING, NON_BLOKING
  };

  /* イベントの情報 */
  struct PadEvent {
    EventType type;
    uint8_t   number;
    int16_t   value;
  };

  /* デバイスファイル関連のデータ */
  struct DevFile {
    int         fd;
    std::string path;
  };

  /**
   * [ PadReader class ]
   * デバイス名を指定することで，それに対応するデバイスファイルを探索
   * デバイスファイルを通してコントローラーのイベントを取得
   * **/
  class PadReader {
   private:
    bool     is_connected_;
    js_event read_event_;
    DevFile  devfile_;
    PadEvent event_;


    void findDeviceName(std::string devname, std::ifstream& read_stream);
    void findDeviceHandler(std::ifstream& read_stream);
    void openDeviceFile();

   public:
    PadReader(std::string devname, ReadMode mode = ReadMode::BLOKING);
    ~PadReader();

    bool connect(std::string devname, ReadMode mode);
    bool update();
    bool isConnected();
    void closeDeviceFile();
    PadEvent getEvent();
  }; 

  /**
   * [ PadData class ]
   * コントローラーのデータを配列により保持
   * - 押下可能なボタンのON/OFF状態 (現在状態と直前状態)
   * - 左右のスティックの座標・R2/L2の深度
   * - ボタンは bool, スティック・深度は符号付き16bit整数
   */
  class PadData {
   private:
    std::vector<bool>    button_data_{0};
    std::vector<bool>    old_button_data_{0};
    std::vector<int16_t> axis_data_{0};

   public:
    PadData(int button_num, int axis_num);
    ~PadData();

    void updateOldButton();
    void setButtonState(int button, bool value);
    void setAxisData(int axis, int16_t value);
    bool getButtonState(int button);
    int16_t getAxisValue(int axis);
    button_pair getOldNewButtonPair(int button);
  };

  inline void PadData::updateOldButton() { old_button_data_ = button_data_; }

  inline void PadData::setButtonState(int button, bool value) { button_data_[button] = value; }

  inline void PadData::setAxisData(int axis, int16_t value) { axis_data_[axis] = value; }

  inline bool PadData::getButtonState(int button) { return button_data_[button]; }

  inline int16_t PadData::getAxisValue(int axis) { return axis_data_[axis]; } 

  inline button_pair PadData::getOldNewButtonPair(int button) {
    return button_pair(old_button_data_[button], button_data_[button]);
  }
}

#endif // PAD_H