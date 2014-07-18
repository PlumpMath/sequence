#ifndef SEQUENCE_LOGICAL_H__
#define SEQUENCE_LOGICAL_H__

#ifndef _CXXSTD_EXPERIMENTAL_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class Predicate>
inline auto all(Predicate p) {
   return sequence_manipulator([p](sequence<auto> s) {
         return all_of(s.begin(), s.end(), p);
      });
}


template<class Predicate>
inline auto any(Predicate p) {
   return sequence_manipulator([p](sequence<auto> s) {
         return any_of(s.begin(), s.end(), p);
      });
}


template<class Predicate>
inline auto none(Predicate p) {
   return sequence_manipulator([p](sequence<auto> s) {
         return none_of(s.begin(), s.end(), p);
      });
}

#endif
