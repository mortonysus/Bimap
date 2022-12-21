#pragma once

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <bool order>
std::pair<typename bimap<Left, Right, CompareLeft, CompareRight>::state,
          typename bimap<Left, Right, CompareLeft, CompareRight>::node_t*>
bimap<Left, Right, CompareLeft, CompareRight>::bin_search(
    const typename bimap<Left, Right, CompareLeft, CompareRight>::node_t::
        template order_type<order>::type& key) const {
  node_t* node = root(order);
  if (!node)
    return {st_none, node}; // nullptr
  while (true) {
    // key < node
    if (compare_type<order>::type::operator()(key,
                                              (node->template data<order>()))) {
      if (node->links(order).left == nullptr)
        return {st_left, node};
      else
        node = node->links(order).left;
    }
    // node < key
    else if (compare_type<order>::type::operator()(
                 (node->template data<order>()), key)) {
      if (node->links(order).right == nullptr)
        return {st_right, node};
      else
        node = node->links(order).right;
    }
    // key == node
    else
      return {st_equal, node};
  }
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <bool order>
typename bimap<Left, Right, CompareLeft, CompareRight>::node_t*
bimap<Left, Right, CompareLeft, CompareRight>::find(
    const typename node_t::template order_type<order>::type& key) const {
  std::pair<state, node_t*> result = bin_search<order>(key);
  return result.first == st_equal ? result.second : nullptr;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <bool order>
typename bimap<Left, Right, CompareLeft,
               CompareRight>::node_t::template order_type<!order>::type const&
bimap<Left, Right, CompareLeft, CompareRight>::at(
    const typename node_t::template order_type<order>::type& key) const {
  node_t* found = find<order>(key);
  if (!found)
    throw std::out_of_range("no such element");
  return found->template data<!order>();
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <bool order>
typename bimap<Left, Right, CompareLeft, CompareRight>::node_t*
bimap<Left, Right, CompareLeft, CompareRight>::at_or_default(
    const typename node_t::template order_type<order>::type& key) {
  // Ищем указатель на элемент с ключом по переданному порядку
  node_t* found = find<order>(key);
  if (found)
    return found;

  // Ищем указатель на дефолтный элемент по обратному порядку
  typename node_t::template order_type<!order>::type default_el =
      typename node_t::template order_type<!order>::type(
          0); // Чтобы два раза не вызывать конструктор. (а можно ли так?)

  found = find<!order>(default_el);
  if (found) {
    // Заменяем элемент пары прямого порядка на переданный
    found->template data<order>() = key;
    return found;
  }

  // Дефолтного элемента нет, ключа нет, вставляем пару в нужном порядке

  // это единственный костыль который я придумал
  // проблема в том что если писать через if то компилируются оба варианта
  // и они не компилируются по причине у вас тип не на месте
  // наверное есть способ получше, но я его пока не нашел(
  auto pr = std::make_pair(key, std::move(default_el));
  return insert_pr(std::get<order>(pr), std::get<!order>(pr));
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <bool order>
typename bimap<Left, Right, CompareLeft,
               CompareRight>::template iterator_type<order>::type
bimap<Left, Right, CompareLeft, CompareRight>::lower_bound(
    const typename node_t::template order_type<order>::type& key) const {
  std::pair<state, node_t*> bin_search_result = bin_search<order>(key);
  return {this, (bin_search_result.first == st_right)
                    ? nextNode(bin_search_result.second, order)
                    : bin_search_result.second};
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <bool order>
typename bimap<Left, Right, CompareLeft,
               CompareRight>::template iterator_type<order>::type
bimap<Left, Right, CompareLeft, CompareRight>::upper_bound(
    const typename node_t::template order_type<order>::type& key) const {
  std::pair<state, node_t*> bin_search_result = bin_search<order>(key);
  return {this, (bin_search_result.first == st_none ||
                 bin_search_result.first == st_left)
                    ? bin_search_result.second
                    : nextNode(bin_search_result.second, order)};
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
void bimap<Left, Right, CompareLeft, CompareRight>::erase(bimap::node_t* node) {
  deleteInOrder(node, LO);
  deleteInOrder(node, RO);
  delete node;
  --_size;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <bool order>
typename bimap<Left, Right, CompareLeft,
               CompareRight>::template iterator_type<order>::type
bimap<Left, Right, CompareLeft, CompareRight>::erase(
    typename bimap<Left, Right, CompareLeft,
                   CompareRight>::template iterator_type<order>::type it) {
  node_t* deleted_node = it.curr;
  ++it;
  erase(deleted_node);
  return it;
}
template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <bool order>
bool bimap<Left, Right, CompareLeft, CompareRight>::erase(
    const typename node_t::template order_type<order>::type& key) {
  node_t* node = find<order>(key);
  if (!node)
    return false;
  erase(node);
  return true;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <bool order>
typename bimap<Left, Right, CompareLeft,
               CompareRight>::template iterator_type<order>::type
bimap<Left, Right, CompareLeft, CompareRight>::erase(
    typename bimap<Left, Right, CompareLeft,
                   CompareRight>::template iterator_type<order>::type first,
    typename bimap<Left, Right, CompareLeft,
                   CompareRight>::template iterator_type<order>::type last) {
  while (last != (first = erase<order>(first)))
    ;
  return last;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <bool order>
std::pair<typename bimap<Left, Right, CompareLeft, CompareRight>::node_t**,
          typename bimap<Left, Right, CompareLeft, CompareRight>::node_t*>
bimap<Left, Right, CompareLeft, CompareRight>::insert_ad(
    const typename node_t::template order_type<order>::type& key) const {
  std::pair<state, node_t*> result = bin_search<order>(key);
  if (result.first == st_equal)
    return {nullptr, nullptr};
  else if (result.first == st_left)
    return {&((result.second)->links(order).left), result.second};
  else
    return {&(result.second->links(order).right), result.second};
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
template <typename left_t_pr, typename right_t_pr>
typename bimap<Left, Right, CompareLeft, CompareRight>::node_t*
bimap<Left, Right, CompareLeft, CompareRight>::insert_pr(left_t_pr&& left,
                                                         right_t_pr&& right) {
  if (_roots.first == nullptr) {
    _roots.first = _roots.second = new node_t(std::forward<left_t_pr>(left),
                                              std::forward<right_t_pr>(right));
    ++_size;
    return _roots.first;
  }

  auto PossibleAddress1 = insert_ad<LO>(std::forward<left_t_pr>(left));
  if (!PossibleAddress1.first)
    return nullptr;
  // Чтобы не делать лишний проход в случае фейла
  auto PossibleAddress2 = insert_ad<RO>(std::forward<right_t_pr>(right));
  if (!PossibleAddress2.first)
    return nullptr;

  *(PossibleAddress1.first) = *(PossibleAddress2.first) =
      new node_t(std::forward<left_t_pr>(left), std::forward<right_t_pr>(right),
                 {PossibleAddress1.second, PossibleAddress2.second});
  ++_size;
  return *PossibleAddress1.first;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
typename bimap<Left, Right, CompareLeft, CompareRight>::node_t*
bimap<Left, Right, CompareLeft, CompareRight>::min(bimap::node_t* node,
                                                   bool order) {
  while (node->links(order).left)
    node = node->links(order).left;
  return node;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
typename bimap<Left, Right, CompareLeft, CompareRight>::node_t*
bimap<Left, Right, CompareLeft, CompareRight>::max(bimap::node_t* node,
                                                   bool order) {
  while (node->links(order).right)
    node = node->links(order).right;
  return node;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
typename bimap<Left, Right, CompareLeft, CompareRight>::node_t*
bimap<Left, Right, CompareLeft, CompareRight>::prevNode(bimap::node_t* node,
                                                        bool order) {
  if (!node)
    return nullptr;
  if (node->links(order).left != nullptr)
    return max(node->links(order).left, order);
  node_t* curr = node->links(order).parent;
  while (curr != nullptr && node == curr->links(order).left) {
    node = curr;
    curr = curr->links(order).parent;
  }
  return curr;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
typename bimap<Left, Right, CompareLeft, CompareRight>::node_t*
bimap<Left, Right, CompareLeft, CompareRight>::nextNode(bimap::node_t* node,
                                                        bool order) {
  if (!node)
    return nullptr;
  if (node->links(order).right != nullptr)
    return min(node->links(order).right, order);

  // next будет первый родитель для которого мы будем слева.
  // Если мы дошли до корня, то у него нет правого потомка, т. к. тогда был бы
  //    возврат из условия выше => next это корень.
  // В случае если пытаемся найти next у максимума - вернется nullptr.
  node_t* curr = node->links(order).parent;
  while (curr != nullptr && node == curr->links(order).right) {
    node = curr;
    curr = curr->links(order).parent;
  }
  return curr;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
void bimap<Left, Right, CompareLeft, CompareRight>::deleteWithBothChildren(
    bimap::node_t* node, bool order) {
  node_t* next =
      nextNode(node, order); // Нашли следующий элемент, у него нет левого
  // потомка (иначе next был бы левым потомком)

  if (next->links(order).parent->links(order).left ==
      next) { // next - левый потомок родителя
    next->links(order).parent->links(order).left =
        next->links(order)
            .right; // Подцепили правого потомка next к родителю next слева
    if (next->links(order).right) // Не забыли про правого потомка next !!!
      next->links(order).right->links(order).parent = next->links(order).parent;
  } else // next - правый потомок родителя
    next->links(order).parent->links(order).right =
        next->links(order)
            .right; // Подцепили правого потомка next к родителю next справа

  next->links(order) =
      node->links(order); // Скопировали все ссылки node в next (в случае
  // корня parent будет nullptr)

  if (next->links(order).left) // У node был потомок слева
    next->links(order).left->links(order).parent =
        next; // У потомка слева родителем будет next
  if (next->links(order).right) // У node был потомок справа
    next->links(order).right->links(order).parent =
        next; // У потомка справа родителем будет next

  if (node->links(order).parent) // node не корень
    if (node->links(order).parent->links(order).left ==
        node) // node - левый потомок родителя
      node->links(order).parent->links(order).left =
          next; // Подцепили next к родителю node слева
    else        // node - правый потомок родителя
      node->links(order).parent->links(order).right =
          next; // Подцепили next к родителю node справа
  else          // node - корень
    root(order) = next; // Корень теперь next
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
void bimap<Left, Right, CompareLeft, CompareRight>::deleteWithLeftChild(
    bimap::node_t* node, bool order) {
  if (node->links(order).parent) { // node не корень
    if (node->links(order).parent->links(order).left ==
        node) // node - левый потомок родителя
      node->links(order).parent->links(order).left =
          node->links(order)
              .left; // Подцепили левого потомка node к родителю слева
    else             // node - правый потомок родителя
      node->links(order).parent->links(order).right =
          node->links(order)
              .left; // Подцепили левого потомка node к родителю справа
    node->links(order).left->links(order).parent =
        node->links(order)
            .parent; // У правого потомка node родителем стал родитель node
  } else {           // node - корень
    root(order) = node->links(order).left; // Левый потомок node - новый корень
    root(order)->links(order).parent = nullptr;
    // Корень не может указывать на удаленную node
  }
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
void bimap<Left, Right, CompareLeft, CompareRight>::deleteWithRightChild(
    bimap::node_t* node, bool order) {
  if (node->links(order).parent) { // node не корень
    if (node->links(order).parent->links(order).left ==
        node) // node - левый потомок родителя
      node->links(order).parent->links(order).left =
          node->links(order)
              .right; // Подцепили правого потомка node к родителю слева
    else              // node - правый потомок родителя
      node->links(order).parent->links(order).right =
          node->links(order)
              .right; // Подцепили правого потомка node к родителю справа
    node->links(order).right->links(order).parent =
        node->links(order)
            .parent; // У правого потомка node родителем стал родитель node
  } else {           // node - корень
    root(order) =
        node->links(order).right; // Правый потомок node - новый корень
    root(order)->links(order).parent =
        nullptr; // Корень не может указывать на удаленную node
  }
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
void bimap<Left, Right, CompareLeft, CompareRight>::deleteWithoutChildren(
    bimap::node_t* node, bool order) {
  if (node->links(order).parent) // node не корень
    if (node->links(order).parent->links(order).left ==
        node) // node - левый потомок родителя
      node->links(order).parent->links(order).left =
          nullptr; // Удалили ссылку на левого потомка у родителя
    else           // node - правый потомок родителя
      node->links(order).parent->links(order).right =
          nullptr; // Удалили ссылку на правого потомка у родителя
  else             // node - корень
    root(order) = nullptr; // Удалили корень
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
void bimap<Left, Right, CompareLeft, CompareRight>::deleteInOrder(
    bimap::node_t* node, bool order) {
  if (node->links(order).left && node->links(order).right)
    deleteWithBothChildren(node, order); // У node есть левый потомок и правый
  // потомок (next точно есть)
  else if (node->links(order).left)
    deleteWithLeftChild(node,
                        order); // У node нет правого потомка, но есть левый
  else if (node->links(order).right)
    deleteWithRightChild(node,
                         order); // У node нет левого потомка, но есть правый
  else
    deleteWithoutChildren(node, order); // У node нет потомков
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
void bimap<Left, Right, CompareLeft, CompareRight>::delete_tree(
    bimap::node_t* node) {
  if (node == nullptr)
    return;
  delete_tree(node->links(LO).left);
  delete_tree(node->links(LO).right);
  delete node;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
bimap<Left, Right, CompareLeft, CompareRight>::bimap(CompareLeft compare_left,
                                                     CompareRight compare_right)
    : CompareWrap<CompareLeft, LO>(std::move(compare_left)),
      CompareWrap<CompareRight, RO>(std::move(compare_right)),
      _roots({nullptr, nullptr}) {}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
bimap<Left, Right, CompareLeft, CompareRight>::bimap(const bimap& other)
    : CompareWrap<CompareLeft, LO>(CompareLeft()),
      CompareWrap<CompareRight, RO>(CompareRight()),
      _roots({nullptr, nullptr}) {
  try {
    // Тут наверное хочется не использовать итераторы, а вручную копировать
    // деревья, потому что insert делает миллиард лишних проверок,
    // которых можно избежать при учете того что дан корректный инвариант.
    // Но с другой стороны не очень хочется.
    for (auto it = other.begin_left(); it != other.end_left(); ++it)
      insert(*it, *it.flip());
  } catch (std::runtime_error const& error) {
    // Если произошла ошибка то нужно удалить то что вставилось.
    this->~bimap();
    throw error;
  }
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
bimap<Left, Right, CompareLeft, CompareRight>::bimap(bimap&& other) noexcept
    : CompareWrap<CompareLeft, LO>(CompareLeft()),
      CompareWrap<CompareRight, RO>(CompareRight()),
      _size(std::move(other)._size), _roots(std::move(other)._roots) {
  std::move(other)._roots = {
      nullptr, nullptr}; // чтобы корректно отработал деструктор у other
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
bimap<Left, Right, CompareLeft, CompareRight>&
bimap<Left, Right, CompareLeft, CompareRight>::operator=(const bimap& other) {
  // Не совсем понятно что делать если произошла ошибка при копировании.
  // Исходя из тестов можно предположить: либо та бимапа куда мы копируем
  // должна стать пустой, либо должна остаться в состоянии до копирования.
  // Если она должна стать пустой, то достаточно удалить то что уже вставлено
  // и занулить.
  // Если она должна остаться неизменной, то это сопряжено с доп расходами.
  if (this != &other) {
    this->~bimap();
    _size = 0;
    _roots.second = _roots.first = nullptr;
    try {
      for (auto it = other.begin_left(); it != other.end_left(); ++it)
        insert(*it, *it.flip());
    } catch (std::exception const& error) {
      this->~bimap();
      _size = 0;
      _roots.second = _roots.first = nullptr;
    }
  }
  return *this;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
bimap<Left, Right, CompareLeft, CompareRight>&
bimap<Left, Right, CompareLeft, CompareRight>::operator=(
    bimap&& other) noexcept {
  if (this != &other) { // тут я обращаюсь с other как c lvalue?
    this->~bimap();
    _size = std::move(other)._size;
    _roots = std::move(other)._roots;
    std::move(other)._roots = {
        nullptr, nullptr}; // чтобы корректно отработал деструктор у other
  }
  return *this;
}

template <typename Left, typename Right, typename CompareLeft,
          typename CompareRight>
void bimap<Left, Right, CompareLeft, CompareRight>::swap(bimap& other) {
  std::swap(_size, other._size);
  std::swap(_roots, other._roots);
}

// Операторы сравнения
// Возможно тут как в конструкторе копирования хочется вместо итераторов
// сравнивать деревья?
template <typename First, typename Second, typename CompareFirst,
          typename CompareSecond>
bool operator==(bimap<First, Second, CompareFirst, CompareSecond> const& a,
                bimap<First, Second, CompareFirst, CompareSecond> const& b) {
  if (a.size() != b.size())
    return false;

  auto ita = a.begin_left();
  auto itb = b.begin_left();
  for (; ita != a.end_left(); ++ita, ++itb)
    if (*ita != *itb || *ita.flip() != *itb.flip())
      return false;
  return true;
}

template <typename First, typename Second, typename CompareFirst,
          typename CompareSecond>
bool operator!=(bimap<First, Second, CompareFirst, CompareSecond> const& a,
                bimap<First, Second, CompareFirst, CompareSecond> const& b) {
  return !(a == b);
}
