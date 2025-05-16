#pragma once

#include "database/DataClass.hpp"

template<typename... Models>
struct CompoundModel : public Models... {
  CompoundModel() = default;

  CompoundModel(Models const &... models) { ((set<Models>(models)), ...); }

  template<typename T>
  void set(T const &model) { (T &) (*this) = model; }

  template<typename T>
  T &get() const { return (T &) (*this); }

  template<typename T>
  Data &get(std::string const &id) const {
    return ((T &) (*this))[id];
  }

  friend std::ostream &operator<<(std::ostream &out,
                                  CompoundModel const &value) {
    for_each<0, Models...>(out, value);

    return out;
  }

private:
  template<std::size_t Index, typename Arg, typename... Args>
  static constexpr void for_each(std::ostream &out,
                                 CompoundModel const &value) {
    if constexpr (Index > 0) {
      out << ", ";
    }

    out << std::quoted(Arg::get_name(), '\'') << ": " << value.get<Arg>();

    if constexpr (sizeof...(Args) > 0) {
      return for_each<Index + 1, Args...>(out, value);
    }
  }

  template<std::size_t Index>
  static constexpr void for_each(std::ostream &out) {
  }
};

template<typename... Models>
struct fmt::formatter<CompoundModel<Models...> > : fmt::ostream_formatter {
};
