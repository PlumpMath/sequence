#ifndef SEQUENCE_PROJECTION_H__
#define SEQUENCE_PROJECTION_H__

#ifndef SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


namespace details_ {

template<class T, class Transform>
class select_many_helper {
   typedef decltype(std::declval<Transform>()(std::declval<T>())) transform_result_sequence;
   typedef decltype(std::declval<typename transform_result_sequence::value_type>()) raw_element_type;

public:
   typedef typename std::decay<raw_element_type>::type element_type;
   typedef sequence<element_type> sequence_type;
};


template<class T, class TSelector, class U, class USelector, class Combiner>
class join_helper {
   typedef decltype(std::declval<TSelector>()(std::declval<T>())) t_key_type;
   typedef decltype(std::declval<USelector>()(std::declval<U>())) u_key_type;

public:
   typedef typename std::common_type<t_key_type, u_key_type>::type key_type;
   typedef decltype(std::declval<Combiner>()(std::declval<T>(), std::declval<U>())) result_type;
};

}


template<class Transform>
inline auto select(Transform transform) {
   return sequence_manipulator([transform=std::move(transform)](sequence<auto> s) mutable {
         typedef std::result_of_t<Transform(typename decltype(s)::value_type)> output_value;

         return sequence<output_value>{[transform=std::move(transform), s=std::move(s)](auto &yield) mutable {
               std::transform(s.begin(), s.end(), sink_iterator(yield), std::move(transform));
            }};
      });
}


template<class Transform>
inline auto select_many(Transform transform) {
   return sequence_manipulator([t=std::move(transform)](sequence<auto> s) mutable {
         typedef typename details_::select_many_helper<typename decltype(s)::value_type,
                                                       Transform>::sequence_type sequence_type;

         return sequence_type{[s=std::move(s), transform=std::move(t)](auto &yield) mutable {
               for (const auto &s_value : s) {
                  for (auto out_value : transform(s_value)) {
                     yield(out_value);
                  }
               }
            }};
      });
}


template<class LSelector, class RSelector, class Combiner, class R, class Alloc=std::allocator<R>, class Comp=std::equal_to<void>>
inline auto join(sequence<R> r_, LSelector select_l, RSelector select_r, Combiner combine, std::size_t reserve=0, Alloc alloc={}, Comp comp={}) {
   return sequence_manipulator([=, rhs=std::move(r_)](sequence<auto> lhs) mutable {
         typedef typename std::remove_reference_t<decltype(lhs)>::value_type L;
         typedef typename details_::join_helper<L, LSelector, R, RSelector, Combiner>::result_type result_type;

         auto f = [=, l=std::move(lhs), r=std::move(rhs)](auto &yield) mutable {
            std::vector<R, Alloc> rvec{std::forward<Alloc>(alloc)};
            rvec.reserve(reserve);
            std::copy(r.begin(), r.end(), std::back_inserter(rvec));

            for (const auto &l_element : l) {
               for (const auto &r_element : rvec) {
                  if (comp(select_l(l_element), select_r(r_element))) {
                     yield(combine(l_element, r_element));
                  }
               }
            }
         };
         return sequence<result_type>{std::move(f)};
      });
}

#endif
