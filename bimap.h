#pragma once

#include "BBSTNode/BBSTNode.h"
#include <cstddef>
#include <functional>
#include <stdexcept>

// Обертка для компараторов, чтобы можно было унаследоваться от компараторов
// одного типа.
template <typename Compare, bool order>
struct CompareWrap : public Compare {
  explicit CompareWrap(Compare&& compare) : Compare(std::move(compare)) {}
};

template <typename Left, typename Right, typename CompareLeft = std::less<Left>,
          typename CompareRight = std::less<Right>>
class bimap : public CompareWrap<CompareLeft, LO>,
              public CompareWrap<CompareRight, RO> {
  using left_t = Left;
  using right_t = Right;
  using node_t = BBSTNode<Left, Right>;

  // Iterators -----------------------------------------------------------------

  // iterator.h
  template <typename Key, typename Value, bool order>
  class iterator;

  // iterator_type<order>::type -> order_iterator type
  template <bool order, typename Dummy = void>
  struct iterator_type;

  template <typename Dummy>
  struct iterator_type<LO, Dummy> {
    using type = iterator<left_t, right_t, LO>;
  };

  template <typename Dummy>
  struct iterator_type<RO, Dummy> {
    using type = iterator<right_t, left_t, RO>;
  };

  // Удобные синонимы
  using left_iterator = typename iterator_type<LO>::type;
  using right_iterator = typename iterator_type<RO>::type;
  // ---------------------------------------------------------------------------

  // Comparators ---------------------------------------------------------------

  // compare_type<order>::type -> CompareWrap<CompareOrder> type
  template <bool order, typename Dummy = void>
  struct compare_type;

  template <typename Dummy>
  struct compare_type<LO, Dummy> {
    using type = CompareWrap<CompareLeft, LO>;
  };

  template <typename Dummy>
  struct compare_type<RO, Dummy> {
    using type = CompareWrap<CompareRight, RO>;
  };
  // ---------------------------------------------------------------------------

  // Состояния при выходе из бинарного поиска для bin_search.
  enum state { st_equal, st_left, st_right, st_none };

private:
  size_t _size{0};
  std::pair<node_t*, node_t*> _roots;

  // Поиск по ключу в BST.
  // Возвращает в какой ветке был выход, и адрес node_t на которой был выход.
  template <bool order>
  std::pair<state, node_t*> bin_search(
      typename node_t::template order_type<order>::type const& key) const;

  // Обертка над bin_search для поиска элемента по ключу.
  // Возвращает найденную адрес node_t или nullptr.
  template <bool order>
  node_t*
  find(typename node_t::template order_type<order>::type const& key) const;

  // Обертка над find для at_left, at_right.
  // Возвращает const& на противоположный элемент по ключу.
  template <bool order>
  typename node_t::template order_type<!order>::type const&
  at(typename node_t::template order_type<order>::type const& key) const;

  // Шаблон для методов at_left_or_default, at_right_or_default.
  // Если находит нужную пару по ключу - не изменяет ее.
  // Иначе если находит дефолтный элемент в противоположном порядке
  //    - изменяет парное к нему значение на ключ.
  // Иначе вставляет пару из ключа и дефолтного элемента.
  // Возвращает адрес node_t, содержащей нужное значение в порядке !order
  template <bool order>
  node_t*
  at_or_default(typename node_t::template order_type<order>::type const& key);

  // Bound ---------------------------------------------------------------------
  // Шаблон для методов lower_bound_left, lower_bound_right
  template <bool order>
  typename iterator_type<order>::type lower_bound(
      typename node_t::template order_type<order>::type const& key) const;

  // Шаблон для методов upper_bound_left, upper_bound_right.
  template <bool order>
  typename iterator_type<order>::type upper_bound(
      typename node_t::template order_type<order>::type const& key) const;
  // ---------------------------------------------------------------------------

  // erase ---------------------------------------------------------------------

  // Удаление по адресу node_t.
  // Без проверки, гарантируется не nullptr.
  void erase(node_t* node);

  // Удаление по итератору.
  // Обертка над erase node_t*.
  // Возвращает итератор на элемент следовавший за удаленным.
  template <bool order>
  typename iterator_type<order>::type
  erase(typename iterator_type<order>::type it);

  // Удаление по ключу.
  // Шаблон для erase_left, erase_right.
  // Возвращает: был ли удален элемент с переданным ключом.
  template <bool order>
  bool erase(typename node_t::template order_type<order>::type const& key);

  // Удаление ренжа.
  // Обертка над erase по итератору.
  // Возвращает итератор на last.
  template <bool order>
  typename iterator_type<order>::type
  erase(typename iterator_type<order>::type first,
        typename iterator_type<order>::type last);
  // ---------------------------------------------------------------------------

  // Вспомогательный метод для insert_pr.
  // Возвращает:
  // {Адрес по которому можно вставить ноду, Адрес предка который его содержит}
  // * Адрес предка который содержит потомка куда мы будем вставлять ноду нужен
  //    для того чтобы указать родителей ноды при создании.
  // * Адрес по которому можно вставить ноду это node**,
  //    т.к. по умолчанию там nullptr => нужен именно адрес адреса.
  template <bool order>
  std::pair<node_t**, node_t*>
  insert_ad(typename node_t::template order_type<order>::type const& key) const;

  // Шаблонная перегрузка с форвардом для insert'ов
  template <typename left_t_pr = left_t, typename right_t_pr = right_t>
  node_t* insert_pr(left_t_pr&& left, right_t_pr&& right);

  // ---------------------------------------------------------------------------

  // min/max, prev/next --------------------------------------------------------
  // Вспомогательный метод для next - поиск минимального элемента по ноде.
  static node_t* min(node_t* node, bool order);

  // Вспомогательный метод для prev - поиск максимального элемента по ноде.
  static node_t* max(node_t* node, bool order);

  // Вспомогательный метод для итератора O(h)
  static node_t* prevNode(node_t* node, bool order);

  // Вспомогательный метод для итератора O(h)
  static node_t* nextNode(node_t* node, bool order);
  // ---------------------------------------------------------------------------

  // Налаживание адресации в деревьях при удалении -----------------------------

  // Комментарии поехали после ctrl+alt+l
  void deleteWithBothChildren(node_t* node, bool order);
  void deleteWithLeftChild(node_t* node, bool order);
  void deleteWithRightChild(node_t* node, bool order);
  void deleteWithoutChildren(node_t* node, bool order);
  void deleteInOrder(node_t* node, bool order);
  // ---------------------------------------------------------------------------

  // Вспомогательный метод для деструктора.
  void delete_tree(node_t* node);

  // Корни деревьев ------------------------------------------------------------

  // Там где нужна копия указателя на корень
  inline node_t* root(bool order) const {
    return (!order ? _roots.first : _roots.second);
  }

  // Там где нужно изменять корень
  inline node_t*& root(bool order) {
    return (!order ? _roots.first : _roots.second);
  }
  // ---------------------------------------------------------------------------
public:
  // Создает bimap не содержащий ни одной пары.
  explicit bimap(CompareLeft compare_left = CompareLeft(),
                 CompareRight compare_right = CompareRight());

  // Конструкторы от других и присваивания -------------------------------------
  bimap(bimap const& other);
  bimap(bimap&& other) noexcept;

  bimap& operator=(bimap const& other);
  bimap& operator=(bimap&& other) noexcept;
  // ---------------------------------------------------------------------------

  // Этого не было в тз, результат - хз
  void swap(bimap& other);

  // Деструктор. Вызывается при удалении объектов bimap.
  // Инвалидирует все итераторы ссылающиеся на элементы этого bimap
  // (включая итераторы ссылающиеся на элементы следующие за последними).
  ~bimap() {
    delete_tree(_roots.first);
  }

  // Вставка
  inline left_iterator insert(left_t const& left, right_t const& right) {
    return {this, insert_pr(left, right)};
  }
  inline left_iterator insert(left_t const& left, right_t&& right) {
    return {this, insert_pr(left, std::move(right))};
  }
  inline left_iterator insert(left_t&& left, right_t const& right) {
    return {this, insert_pr(std::move(left), right)};
  }
  inline left_iterator insert(left_t&& left, right_t&& right) {
    return {this, insert_pr(std::move(left), std::move(right))};
  }

  // Удаляет элемент и соответствующий ему парный.
  // Erase невалидного итератора неопределен.
  // erase(end_left()) и erase(end_right()) неопределены.
  // Пусть it ссылается на некоторый элемент e.
  // Erase инвалидирует все итераторы ссылающиеся на e и на элемент парный к e.
  inline left_iterator erase_left(left_iterator it) {
    return erase<LO>(it);
  }
  inline right_iterator erase_right(right_iterator it) {
    return erase<RO>(it);
  }

  // Аналогично erase, но по ключу, удаляет элемент если он присутствует,
  // иначе не делает ничего, возвращает была ли пара удалена
  inline bool erase_left(left_t const& key) {
    return erase<LO>(key);
  }
  inline bool erase_right(right_t const& key) {
    return erase<RO>(key);
  }

  // erase от ренжа, удаляет [first, last), возвращает итератор на последний
  // элемент за удаленной последовательностью
  inline left_iterator erase_left(left_iterator first, left_iterator last) {
    return erase<LO>(first, last);
  }
  inline right_iterator erase_right(right_iterator first, right_iterator last) {
    return erase<RO>(first, last);
  }

  // Возвращает противоположный элемент по элементу
  // Если элемента не существует -- бросает std::out_of_range
  inline right_t const& at_left(left_t const& key) const {
    return at<LO>(key);
  }
  inline left_t const& at_right(right_t const& key) const {
    return at<RO>(key);
  }

  // Возвращает итератор по элементу. Если не найден - соответствующий end()
  inline left_iterator find_left(left_t const& key) const {
    return {this, find<LO>(key)};
  }
  inline right_iterator find_right(right_t const& key) const {
    return {this, find<RO>(key)};
  }

  // Возвращает противоположный элемент по элементу
  // Если элемента не существует, добавляет его в bimap и на противоположную
  // сторону кладет дефолтный элемент, ссылку на который и возвращает
  // Если дефолтный элемент уже лежит в противоположной паре - должен поменять
  // соответствующий ему элемент на запрашиваемый (смотри тесты)
  inline right_t const& at_left_or_default(left_t const& key) {
    return at_or_default<LO>(key)->template data<RO>();
  }
  inline left_t const& at_right_or_default(right_t const& key) {
    return at_or_default<RO>(key)->template data<LO>();
  }

  // lower и upper bound'ы по каждой стороне
  // Возвращают итераторы на соответствующие элементы
  // Смотри std::lower_bound, std::upper_bound.
  inline left_iterator lower_bound_left(const left_t& key) const {
    return lower_bound<LO>(key);
  }
  inline left_iterator upper_bound_left(const left_t& key) const {
    return upper_bound<LO>(key);
  }
  inline right_iterator lower_bound_right(const right_t& key) const {
    return lower_bound<RO>(key);
  }
  inline right_iterator upper_bound_right(const right_t& key) const {
    return upper_bound<RO>(key);
  }

  // Возвращает итератор на минимальный по порядку left.
  inline left_iterator begin_left() const {
    return {this, min(_roots.first, LO)};
  }
  // Возвращает итератор на минимальный по порядку right.
  inline right_iterator begin_right() const {
    return {this, min(_roots.second, RO)};
  }
  // Возвращает итератор на следующий за последним по порядку left.
  inline left_iterator end_left() const {
    return {this, nullptr};
  }
  // Возвращает итератор на следующий за последним по порядку right.
  inline right_iterator end_right() const {
    return {this, nullptr};
  }

  // Проверка на пустоту
  [[nodiscard]] inline bool empty() const {
    return size() == 0;
  }
  // Возвращает размер бимапы (кол-во пар)
  [[nodiscard]] inline size_t size() const {
    return _size;
  }

  // Операторы сравнения реализованы как внешние функции, но если очень надо
  // можно определить их тут.
  // Объявить здесь friend а реализацию сделать
  // снаружи не получилось. Так же не понятно почему в изначальном хедере
  // bimap представляет из себя struct(что само по себе странно)
  // friend bool operator==(bimap const& a, bimap const& b);
  // friend bool operator!=(bimap const& a, bimap const& b);
};

// Очень хочется разделить интерфейс и определение (размер шапок - мое почтение)
#include "bimap.hpp"
#include "iterator/iterator.h"
