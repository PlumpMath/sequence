#ifndef SEQUENCE_H__
#define SEQUENCE_H__

#include <boost/coroutine/coroutine.hpp>
#include <functional>
#include <iterator>
#include <type_traits>


template<class> class sequence;


namespace details_ {

template<class T, class Transform>
class select_many_helper {
   typedef decltype(std::declval<Transform>()(std::declval<T>())) transform_result_sequence;

public:
   typedef decltype(std::declval<transform_result_sequence>().first()) element_type;
   typedef sequence<element_type> sequence_type;
};

}


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


template<class T>
class sequence {
   template<class U> friend class sequence;
   typedef boost::coroutines::coroutine<T()> coro_t;

public:
   typedef sequence_iterator<T> iterator;
   typedef sequence_iterator<T> const_iterator;
   typedef typename coro_t::caller_type sink_type;

   template<class Fun>
   explicit inline sequence(Fun f) :
      coro(std::make_shared<coro_t>(std::move(f)))
   {
   }

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
      auto iter = std::find_if(begin(), end(), predicate);
      return iter != end();
   }

   template<class Predicate>
   inline bool all(Predicate predicate) {
      auto iter = std::find_if(begin(), end(), [predicate](const T &t) { return !predicate(t); });
      return iter == end();
   }

   inline T first() {
      auto iter = begin();
      if (iter == end()) {
         throw std::range_error("First cannot be computed on empty sequence.");
      }
      return *iter;
   }

   inline T first_or_default(const T &default_value=T()) {
      auto iter = begin();
      if (iter == end()) {
         return default_value;
      }
      return *iter;
   }

   inline T last() {
      auto i = begin();
      auto e = end();
      if (i == e) {
         throw std::range_error("Last cannot be computed on empty sequence.");
      }

      T result = *i;
      for (++i; i != e; ++i) {
         result = *i;
      }
      return result;
   }

   inline T last_or_default(T default_value=T()) {
      for (auto i = begin(); i != end(); ++i) {
         default_value = *i;
      }
      return default_value;
   }

   inline T element_at(std::size_t index) {
      try {
         return skip(index).first();
      }
      catch (const std::range_error &) {
         throw std::range_error("Element at index cannot be retrieved because there aren't enough elements in the sequence.");
      }
   }

   inline T element_at_or_default(std::size_t index, T default_value=T()) {
      return skip(index).first_or_default(default_value);
   }

   inline T single() {
      auto iter = begin();
      if (iter == end()) {
         throw std::range_error("Cannot retrieve single result from empty sequence.");
      }
      T result = *iter++;

      if (iter != end()) {
         throw std::range_error("More than one element present in sequence.");
      }

      return std::move(result);
   }

   inline T single_or_default(T default_value=T()) {
      auto iter = begin();
      if (iter == end()) {
         return default_value;
      }
      T result = *iter++;

      if (iter != end()) {
         return default_value;
      }

      return std::move(result);
   }

   template<class Comp=std::less<T>, class Alloc=std::allocator<T>>
   inline sequence<T> sort(std::size_t reserve=0, Comp comp=Comp(), Alloc alloc=Alloc()) {
      auto co = coro;
      return sequence<T>([co, reserve, comp, alloc](sink_type &put) {
            std::vector<T, Alloc> v(alloc);
            v.reserve(reserve);
            v.insert(v.end(), boost::begin(*co), boost::end(*co));
            std::sort(v.begin(), v.end(), comp);
            std::for_each(v.begin(), v.end(), std::ref(put));
         });
   }

   template<class Alloc=std::allocator<T>>
   inline sequence<T> reverse(std::size_t reserve=0, Alloc alloc=Alloc());

   inline bool contains(const T &value) {
      return std::find(begin(), end(), value) != end();
   }

   inline std::size_t count() const {
      return static_cast<std::size_t>(std::distance(begin(), end()));
   }

   template<class Predicate>
   inline std::size_t count(Predicate predicate) {
      return std::count_if(begin(), end(), std::move(predicate));
   }

   template<class U>
   inline sequence<std::pair<T, U>> zip_with(const sequence<U> &other) {
      auto t_coro = this->coro;
      auto u_coro = other.coro;
      return sequence<std::pair<T, U>>([t_coro, u_coro](typename sequence<std::pair<T, U>>::coro_t::caller_type &put) {
            auto ti = boost::begin(*t_coro);
            auto tend = boost::end(*t_coro);
            auto ui = boost::begin(*u_coro);
            auto uend = boost::end(*u_coro);

            while (ti != tend && ui != uend) {
               put({ *ti++, *ui++ });
            }
         });
   }

   inline sequence<std::pair<T, T>> pairwise(bool capture_remainder=false) {
      auto co = this->coro;
      return sequence<std::pair<T, T>>([co, capture_remainder](typename sequence<std::pair<T, T>>::coro_t::caller_type &put) {
            auto i = boost::begin(*co);
            auto e = boost::end(*co);

            while (i != e) {
               T first = *i++;
               if (i != e) {
                  put({ first, *i++ });
               }
               else if (capture_remainder) {
                  put({ first, T() });
               }
            }
         });
   }

   template<class Comp=std::less<T>>
   inline T max(Comp comp=Comp()) {
      auto iter = std::max_element(begin(), end(), comp);
      if (iter == end()) {
         throw std::range_error("Max cannot be computed on empty sequence.");
      }
      return *iter;
   }

   template<class Comp=std::less<T>>
   inline T min(Comp comp=Comp()) {
      auto iter = std::min_element(begin(), end(), comp);
      if (iter == end()) {
         throw std::range_error("Min cannot be computed on empty sequence.");
      }
      return *iter;
   }

