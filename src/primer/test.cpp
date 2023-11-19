#include <memory>
#include <optional>
#include <shared_mutex>
#include <utility>
#include "common/exception.h"
#include "primer/trie.h"
#include "primer/trie_store.h"

namespace bustub {

template <class T>
auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<T>> {
  root_lock_.lock();
  size_t size = key.size();
  auto target = this->root_.GetRoot();
  root_lock_.unlock();

  for (size_t i = 0; i < size; i++) {
    char k = key[i];
    if (target && target->children_.find(k) != target->children_.end()) {
      target = target->children_.at(k);
    } else {
      return std::nullopt;
    }
  }

  auto res = dynamic_cast<const TrieNodeWithValue<T> *>(target.get());

  if (res && res->is_value_node_) {
    ValueGuard<T> value(root_, *res->value_.get());
    return std::make_optional(std::move(value));
  }
  return std::nullopt;

  // Pseudo-code:
  // (1) Take the root lock, get the root, and release the root lock. Don't lookup the value in the
  //     trie while holding the root lock.
  // (2) Lookup the value in the trie.
  // (3) If the value is found, return a ValueGuard object that holds a reference to the value and the
  //     root. Otherwise, return std::nullopt.
}

template <class T>
void TrieStore::Put(std::string_view key, T value) {
  write_lock_.lock();
  size_t size = key.size();
  std::shared_ptr<const TrieNode> target = std::make_shared<const TrieNode>(this->root_.GetRoot()->children_);
  auto value_ptr = std::make_shared<T>(std::move(value));
  if (size == 0) {
    return;
  }
  auto stack = std::vector<std::shared_ptr<const TrieNode>>(size + 1, nullptr);
  stack[0] = target;
  write_lock_.unlock();

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
    write_lock_.lock();
    if (i == size - 1) {
      if (target) {
        stack[i + 1] = std::make_shared<const TrieNodeWithValue<T>>(target->children_, value_ptr);
      } else {
        stack[i + 1] = std::make_shared<const TrieNodeWithValue<T>>(value_ptr);
      }
    }
    write_lock_.unlock();
  }

  // build node down to top
  for (int i = static_cast<int>(size) - 1; i >= 0; i--) {
    char k = key[i];
    auto target = stack[i];
    write_lock_.lock();
    if (target) {
      stack[i] = stack[i]->Clone();
      const_cast<TrieNode *>(stack[i].get())->children_[k] = stack[i + 1];
    } else {
      std::map<char, std::shared_ptr<const TrieNode>> mp{{k, stack[i + 1]}};
      stack[i] = std::make_shared<const TrieNode>(mp);
    }
    write_lock_.unlock();
  }

  // You will need to ensure there is only one writer at a time. Think of how you can achieve this.
  // The logic should be somehow similar to `TrieStore::Get`.
}

void TrieStore::Remove(std::string_view key) {
  write_lock_.lock();
  size_t size = key.size();
  std::shared_ptr<const TrieNode> target = std::make_shared<const TrieNode>(this->root_.GetRoot()->children_);
  write_lock_.unlock();
  auto stack = std::vector<std::shared_ptr<const TrieNode>>(size + 1, nullptr);
  stack[0] = target;

  // record the remove path
  for (size_t i = 0; i < size; i++) {
    char k = key[i];
    if (target && target->children_.find(k) != target->children_.end()) {
      target = target->children_.at(k);
      stack[i + 1] = target;
    } else {
      return;
    }
  }

  if (!stack.back()->is_value_node_) {
    return;
  }

  // handle last node on delete path
  write_lock_.lock();
  stack[stack.size() - 1] = std::make_shared<const TrieNode>(stack.back()->children_);
  write_lock_.unlock();
  // handle prefix node's children map
  // handle  from last second node
  for (int i = static_cast<int>(size) - 1; i >= 0; i--) {
    char k = key[i];
    write_lock_.lock();
    stack[i] = stack[i]->Clone();

    if (stack[i + 1] && stack[i + 1]->children_.empty() && !stack[i + 1]->is_value_node_) {
      const_cast<TrieNode *>(stack[i].get())->children_.erase(k);
    } else {
      const_cast<TrieNode *>(stack[i].get())->children_[k] = stack[i + 1];
    }
  }

  if (stack[0] && stack[0]->children_.empty() && !stack[0]->is_value_node_) {
    return;
  }

  // You will need to ensure there is only one writer at a time. Think of how you can achieve this.
  // The logic should be somehow similar to `TrieStore::Get`.
}

// Below are explicit instantiation of template functions.

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<uint32_t>>;
template void TrieStore::Put(std::string_view key, uint32_t value);

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<std::string>>;
template void TrieStore::Put(std::string_view key, std::string value);

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<Integer>>;
template void TrieStore::Put(std::string_view key, Integer value);

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<MoveBlocked>>;
template void TrieStore::Put(std::string_view key, MoveBlocked value);

}  // namespace bustub
