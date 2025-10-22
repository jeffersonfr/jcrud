#pragma once

#include "api/mapping/MappingTraits.hpp"

struct Mapping {
    // generic implementations
    template<typename Range>
    static crow::json::wvalue range_to_json(Range modelList) {
        std::vector<crow::json::wvalue> itemList;

        std::ranges::for_each(modelList,
                              [&](auto const &item) {
                                  itemList.push_back(item_to_json(item));
                              });

        return itemList;
    }

    template<typename Begin, typename End>
    static crow::json::wvalue range_to_json(Begin itBegin, End itEnd) {
        std::vector<crow::json::wvalue> itemList;

        std::ranges::for_each(itBegin, itEnd,
                              [&](auto const &item) {
                                  itemList.push_back(item_to_json(item));
                              });

        return itemList;
    }

    template<typename Model, typename Range>
    static crow::json::wvalue range_to_json(Range modelList) {
        std::vector<crow::json::wvalue> itemList;

        std::ranges::for_each(modelList,
                              [&](auto const &item) {
                                  Model model = item.template get<Model>();

                                  itemList.push_back(item_to_json(model));
                              });

        return itemList;
    }

    template<typename Model, typename Begin, typename End>
    static crow::json::wvalue range_to_json(Begin itBegin, End itEnd) {
        std::vector<crow::json::wvalue> itemList;

        std::ranges::for_each(itBegin, itEnd,
                              [&](auto const &item) {
                                  Model model = item.template get<Model>();

                                  itemList.push_back(item_to_json(model));
                              });

        return itemList;
    }

    template<typename Model>
    static crow::json::wvalue item_to_json(Model const &model) {
        return to_json(model);
    }

    template<typename Model>
    static Model item_from_json(crow::json::wvalue const &item) {
        return from_json<Model>(item);
    }
};
