#ifndef SEQUENCING_SEQUENCE_H__
#define SEQUENCING_SEQUENCE_H__

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#include <boost/coroutine/coroutine.hpp>
#pragma GCC diagnostic pop
#include <functional>
#include <iterator>
#include <type_traits>


namespace sequencing {

template<class> class sequence;


#if 0
template<class T>
class sequence_iterator : public std::iterator<std::input_iterator_tag, T> {
   friend class sequence<T>;
   typedef typename boost::coroutines::asymmetric_coroutine<T>::push_type coro_t;
   typedef decltype(boost::begin(std::declval<coro_t &>())) base_iterator;

public:
   inline T operator*() {
      return iter.operator*();
   }

   inline sequence_iterator<T> & operator++() {
      ++iter;
      return *this;
   }

   inline sequence_iterator<T> operator++(int) {
      return sequence_iterator(iter++);
   }

   inline bool operator==(const sequence_iterator &rhs) const {
      return const_cast<sequence_iterator &>(*this).iter == const_cast<sequence_iterator &>(rhs).iter;
   }

   inline bool operator!=(const sequence_iterator &rhs) const {
      return const_cast<sequence_iterator &>(*this).iter != const_cast<sequence_iterator &>(rhs).iter;
   }

private:
   inline sequence_iterator(base_iterator i) :
      iter(std::move(i))
   {
   }

   base_iterator iter;
};
#else
template<class T>
using sequence_iterator = typename boost::range_iterator<typename boost::coroutines::asymmetric_coroutine<T>::pull_type>::type;
#endif


template<class Op>
class sequence_operation {
   template<class T> friend class sequence;
   Op op;

public:
   explicit inline sequence_operation(Op &&op_) :
      op(std::move(op_))
   {
   }

   sequence_operation(const sequence_operation<Op> &) = delete;
   sequence_operation(sequence_operation<Op> &&) = default;

   template<class S>
   inline auto operator()(sequence<S> &&s) {
      return op(std::move(s));
   }
};


template<class F>
inline sequence_operation<F> sequence_manipulator(F &&f) {
   return sequence_operation<F>{std::move(f)};
}


namespace details_ {

template<class T>
class has_caller_type {
   typedef char yes;
   typedef char no[2];

   template<class U>
   static yes & test(T *, typename U::caller_type *dummy=nullptr);
   template<class U>
   static no  & test(...);

public:
   static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
};


template<bool, class> struct sink_type_helper;

template<class T>
struct sink_type_helper<true, T> {
   typedef typename T::caller_type type;
};

template<class T>
struct sink_type_helper<false, T> {
   typedef typename T::pull_type type;
};

}


template<class T>
class sequence {
   template<class U> friend class sequence;
   typedef typename boost::coroutines::asymmetric_coroutine<T>::pull_type coro_t;

public:
   typedef T value_type;
   typedef sequence_iterator<T> iterator;
   typedef size_t size_type;
   typedef typename boost::coroutines::asymmetric_coroutine<T>::push_type sink_type;

   template<class Fun, class Alloc>
   explicit inline sequence(std::allocator_arg_t, const Alloc &alloc, Fun &&f) :
      coro{std::allocate_shared<coro_t>(alloc, std::move(f))}
   {
   }

   template<class Fun>
   explicit inline sequence(Fun &&f) :
      sequence{std::allocator_arg, std::allocator<void>{}, std::move(f)}
   {
   }

   sequence() :
      sequence{[](sink_type &) {}}
   {
   }

   sequence(sequence &&) = default;
   sequence(const sequence &) = delete;
   ~sequence() = default;

   sequence & operator =(sequence &&) = default;
   sequence & operator =(const sequence &) = delete;

   inline iterator begin() {
      return boost::begin(*coro);
   }

   inline iterator end() {
      return boost::end(*coro);
   }

   inline bool empty() const {
      return !(coro && *coro);
   }

private:
   std::shared_ptr<coro_t> coro;
};


template<class L, class R>
inline bool operator==(const sequence<L> &l, const sequence<R> &r) {
   using std::begin;
   using std::end;

   auto li = begin(l);
   auto le = end(l);
   auto ri = begin(r);
   auto re = end(r);

   while (li != le && ri != re) {
      if (*li++ != *ri++) {
         return false;
      }
   }

   return li == le && ri == re;
}


template<class L, class R>
inline bool operator!=(const sequence<L> &l, const sequence<R> &r) {
   return !(l == r);
}


template<class S, class Op>
inline auto operator|(sequence<S> &s, sequence_operation<Op> sop) {
   using std::move;

   return sop(move(s));
}


template<class S, class Op>
inline auto operator|(sequence<S> &&s, sequence_operation<Op> sop) {
   using std::move;

   return sop(move(s));
}


template<class Sink>
class sequence_sink_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
public:
   typedef Sink sink_type;

   class reference {
   public:
      explicit inline reference(sink_type &s) :
         sink{s}
      {
      }

      template<class T>
      inline reference &operator =(T &&t) {
         using std::forward;

         sink(forward<T>(t));
         return *this;
      }

   private:
      sink_type &sink;
   };

   explicit inline sequence_sink_iterator(sink_type &s) :
      sink(s)
   {
   }

   inline reference operator*() {
      return reference(sink);
   }

   inline sequence_sink_iterator<Sink> &operator++() {
      return *this;
   }

   inline sequence_sink_iterator<Sink> &operator++(int) {
      return *this;
   }

private:
   sink_type &sink;
};


template<class Sink>
inline sequence_sink_iterator<Sink> sink_iterator(Sink &sink) {
   return sequence_sink_iterator<Sink>{sink};
}


#include "details/aggregate.h"
#include "details/container.h"
#include "details/element_access.h"
#include "details/logical.h"
#include "details/ordering.h"
#include "details/partitioning.h"
#include "details/projection.h"
#include "details/restriction.h"
#include "details/set_operations.h"

}


namespace std {

template<class, class> struct uses_allocator;

template<class SequenceType, class Alloc>
struct uses_allocator<sequencing::sequence<SequenceType>, Alloc> : std::true_type {};

}

#endif
