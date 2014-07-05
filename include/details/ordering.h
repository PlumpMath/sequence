#ifndef SEQUENCE_ORDERING_H__
#define SEQUENCE_ORDERING_H__

#ifndef SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class Comp=std::less<void>, class Alloc=std::allocator<void>>
inline auto sort(std::size_t reserve=0, Comp comp={}, Alloc alloc={}) {
   return sequence_manipulator([=](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;
         typedef sequence<S> sequence_type;

         return sequence_type{[=, s=std::move(s)](auto &yield) mutable {
            typedef typename Alloc::template rebind<S>::other v_alloc;

            std::vector<S, v_alloc> v{v_alloc{alloc}};
            v.reserve(reserve);
            std::copy(s.begin(), s.end(), std::back_inserter(v));
            std::stable_sort(v.begin(), v.end(), comp);
            std::for_each(v.begin(), v.end(), std::ref(yield));
         }};
      });
}


template<class Alloc=std::allocator<void>>
inline auto reverse(std::size_t reserve=0, Alloc alloc={}) {
   return sequence_manipulator([=](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         return sequence<S>{[=, s=std::move(s)](auto &yield) mutable {
               typedef typename Alloc::template rebind<S>::other v_alloc;

               std::vector<S, v_alloc> v{v_alloc{alloc}};
               v.reserve(reserve);
               std::copy(s.begin(), s.end(), std::back_inserter(v));
               std::for_each(v.rbegin(), v.rend(), std::ref(yield));
            }};
      });
}

#endif
