/**
* implement a container like std::map
*/
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
   class Key,
   class T,
   class Compare = std::less <Key>
   > class map {
  public:
   /**
 * the internal type of data.
 * it should have a default constructor, a copy constructor.
 * You can use sjtu::map as value_type by typedef.
   */
   typedef pair<const Key, T> value_type;

   struct Node {
      value_type data;
      Node *parent;
      Node *left;
      Node *right;
      int height;

      Node() : parent(nullptr), left(nullptr), right(nullptr), height(1) {}
      Node(const value_type &d, Node *p) : data(d), parent(p), left(nullptr), right(nullptr), height(1) {}
   };

   class const_iterator;

   class iterator {
      private:
       Node *node;
       friend class map;
       friend class const_iterator;
      public:
       iterator() : node(nullptr) {}
       explicit iterator(Node *n) : node(n) {}

       iterator(const iterator &other) : node(other.node) {}

       iterator operator++(int) {
           iterator tmp(*this);
           ++(*this);
           return tmp;
       }

       iterator &operator++() {
           if (node == nullptr) {
               throw invalid_iterator();
           }
           if (node->right != nullptr) {
               node = node->right;
               while (node->left != nullptr) {
                   node = node->left;
               }
           } else {
               Node *p = node->parent;
               while (p != nullptr && node == p->right) {
                   node = p;
                   p = p->parent;
               }
               node = p;
           }
           return *this;
       }

       iterator operator--(int) {
           iterator tmp(*this);
           --(*this);
           return tmp;
       }

       iterator &operator--() {
           if (node == nullptr) {
               throw invalid_iterator();
           }
           if (node->left != nullptr) {
               node = node->left;
               while (node->right != nullptr) {
                   node = node->right;
               }
           } else {
               Node *p = node->parent;
               while (p != nullptr && node == p->left) {
                   node = p;
                   p = p->parent;
               }
               node = p;
           }
           return *this;
       }

       value_type &operator*() const {
           if (node == nullptr) {
               throw invalid_iterator();
           }
           return node->data;
       }

       bool operator==(const iterator &rhs) const {
           return node == rhs.node;
       }

       bool operator==(const const_iterator &rhs) const {
           return node == rhs.node;
       }

       bool operator!=(const iterator &rhs) const {
           return node != rhs.node;
       }

       bool operator!=(const const_iterator &rhs) const {
           return node != rhs.node;
       }

       value_type *operator->() const noexcept {
           return &node->data;
       }
   };

   class const_iterator {
       private:
        Node *node;
        friend class map;
       public:
        const_iterator() : node(nullptr) {}
        explicit const_iterator(Node *n) : node(n) {}

        const_iterator(const const_iterator &other) : node(other.node) {}

        const_iterator(const iterator &other) : node(other.node) {}

        const_iterator operator++(int) {
            const_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        const_iterator &operator++() {
            if (node == nullptr) {
                throw invalid_iterator();
            }
            if (node->right != nullptr) {
                node = node->right;
                while (node->left != nullptr) {
                    node = node->left;
                }
            } else {
                Node *p = node->parent;
                while (p != nullptr && node == p->right) {
                    node = p;
                    p = p->parent;
                }
                node = p;
            }
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp(*this);
            --(*this);
            return tmp;
        }

        const_iterator &operator--() {
            if (node == nullptr) {
                throw invalid_iterator();
            }
            if (node->left != nullptr) {
                node = node->left;
                while (node->right != nullptr) {
                    node = node->right;
                }
            } else {
                Node *p = node->parent;
                while (p != nullptr && node == p->left) {
                    node = p;
                    p = p->parent;
                }
                node = p;
            }
            return *this;
        }

        const value_type &operator*() const {
            if (node == nullptr) {
                throw invalid_iterator();
            }
            return node->data;
        }

        bool operator==(const const_iterator &rhs) const {
            return node == rhs.node;
        }

        bool operator==(const iterator &rhs) const {
            return node == rhs.node;
        }

        bool operator!=(const const_iterator &rhs) const {
            return node != rhs.node;
        }

        bool operator!=(const iterator &rhs) const {
            return node != rhs.node;
        }

        const value_type *operator->() const noexcept {
            return &node->data;
        }
   };

   /**
 * TODO two constructors
   */
   map() : root_(nullptr), sz_(0) {}

   map(const map &other) : root_(nullptr), sz_(0) {
       if (other.root_ != nullptr) {
           root_ = copySubtree(other.root_, nullptr);
           sz_ = other.sz_;
       }
   }

   /**
 * TODO assignment operator
   */
   map &operator=(const map &other) {
       if (this == &other) return *this;
       clear();
       if (other.root_ != nullptr) {
           root_ = copySubtree(other.root_, nullptr);
           sz_ = other.sz_;
       }
       return *this;
   }

   /**
 * TODO Destructors
   */
   ~map() {
       clear();
   }

   /**
 * TODO
 * access specified element with bounds checking
 * Returns a reference to the mapped value of the element with key equivalent to key.
 * If no such element exists, an exception of type `index_out_of_bound'
   */
   T &at(const Key &key) {
       Node *n = findNode(key);
       if (n == nullptr) {
           throw index_out_of_bound();
       }
       return n->data.second;
   }

   const T &at(const Key &key) const {
       Node *n = findNode(key);
       if (n == nullptr) {
           throw index_out_of_bound();
       }
       return n->data.second;
   }

   /**
 * TODO
 * access specified element
 * Returns a reference to the value that is mapped to a key equivalent to key,
 *   performing an insertion if such key does not already exist.
   */
   T &operator[](const Key &key) {
       Node *n = findNode(key);
       if (n != nullptr) {
           return n->data.second;
       }
       value_type v(key, T());
       pair<iterator, bool> ret = insert(v);
       return ret.first->second;
   }

   /**
 * behave like at() throw index_out_of_bound if such key does not exist.
   */
   const T &operator[](const Key &key) const {
       return at(key);
   }

   /**
 * return a iterator to the beginning
   */
   iterator begin() {
       if (root_ == nullptr) return iterator(nullptr);
       Node *n = root_;
       while (n->left != nullptr) n = n->left;
       return iterator(n);
   }

   const_iterator cbegin() const {
       if (root_ == nullptr) return const_iterator(nullptr);
       Node *n = root_;
       while (n->left != nullptr) n = n->left;
       return const_iterator(n);
   }

   /**
 * return a iterator to the end
 * in fact, it returns past-the-end.
   */
   iterator end() {
       return iterator(nullptr);
   }

   const_iterator cend() const {
       return const_iterator(nullptr);
   }

   /**
 * checks whether the container is empty
 * return true if empty, otherwise false.
   */
   bool empty() const {
       return sz_ == 0;
   }

   /**
 * returns the number of elements.
   */
   size_t size() const {
       return sz_;
   }

   /**
 * clears the contents
   */
   void clear() {
       deleteSubtree(root_);
       root_ = nullptr;
       sz_ = 0;
   }

   /**
 * insert an element.
 * return a pair, the first of the pair is
 *   the iterator to the new element (or the element that prevented the insertion),
 *   the second one is true if insert successfully, or false.
   */
   pair<iterator, bool> insert(const value_type &value) {
       if (root_ == nullptr) {
           root_ = new Node(value, nullptr);
           sz_ = 1;
           return pair<iterator, bool>(iterator(root_), true);
       }
       Node *cur = root_;
       Node *parent = nullptr;
       while (cur != nullptr) {
           parent = cur;
           if (cmp_(value.first, cur->data.first)) {
               cur = cur->left;
           } else if (cmp_(cur->data.first, value.first)) {
               cur = cur->right;
           } else {
               return pair<iterator, bool>(iterator(cur), false);
           }
       }
       Node *newNode = new Node(value, parent);
       if (cmp_(value.first, parent->data.first)) {
           parent->left = newNode;
       } else {
           parent->right = newNode;
       }
       ++sz_;
       rebalanceInsert(newNode);
       return pair<iterator, bool>(iterator(newNode), true);
   }

   /**
 * erase the element at pos.
 *
 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
   void erase(iterator pos) {
       if (pos.node == nullptr) {
           throw invalid_iterator();
       }
       Node *n = pos.node;
       eraseNode(n);
       --sz_;
   }

   /**
 * Returns the number of elements with key
 *   that compares equivalent to the specified argument,
 *   which is either 1 or 0
 *   since this container does not allow duplicates.
 * The default method of check the equivalence is !(a < b || b > a)
   */
   size_t count(const Key &key) const {
       return findNode(key) != nullptr ? 1 : 0;
   }

   /**
 * Finds an element with key equivalent to key.
 * key value of the element to search for.
 * Iterator to an element with key equivalent to key.
 *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
   iterator find(const Key &key) {
       Node *n = findNode(key);
       if (n == nullptr) return iterator(nullptr);
       return iterator(n);
   }

   const_iterator find(const Key &key) const {
       Node *n = findNode(key);
       if (n == nullptr) return const_iterator(nullptr);
       return const_iterator(n);
   }

  private:
   Node *root_;
   size_t sz_;
   Compare cmp_;

   Node *findNode(const Key &key) const {
       Node *cur = root_;
       while (cur != nullptr) {
           if (cmp_(key, cur->data.first)) {
               cur = cur->left;
           } else if (cmp_(cur->data.first, key)) {
               cur = cur->right;
           } else {
               return cur;
           }
       }
       return nullptr;
   }

   int getHeight(Node *n) const {
       return n == nullptr ? 0 : n->height;
   }

   int getBalance(Node *n) const {
       return n == nullptr ? 0 : getHeight(n->left) - getHeight(n->right);
   }

   void updateHeight(Node *n) {
       n->height = 1 + max(getHeight(n->left), getHeight(n->right));
   }

   int max(int a, int b) const {
       return a > b ? a : b;
   }

   Node *rotateRight(Node *y) {
       Node *x = y->left;
       Node *T2 = x->right;

       x->right = y;
       y->parent = x;
       x->parent = y->parent;
       y->left = T2;
       if (T2 != nullptr) T2->parent = y;

       updateHeight(y);
       updateHeight(x);
       return x;
   }

   Node *rotateLeft(Node *x) {
       Node *y = x->right;
       Node *T2 = y->left;

       y->left = x;
       x->parent = y;
       y->parent = x->parent;
       x->right = T2;
       if (T2 != nullptr) T2->parent = x;

       updateHeight(x);
       updateHeight(y);
       return y;
   }

   void rebalanceInsert(Node *n) {
       while (n != nullptr) {
           updateHeight(n);
           int balance = getBalance(n);

           if (balance > 1) {
               if (n->left != nullptr && cmp_(n->left->data.first, n->data.first) == false &&
                   cmp_(n->data.first, n->left->data.first) == false) {
                   n->left = rotateLeft(n->left);
                   if (n->left != nullptr) n->left->parent = n;
               }
               if (n->parent != nullptr) {
                   if (n->parent->left == n) n->parent->left = rotateRight(n);
                   else n->parent->right = rotateRight(n);
               } else {
                   root_ = rotateRight(n);
                   root_->parent = nullptr;
               }
           } else if (balance < -1) {
               if (n->right != nullptr && cmp_(n->data.first, n->right->data.first) == false &&
                   cmp_(n->right->data.first, n->data.first) == false) {
                   n->right = rotateRight(n->right);
                   if (n->right != nullptr) n->right->parent = n;
               }
               if (n->parent != nullptr) {
                   if (n->parent->left == n) n->parent->left = rotateLeft(n);
                   else n->parent->right = rotateLeft(n);
               } else {
                   root_ = rotateLeft(n);
                   root_->parent = nullptr;
               }
           }
           n = n->parent;
       }
   }

   Node *findMin(Node *n) const {
       while (n->left != nullptr) n = n->left;
       return n;
   }

   void eraseNode(Node *z) {
       Node *y = z;
       Node *x = nullptr;

       if (z->left == nullptr) {
           x = z->right;
       } else if (z->right == nullptr) {
           x = z->left;
       } else {
           y = findMin(z->right);
           x = y->right;
       }

       if (x != nullptr) x->parent = y->parent;

       if (y->parent == nullptr) {
           root_ = x;
       } else {
           if (y == y->parent->left) {
               y->parent->left = x;
           } else {
               y->parent->right = x;
           }
       }

       if (y != z) {
           z->data.~value_type();
           new (&z->data) value_type(y->data);
       }

       if (y != z) {
           rebalanceAfterErase(y->parent);
       } else {
           rebalanceAfterErase(x);
       }

       delete y;
   }

   void rebalanceAfterErase(Node *n) {
       while (n != nullptr) {
           updateHeight(n);
           int balance = getBalance(n);

           if (balance > 1) {
               if (n->left != nullptr && getBalance(n->left) < 0) {
                   n->left = rotateLeft(n->left);
                   if (n->left != nullptr) n->left->parent = n;
               }
               if (n->parent != nullptr) {
                   if (n->parent->left == n) n->parent->left = rotateRight(n);
                   else n->parent->right = rotateRight(n);
               } else {
                   root_ = rotateRight(n);
                   root_->parent = nullptr;
               }
           } else if (balance < -1) {
               if (n->right != nullptr && getBalance(n->right) > 0) {
                   n->right = rotateRight(n->right);
                   if (n->right != nullptr) n->right->parent = n;
               }
               if (n->parent != nullptr) {
                   if (n->parent->left == n) n->parent->left = rotateLeft(n);
                   else n->parent->right = rotateLeft(n);
               } else {
                   root_ = rotateLeft(n);
                   root_->parent = nullptr;
               }
           }
           n = n->parent;
       }
   }

   Node *copySubtree(Node *src, Node *parent) {
       if (src == nullptr) return nullptr;
       Node *n = new Node(src->data, parent);
       n->height = src->height;
       n->left = copySubtree(src->left, n);
       n->right = copySubtree(src->right, n);
       return n;
   }

   void deleteSubtree(Node *n) {
       if (n == nullptr) return;
       deleteSubtree(n->left);
       deleteSubtree(n->right);
       delete n;
   }
};

}

#endif
