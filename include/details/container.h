#ifndef SEQUENCE_CONTAINER_H__
#define SEQUENCE_CONTAINER_H__

#ifndef SEQUENCING_SEQUENCE_H__
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
   using std::begin;
   using std::end;
   using std::find;

   return sequence_manipulator([t](sequence<auto> s) mutable {
         auto e = end(s);
         return find(begin(s), e, t) != e;
      });
}


template<class InputIterator, class Alloc=std::allocator<void>>
inline sequence<typename std::iterator_traits<InputIterator>::value_type> from(InputIterator b, InputIterator e, const Alloc &alloc={}) {
   using std::for_each;
   using std::ref;

   typedef sequence<typename std::iterator_traits<InputIterator>::value_type> sequence_type;

   return sequence_type{std::allocator_arg, alloc, [=, i=b](auto &yield) mutable {
         for_each(b, e, ref(yield));
      }};
}


template<class Container>
inline sequence<typename Container::value_type> from(Container const &c) {
   using std::begin;
   using std::end;

   return from(begin(c), end(c), c.get_allocator());
}


template<class Container>
inline sequence<typename Container::value_type> from(Container &&c) {
   using std::begin;
   using std::end;

   return from(begin(c), end(c), c.get_allocator());
}


template<class T, class Alloc=std::allocator<void>>
inline sequence<T> from(std::initializer_list<T> c, const Alloc &alloc={}) {
   using std::begin;
   using std::end;

   return from(begin(c), end(c), alloc);
}


template<class T, class Alloc=std::allocator<void>>
inline sequence<T> from(const T *c, const Alloc &alloc={}) {
   return sequence<T>(std::allocator_arg, alloc, [c] (auto &yield) mutable {
         for (; *c; ++c) {
            yield(*c);
         }
      });
}


template<class Generator, class Alloc=std::allocator<void>>
static inline sequence<std::result_of_t<Generator()>> generate(Generator generate, std::size_t n, const Alloc &alloc={}) {
   typedef sequence<std::result_of_t<Generator()>> sequence_type;

   return sequence_type(std::allocator_arg, alloc, [=](auto &yield) mutable {
         for (std::size_t i = 0; i < n; ++i) {
            yield(generate());
         }
      });
}


template<class T, class Alloc=std::allocator<void>>
inline sequence<T> range(T start, T finish, T delta=1, const Alloc &alloc={}) {
   details_::check_delta(delta, std::is_signed<T>{});

   if (start < finish) {
      return sequence<T>(std::allocator_arg, alloc, [=](auto &yield) mutable {
            for (T i = start; i < finish; i += delta) {
               yield(i);
            }
         });
   }
   else if (finish < start) {
      return sequence<T>(std::allocator_arg, alloc, [=](auto &yield) mutable {
            for (T i = start; i > finish; i -= delta) {
               yield(i);
            }
         });
   }

   return sequence<T>{};
}


template<class R, class Alloc=std::allocator<void>>
inline auto zip_with(sequence<R> rhs, const Alloc &alloc={}) {
   using std::begin;
   using std::end;
   using std::move;

   return sequence_manipulator([alloc, r_=move(rhs)](sequence<auto> l) mutable {
         typedef typename decltype(l)::value_type L;
         return sequence<std::pair<L, R>>(std::allocator_arg, alloc, [l=move(l), r=move(r_)](auto &yield) mutable {
               auto li = begin(l);
               auto le = end(l);
               auto ri = begin(r);
               auto re = end(r);

               while (li != le && ri != re) {
                  yield({ *li, *ri });
                  ++li;
                  ++ri;
               }
            });
      });
}


enum class pairwise_capture {
   use_remainder,
   ignore_remainder
};


template<class Alloc=std::allocator<void>>
inline auto pairwise(pairwise_capture capture=pairwise_capture::ignore_remainder, const Alloc &alloc={}) {
   using std::begin;
   using std::end;
   using std::forward;
   using std::move;

   return sequence_manipulator([=](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         return sequence<std::pair<S, S>>{std::allocator_arg, alloc, [s=move(s), capture=capture](auto &yield) mutable {
               auto i = begin(s);
               auto e = end(s);

               while (i != e) {
                  S first{ *i };
                  ++i;
                  if (i != e) {
                     yield({ forward<S>(first), *i });
                     ++i;
                  }
                  else if (capture == pairwise_capture::use_remainder) {
                     yield({ forward<S>(first), S{} });
                  }
               }
            }};
      });
}


template<class T, class Alloc=std::allocator<void>>
inline auto concat(sequence<T> rhs, const Alloc &alloc={}) {
   using std::begin;
   using std::end;
   using std::move;

   return sequence_manipulator([alloc, r_=move(rhs)](sequence<T> l_) mutable {
         auto f = [r=move(r_), l=move(l_)](auto &yield) mutable {
               copy(begin(l), end(l), sink_iterator(yield));
               copy(begin(r), end(r), sink_iterator(yield));
            };
         return sequence<T>{std::allocator_arg, alloc, move(f)};
      });
}

#endif
