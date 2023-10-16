#include "primer/trie.h"
#include <memory>
#include <string_view>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  throw NotImplementedException("Trie::Get is not implemented.");
  
  // std::shared_ptr<const TrieNode> ptr_node = root_;

  // for (auto c : key) {
  //   auto it = ptr_node->children_.find(c);
  //   if (it == ptr_node->children_.end()) {
  //     return nullptr;
  //   }
  //   ptr_node = it->second;
  // }

  // auto value_node = dynamic_cast<const TrieNodeWithValue<T> *>(ptr_node.get());
  // if (value_node == nullptr) {
  //   return nullptr;
  // }
  // return value_node->value_.get();

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  std::shared_ptr<const TrieNode> cur_node = root_;
  std::size_t key_size = key.size();
  decltype(key_size) idx = 0;
  while (idx < key_size && cur_node) {//遍历到key的最后一个字符或者遍历到cur_node为空
    char ch = key[idx++];
    cur_node = (cur_node->children_.find(ch) != cur_node->children_.end()) ? cur_node->children_.at(ch) : nullptr;
  }
  if (idx != key_size || !cur_node || !cur_node->is_value_node_) {
    return nullptr;
  }
  const auto *leaf = dynamic_cast<const TrieNodeWithValue<T> *>(cur_node.get());
  return leaf ? leaf->value_.get() : nullptr;
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  throw NotImplementedException("Trie::Put is not implemented.");

  // std::shared_ptr<TrieNode> new_root;
  // auto clone_node = this->root_->Clone();
  // new_root = std::shared_ptr<TrieNode>(std::move(clone_node));
  // auto node_ptr = new_root;

  // for (auto c : key) {
  //   // it return type : std::shared_ptr<const TrieNode>
  //   auto it = node_ptr->children_.find(c);
  //   if (it == new_root->children_.end()) {
  //     // c not exists, create a new node with no children
  //     std::shared_ptr<TrieNode> new_node = std::make_shared<TrieNode>();
  //     node_ptr->children_[c] = new_node;
  //     // move to next node
  //     node_ptr = new_node;
  //   } else {
  //     // c exists, clone "children" node 
  //     auto node = it->second->Clone();
  //     auto cloned_node = std::shared_ptr<TrieNode>(std::move(node));
  //     // move to next node
  //     node_ptr = cloned_node;
  //   }
  // }   // end of loop

  // // handle last node
  // auto value_node = std::dynamic_pointer_cast<TrieNodeWithValue<T>>(node_ptr);
  // if (value_node == nullptr) {
  //   // not a value node, create a new value node
  //   auto new_node = std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value)));
  //   value_node = new_node;
  //   node_ptr->is_value_node_ = false;
  //   node_ptr = value_node;
  // } else {
  //   // is a value node, update value
  //   value_node->value_ = std::make_shared<T>(std::move(value));
  // }
  // node_ptr->is_value_node_ = true;

  // return Trie(new_root);
  
  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
  std::shared_ptr<T> shared_value = std::make_shared<T>(std::move(value));
  std::vector<std::shared_ptr<const TrieNode>> node_stack;  // store the same node
  std::shared_ptr<const TrieNode> cur_node = root_;
  std::size_t key_size = key.size();
  decltype(key_size) idx = 0;
  // 1.store the same node
  while (idx < key_size && cur_node) {
    char ch = key[idx++];
    node_stack.push_back(cur_node);
    cur_node = cur_node->children_.find(ch) != cur_node->children_.end() ? cur_node->children_.at(ch) : nullptr;
  }
  // 2.create diff node
  // 2.1create leaf node
  std::shared_ptr<const TrieNodeWithValue<T>> leaf_node =
      cur_node ? std::make_shared<const TrieNodeWithValue<T>>(cur_node->children_, shared_value)
               : std::make_shared<const TrieNodeWithValue<T>>(shared_value);
  // 2.2create diff inner node
  std::shared_ptr<const TrieNode> child_node = leaf_node;
  while (idx < key_size) {
    char ch = key[--key_size];
    std::map<char, std::shared_ptr<const TrieNode>> children{{ch, child_node}};
    cur_node = std::make_shared<const TrieNode>(children);
    child_node = cur_node;
  }
  // 3.copy same node
  cur_node = child_node;
  for (size_t i = node_stack.size() - 1; i < node_stack.size(); --i) {
    cur_node = std::shared_ptr<const TrieNode>(node_stack[i]->Clone());
    const_cast<TrieNode *>(cur_node.get())->children_[key[i]] = child_node;
    child_node = cur_node;
  }
  return Trie(cur_node);
}


auto Trie::Remove(std::string_view key) const -> Trie {
  throw NotImplementedException("Trie::Remove is not implemented.");

  std::vector<std::shared_ptr<const TrieNode>> node_stack;  // store the same node
  std::shared_ptr<const TrieNode> cur_node = root_;
  std::size_t key_size = key.size();
  decltype(key_size) idx = 0;
  // 1.store the same node
  while (idx < key_size && cur_node) {
    char ch = key[idx++];
    node_stack.push_back(cur_node);
    cur_node = cur_node->children_.find(ch) != cur_node->children_.end() ? cur_node->children_.at(ch) : nullptr;
  }
  if (idx != key_size || !cur_node || !cur_node->is_value_node_) {
    return *this;
  }
  // 2.create end node
  std::shared_ptr<const TrieNode> end_node =
      cur_node->children_.empty() ? nullptr : std::make_shared<const TrieNode>(cur_node->children_);
  // 3.copy same node
  std::shared_ptr<const TrieNode> child_node = end_node;
  cur_node = end_node;
  for (size_t i = node_stack.size() - 1; i < node_stack.size(); --i) {
    cur_node = std::shared_ptr<const TrieNode>(node_stack[i]->Clone());
    const_cast<TrieNode *>(cur_node.get())->children_[key[i]] = child_node;
    child_node = cur_node;
  }
  return Trie(cur_node);

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