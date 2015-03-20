#ifndef SEQUENCE_PROJECTION_H__
#define SEQUENCE_PROJECTION_H__

#ifndef SEQUENCING_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


namespace details_ {

template<class T, class Transform>
class select_many_helper {
   typedef decltype(std::declval<Transform>()(std::declval<T>())) transform_result_sequence;
   typedef decltype(std::declval<typename transform_result_sequence::value_type>()) raw_element_type;

public:
   typedef std::decay_t<raw_element_type> element_type;
   typedef sequence<element_type> sequence_type;
};


template<class T, class TSelector, class U, class USelector, class Combiner>
class join_helper {
   typedef decltype(std::declval<TSelector>()(std::declval<T>())) t_key_type;
   typedef decltype(std::declval<USelector>()(std::declval<U>())) u_key_type;

public:
   typedef std::common_type_t<t_key_type, u_key_type> key_type;
   typedef decltype(std::declval<Combiner>()(std::declval<T>(), std::declval<U>())) result_type;
};

}


template<class Transform, class Alloc=std::allocator<void>>
inline auto select(Transform f, const Alloc &alloc={}) {
   using std::begin;
   using std::end;
   using std::move;
   using std::transform;

   return sequence_manipulator([alloc, f=move(f)](sequence<auto> s) mutable {
         typedef std::result_of_t<Transform(typename decltype(s)::value_type)> output_value;

         return sequence<output_value>{std::allocator_arg, alloc, [f=move(f), s=move(s)](auto &yield) mutable {
               transform(begin(s), end(s), sink_iterator(yield), move(f));
            }};
      });
}


template<class Transform, class Alloc=std::allocator<void>>
inline auto select_many(Transform transform, const Alloc &alloc={}) {
   using std::move;

   return sequence_manipulator([alloc, t=move(transform)](sequence<auto> s) mutable {
         typedef typename details_::select_many_helper<typename decltype(s)::value_type,
                                                       Transform>::sequence_type sequence_type;

         return sequence_type{std::allocator_arg, alloc, [s=move(s), transform=move(t)](auto &yield) mutable {
               for (const auto &s_value : s) {
                  for (auto out_value : transform(s_value)) {
                     yield(out_value);
                  }
               }
            }};
      });
}


template<class LSelector, class RSelector, class Combiner, class R, class Alloc=std::allocator<R>, class Comp=std::equal_to<void>>
inline auto join(sequence<R> r_, LSelector select_l, RSelector select_r, Combiner combine, std::size_t reserve, const Alloc &alloc={}, Comp comp={}) {
   using std::begin;
   using std::end;
   using std::copy;
   using std::move;

   return sequence_manipulator([r=move(r_), select_l, select_r, combine, comp, reserve, alloc](sequence<auto> lhs) mutable {
         typedef typename std::remove_reference_t<decltype(lhs)>::value_type L;
         typedef typename details_::join_helper<L, LSelector, R, RSelector, Combiner>::result_type result_type;

         std::vector<R, Alloc> rvec(move(alloc));
         rvec.reserve(reserve);
         copy(begin(r), end(r), back_inserter(rvec));

         auto f = [rvec=move(rvec), l=move(lhs), select_l, select_r, combine, comp](auto &yield) mutable {
            for (const auto &l_element : l) {
               for (const auto &r_element : rvec) {
                  if (comp(select_l(l_element), select_r(r_element))) {
                     yield(combine(l_element, r_element));
                  }
               }
            }
         };
         return sequence<result_type>{std::allocator_arg, alloc, move(f)};
      });
}


template<class Apply>
inline auto for_each(Apply apply) {
   using std::begin;
   using std::end;
   using std::for_each;
   using std::move;

   return sequence_manipulator([apply=move(apply)](sequence<auto> s) mutable {
         return for_each(begin(s), end(s), apply);
      });
}

#endif
