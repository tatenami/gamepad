#ifndef PAD_BASE_H
#define PAD_BASE_H

#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
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

  const float default_deadzone = 0.05;
  const int default_button_num = 20;
  const int default_axis_num = 8;

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

  /**
   * @brief 
   * 
   */
  struct PadEvent {
    uint16_t  code;
    int32_t   value;
    EventType type;
  };


  /**
   * @brief read event data of game controller 
   * 
   */
  class PadReader {
   private:
    std::string path = "/dev/input/";

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
    uint8_t id;
    bool    state;
  };

  struct AxisEvent {
    uint8_t id;
    float   value;
  };

  using code_id_map = std::unordered_map<uint, uint8_t>;

  /**
   * @brief 
   * 
   */
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
    void addCodeIdEntry(uint event_code, uint8_t ui_id);
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

      /**
   * @brief 
   * 
   * @tparam T 
   */
  template <typename T>
  class InputData {
   protected: 
    uint size_;
    EventType type_;
    std::vector<T> list_; 

   public:
    InputData(uint total_input) {
      this->list_.resize(total_input);
      this->size_ = list_.size();
    }

    std::vector<T> getVector() {
      return this->list_;
    }

    int getSize() {
      return this->list_.size();
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

    inline bool IDUpdated(uint8_t id) {
      return update_flag_ && (event_.id == id);
    }

    inline void resetUpdateFlag() {
      this->update_flag_ = false;
    }

    inline bool getState(uint8_t id) {
      return this->list_.at(id);
    }
  };
 
  class AxisData: public InputData<float> {
   protected:
    AxisEvent event_{0, 0.0};

   public:  
    AxisData(uint total_input);
    void clear() override;
    void update(PadEventEditor& editor) override;

    inline float getValue(uint8_t id) {
      return this->list_.at(id);
    }
  };  
  

  template <class Editor>
  class BasePad {
   protected:
    std::unique_ptr<PadEventEditor> editor_;
    PadReader   reader_;
    ButtonData  buttons_;
    AxisData    axes_;
    bool is_connected_{false};

   public:
    BasePad(std::string device_name, 
            int button_num = default_button_num, 
            int axis_num = default_axis_num):
      buttons_(button_num),
      axes_(axis_num)
    {
      this->editor_ = std::make_unique<Editor>();
      this->is_connected_ = reader_.connect(device_name);
    }

    ~BasePad() {
      reader_.disconnect();
    }

    bool isConnected() {
      return is_connected_;
    }
    
    void setDeadZone(float deadzone) {
      editor_->setDeadZone(deadzone);
    }

    void resizeInputTotal(int total_button, int total_axis) {
      buttons_.resize(total_button);
      axes_.resize(total_axis);
    }

    void update() {
      if (!reader_.isConnected()) {
        is_connected_ = false;
        buttons_.clear();
        axes_.clear();
        return;
      }

      if (reader_.readEvent()) {
        (*editor_).editEvent(reader_);

        buttons_.update(*editor_);
        axes_.update(*editor_);
      }
    }
  };

}
#endif // PAD_BASE_H