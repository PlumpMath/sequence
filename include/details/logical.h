#ifndef SEQUENCE_LOGICAL_H__
#define SEQUENCE_LOGICAL_H__

#ifndef SEQUENCING_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class Predicate>
inline auto all(Predicate p) {
   using std::begin;
   using std::end;
   using std::all_of;

   return sequence_manipulator([p](sequence<auto> s) {
         return all_of(begin(s), end(s), p);
      });
}


template<class Predicate>
inline auto any(Predicate p) {
   using std::begin;
   using std::end;
   using std::any_of;

   return sequence_manipulator([p](sequence<auto> s) {
         return any_of(begin(s), end(s), p);
      });
}


template<class Predicate>
inline auto none(Predicate p) {
   using std::begin;
   using std::end;
   using std::none_of;

   return sequence_manipulator([p](sequence<auto> s) {
         return none_of(begin(s), end(s), p);
      });
}

#endif
