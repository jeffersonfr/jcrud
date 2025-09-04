#pragma once

template <typename Model>
crow::json::wvalue to_json(Model const &model) {
  crow::json::wvalue item;

  Model::get_fields(
    [&]<typename Field>() {
      std::string const id = Field::get_name();

      if (Field::get_type() == jdb::FieldType::Serial) {
        item[id] = model[id].get_int().value_or(-1L);
      } else if (Field::get_type() == jdb::FieldType::Bool) {
        item[id] = model[id].get_bool().value_or(false);
      } else if (Field::get_type() == jdb::FieldType::Int) {
        item[id] = model[id].get_int().value_or(-1L);
      } else if (Field::get_type() == jdb::FieldType::Decimal) {
        item[id] = model[id].get_decimal().value_or(0.0);
      } else if (Field::get_type() == jdb::FieldType::Text) {
        item[id] = model[id].get_text().value_or("");
      } else if (Field::get_type() == jdb::FieldType::Timestamp) {
        item[id] = model[id].get_text().value_or("");
      }
    });

  return item;
}

template <typename Model>
Model from_json(crow::json::wvalue const &item) {
  Model model;

  Model::get_fields(
  [&]<typename Field>() {
    std::string const id = Field::get_name();

    if (Field::get_type() == jdb::FieldType::Serial) {
      model[id] = crow::json::wvalue_reader{item[id]}.get(-1L);
    } else if (Field::get_type() == jdb::FieldType::Bool) {
      model[id] = crow::json::wvalue_reader{item[id]}.get(false);
    } else if (Field::get_type() == jdb::FieldType::Int) {
      model[id] = crow::json::wvalue_reader{item[id]}.get(-1L);
    } else if (Field::get_type() == jdb::FieldType::Decimal) {
      model[id] = crow::json::wvalue_reader{item[id]}.get(0.0);
    } else if (Field::get_type() == jdb::FieldType::Text) {
      model[id] = crow::json::wvalue_reader{item[id]}.get(std::string{});
    } else if (Field::get_type() == jdb::FieldType::Timestamp) {
      model[id] = crow::json::wvalue_reader{item[id]}.get(std::string{});
    }
  });

  return model;
}