#if 0
   template<class Comp=std::less<T>>
   inline std::pair<T, T> minmax(Comp comp=Comp()) {
      auto p = std::minmax_element(begin(), end(), comp);
      if (p.first == end()) {
         throw std::range_error("Min-Max cannot be computed on empty sequence.");
      }
      return { *p.first, *p.second };
   }
#endif

   template<class Add=std::plus<T>>
   inline T sum(T initial=T(), Add add=Add()) {
      return std::accumulate(begin(), end(), initial, add);
   }

   template<class U, class R=typename std::common_type<T, U>::type, class BinOp2=std::multiplies<typename std::common_type<T, U>::type>,
            class BinOp1=std::plus<typename std::common_type<T, U>::type>>
   inline R inner_product(const sequence<U> &rhs, R init=R(), BinOp1 binOp1=BinOp1(), BinOp2 binOp2=BinOp2()) {
      return std::inner_product(begin(), end(), rhs.begin(), init, binOp1, binOp2);
   }

   template<class Transform>
   inline sequence<decltype(std::declval<Transform>()(std::declval<T>()))> select(Transform transform) {
      typedef typename std::result_of<Transform(T)>::type result_type;
      typedef sequence<result_type> result_sequence;
      auto co = coro;
      return result_sequence([co, transform](typename result_sequence::sink_type &put) {
            for (const T &t : *co) {
               put(transform(t));
            }
         });
   }

   template<class Transform>
   inline sequence<typename details_::select_many_helper<T, Transform>::element_type> select_many(Transform transform) {
      typedef typename details_::select_many_helper<T, Transform>::sequence_type result_sequence;
      auto co = coro;
      return result_sequence([co, transform](typename result_sequence::coro_t::caller_type &put) {
            for (const T &t : *co) {
               for (const auto &s : transform(t)) {
                  put(s);
               }
            }
         });
   }

   template<class Predicate>
   inline sequence<T> where(Predicate predicate) {
      auto co = coro;
      auto f = [co, predicate](sink_type &put) {
            for (const T &t : *co) {
               if (predicate(t)) {
                  put(t);
               }
            }
         };
      return sequence<T>(std::move(f));
   }

   inline sequence<T> concat(const sequence<T> &other) {
      auto l_co = coro;
      auto r_co = other.coro;
      return sequence<T>([l_co, r_co](sink_type &put) {
            for (const T &t : *l_co) {
               put(t);
            }
            for (const T &t : *r_co) {
               put(t);
            }
         });
   }

   inline sequence<T> take(std::size_t n) {
      auto co = coro;
      return sequence<T>([co, n](sink_type &put) {
            std::size_t i = 0;
            auto iter = boost::begin(*co);
            auto eiter = boost::end(*co);

            while (i < n && iter != eiter) {
               put(*iter);
               ++iter; ++i;
            }
         });
   }

   template<class Predicate>
   inline sequence<T> take_while(Predicate predicate) {
      auto co = coro;
      return sequence<T>([co, predicate](sink_type &put) {
            auto i = boost::begin(*co);
            auto iend = boost::end(*co);

            while (i != iend && predicate(*i)) {
               put(*i++);
            }
         });
   }

   inline sequence<T> skip(std::size_t n) {
      auto co = coro;
      return sequence<T>([co, n](sink_type &put) {
            std::size_t i = 0;
            auto iter = boost::begin(*co);
            auto eiter = boost::end(*co);

            while (i < n && iter != eiter) {
               ++iter; ++i;
            }

            while (iter != eiter) {
               put(*iter++);
            }
         });
   }

   template<class Predicate>
   inline sequence<T> skip_while(Predicate predicate) {
      auto co = coro;
      return sequence<T>([co, predicate](sink_type &put) {
            auto i = boost::begin(*co);
            auto iend = boost::end(*co);

            while (i != iend && predicate(*i)) {
               ++i;
            }

            while (i != iend) {
               put(*i++);
            }
         });
   }

   inline sequence<T> page(std::size_t page_number, std::size_t page_size) {
      return skip(page_number * page_size)
            .take(page_size);
   }

   template<class Comp=std::less<T>>
   inline sequence<T> union_with(const sequence<T> &other, Comp comp=Comp());

   template<class Comp=std::less<T>>
   inline sequence<T> intersect_with(const sequence<T> &other, Comp comp=Comp());

   template<class Comp=std::less<T>>
   inline sequence<T> except(const sequence<T> &other, Comp comp=Comp());

   template<class Comp=std::less<T>>
   inline sequence<T> symmetric_difference(const sequence<T> &other, Comp comp=Comp());

   template<class Container>
   static inline sequence<T> from(Container & c) {
      return from(std::begin(c), std::end(c));
   }

   static inline sequence<T> from(std::initializer_list<T> l) {
      return sequence<T>([l](sink_type &put) {
            std::for_each(l.begin(), l.end(), std::ref(put));
         });
   }

   template<class InputIterator>
   static inline sequence<T> from(InputIterator b, InputIterator e) {
      return sequence<T>([b, e](sink_type &put) {
            std::for_each(b, e, std::ref(put));
         });
   }

   template<class Generator>
   static inline sequence<T> generate(Generator generate, std::size_t n) {
      return sequence<T>([generate, n](sink_type &put) {
            for (std::size_t i = 0; i < n; ++i) {
               put(generate());
            }
         });
   }

   static inline sequence<T> range(T start, T finish, T delta=1) {
      check_delta(delta, std::is_signed<T>());

      if (start < finish) {
         return sequence<T>([start, finish, delta](sink_type &put) {
               for (T i = start; i < finish; i+=delta) {
                  put(i);
               }
            });
      }
      else if (finish < start) {
         return sequence<T>([start, finish, delta](sink_type &put) {
               for (T i = start; i > finish; i-=delta) {
                  put(i);
               }
            });
      }

      return empty_sequence();
   }

   static inline sequence<T> empty_sequence() {
      return sequence<T>([](sink_type &){});
   }

