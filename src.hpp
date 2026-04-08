#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include <cstddef>
#include <type_traits>
#include <stdexcept>

namespace sjtu {

template <typename T> class list {
protected:
  struct node {
    node* prev;
    node* next;
    typename std::aligned_storage<sizeof(T), alignof(T)>::type data;
    T* valptr() { return reinterpret_cast<T*>(&data); }
    const T* valptr() const { return reinterpret_cast<const T*>(&data); }
  };

  node* head;
  size_t sz;

  node *insert(node *pos, node *cur) {
    cur->next = pos;
    cur->prev = pos->prev;
    pos->prev->next = cur;
    pos->prev = cur;
    sz++;
    return cur;
  }

  node *erase(node *pos) {
    pos->prev->next = pos->next;
    pos->next->prev = pos->prev;
    sz--;
    return pos;
  }

public:
  class const_iterator;
  class iterator {
    friend class list<T>;
    friend class const_iterator;
  private:
    node* ptr;
    const list<T>* lst;
  public:
    iterator(node* p = nullptr, const list<T>* l = nullptr) : ptr(p), lst(l) {}
    
    iterator operator++(int) {
      iterator tmp = *this;
      ptr = ptr->next;
      return tmp;
    }
    iterator &operator++() {
      ptr = ptr->next;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp = *this;
      ptr = ptr->prev;
      return tmp;
    }
    iterator &operator--() {
      ptr = ptr->prev;
      return *this;
    }

    T &operator*() const {
      if (!ptr || ptr == lst->head) throw std::runtime_error("invalid iterator");
      return *(ptr->valptr());
    }
    T *operator->() const noexcept {
      return ptr->valptr();
    }

    bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator==(const const_iterator &rhs) const;
    bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
    bool operator!=(const const_iterator &rhs) const;
  };

  class const_iterator {
    friend class list<T>;
    friend class iterator;
  private:
    node* ptr;
    const list<T>* lst;
  public:
    const_iterator(node* p = nullptr, const list<T>* l = nullptr) : ptr(p), lst(l) {}
    const_iterator(const iterator& other) : ptr(other.ptr), lst(other.lst) {}

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ptr = ptr->next;
      return tmp;
    }
    const_iterator &operator++() {
      ptr = ptr->next;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator tmp = *this;
      ptr = ptr->prev;
      return tmp;
    }
    const_iterator &operator--() {
      ptr = ptr->prev;
      return *this;
    }

    const T &operator*() const {
      if (!ptr || ptr == lst->head) throw std::runtime_error("invalid iterator");
      return *(ptr->valptr());
    }
    const T *operator->() const noexcept {
      return ptr->valptr();
    }

    bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
    bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
  };

  list() {
    head = new node();
    head->prev = head->next = head;
    sz = 0;
  }
  
  list(const list &other) {
    head = new node();
    head->prev = head->next = head;
    sz = 0;
    try {
      for (auto it = other.cbegin(); it != other.cend(); ++it) {
        push_back(*it);
      }
    } catch (...) {
      clear();
      delete head;
      throw;
    }
  }
  
  virtual ~list() {
    clear();
    delete head;
  }
  
  list &operator=(const list &other) {
    if (this == &other) return *this;
    list tmp(other);
    node* tmp_head = head;
    size_t tmp_sz = sz;
    head = tmp.head;
    sz = tmp.sz;
    tmp.head = tmp_head;
    tmp.sz = tmp_sz;
    return *this;
  }
  
  T &front() {
    if (empty()) throw std::runtime_error("container is empty");
    return *(head->next->valptr());
  }
  T &back() {
    if (empty()) throw std::runtime_error("container is empty");
    return *(head->prev->valptr());
  }
  const T &front() const {
    if (empty()) throw std::runtime_error("container is empty");
    return *(head->next->valptr());
  }
  const T &back() const {
    if (empty()) throw std::runtime_error("container is empty");
    return *(head->prev->valptr());
  }
  
  iterator begin() { return iterator(head->next, this); }
  const_iterator begin() const { return const_iterator(head->next, this); }
  const_iterator cbegin() const { return const_iterator(head->next, this); }
  
  iterator end() { return iterator(head, this); }
  const_iterator end() const { return const_iterator(head, this); }
  const_iterator cend() const { return const_iterator(head, this); }
  
  virtual bool empty() const { return sz == 0; }
  virtual size_t size() const { return sz; }

  virtual void clear() {
    while (!empty()) {
      pop_back();
    }
  }
  
  virtual iterator insert(iterator pos, const T &value) {
    if (pos.lst != this) throw std::runtime_error("invalid iterator");
    node* cur = new node();
    try {
      new (cur->valptr()) T(value);
    } catch (...) {
      delete cur;
      throw;
    }
    insert(pos.ptr, cur);
    return iterator(cur, this);
  }
  
  virtual iterator erase(iterator pos) {
    if (pos.lst != this || pos.ptr == head) throw std::runtime_error("invalid iterator");
    node* nxt = pos.ptr->next;
    erase(pos.ptr);
    pos.ptr->valptr()->~T();
    delete pos.ptr;
    return iterator(nxt, this);
  }
  
  void push_back(const T &value) {
    insert(end(), value);
  }
  
  void pop_back() {
    if (empty()) throw std::runtime_error("container is empty");
    erase(--end());
  }
  
  void push_front(const T &value) {
    insert(begin(), value);
  }
  
  void pop_front() {
    if (empty()) throw std::runtime_error("container is empty");
    erase(begin());
  }
};

template <typename T>
bool list<T>::iterator::operator==(const const_iterator &rhs) const {
  return ptr == rhs.ptr;
}

template <typename T>
bool list<T>::iterator::operator!=(const const_iterator &rhs) const {
  return ptr != rhs.ptr;
}

} // namespace sjtu

#endif // SJTU_LIST_HPP