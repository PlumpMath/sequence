#ifndef SEQUENCE_RESTRICTION_H__
#define SEQUENCE_RESTRICTION_H__

#ifndef _CXXSTD_EXPERIMENTAL_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class Predicate>
inline auto where(Predicate p) {
   return sequence_manipulator([=](sequence<auto> s) mutable {
         return decltype(s){[p, s=move(s)](auto &yield) mutable {
               for (const auto &element : s) {
                  if (p(element)) {
                     yield(element);
                  }
               }
            }};
      });
}

#endif