private:
   static inline void check_delta(T delta, std::true_type) {
      if (!(0 < delta)) {
         throw std::domain_error("Delta must be positive.");
      }
   }

   static inline void check_delta(T, std::false_type) noexcept {
   }

   std::shared_ptr<coro_t> coro;
};


template<class T>
class sequence_sink_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
public:
   typedef sequence<T> sequence_type;
   typedef typename sequence_type::sink_type sink_type;
   class reference {
   public:
      explicit inline reference(sink_type &s) :
         sink(s)
      {
      }

      inline reference &operator =(const T &t) {
         sink(t);
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

   inline sequence_sink_iterator<T> &operator++() {
      return *this;
   }

   inline sequence_sink_iterator<T> &operator++(int) {
      return *this;
   }

private:
   sink_type &sink;
};


template<class T>
template<class Comp>
inline sequence<T> sequence<T>::union_with(const sequence<T> &other, Comp comp) {
   auto l_co = coro;
   auto r_co = other.coro;

   return sequence<T>([l_co, r_co, comp](sink_type &sink) {
         std::set_union(boost::begin(*l_co), boost::end(*l_co),
                        boost::begin(*r_co), boost::end(*r_co),
                        sequence_sink_iterator<T>(sink));
      });
}


template<class T>
template<class Comp>
inline sequence<T> sequence<T>::intersect_with(const sequence<T> &other, Comp comp) {
   auto l_co = coro;
   auto r_co = other.coro;

   return sequence<T>([l_co, r_co, comp](sink_type &sink) {
         std::set_intersection(boost::begin(*l_co), boost::end(*l_co),
                               boost::begin(*r_co), boost::end(*r_co),
                               sequence_sink_iterator<T>(sink));
      });
}


template<class T>
template<class Comp>
inline sequence<T> sequence<T>::except(const sequence<T> &other, Comp comp) {
   auto l_co = coro;
   auto r_co = other.coro;

   return sequence<T>([l_co, r_co, comp](sink_type &sink) {
         std::set_difference(boost::begin(*l_co), boost::end(*l_co),
                             boost::begin(*r_co), boost::end(*r_co),
                             sequence_sink_iterator<T>(sink));
      });
}


template<class T>
template<class Comp>
inline sequence<T> sequence<T>::symmetric_difference(const sequence<T> &other, Comp comp) {
   auto l_co = coro;
   auto r_co = other.coro;

   return sequence<T>([l_co, r_co, comp](sink_type &sink) {
         std::set_symmetric_difference(boost::begin(*l_co), boost::end(*l_co),
                                       boost::begin(*r_co), boost::end(*r_co),
                                       sequence_sink_iterator<T>(sink));
      });
}


template<class T>
template<class Alloc>
inline sequence<T> sequence<T>::reverse(std::size_t reserve, Alloc alloc) {
   auto co = coro;
   return sequence<T>([co, reserve, alloc](sink_type &put) {
         std::vector<T, Alloc> v(alloc);
         v.reserve(reserve);
         v.insert(v.end(), boost::begin(*co), boost::end(*co));
         std::copy(v.rbegin(), v.rend(), sequence_sink_iterator<T>(put));
      });
}

#endif
