#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <regex>

namespace pad {

  /* ゲームコントローラー処理に必要な共通変数 */
  namespace common {
    const int16_t default_deadzone = 2000;
    const std::string devicefile_path = "/dev/input/";
    const std::string deviceinfo_path = "/proc/bus/input/devices";
    const int8_t event_type_filter = ~(static_cast<int8_t>(JS_EVENT_INIT));
  }

  /* 読み取られるイベントの種類 */
  enum class EventType {
    EVENT_NONE, EVENT_BUTTON, EVENT_AXIS
  };

  enum class ButtonAction {
    None, Push, Release
  };

  namespace button_state {
    const bool ON  = true;
    const bool OFF = false;
  };

  /* イベントの情報 */
  struct Event {
    EventType type;
    uint8_t   number;
    int16_t   value;
  };

  struct ButtonEvent {
    int  number;
    bool state;
    ButtonAction action;
  };

  struct AxisEvent {
    int number;
    int16_t value;
  };

  /* デバイスファイル関連データ */
  struct DevFile {
    int fd;
    std::string path;
  };


  struct Vec2 {
    int x;
    int y;
  };

  // [ ButtonData class ]
  // class ButtonData {
  //  private:
  //   const int total_num_;
  //   std::vector<Button*> list_;
  //   ButtonEvent event_; 

  //  public:
  //   ButtonData(int numof_button);
  //   void resister(std::vector<Button>& buttons);
  //   void update(ButtonEvent event);
  //   void resetAction();
  //   Button* getData(int num);
  // };

  // class AxisData {
  //  private:
  //   const int total_num_;
  //   std::vector<Axis*> list_;
  //   AxisEvent event_;

  //  public:
  //   AxisData(int numof_axis);
  //   void  resister(std::vector<Axis>& axes);
  //   void  update(AxisEvent event);
  //   Axis* getData(int num);
  // };

  class Button;
  class Axis;

  // [ PadData class ]  
  class PadData {
   protected:
    // const int numof_buttons_;
    // const int numof_axes_;
    std::vector<Button*> buttons_;
    std::vector<Axis*>   axes_;
    ButtonEvent button_event_;
    AxisEvent   axis_event_;
    int16_t     deadzone_;
    Event       event_;

    void resetButtonAction();
    virtual void handleButtonEvent(Event event) = 0;
    virtual void handleAxisEvent(Event event) = 0;
    void updateButton(ButtonEvent event);
    void updateAxis(AxisEvent event);
    void cutDeadzone();

   public:
    PadData();
    // PadData(int numof_buttons, int numof_axes);
    virtual void update(Event event) = 0;
    void resister(std::vector<Button*> buttons);
    void resister(std::vector<Axis*> axes);
    void clear();
  };

  // [ Button class ]
  class Button {
   protected:
    const int number_;
    bool state_;
    ButtonAction action_;

    void update(bool state, ButtonAction action);
    void update(ButtonAction action);

    friend PadData;

   public:
    Button(int event_number);
    Button* get();
    int  getNumber();
    bool isON();
    bool isPushed();
    bool isReleased();
  };

  inline Button* Button::get() { return this; }

  inline int Button::getNumber() { return this->number_; }

  // [ Axis class ]
  class Axis {
   private:
    const int number_;
    int16_t value_;

    void update(int16_t value);

    friend PadData;
    
   public:
    Axis(int event_number);
    Axis* get();
    int getNumber();
    int getValue();
  };

  inline Axis* Axis::get() { return this; }

  inline int Axis::getNumber() { return this->number_; }

  /**
   * [ TriggerButton class ]
   */

  class TriggerButton: public Button {
   private:
    Axis depth_;

   public:
    TriggerButton(int b_evnum, int a_evnum);
    using Button::update;
    int   getDepth();
    Axis* getAxis(); 
  };

  inline int TriggerButton::getDepth() { return depth_.getValue(); }

  inline Axis* TriggerButton::getAxis() { return this->depth_.get(); }

  /**
   * [ Stick class ]
   */

  class Stick {
   public:
    Axis x;
    Axis y;

    Stick(int x_evnum, int y_evnum);
  };

  /**
   * [ PadReader class ]
   * デバイス名を指定することで，それに対応するデバイスファイルを探索
   * デバイスファイルを通してコントローラーのイベントを取得
   * **/

  class PadReader {
   private:
    bool      is_connected_;
    js_event  raw_event_;
    DevFile   devfile_;
    Event     event_;

    void findDeviceName(std::string devname, std::ifstream& read_stream);
    void findDeviceHandler(std::ifstream& read_stream);
    void openDeviceFile();

   public:
    PadReader();
    ~PadReader();

    bool  connect(std::string devname);
    void  disconnect();
    bool  readData();
    bool  isConnected();
    Event getEvent();
  };

  inline Event PadReader::getEvent() { return event_; }

  inline bool PadReader::isConnected() { return is_connected_; }


}
