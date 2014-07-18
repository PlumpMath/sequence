#ifndef SEQUENCE_ORDERING_H__
#define SEQUENCE_ORDERING_H__

#ifndef _CXXSTD_EXPERIMENTAL_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class Comp=less<void>, class Alloc=allocator<void>>
inline auto sort(size_t reserve=0, Comp comp={}, Alloc alloc={}) {
   return sequence_manipulator([=](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;
         typedef sequence<S> sequence_type;
         typedef typename Alloc::template rebind<S>::other v_alloc;

         vector<S, v_alloc> v{v_alloc{alloc}};
         v.reserve(reserve);
         copy(s.begin(), s.end(), back_inserter(v));
         stable_sort(v.begin(), v.end(), comp);

         return sequence_type{[v=move(v)](auto &yield) mutable {
            for_each(v.begin(), v.end(), ref(yield));
         }};
      });
}


template<class Alloc=allocator<void>>
inline auto reverse(size_t reserve=0, Alloc alloc={}) {
   return sequence_manipulator([=](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         return sequence<S>{[=, s=move(s)](auto &yield) mutable {
               typedef typename Alloc::template rebind<S>::other v_alloc;

               vector<S, v_alloc> v{v_alloc{alloc}};
               v.reserve(reserve);
               copy(s.begin(), s.end(), back_inserter(v));
               for_each(v.rbegin(), v.rend(), ref(yield));
            }};
      });
}

#endif
