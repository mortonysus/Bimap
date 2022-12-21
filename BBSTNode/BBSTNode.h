#pragma once

#include "BSTNodeLinks.hpp"
#include <utility>

// Может быть тут хочется #define, а может и не хочется.
constexpr bool LO = false; // LeftOrder
constexpr bool RO = true;  // RightOrder

// BBSTNode - Узел в графе, в котором каждая вершина связана одновременно по
// двум правилам BST.
// Реализация данные-связи в одном месте сделана для уменьшения кол-ва
// аллокаций.
template <typename Left, typename Right>
class BBSTNode {
  using NodeLinks = BSTNodeLinks<BBSTNode>;
  using ThisType = BBSTNode<Left, Right>;

private:
  std::pair<Left, Right> _data;
  std::pair<NodeLinks, NodeLinks> _links;

public:
  // order ---------------------------------------------------------------------

  // В public для того чтобы можно было использовать в bimap.

  // order_type<order>::type -> Left / Right
  template <bool order, typename Dummy = void>
  struct order_type;

  template <typename Dummy>
  struct order_type<LO, Dummy> {
    using type = Left;
  };

  template <typename Dummy>
  struct order_type<RO, Dummy> {
    using type = Right;
  };
  // ---------------------------------------------------------------------------

  // Шаблонный конструктор с форвардом (удар !!!)
  template <typename left_pr = Left, typename right_pr = Right>
  explicit BBSTNode(left_pr&& left, right_pr&& right,
                    std::pair<ThisType*, ThisType*> const& parents = {nullptr,
                                                                      nullptr});
  /*
    * Доступ к data:
  * Метод data используется в других шаблонах, поэтому требуется использовать
      статический полиморфизм.
  * Нужно с помощью статического полиморфизма возвращать Left или Right.
  * Left и Right могут не совпадать, поэтому нужен тип возвращаемого значения.
  * Left и Right могут совпадать => тип возвращаемого значения не может
      однозначно указывать на то какой элемент пары нужно вернуть =>
      нужен некоторый тэг который будет однозначно относиться к Left или Right.
  * Тэг сам по себе содержит информацию о том какой тип нужен =>
      логичным будет вызов data<order>().
  * В константной версии нет необходимости.
  */
  template <bool order>
  inline typename order_type<order>::type& data() {
    return std::get<order>(_data);
  }

  // Возвращает ссылку на родственников узла
  // В константной версии нет необходимости.
  inline NodeLinks& links(bool order) {
    return !order ? _links.first : _links.second;
  }
};

#include "BBSTNode.hpp"
