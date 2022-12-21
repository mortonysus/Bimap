#pragma once

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <typename Key, typename Value, bool order>
bimap<Left, Right, CompareLeft, CompareRight>::iterator<Key, Value, order>::
    iterator(
        const bimap<Left, Right, CompareLeft, CompareRight>* bmap,
        typename bimap<Left, Right, CompareLeft, CompareRight>::node_t* curr)
    : bmap(bmap), curr(curr) {}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <typename Key, typename Value, bool order>
typename bimap<Left, Right, CompareLeft,
               CompareRight>::template iterator<Key, Value, order>&
bimap<Left, Right, CompareLeft, CompareRight>::iterator<Key, Value, order>::
operator=(const bimap<Left, Right, CompareLeft,
                      CompareRight>::iterator<Key, Value, order>& other) {
  if (this != &other) {
    this->bmap = other.bmap;
    this->curr = other.curr;
  }
  return *this;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <typename Key, typename Value, bool order>
typename bimap<Left, Right, CompareLeft,
               CompareRight>::template iterator<Key, Value, order>&
bimap<Left, Right, CompareLeft, CompareRight>::iterator<Key, Value,
                                                        order>::operator++() {
  curr = nextNode(curr, order);
  return *this;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <typename Key, typename Value, bool order>
typename bimap<Left, Right, CompareLeft,
               CompareRight>::template iterator<Key, Value, order>
bimap<Left, Right, CompareLeft,
      CompareRight>::iterator<Key, Value, order>::operator++(int) {
  auto ret = iterator(*this);
  ++*this;
  return ret;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <typename Key, typename Value, bool order>
typename bimap<Left, Right, CompareLeft,
               CompareRight>::template iterator<Key, Value, order>&
bimap<Left, Right, CompareLeft, CompareRight>::iterator<Key, Value,
                                                        order>::operator--() {
  curr = (curr ? prevNode(curr, order) : max(bmap->root(order), order));
  return *this;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <typename Key, typename Value, bool order>
typename bimap<Left, Right, CompareLeft,
               CompareRight>::template iterator<Key, Value, order>
bimap<Left, Right, CompareLeft,
      CompareRight>::iterator<Key, Value, order>::operator--(int) {
  auto ret = iterator(*this);
  --(*this);
  return ret;
}
