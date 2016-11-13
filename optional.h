#include <iterator>
#include <cstddef>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <functional>


/**
 * A C++ optional container that supports iteration and has emplace
 * semantics to avoid copies.
 *
 * It uses variadic templates to forward all args by reference to the
 * instantiable type T.  Again, this is to elide copies and is the emplace
 * semantic.
 *
 * This loosely follows the Scala Option class API.  Necessity being the father of
 * invention, this spawned from the missing features in the C++ language, namely
 * Optional.
 *
 * Having used Scala's Option class, going back to C++ and having to check
 * unique or shared pointers to verify whether or not they are set seems very
 * primitive.  It can also lead to some bad situations if it isn't checked,
 * which is the real reason for this class.
 *
 * The beautiful thing about Optional and Scala optionals is that they support
 * iteration, which either iterates zero or one time, depending on whether it is
 * defined.  This leads to very easy to reason about code with minimal if
 * statements and clear meaning.  It is meant to help stop the spread of null
 * values around a code-base.
 *
 * Here is the license information:
 *
 * Copyright (c) 2016, Darin Velarde
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Darin Velarde nor the
 *       names of project contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Darin Velarde BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//------------------------------------------------------------------------------
// Pointer vs Stack Helpers
//------------------------------------------------------------------------------
/**
 * If T is a pointer initialize it with nullptr
 */
template<class P>
inline void
pointerInit(P* &p)
{
  p = nullptr;
}


/**
 * Otherwise do nothing.  In the case that T is not a pointer type then call the
 * other function.
 */
template<class P>
inline void
pointerInit(P const&) {
  ; // No op
}


/**
 * If T is a pointer delete it
 */
template<class P>
inline void
pointerDelete(P* &p)
{
  if (p != nullptr) {
    delete p;
    p = nullptr;
  }
}


/**
 * Otherwise do nothing.  In the case that T is not a pointer type then call the
 * other function.
 */
template<class P>
inline void
pointerDelete(P const&) {
  ; // No op
}


//------------------------------------------------------------------------------
// Optional Class
//------------------------------------------------------------------------------
template <typename T>
class Optional
{
 private:
  /**
   * Because both internal classes need type T, embed them so they inherit T
   * from Optional
   */

  /**
   * Item class to allow iteration
   *
   * ** Not for external use **
   */
  struct Item {
    Item* next;
    T data;

    Item()
      : next(this)
    {
      pointerInit(data);
    }

    ~Item()
    {
      pointerDelete(data);
    }
  };

 private:
  /**
   * The iterator type returned by begin() and end() in Optional
   *
   * ** Not for external use **
   *
   * This holds a Item pointer to the object that is held by the optional.
   */
  class iterator
  {
    friend class Optional;

   private:
    explicit
    iterator(Item* node)
      : m_node(node)
    { }

   public:
    iterator() = delete;

    iterator&
    operator++() {
      m_node = m_node->next;
      return *this;
    }

    T&
    operator*()
    {
      // Dereferencing the end node will most likely segfault
      return m_node->data;
    }

    bool
    operator==(const iterator& other) const
    {
      return (m_node == other.m_node);
    }

    bool
    operator!=(const iterator& other) const
    {
      return (m_node != other.m_node);
    }

   private:
    Item* m_node; // This is cleaned up in the Optional class
  };

 public:
  /**
   * No arg constructor, used to make an empty Optional
   */
  Optional()
    : m_end(),
      m_container(nullptr)
  { }

  /**
   * Single arg constructor
   * @param data: A copyable object to add to the the Optional at construction
   */
  explicit
  Optional(const T& data)
    : m_end(),
      m_container(nullptr)
  {
    update(data);
  }

  /**
   * Emplacement Constructor
   * @param args: The constructor args for type T to construct it in place.
   */
  template <typename... Args>
  Optional(Args&&... args)
    : m_end(),
      m_container(nullptr)
  {
    // Instantiate a T with variadic template args (emplace)
    update(std::forward<Args>(args)...);
  }

  /**
   * Destructor
   */
  ~Optional()
  {
    delete m_container;
  }

  /**
   * Begin iterator method
   */
  iterator
  begin()
  {
    return iterator(m_end.next);
  }

  /**
   * End iterator method
   */
  iterator
  end()
  {
    return iterator(&m_end);
  }

  /**
   * Empty method
   *
   * Return true if empty
   */
  bool
  empty()
  {
    return (m_container == nullptr);
  }

  /**
   * Defined method
   *
   * Return true if defined/set
   */
  bool
  defined()
  {
    return (m_container != nullptr);
  }

  /**
   * Get method to get the contained object or throw if there is none
   */
  T&
  get()
  {
    if (m_container) {
      return m_container->data;
    }
    else {
      std::stringstream ss;
      ss << "The Optional needs to be set before access ("
         << __FILE__
         << ":"
         << __LINE__
         << ")";
      throw std::runtime_error(ss.str());
    }
  }

 private:
  /**
   * Update method
   *
   * Sets/updates the contained data.
   *
   * @param args: The constructor args for a new type T object to contain.
   */
  template <typename... Args>
  void
  update(Args&&... args)
  {
    T data(std::forward<Args>(args)...);
    if (m_container) {
      throw std::runtime_error("Something bad happened in Optional");
    }
    m_container = new Item;
    m_container->data = std::move(data);
    m_container->next = &m_end;
    m_end.next = m_container;
  }

  Item m_end;
  Item* m_container; // Pointer to use nullptr as a sentinal
};

