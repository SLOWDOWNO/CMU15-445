#include "primer/trie.h"
#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  // throw NotImplementedException("Trie::Get is not implemented.");

  size_t size = key.size();
  auto target = this->root_;
  for (size_t i = 0; i < size; i++) {
    char k = key[i];
    if (target && target->children_.find(k) != target->children_.end()) {
      target = target->children_.at(k);
    } else {
      return nullptr;
    }
  }

  auto res = dynamic_cast<const TrieNodeWithValue<T> *>(target.get());

  if (res && res->is_value_node_) {
    return res->value_.get();
  }
  return nullptr;

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  // throw NotImplementedException("Trie::Put is not implemented.");

  size_t size = key.size();
  auto target = this->root_;
  auto value_ptr = std::make_shared<T>(std::move(value));

  if (size == 0) {
    return Trie(std::make_shared<const TrieNodeWithValue<T>>(target->children_, value_ptr));
  }

  auto stack = std::vector<std::shared_ptr<const TrieNode>>(size + 1, nullptr);
  stack[0] = this->root_;

  // record the put path
  for (size_t i = 0; i < size; i++) {
    char k = key[i];
    if (target && target->children_.find(k) != target->children_.end()) {
      target = target->children_.at(k);
      stack[i + 1] = target;
    } else {
      target = nullptr;
      stack[i + 1] = target;
    }
    // handle value node
    if (i == size - 1) {
      if (target) {
        stack[i + 1] = std::make_shared<const TrieNodeWithValue<T>>(target->children_, value_ptr);
      } else {
        stack[i + 1] = std::make_shared<const TrieNodeWithValue<T>>(value_ptr);
      }
    }
  }

  // build node down to top
  for (int i = static_cast<int>(size) - 1; i >= 0; i--) {
    char k = key[i];
    auto target = stack[i];
    if (target) {
      stack[i] = stack[i]->Clone();
      const_cast<TrieNode *>(stack[i].get())->children_[k] = stack[i + 1];
    } else {
      std::map<char, std::shared_ptr<const TrieNode>> mp{{k, stack[i + 1]}};
      stack[i] = std::make_shared<const TrieNode>(mp);
    }
  }

  return Trie(stack[0]);

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
}

auto Trie::Remove(std::string_view key) const -> Trie {
  // throw NotImplementedException("Trie::Remove is not implemented.");

  size_t size = key.size();
  auto target = this->root_;
  auto stack = std::vector<std::shared_ptr<const TrieNode>>(size + 1, nullptr);
  stack[0] = this->root_;

  // record the remove path
  for (size_t i = 0; i < size; i++) {
    char k = key[i];
    if (target && target->children_.find(k) != target->children_.end()) {
      target = target->children_.at(k);
      stack[i + 1] = target;
    } else {
      return Trie(stack[0]);
    }
  }

  if (!stack.back()->is_value_node_) {
    return Trie(stack[0]);
  }

  // handle last node on delete path
  stack[stack.size() - 1] = std::make_shared<const TrieNode>(stack.back()->children_);

  // handle prefix node's children map
  // handle  from last second node
  for (int i = static_cast<int>(size) - 1; i >= 0; i--) {
    char k = key[i];
    stack[i] = stack[i]->Clone();

    if (stack[i + 1] && stack[i + 1]->children_.empty() && !stack[i + 1]->is_value_node_) {
      const_cast<TrieNode *>(stack[i].get())->children_.erase(k);
    } else {
      const_cast<TrieNode *>(stack[i].get())->children_[k] = stack[i + 1];
    }
  }

  if (stack[0] && stack[0]->children_.empty() && !stack[0]->is_value_node_) {
    return Trie(nullptr);
  }

  return Trie(stack[0]);

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub