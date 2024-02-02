#pragma once

#include <functional>
#include <stdio.h>

template <typename T> struct MutableState;

template <typename T> struct State {
  State(MutableState<T> &state) : mState{&state} {}

  virtual void observe(std::function<void(T const &)> callback) {
    mState->mCallback = callback;
  }

protected:
  State() : mState{nullptr} {}

private:
  MutableState<T> *mState = nullptr;
};

template <typename T> struct MutableState : public State<T> {
  friend class State<T>;

  MutableState() : State<T>() {}

  void observe(std::function<void(T const &)> callback) override {
    mCallback = callback;
  }

  void notify(T const &data) { mCallback(data); }

private:
  std::function<void(T const &)> mCallback;
};
