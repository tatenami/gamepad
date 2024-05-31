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

namespace Pad {

  namespace Common {
    const std::string devfile_path = "/dev/input/";
    const std::string devinfo_path = "/proc/bus/input/devices";
    const int8_t event_type_filter = ~(static_cast<int8_t>(JS_EVENT_INIT));
  } 

  enum class EventType {
    NONE, EVENT_BUTTON, EVENT_AXIS
  };

  enum class ReadMode {
    BLOKING, NON_BLOKING
  };

  struct PadEvent {
    EventType type;
    uint8_t   number;
    int16_t   value;
  };

  struct DevFile {
    int         fd;
    std::string path;
  };

  struct PadInfo {
    uint8_t     num_of_button;
    uint8_t     num_of_axis;
    std::string pad_name;
  };

}

#endif // PAD_H