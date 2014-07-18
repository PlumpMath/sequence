#ifndef SEQUENCE_CONTAINER_H__
#define SEQUENCE_CONTAINER_H__

#ifndef _CXXSTD_EXPERIMENTAL_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


namespace details_ {

template<class T>
inline void check_delta(T delta, true_type) {
   if (!(0 < delta)) {
      throw domain_error("Delta must be positive.");
   }
}


template<class T>
inline void check_delta(T, false_type) noexcept {
}

}


inline auto empty() {
   return sequence_manipulator([](sequence<auto> s) mutable { return s.empty; });
}


template<class T>
inline auto contains(const T &t) {
   return sequence_manipulator([t](sequence<auto> s) mutable {
         auto e = s.end();
         return find(s.begin(), e, t) != e;
      });
}


template<class InputIterator>
inline sequence<typename iterator_traits<InputIterator>::value_type> from(InputIterator b, InputIterator e) {
   typedef sequence<typename iterator_traits<InputIterator>::value_type> sequence_type;

   return sequence_type([=](auto &yield) mutable {
         for_each(b, e, ref(yield));
      });
}


template<class Container>
inline sequence<typename Container::value_type> from(Container const &c) {
   return from(begin(c), end(c));
}


template<class T>
inline sequence<T> from(initializer_list<T> c) {
   return from(begin(c), end(c));
}


template<class T>
inline sequence<T> from(const T *c) {
   return sequence<T>([c] (auto &yield) mutable {
         for (; *c; ++c) {
            yield(*c);
         }
      });
}


template<class Generator>
static inline sequence<typename result_of<Generator()>::type> generate(Generator generate, size_t n) {
   typedef sequence<typename result_of<Generator()>::type> sequence_type;

   return sequence_type([=](auto &yield) mutable {
         for (size_t i = 0; i < n; ++i) {
            yield(generate());
         }
      });
}


template<class T>
inline sequence<T> range(T start, T finish, T delta=1) {
   details_::check_delta(delta, is_signed<T>());

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
   return sequence_manipulator([r_=move(rhs)](sequence<auto> l) mutable {
         typedef typename decltype(l)::value_type L;
         return sequence<pair<L, R>>([l=move(l), r=move(r_)](auto &yield) mutable {
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

         return sequence<pair<S, S>>{[s=move(s), capture=capture](auto &yield) mutable {
               auto i = s.begin();
               auto e = s.end();

               while (i != e) {
                  S first{ *i++ };
                  if (i != e) {
                     yield({ forward<S>(first), *i++ });
                  }
                  else if (capture == pairwise_capture::use_remainder) {
                     yield({ forward<S>(first), S{} });
                  }
               }
            }};
      });
}


template<class T>
inline auto concat(sequence<T> rhs) {
   return sequence_manipulator([r_=move(rhs)](sequence<T> l_) mutable {
         auto f = [r=move(r_), l=move(l_)](auto &yield) mutable {
               copy(l.begin(), l.end(), sink_iterator(yield));
               copy(r.begin(), r.end(), sink_iterator(yield));
            };
         return sequence<T>{move(f)};
      });
}

#endif
