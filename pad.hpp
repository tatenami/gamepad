#ifndef PAD_H
#define PAD_H

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
#include <memory>
#include <cmath>

namespace pad {

  namespace {
    const std::string devlist_path = " /proc/bus/input/devices";
  }

  enum class Connect {
    USB, Bluetooth
  };

  enum class EventType {
    None, Button, Axis
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

    PadEvent getPadEvent() {
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

  /**
   * @brief 
   * 
   */

  class PadEventHandler {
   protected:
    std::unordered_map<uint, uint8_t> id_map_;
    PadEvent    event_;
    ButtonEvent button_event_{0, false};
    AxisEvent   axis_event_{0, 0.0};
    float       deadzone_{0.0};
    uint32_t    axis_max_;

    virtual void editButtonEvent() = 0;
    virtual void editAxisEvent() = 0;

   public:
    PadEventHandler(uint32_t axis_max);
    void setDeadzone(float deadzone); 
    void editEvent(PadReader& reader);
    void addCodeIdEntry(uint event_code, uint8_t ui_id);

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
    EventType type_;
    std::vector<T> list_; 

   public:
    InputData(uint total_input);
    virtual void clear() = 0;
    const std::vector<T> getVector();
  };

  class ButtonData: public InputData<bool> {
   protected:
    bool update_flag_{false};
    ButtonEvent event_{0, false};

   public:
    ButtonData(uint total_input);
    void update(ButtonEvent event);
    void clear() override;

    inline bool isUpdated() {
      return this->update_flag_;
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
    void update(AxisEvent event);
    void clear() override;

    inline float getValue(uint8_t id) {
      return this->list_.at(id);
    }
  };  

  /**
   * @brief 
   * 
   */
  class PadUI {
   protected:
    const uint8_t id_;

   public:  
    PadUI(const uint8_t id);
    uint8_t getID();
  };


  class Button: public PadUI {  
   protected:
    ButtonData *ad_;

   public:
    using PadUI::PadUI;
    Button& get();
    void attach(ButtonData& ad);
    bool pressed();
    bool pushed();
    bool released();
  };

  class Axis: public PadUI {
   protected:
    AxisData *ad_;

   public:
    using PadUI::PadUI;
    Axis& get();
    void  attach(AxisData& ad);
    float getValue();
  };


  class Trigger: public Button {
   private:
    Axis depth_;

   public:
    Trigger(uint8_t b_event_num, uint8_t a_event_num);
    float getDepth();
    Axis& getAxis();
  };


  class Stick {
   public:
    Axis x;
    Axis y;

    Stick(uint8_t x_event_num, uint8_t y_event_num);
    float angleDeg();
    float angleRad();
  };
  
  template <class Handler>
  class BasePad {
   protected:
    std::unique_ptr<PadEventHandler> handler_;
    PadReader   reader_;
    ButtonData  buttons_;
    AxisData    axes_;
    bool is_connected_{false};

   public:
    BasePad(uint b_total, uint a_total);

    ~BasePad() {
      reader_.disconnect();
    }

    bool isConnected() {
      return is_connected_;
    }
    
    virtual void update();
  };
}
#endif // PAD_H