#ifndef SEQUENCE_AGGREGATE_H__
#define SEQUENCE_AGGREGATE_H__

#ifndef SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


inline auto count() {
   return sequence_manipulator([](sequence<auto> s) mutable {
         return static_cast<std::size_t>(std::distance(s.begin(), s.end()));
      });
}


template<class Predicate>
inline auto count(Predicate p) {
   return sequence_manipulator([=](sequence<auto> s) mutable {
         return std::count_if(s.begin(), s.end(), p);
      });
}


template<class Comp=std::less<void>>
inline auto max(Comp &&comp={}) {
   return sequence_manipulator([comp](sequence<auto> s) mutable {
         auto e = s.end();
         auto i = std::max_element(s.begin(), e, comp);
         if (i == e) {
            throw std::range_error("Max cannot be computed on empty sequence.");
         }

         return *i;
      });
}


template<class Comp=std::less<void>>
inline auto min(Comp &&comp={}) {
   return sequence_manipulator([comp](sequence<auto> s) mutable {
         auto e = s.end();
         auto i = std::min_element(s.begin(), e, comp);
         if (i == e) {
            throw std::range_error("Min cannot be computed on empty sequence.");
         }

         return *i;
      });
}


template<class Comp=std::less<void>>
inline auto minmax(Comp &&comp={}) {
   return sequence_manipulator([comp](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         auto i = s.begin();
         auto e = s.end();
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

         return std::make_pair(min_result, max_result);
      });
}


template<class T, class Add=std::plus<void>>
inline auto sum(T init={}, Add &&add=Add{}) {
   return sequence_manipulator([=](sequence<auto> s) mutable {
         return std::accumulate(s.begin(), s.end(), init, add);
      });
}


template<class Add=std::plus<void>, class Divide=std::divides<void>>
inline auto avg(Add &&add={}, Divide &&divide={}) {
   return sequence_manipulator([=](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         auto i = s.begin();
         auto e = s.end();
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
   return sequence_manipulator([r=std::move(r), init, add, multiply](sequence<auto> l) mutable {
         return std::inner_product(l.begin(), l.end(), r.begin(), init, add, multiply);
      });
}

#endif
