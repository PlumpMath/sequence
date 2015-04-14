#ifndef SEQUENCE_SET_OPERATIONS_H__
#define SEQUENCE_SET_OPERATIONS_H__

#ifndef SEQUENCING_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class T, class Comp=std::less<void>, class Alloc=std::allocator<void>>
inline auto union_with(sequence<T> l, sequence<T> r, Comp comp={}, const Alloc &alloc={}) {
   using std::begin;
   using std::end;
   using std::move;
   using std::set_union;

   return sequence<T>{std::allocator_arg, alloc, [l=move(l), r=move(r), comp](auto &yield) mutable {
         set_union(begin(l), end(l), begin(r), end(r), sink_iterator(yield), comp);
      }};
}


template<class T, class Comp=std::less<void>, class Alloc=std::allocator<void>>
inline auto intersect_with(sequence<T> l, sequence<T> r, Comp comp={}, const Alloc &alloc={}) {
   using std::begin;
   using std::end;
   using std::move;
   using std::set_intersection;

   return sequence<T>{std::allocator_arg, alloc, [l=move(l), r=move(r), comp](auto &yield) mutable {
         set_intersection(begin(l), end(l), begin(r), end(r), sink_iterator(yield), comp);
      }};
}


template<class T, class Comp=std::less<void>, class Alloc=std::allocator<void>>
inline auto except(sequence<T> l, sequence<T> r, Comp comp={}, const Alloc &alloc={}) {
   using std::begin;
   using std::end;
   using std::move;
   using std::set_difference;

   return sequence<T>{std::allocator_arg, alloc, [l=move(l), r=move(r), comp](auto &yield) mutable {
         set_difference(begin(l), end(l), begin(r), end(r), sink_iterator(yield), comp);
      }};
}


template<class T, class Comp=std::less<void>, class Alloc=std::allocator<void>>
inline auto symmetric_difference(sequence<T> l, sequence<T> r, Comp comp={}, const Alloc &alloc={}) {
   using std::begin;
   using std::end;
   using std::move;
   using std::set_symmetric_difference;

   return sequence<T>{std::allocator_arg, alloc, [l=move(l), r=move(r), comp](auto &yield) mutable {
         set_symmetric_difference(begin(l), end(l), begin(r), end(r), sink_iterator(yield), comp);
      }};
}

#endif
