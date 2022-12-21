#pragma once

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <typename Key, typename Value, bool order>
class bimap<Left, Right, CompareLeft, CompareRight>::iterator {
  using InversedIterator = iterator<Value, Key, !order>;

private:
  bimap const* bmap;
  node_t* curr;

public:
  friend class bimap;

  // Видно реализацию методов? А она есть и я ее вижу, а Clion не видит.
  // Сложно его винить за это.

  iterator(bimap const* bmap, node_t* curr);
  iterator& operator=(iterator const& other);

  // Элемент на который сейчас ссылается итератор.
  // Разыменование итератора end_left() неопределено.
  // Разыменование невалидного итератора неопределено.
  inline Key const& operator*() const {
    return curr->template data<order>();
  }
  inline Key const* operator->() const {
    return &curr->template data<order>();
  }

  // Переход к следующему по величине left'у.
  // Инкремент итератора end_left() неопределен.
  // Инкремент невалидного итератора неопределен.
  iterator& operator++();
  iterator operator++(int);

  // Переход к предыдущему по величине left'у.
  // Декремент итератора begin_left() неопределен.
  // Декремент невалидного итератора неопределен.
  iterator& operator--();
  iterator operator--(int);

  // iterator ссылается на элемент некоторой пары.
  // Эта функция возвращает итератор на элемент той же пары в другом порядке.
  // end_left().flip() возвращает end_right().
  // end_right().flip() возвращает end_left().
  // flip() невалидного итератора неопределен.
  inline InversedIterator flip() const {
    return {bmap, curr};
  }

  // Должно проверяться что итераторы итерируются по одному и тому же объекту,
  inline bool operator==(iterator const& other) const {
    return this->bmap->root(LO) == other.bmap->root(LO) &&
           this->curr == other.curr;
  }
  inline bool operator!=(iterator const& other) const {
    return !this->operator==(other);
  }
};

#include "iterator.hpp"
