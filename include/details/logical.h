#ifndef SEQUENCE_LOGICAL_H__
#define SEQUENCE_LOGICAL_H__

#ifndef SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class Predicate>
inline auto all(Predicate p) {
   return sequence_manipulator([p](sequence<auto> s) {
         return std::all_of(s.begin(), s.end(), p);
      });
}


template<class Predicate>
inline auto any(Predicate p) {
   return sequence_manipulator([p](sequence<auto> s) {
         return std::any_of(s.begin(), s.end(), p);
      });
}


template<class Predicate>
inline auto none(Predicate p) {
   return sequence_manipulator([p](sequence<auto> s) {
         return std::none_of(s.begin(), s.end(), p);
      });
}

#endif
