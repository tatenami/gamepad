#ifndef PAD_BASE_H
#define PAD_BASE_H

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>

#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include <unordered_map>
#include <vector>
#include <cmath>

namespace pad {

  using ui_id = unsigned char;
  // デバイスファイルのCODE->インターフェースのID の変換テーブルmap
  using code_id_map = std::unordered_map<uint, ui_id>;

  const float default_deadzone = 0.05;
  const int num_default_buttons = 20;
  const int num_default_axes = 8;

  namespace {
    const std::string devlist_path = "/proc/bus/input/devices";
  }

  enum class EventType {
    None, Button, Axis
  };

  enum class ErrorCode {
    None,
    FindDeviceName,
    FindDeviceFile,
    OpenDeviceFile
  };


  struct PadEvent {
    unsigned short code;
    int32_t   value;
    EventType type;
  };


  class PadReader {
   private:
    std::string dev_path_ = "/dev/input/";

    bool connection_;
    int  fd_;
    ErrorCode code_;
    input_event raw_event_;
    PadEvent    event_;

    void findDeviceName(std::string devname, std::ifstream& stream);
    void findDeviceHandler(std::ifstream& stream);
    void openDeviceFile();

   public:
    ~PadReader();

    bool connect(std::string devname);
    void disconnect();
    bool readEvent();

    inline bool isConnected() {
      return this->connection_;
    }

    inline int getErrorCode() {
      return static_cast<int>(this->code_);
    }

    inline PadEvent getPadEvent() {
      return this->event_;
    }
  };


  struct ButtonEvent {
    ui_id id;
    bool  state;
  };

  struct AxisEvent {
    ui_id id;
    float value;
  };


  class PadEventEditor {
   protected:
    code_id_map id_map_;
    PadEvent    event_;
    ButtonEvent button_event_{0, false};
    AxisEvent   axis_event_{0, 0.0};
    float deadzone_{default_deadzone};

    virtual void editButtonEvent() = 0;
    virtual void editAxisEvent() = 0;

   public:
    void editEvent(PadReader& reader);
    void addCodeIdEntry(uint event_code, ui_id id);
    void setDeadZone(float deadzone);

    inline EventType getEventType() { 
      return this->event_.type; 
    }

    inline ButtonEvent& getButtonEvent() {
      return this->button_event_;
    }

    inline AxisEvent& getAxisEvent() {
      return this->axis_event_;
    }    
  };


  template <typename T>
  class InputData {
   protected: 
    uint size_;
    std::vector<T> list_; 

   public:
    InputData(uint total_input) {
      this->list_.resize(total_input);
      this->size_ = list_.size();
    }

    std::vector<T> getVector() {
      return this->list_;
    }

    void resize(int total_input) {
      this->list_.resize(total_input);
    }

    virtual void clear() = 0;
    virtual void update(PadEventEditor& editor) = 0;
  };

  class ButtonData: public InputData<bool> {
   protected:
    bool update_flag_{false};
    ButtonEvent event_{0, false};

   public:
    ButtonData(uint total_input);
    void clear() override;
    void update(PadEventEditor& editor) override;

    inline void resetUpdateFlag() {
      this->update_flag_ = false;
    }

    inline bool updated() {
      return update_flag_;
    }

    inline ui_id getEventId() {
      return event_.id;
    }

    inline bool getState(ui_id id) {
      if (id >= this->size_) {
        return false;
      }
      return this->list_[id];
    }
  };
 
  class AxisData: public InputData<float> {
   protected:
    AxisEvent event_{0, 0.0};

   public:  
    AxisData(uint total_input);
    void clear() override;
    void update(PadEventEditor& editor) override;

    inline float getValue(ui_id id) {
      if (id >= this->size_)   {
        return (float)0.0;
      }
      return this->list_[id];
    }
  };  
  

  template <class Editor>
  class BasePad {
   protected:
    bool is_connected_{false};
    std::string device_name_;
    PadReader   reader_;
    ButtonData  buttons_;
    AxisData    axes_;
    std::unique_ptr<PadEventEditor> editor_;

   public:
    BasePad(std::string device_name, 
            int num_buttons = num_default_buttons,
            int num_axes = num_default_axes):
      buttons_(num_buttons),
      axes_(num_axes)
    {
      this->device_name_ = device_name;
      this->editor_ = std::make_unique<Editor>();
      this->is_connected_ = this->reader_.connect(device_name);
    }

    ~BasePad() {
      this->reader_.disconnect();
    }

    bool isConnected() {
      return is_connected_;
    }

    bool reconnect() {
      if (this->is_connected_) {
        return false;
      }

      reader_.disconnect();
      return reader_.connect(device_name_);
    }
    
    void setDeadZone(float deadzone) {
      this->editor_->setDeadZone(deadzone);
    }

    void update() {
      if (!(reader_.isConnected())) {
        this->is_connected_ = false;
        buttons_.clear();
        axes_.clear();
        return;
      }

      if (this->reader_.readEvent()) {
        (*editor_).editEvent(reader_);

        buttons_.update(*editor_);
        axes_.update(*editor_);
      }
    }
  };

}
#endif // PAD_BASE_H