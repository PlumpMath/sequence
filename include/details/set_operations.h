#ifndef SEQUENCE_SET_OPERATIONS_H__
#define SEQUENCE_SET_OPERATIONS_H__

#ifndef _CXXSTD_EXPERIMENTAL_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class T, class Comp=less<void>>
inline auto union_with(sequence<T> r, Comp comp={}) {
   return sequence_manipulator([rhs=move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=move(lhs), r_=move(rhs), comp](auto &yield) mutable {
               set_union(l_.begin(), l_.end(), r_.begin(), r_.end(), sink_iterator(yield), comp);
            };
         return sequence<T>{move(f)};
      });
}


template<class T, class Comp=less<void>>
inline auto intersect_with(sequence<T> r, Comp comp={}) {
   return sequence_manipulator([rhs=move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=move(lhs), r_=move(rhs), comp](auto &yield) mutable {
               set_intersection(l_.begin(), l_.end(), r_.begin(), r_.end(), sink_iterator(yield), comp);
            };
         return sequence<T>{move(f)};
      });
}


template<class T, class Comp=less<void>>
inline auto except(sequence<T> r, Comp comp={}) {
   return sequence_manipulator([rhs=move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=move(lhs), r_=move(rhs), comp](auto &yield) mutable {
               set_difference(l_.begin(), l_.end(), r_.begin(), r_.end(), sink_iterator(yield), comp);
            };
         return sequence<T>{move(f)};
      });
}


template<class T, class Comp=less<void>>
inline auto symmetric_difference(sequence<T> r, Comp comp={}) {
   return sequence_manipulator([rhs=move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=move(lhs), r_=move(rhs), comp](auto &yield) mutable {
               set_symmetric_difference(l_.begin(), l_.end(), r_.begin(), r_.end(), sink_iterator(yield), comp);
            };
         return sequence<T>{move(f)};
      });
}

#endif
