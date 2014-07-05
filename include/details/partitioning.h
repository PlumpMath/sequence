#ifndef SEQUENCE_PARTIONING_H__
#define SEQUENCE_PARTIONING_H__

#ifndef SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


inline auto take(std::size_t n) {
   return sequence_manipulator([n](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         return sequence<S>{[s=std::move(s), n](auto &yield) mutable {
               for (const S &element : s) {
                  if (n-- == 0) {
                     break;
                  }
                  yield(element);
               }
            }};
      });
}


template<class Predicate>
inline auto take_while(Predicate predicate) {
   return sequence_manipulator([p=predicate](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         return sequence<S>{[s=std::move(s), p](auto &yield) mutable {
               for (const S &element : s) {
                  if (!p(element)) {
                     break;
                  }
                  yield(element);
               }
            }};
      });
}


inline auto skip(std::size_t n) {
   return sequence_manipulator([n](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         return sequence<S>{[s=std::move(s), n](auto &yield) mutable {
               auto i = s.begin();
               auto e = s.end();

               for (; i != e && n > 0; ++i) {
                  --n;
               }

               for (; i != e; ++i) {
                  yield(*i);
               }
            }};
      });
}


template<class Predicate>
inline auto skip_while(Predicate predicate) {
   return sequence_manipulator([p=predicate](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         return sequence<S>{[s=std::move(s), p](auto &yield) mutable {
               auto i = s.begin();
               auto e = s.end();

               for (; i != e; ++i) {
                  if (!p(*i)) {
                     break;
                  }
               }

               for (; i != e; ++i) {
                  yield(*i);
               }
            }};
      });
}


inline auto page(std::size_t page_index, std::size_t page_size) {
   return sequence_manipulator([=](sequence<auto> s) mutable {
         // Effective result: return s >> skip(page_index * page_size) >> take(page_size);
         // This should be a little more efficient due to not having to create a
         // separate stack to execute.
         typedef typename decltype(s)::value_type S;

         return sequence<S>{[s=std::move(s), skip_n=page_index * page_size, take_n=page_size](auto &yield) mutable {
               for (const S &element : s) {
                  if (skip_n-- == 0) {
                     break;
                  }
               }
               for (const S &element : s) {
                  if (take_n-- == 0) {
                     break;
                  }
                  yield(element);
               }
            }};
      });
}

#endif
