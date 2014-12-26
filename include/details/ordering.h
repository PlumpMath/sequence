#ifndef SEQUENCE_ORDERING_H__
#define SEQUENCE_ORDERING_H__

#ifndef SEQUENCING_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class Comp=std::less<void>, class Alloc=std::allocator<void>>
inline auto sort(std::size_t reserve=0, Comp comp={}, Alloc alloc={}) {
   using std::begin;
   using std::end;
   using std::back_inserter;
   using std::copy;
   using std::for_each;
   using std::move;
   using std::ref;
   using std::stable_sort;

   return sequence_manipulator([=](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;
         typedef sequence<S> sequence_type;
         typedef typename Alloc::template rebind<S>::other v_alloc;

         std::vector<S, v_alloc> v{v_alloc{alloc}};
         v.reserve(reserve);
         copy(begin(s), end(s), back_inserter(v));
         stable_sort(begin(v), end(v), comp);

         return sequence_type{[v=move(v)](auto &yield) mutable {
            for_each(begin(v), end(v), ref(yield));
         }};
      });
}


template<class Alloc=std::allocator<void>>
inline auto reverse(std::size_t reserve=0, Alloc alloc={}) {
   using std::begin;
   using std::end;
   using std::back_inserter;
   using std::copy;
   using std::for_each;
   using std::move;
   using std::ref;

   return sequence_manipulator([=](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         return sequence<S>{[=, s=move(s)](auto &yield) mutable {
               typedef typename Alloc::template rebind<S>::other v_alloc;

               std::vector<S, v_alloc> v{v_alloc{alloc}};
               v.reserve(reserve);
               copy(begin(s), end(s), back_inserter(v));
               for_each(v.rbegin(), v.rend(), ref(yield));
            }};
      });
}

#endif
