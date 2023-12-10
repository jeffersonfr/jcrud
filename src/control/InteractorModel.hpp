#pragma once

#include "model/DataClass.hpp"

template <typename ...Models>
struct InteractorModel : public Models... {
  InteractorModel() = default;

  InteractorModel(Models const &...models) {
    ((set<Models>(models)), ...);
  }

  template <typename T>
  void set(T const &model) {
    (T &)(*this) = model;
  }

  template <typename T>
  T & get() const {
    return (T &)(*this);
  }

  template <typename T>
  Data & get(std::string const &id) const {
    return ((T &)(*this))[id];
  }
};