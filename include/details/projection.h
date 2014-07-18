#ifndef SEQUENCE_PROJECTION_H__
#define SEQUENCE_PROJECTION_H__

#ifndef _CXXSTD_EXPERIMENTAL_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


namespace details_ {

template<class T, class Transform>
class select_many_helper {
   typedef decltype(declval<Transform>()(declval<T>())) transform_result_sequence;
   typedef decltype(declval<typename transform_result_sequence::value_type>()) raw_element_type;

public:
   typedef typename decay<raw_element_type>::type element_type;
   typedef sequence<element_type> sequence_type;
};


template<class T, class TSelector, class U, class USelector, class Combiner>
class join_helper {
   typedef decltype(declval<TSelector>()(declval<T>())) t_key_type;
   typedef decltype(declval<USelector>()(declval<U>())) u_key_type;

public:
   typedef typename common_type<t_key_type, u_key_type>::type key_type;
   typedef decltype(declval<Combiner>()(declval<T>(), declval<U>())) result_type;
};

}


template<class Transform>
inline auto select(Transform f) {
   return sequence_manipulator([f=move(f)](sequence<auto> s) mutable {
         typedef result_of_t<Transform(typename decltype(s)::value_type)> output_value;

         return sequence<output_value>{[f=move(f), s=move(s)](auto &yield) mutable {
               transform(s.begin(), s.end(), sink_iterator(yield), move(f));
            }};
      });
}


template<class Transform>
inline auto select_many(Transform transform) {
   return sequence_manipulator([t=move(transform)](sequence<auto> s) mutable {
         typedef typename details_::select_many_helper<typename decltype(s)::value_type,
                                                       Transform>::sequence_type sequence_type;

         return sequence_type{[s=move(s), transform=move(t)](auto &yield) mutable {
               for (const auto &s_value : s) {
                  for (auto out_value : transform(s_value)) {
                     yield(out_value);
                  }
               }
            }};
      });
}


template<class LSelector, class RSelector, class Combiner, class R, class Alloc=allocator<R>, class Comp=equal_to<void>>
inline auto join(sequence<R> r_, LSelector select_l, RSelector select_r, Combiner combine, size_t reserve, Alloc alloc={}, Comp comp={}) {
   return sequence_manipulator([r=move(r_), select_l, select_r, combine, comp, reserve, alloc](sequence<auto> lhs) mutable {
         typedef typename remove_reference_t<decltype(lhs)>::value_type L;
         typedef typename details_::join_helper<L, LSelector, R, RSelector, Combiner>::result_type result_type;

         vector<R, Alloc> rvec(move(alloc));
         rvec.reserve(reserve);
         copy(r.begin(), r.end(), back_inserter(rvec));

         auto f = [rvec=move(rvec), l=move(lhs), select_l, select_r, combine, comp](auto &yield) mutable {
            for (const auto &l_element : l) {
               for (const auto &r_element : rvec) {
                  if (comp(select_l(l_element), select_r(r_element))) {
                     yield(combine(l_element, r_element));
                  }
               }
            }
         };
         return sequence<result_type>{move(f)};
      });
}


template<class Apply>
inline auto for_each(Apply apply) {
   return sequence_manipulator([apply=move(apply)](sequence<auto> s) mutable {
         return for_each(s.begin(), s.end(), apply);
      });
}

#endif
