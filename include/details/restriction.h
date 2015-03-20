#ifndef SEQUENCE_RESTRICTION_H__
#define SEQUENCE_RESTRICTION_H__

#ifndef SEQUENCING_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class Predicate, class Alloc=std::allocator<void>>
inline auto where(Predicate p, const Alloc &alloc={}) {
   using std::move;

   return sequence_manipulator([=](sequence<auto> s) mutable {
         return decltype(s){std::allocator_arg, alloc, [p, s=move(s)](auto &yield) mutable {
               for (const auto &element : s) {
                  if (p(element)) {
                     yield(element);
                  }
               }
            }};
      });
}

#endif
