#pragma once

template <typename Node>
struct BSTNodeLinks {
  Node *parent, *left, *right;
  explicit BSTNodeLinks(Node* parent, Node* left = nullptr,
                        Node* right = nullptr)
      : parent(parent), left(left), right(right) {}
};
