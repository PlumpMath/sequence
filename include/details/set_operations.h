#ifndef SEQUENCE_SET_OPERATIONS_H__
#define SEQUENCE_SET_OPERATIONS_H__

#ifndef SEQUENCING_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class T, class Comp=std::less<void>>
inline auto union_with(sequence<T> r, Comp comp={}) {
   using std::begin;
   using std::end;
   using std::move;
   using std::set_union;

   return sequence_manipulator([rhs=move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=move(lhs), r_=move(rhs), comp](auto &yield) mutable {
               set_union(begin(l_), end(l_), begin(r_), end(r_), sink_iterator(yield), comp);
            };
         return sequence<T>{move(f)};
      });
}


template<class T, class Comp=std::less<void>>
inline auto intersect_with(sequence<T> r, Comp comp={}) {
   using std::begin;
   using std::end;
   using std::move;
   using std::set_intersection;

   return sequence_manipulator([rhs=move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=move(lhs), r_=move(rhs), comp](auto &yield) mutable {
               set_intersection(begin(l_), end(l_), begin(r_), end(r_), sink_iterator(yield), comp);
            };
         return sequence<T>{move(f)};
      });
}


template<class T, class Comp=std::less<void>>
inline auto except(sequence<T> r, Comp comp={}) {
   using std::begin;
   using std::end;
   using std::move;
   using std::set_difference;

   return sequence_manipulator([rhs=move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=move(lhs), r_=move(rhs), comp](auto &yield) mutable {
               set_difference(begin(l_), end(l_), begin(r_), end(r_), sink_iterator(yield), comp);
            };
         return sequence<T>{move(f)};
      });
}


template<class T, class Comp=std::less<void>>
inline auto symmetric_difference(sequence<T> r, Comp comp={}) {
   using std::begin;
   using std::end;
   using std::move;
   using std::set_symmetric_difference;

   return sequence_manipulator([rhs=move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=move(lhs), r_=move(rhs), comp](auto &yield) mutable {
               set_symmetric_difference(begin(l_), end(l_), begin(r_), end(r_), sink_iterator(yield), comp);
            };
         return sequence<T>{move(f)};
      });
}

#endif
