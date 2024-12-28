#include "pad.hpp"

namespace pad {
  /* [ PadUI member functions ] */

  PadUI::PadUI(const uint8_t id): id_{id} {}

  uint8_t PadUI::getID() { return this->id_; }


  void Button::attach(ButtonData& ad) {
    this->ad_ = &ad;
  }

  bool Button::pressed() {
    return ad_->getState(this->id_);
  }

  bool Button::pushed() {
    if (!ad_->IDUpdated(this->id_)) return false;

    if (ad_->getState(this->id_) == true) {
      return true;
    }
    else {
      return false;
    }
  }

  bool Button::released() {
    if (!ad_->IDUpdated(this->id_)) return false;

    if (ad_->getState(this->id_) == false) {
      return true;
    }
    else {
      return false;
    }
  }


  void Axis::attach(AxisData& ad) {
    this->ad_ = &ad;
  }

  float Axis::getValue() {
    return ad_->getValue(this->id_);
  }


  Trigger::Trigger(uint8_t button_id, uint8_t axis_id): 
    Button(button_id),
    depth(axis_id)
  {

  }

  float Trigger::getDepth() { return this->depth.getValue(); }


  Stick::Stick(uint8_t x_id, uint8_t y_id):
    x(x_id),
    y(y_id)
  {

  }

  float Stick::angleDeg() {
    float rad = atan2f(y.getValue(), x.getValue());
    return rad * (180 / M_PI);
  }

  float Stick::angleRad() {
    return atan2f(y.getValue(), x.getValue());
  }
}