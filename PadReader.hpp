#ifndef PAD_READER_H
#define PAD_READER_H

#include "Pad.hpp"

namespace Pad {

  class PadReader {
   private:
    js_event read_event_;
    DevFile  devfile_;
    PadInfo  padinfo_;
    PadEvent event_;
    bool     is_connected_;

    void findDeviceName(std::string devname, std::ifstream& read_stream);
    void findDeviceHandler(std::ifstream& read_stream);
    void openDeviceFile();

   public:
    PadReader(std::string devname, ReadMode mode);
    ~PadReader();

    bool connect(std::string devname, ReadMode mode);
    void update();

    PadEvent getEvent();
    PadInfo  getPadInfo();
    bool     isConnected();
  }; 
}

#endif // PAD_READER_H