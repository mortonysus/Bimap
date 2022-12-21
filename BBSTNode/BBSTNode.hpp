#pragma once

template <typename Left, typename Right>
template <typename left_pr, typename right_pr>
BBSTNode<Left, Right>::BBSTNode(left_pr&& left, right_pr&& right,
                                const std::pair<ThisType*, ThisType*>& parents)
    : _data({std::forward<left_pr>(left), std::forward<right_pr>(right)}),
      _links({NodeLinks(parents.first), NodeLinks(parents.second)}) {}
