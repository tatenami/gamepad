#include "pad.hpp"

namespace pad {

  /* [ PadUI member functions ] */

  PadUI::PadUI(const uint8_t id): id_{id} {}

  uint8_t PadUI::getID() { return this->id_; }
  
  /* [ InputData member functions ] */

  template <typename T>
  InputData<T>::InputData(uint total_input) {
    this->list_.resize(total_input);
  }

  template <typename T>
  const std::vector<T> InputData<T>::getVector() {
    return this->list_;
  }


  ButtonData::ButtonData(uint total_input):
    InputData(total_input)
  {
    this->type_ = EventType::Button;
    this->clear();
  }

  void ButtonData::update(ButtonEvent event) {
    update_flag_ = true;
    event_ = event;
    list_.at(event_.id) = event_.state;
  }

  void ButtonData::clear() {
    for (auto e: this->list_) {
      e = false;
    }
  }



  AxisData::AxisData(uint total_input):
    InputData(total_input) 
  {
    this->type_ = EventType::Axis;
    this->clear();
  }

  void AxisData::update(AxisEvent event) {
    event_ = event;
    list_.at(event_.id) = event_.value;
  }

  void AxisData::clear() {
    for (auto e: this->list_) {
      e = 0.0;
    }
  }


  Button& Button::get() {
    return *this;
  }

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


  Axis& Axis::get() {
    return *this;
  }

  void Axis::attach(AxisData& ad) {
    this->ad_ = &ad;
  }

  float Axis::getValue() {
    return ad_->getValue(this->id_);
  }


  Trigger::Trigger(uint8_t button_id, uint8_t axis_id): 
    Button(button_id),
    depth_(axis_id)
  {

  }

  float Trigger::getDepth() { return this->depth_.getValue(); }

  Axis& Trigger::getAxis() { return this->depth_.get(); }


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