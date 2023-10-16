#include "primer/trie.h"
#include <memory>
#include <string_view>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  throw NotImplementedException("Trie::Get is not implemented.");

  std::shared_ptr<const TrieNode> ptr_node = root_;

  for (auto c : key) {
    auto it = ptr_node->children_.find(c);
    if (it == ptr_node->children_.end()) {
      return nullptr;
    }
    ptr_node = it->second;
  }

  auto value_node = dynamic_cast<const TrieNodeWithValue<T> *>(ptr_node.get());
  if (value_node == nullptr) {
    return nullptr;
  }
  return value_node->value_.get();
  
  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  throw NotImplementedException("Trie::Put is not implemented.");

  auto new_root = std::shared_ptr<TrieNode> (std::move(this->root_->Clone()));
  std::shared_ptr<TrieNode> ptr_node = new_root;        // ptr_node用于遍历Trie
  
  for (auto c : key) {
    auto it = ptr_node->children_.find(c);
    // key is not exists.
    if (it == ptr_node->children_.end()) {
      std::shared_ptr<TrieNode> new_node = std::make_shared<TrieNode>();
      ptr_node->children_[c] = new_node;
      ptr_node = new_node;
    } else {
      // key存在的情况下，移动到下一个结点，复制结点并且前一个结点指向它
      std::shared_ptr<TrieNode> next_node = std::const_pointer_cast<TrieNode>(it->second);
      ptr_node = std::shared_ptr<TrieNode>(std::move(next_node->Clone()));
    }
  }
  
  auto value_node = std::dynamic_pointer_cast<TrieNodeWithValue<T>>(ptr_node);
  // 转换失败说明不是一个value node,我们要create一个node加入到node的子节点
  if (value_node == nullptr) {
    auto node = std::make_shared<TrieNodeWithValue<T>>(std::move(value));
    value_node = node;
    ptr_node->is_value_node_ = false;
    ptr_node = value_node;
  } else {
    value_node->value_ = std::make_shared<T>(std::move(value));
  }
  ptr_node->is_value_node_ = true;

  return Trie(new_root);
  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
}

auto Trie::Remove(std::string_view key) const -> Trie {
  throw NotImplementedException("Trie::Remove is not implemented.");

  auto new_root = std::shared_ptr<TrieNode>(std::move(this->root_->Clone()));
  std::shared_ptr<TrieNode> ptr_node = new_root;
  std::shared_ptr<TrieNode> last_value_node;
  char last_char;

  for (auto c : key) {
    auto it = ptr_node->children_.find(c);
    if (it == ptr_node->children_.end()) {
      return Trie(new_root);
    }
    // 是value node的话，直接删除
    if (ptr_node->is_value_node_) {
      last_value_node = ptr_node;
      last_char = c;
    }
    ptr_node = std::const_pointer_cast<TrieNode>(it->second);
  }
  // 叶子结点没有值
  if (!ptr_node->is_value_node_) {
    return Trie(new_root);
  }
  // 处理value node的情况：
  ptr_node->is_value_node_ = false;
  if (ptr_node->children_.empty()) {
    if (last_value_node != nullptr) {
      last_value_node->children_.erase(last_char);
    }
    while (last_value_node != nullptr && last_value_node->is_value_node_ && last_value_node->children_.size() == 1) {
      auto it = last_value_node->children_.begin();
      last_char = it->first;
      last_value_node = std::const_pointer_cast<TrieNode>(it->second);
    }
    if (last_value_node == nullptr) {
      return Trie(std::make_shared<TrieNode>());
    }
    last_value_node->children_.erase(last_char);
  }

  return Trie(new_root);

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
