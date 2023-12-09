#pragma once

template <typename ...Models>
struct InteractorModel : public Models... {
  template <typename T>
  void set(T const &model) {
    *this = model;
  }

  template <typename T>
  T & get() const {
    return static_cast<T const &>(*this);
  }
};
