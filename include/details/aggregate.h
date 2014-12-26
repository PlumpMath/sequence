#ifndef SEQUENCING_AGGREGATE_H__
#define SEQUENCING_AGGREGATE_H__

#ifndef SEQUENCING_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


inline auto count() {
   using std::begin;
   using std::end;
   using std::distance;
   return sequence_manipulator([](sequence<auto> s) mutable {
         return static_cast<std::size_t>(distance(begin(s), end(s)));
      });
}


template<class Predicate>
inline auto count(Predicate p) {
   using std::begin;
   using std::end;
   using std::count_if;
   return sequence_manipulator([=](sequence<auto> s) mutable {
         return count_if(begin(s), end(s), p);
      });
}


template<class Comp=std::less<void>>
inline auto max(Comp &&comp={}) {
   using std::begin;
   using std::end;
   using std::max_element;

   return sequence_manipulator([comp](sequence<auto> s) mutable {
         auto e = end(s);
         auto i = max_element(begin(s), e, comp);
         if (i == e) {
            throw std::range_error("Max cannot be computed on empty sequence.");
         }

         return *i;
      });
}


template<class Comp=std::less<void>>
inline auto min(Comp &&comp={}) {
   using std::begin;
   using std::end;
   using std::min_element;

   return sequence_manipulator([comp](sequence<auto> s) mutable {
         auto e = end(s);
         auto i = min_element(begin(s), e, comp);
         if (i == e) {
            throw std::range_error("Min cannot be computed on empty sequence.");
         }

         return *i;
      });
}


template<class Comp=std::less<void>>
inline auto minmax(Comp &&comp={}) {
   using std::begin;
   using std::end;
   using std::make_pair;

   return sequence_manipulator([comp](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         auto i = begin(s);
         auto e = end(s);
         if (i == e) {
            throw std::range_error("Min/Max cannot be computed on empty sequence.");
         }

         S min_result{*i};
         S max_result{*i};
         for (++i; i != e; ++i) {
            if (comp(*i, min_result)) {
               min_result = *i;
            }
            else if (comp(max_result, *i)) {
               max_result = *i;
            }
         }

         return make_pair(min_result, max_result);
      });
}


template<class T, class Add=std::plus<void>>
inline auto sum(T init={}, Add &&add=Add{}) {
   using std::begin;
   using std::end;
   using std::accumulate;

   return sequence_manipulator([=](sequence<auto> s) mutable {
         return accumulate(begin(s), end(s), init, add);
      });
}


template<class Add=std::plus<void>, class Divide=std::divides<void>>
inline auto avg(Add &&add={}, Divide &&divide={}) {
   using std::begin;
   using std::end;

   return sequence_manipulator([=](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         auto i = begin(s);
         auto e = end(s);
         if (i == e) {
            throw std::domain_error("Cannot compute average on empty sequence.");
         }

         S den{1};
         S num{*i};
         for (++i; i != e; ++i) {
            num = add(num, *i);
            den += S{1};
         }
         return divide(num, den);
      });
}


template<class T, class R, class Add=std::plus<void>, class Multiply=std::multiplies<void>>
inline auto inner_product(sequence<R> r, T init={}, Add &&add={}, Multiply &&multiply={}) {
   using std::begin;
   using std::end;
   using std::inner_product;
   using std::move;

   return sequence_manipulator([r=move(r), init, add, multiply](sequence<auto> l) mutable {
         return inner_product(begin(l), end(l), begin(r), init, add, multiply);
      });
}

#endif
