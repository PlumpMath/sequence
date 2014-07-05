#ifndef SEQUENCE_CONTAINER_H__
#define SEQUENCE_CONTAINER_H__

#ifndef SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


namespace details_ {

template<class T>
inline void check_delta(T delta, std::true_type) {
   if (!(0 < delta)) {
      throw std::domain_error("Delta must be positive.");
   }
}


template<class T>
inline void check_delta(T, std::false_type) noexcept {
}

}


inline auto empty() {
   return sequence_manipulator([](sequence<auto> s) mutable { return s.empty; });
}


template<class T>
inline auto contains(const T &t) {
   return sequence_manipulator([t](sequence<auto> s) mutable {
         auto e = s.end();
         return std::find(s.begin(), e, t) != e;
      });
}


template<class InputIterator>
inline sequence<typename std::iterator_traits<InputIterator>::value_type> from(InputIterator b, InputIterator e) {
   typedef sequence<typename std::iterator_traits<InputIterator>::value_type> sequence_type;

   return sequence_type([=](auto &yield) mutable {
         std::for_each(b, e, std::ref(yield));
      });
}


template<class Container>
inline sequence<typename Container::value_type> from(Container const &c) {
   return from(std::begin(c), std::end(c));
}


template<class T>
inline sequence<T> from(std::initializer_list<T> c) {
   // Not entirely sure if we should be moving c to the stack or not.
#if 0
   typedef typename sequence<T>::sink_type sink_type;
   return sequence<T>([=](auto &yield) {
         std::for_each(std::begin(c), std::end(c), std::ref(yield));
      });
#else
   return from(std::begin(c), std::end(c));
#endif
}


inline sequence<char> from(const char *c) {
   return sequence<char>([c] (auto &yield) mutable {
         for (; *c; ++c) {
            yield(*c);
         }
      });
}


inline sequence<wchar_t> from(const wchar_t *c) {
   return sequence<wchar_t>([c] (auto &yield) mutable {
         for (; *c; ++c) {
            yield(*c);
         }
      });
}


template<class Generator>
static inline sequence<typename std::result_of<Generator()>::type> generate(Generator generate, std::size_t n) {
   typedef sequence<typename std::result_of<Generator()>::type> sequence_type;

   return sequence_type([=](auto &yield) mutable {
         for (std::size_t i = 0; i < n; ++i) {
            yield(generate());
         }
      });
}


template<class T>
inline sequence<T> range(T start, T finish, T delta=1) {
   details_::check_delta(delta, std::is_signed<T>());

   if (start < finish) {
      return sequence<T>([=](auto &yield) mutable {
            for (T i = start; i < finish; i += delta) {
               yield(i);
            }
         });
   }
   else if (finish < start) {
      return sequence<T>([=](auto &yield) mutable {
            for (T i = start; i > finish; i -= delta) {
               yield(i);
            }
         });
   }

   return sequence<T>{};
}


template<class R>
inline auto zip_with(sequence<R> rhs) {
   return sequence_manipulator([r_=std::move(rhs)](sequence<auto> l) mutable {
         typedef typename decltype(l)::value_type L;
         return sequence<std::pair<L, R>>([l=std::move(l), r=std::move(r_)](auto &yield) mutable {
               auto li = l.begin();
               auto le = l.end();
               auto ri = r.begin();
               auto re = r.end();

               while (li != le && ri != re) {
                  yield({ *li++, *ri++ });
               }
            });
      });
}


enum class pairwise_capture {
   use_remainder,
   ignore_remainder
};


inline auto pairwise(pairwise_capture capture=pairwise_capture::ignore_remainder) {
   return sequence_manipulator([=](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         return sequence<std::pair<S, S>>{[s=std::move(s), capture=capture](auto &yield) mutable {
               auto i = s.begin();
               auto e = s.end();

               while (i != e) {
                  S first{ *i++ };
                  if (i != e) {
                     yield({ std::forward<S>(first), *i++ });
                  }
                  else if (capture == pairwise_capture::use_remainder) {
                     yield({ std::forward<S>(first), S{} });
                  }
               }
            }};
      });
}


template<class T>
inline auto concat(sequence<T> rhs) {
   return sequence_manipulator([r_=std::move(rhs)](sequence<T> l_) mutable {
         auto f = [r=std::move(r_), l=std::move(l_)](auto &yield) mutable {
               std::copy(l.begin(), l.end(), sink_iterator(yield));
               std::copy(r.begin(), r.end(), sink_iterator(yield));
            };
         return sequence<T>{std::move(f)};
      });
}

#endif
