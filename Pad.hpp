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

  namespace bstate {  
    const bool ON  = true;
    const bool OFF = false;
  }

  /* 読み取られるイベントの種類 */
  enum class EventType {
    NONE, BUTTON, AXIS
  };

  enum class ButtonAction {
    None, Push, Release
  };

  /**----------------------------- 
   * [ PadEvent struct ]
   * 読み取られるイベントの情報
   * ・type  : イベントの種類
   * ・id    : イベントの番号
   * ・value : 検出されたイベントの値
  --------------------------------*/
  struct PadEvent {
    EventType type;
    uint8_t   id;
    int16_t   value;
  };

  /**
   * [ ButtonEvent struct ]
   * id     : ユニークなイベントの番号
   * state  : 押下の状態(true:on, false:off)
   */
  struct ButtonEvent {
    int  id;
    bool state;
  };

  struct AxisEvent {
    int     id;
    int16_t value;
  };

  /* デバイスファイル関連データ */
  struct DevFile {
    int fd;
    std::string path;
  };

  /**----------------------------------------------
   * [ Button class ]
   * ユーザーにボタンの状態とその変化をデータとして提供する
   * インターフェース的なイメージ，IDによりボタンと1対1対応
   * ユーザー側はデータを書き換えれない
   -----------------------------------------------*/
  class Button {
   protected:
    const int    id_;
    bool         state_{bstate::OFF};
    ButtonAction action_{ButtonAction::None};

    void update(bool state, ButtonAction action);
    void update(ButtonAction action);

    friend class ButtonData;

   public:
    Button(int event_number);
    Button* get();
    int  getID();
    bool pressed();
    bool pushed();
    bool released();
  };

  inline Button* Button::get() { return this; }

  inline int Button::getID() { return this->id_; }

  /**------------------------------------------
   * [ Axis class ]
   * スティック・ボタン深度の，可変値をデータとして提供
   * その他Buttonクラスと同様の仕様
   -------------------------------------------*/
  class Axis {
   private:
    const int id_;
    int16_t   value_{0};

    void update(int16_t value);

    friend class AxisData;
    
   public:
    Axis(int event_number);
    Axis* get();
    int getID();
    int getValue();
  };

  inline Axis* Axis::get() { return this; }

  inline int Axis::getID() { return this->id_; }

  /**--------------------------------
   * [ TriggerButton class ]
   * メインはButtonクラス(継承)
   * 深度用のAxisクラスをデータとして持つ
   * L2R2ボタンに採用
   ---------------------------------*/
  class TriggerButton: public Button {
   private:
    Axis depth_;

    //friend void ButtonData::update(ButtonEvent event);

   public:
    TriggerButton(int b_id, int a_id);
    using Button::update;
    int   getDepth();
    Axis* getAxis(); 
  };

  inline int TriggerButton::getDepth() { return depth_.getValue(); }

  inline Axis* TriggerButton::getAxis() { return this->depth_.get(); }

  /**---------------------
   * [ Stick class ]
   * X軸Y軸の2つのAxisを持つ
   -----------------------*/
  class Stick {
   public:
    Axis x;
    Axis y;

    Stick(int x_id, int y_id);
  };

  /**----------------------------------------
   * [ ButtonData class ]
   * Padが持つButtonクラスを管理する
   * 各ButtonのポインタをID順に配列で保持
   * Eventを受け取ることでポインタ経由でデータ更新
   -----------------------------------------*/
  class ButtonData {
   private:
    ButtonEvent event_;
    std::vector<Button*> list_{nullptr};

   public:
    ButtonData();
    void resister(std::vector<Button*> buttons);
    void update(ButtonEvent event);
    void resetAction();
    void clear();
  };

  /**----------------------------------------
   * [ AxisData class ]
   * Padが持つAxisクラスを管理する
   * 各AxisのポインタをID順に配列で保持
   * Eventを受け取ることでポインタ経由でデータ更新
   -----------------------------------------*/
  class AxisData {
   private:
    AxisEvent event_;
    std::vector<Axis*> list_;

   public:
    AxisData();
    void resister(std::vector<Axis*> axes);
    void update(AxisEvent event);
    void clear();
  };

  /**--------------------------------------------
   * [ EventHandler class ]
   * PadReaderから送られたイベントを受け取る
   * コントローラーのデータに整合するようにイベントを編集
   * イベントの編集関数を各コントローラー用にオーバーライドして実装
   ----------------------------------------------*/
  class EventHandler {
   protected:
    PadEvent    event_;
    ButtonEvent button_event_{0, bstate::OFF};
    AxisEvent   axis_event_{0, 0};

    virtual void handleButtonEvent() = 0;
    virtual void handleAxisEvent() = 0;

   public:
    EventType   getEventType();
    ButtonEvent getButtonEvent();
    AxisEvent   getAxisEvent();
    virtual void editEvent(PadEvent event) = 0;
  };

  inline EventType EventHandler::getEventType() { return this->event_.type; }

  inline ButtonEvent EventHandler::getButtonEvent() { return this->button_event_; }

  inline AxisEvent EventHandler::getAxisEvent() { return this->axis_event_; }

  /**-----------------------------------------------------
   * [ PadReader class ]
   * デバイス名を指定することで，それに対応するデバイスファイルを探索
   * デバイスファイルを通してコントローラーのイベントを取得
   ------------------------------------------------------*/
  class PadReader {
   private:
    bool      is_connected_;
    js_event  raw_event_;
    DevFile   devfile_;
    PadEvent  event_;

    void findDeviceName(std::string devname, std::ifstream& read_stream);
    void findDeviceHandler(std::ifstream& read_stream);
    void openDeviceFile();

   public:
    PadReader();
    ~PadReader();

    bool connect(std::string devname);
    void disconnect();
    bool readData();
    bool isConnected();
    PadEvent getPadEvent();
  };

  inline bool PadReader::isConnected() { return this->is_connected_; }

  inline PadEvent PadReader::getPadEvent() { return this->event_; }
}
