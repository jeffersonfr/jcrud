#pragma once

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

    // local implementations .. try to use some generic implementation to avoid do this manually
    static crow::json::wvalue item_to_json(ProdutoModel model) {
        crow::json::wvalue item;

        item["id"] = *model["id"].get_int();
        item["categoria_id"] = *model["categoria_id"].get_int();
        item["nome"] = *model["nome"].get_text();
        item["descricao"] = *model["descricao"].get_text();

        return item;
    }

    static crow::json::wvalue item_to_json(CategoriaProdutoModel model) {
        crow::json::wvalue item;

        item["id"] = *model["id"].get_int();
        item["descricao"] = *model["descricao"].get_text();

        return item;
    }
};
