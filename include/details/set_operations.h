#ifndef SEQUENCE_SET_OPERATIONS_H__
#define SEQUENCE_SET_OPERATIONS_H__

#ifndef SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class T, class Comp=std::less<void>>
inline auto union_with(sequence<T> r, Comp comp={}) {
   return sequence_manipulator([rhs=std::move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=std::move(lhs), r_=std::move(rhs), comp](auto &yield) mutable {
               std::set_union(l_.begin(), l_.end(), r_.begin(), r_.end(), sink_iterator(yield), comp);
            };
         return sequence<T>{std::move(f)};
      });
}


template<class T, class Comp=std::less<void>>
inline auto intersect_with(sequence<T> r, Comp comp={}) {
   return sequence_manipulator([rhs=std::move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=std::move(lhs), r_=std::move(rhs), comp](auto &yield) mutable {
               std::set_intersection(l_.begin(), l_.end(), r_.begin(), r_.end(), sink_iterator(yield), comp);
            };
         return sequence<T>{std::move(f)};
      });
}


template<class T, class Comp=std::less<void>>
inline auto except(sequence<T> r, Comp comp={}) {
   return sequence_manipulator([rhs=std::move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=std::move(lhs), r_=std::move(rhs), comp](auto &yield) mutable {
               std::set_difference(l_.begin(), l_.end(), r_.begin(), r_.end(), sink_iterator(yield), comp);
            };
         return sequence<T>{std::move(f)};
      });
}


template<class T, class Comp=std::less<void>>
inline auto symmetric_difference(sequence<T> r, Comp comp={}) {
   return sequence_manipulator([rhs=std::move(r), comp](sequence<T> lhs) mutable {
         auto f = [l_=std::move(lhs), r_=std::move(rhs), comp](auto &yield) mutable {
               std::set_symmetric_difference(l_.begin(), l_.end(), r_.begin(), r_.end(), sink_iterator(yield), comp);
            };
         return sequence<T>{std::move(f)};
      });
}

#endif
