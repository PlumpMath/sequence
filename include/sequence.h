#ifndef SEQUENCE_H__
#define SEQUENCE_H__

#include <boost/coroutine/coroutine.hpp>
#include <functional>
#include <iterator>
#include <type_traits>


template<class> class sequence;


template<class T>
class sequence_iterator : public std::iterator<std::input_iterator_tag, T> {
   friend class sequence<T>;
   typedef boost::coroutines::coroutine<T()> coro_t;
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

   inline bool operator==(const sequence_iterator &rhs) {
      return iter == rhs.iter;
   }

   inline bool operator!=(const sequence_iterator &rhs) {
      return iter != rhs.iter;
   }

private:
   inline sequence_iterator(base_iterator i) :
      iter(std::move(i))
   {
   }

   base_iterator iter;
};


template<class T>
class sequence {
   typedef boost::coroutines::coroutine<T()> coro_t;

public:
   typedef sequence_iterator<T> iterator;
   typedef sequence_iterator<T> const_iterator;

   sequence() = delete;
   sequence(sequence &&) = default;
   sequence(const sequence &) = delete;
   ~sequence() = default;

   sequence & operator =(sequence &&) = default;
   sequence & operator =(const sequence &) = delete;

   inline const_iterator begin() const {
      return boost::begin(*coro);
   }

   inline const_iterator end() const {
      return boost::end(*coro);
   }

   inline iterator begin() {
      return boost::begin(*coro);
   }

   inline iterator end() {
      return boost::end(*coro);
   }

   inline bool empty() const {
      return !(*coro);
   }

   template<class Predicate>
   inline bool any(Predicate predicate) {
      for (auto t : *coro) {
         if (predicate(t)) {
            return true;
         }
      }
      return false;
   }

   template<class Predicate>
   inline bool all(Predicate predicate) {
      for (auto t : *coro) {
         if (!predicate(t)) {
            return false;
         }
      }
      return true;
   }

   template<class Comp=std::less<T>>
   inline T max(Comp comp=Comp()) const {
      auto iter = std::max_element(begin(), end(), comp);
      if (iter == end()) {
         throw std::domain_error("Max cannot be computed on empty sequence.");
      }
      return *iter;
   }

   template<class Comp=std::less<T>>
   inline T min(Comp comp=Comp()) {
      auto iter = std::min_element(begin(), end(), comp);
      if (iter == end()) {
         throw std::domain_error("Min cannot be computed on empty sequence.");
      }
      return *iter;
   }

   template<class Add=std::plus<T>>
   inline T sum(T initial=T(), Add add=Add()) const {
      return std::accumulate(begin(), end(), initial, add);
   }

   template<class Predicate>
   inline sequence<T> where(Predicate predicate) {
      auto co = coro;
      auto f = [co, predicate](typename coro_t::caller_type &ca) {
            for (const T &t : *co) {
               if (predicate(t)) {
                  ca(t);
               }
            }
         };
      return sequence<T>(std::move(f));
   }

   static inline typename std::enable_if<std::is_arithmetic<T>::value, sequence<T>>::type range(T start, T finish, T delta=1) {
      check_delta(delta, std::is_signed<T>());

      if (start < finish) {
         return sequence<T>([start, finish, delta](typename coro_t::caller_type &ca) {
               for (T i = start; i < finish; i+=delta) {
                  ca(i);
               }
            });
      }
      else if (finish < start) {
         return sequence<T>([start, finish, delta](typename coro_t::caller_type &ca) {
               for (T i = start; i > finish; i-=delta) {
                  ca(i);
               }
            });
      }

      return empty_sequence();
   }

   static inline sequence<T> empty_sequence() {
      return sequence<T>([](typename coro_t::caller_type &){});
   }

private:
   template<class Fun>
   explicit inline sequence(Fun f) :
      coro(std::make_shared<coro_t>(std::move(f)))
   {
   }

   static inline void check_delta(T delta, std::true_type) {
      if (!(0 < delta)) {
         throw std::domain_error("Delta must be positive.");
      }
   }

   static inline void check_delta(T, std::false_type) noexcept {
   }

   std::shared_ptr<coro_t> coro;
};

#endif
