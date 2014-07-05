#ifndef SEQUENCE_ELEMENT_ACCESS_H__
#define SEQUENCE_ELEMENT_ACCESS_H__

#ifndef SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class T>
inline auto first_or_default(T &&t) {
   return sequence_manipulator([t=std::forward<T>(t)](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;
         static_assert(std::is_convertible<T, S>::value, "Unable to convert default value type T to sequence value type S.");

         auto iter = s.begin();
         if (iter == s.end()) {
            return S{t};
         }
         return *iter;
      });
}


inline auto first_or_default() {
   return sequence_manipulator([](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;

         auto iter = s.begin();
         if (iter == s.end()) {
            return S{};
         }
         return *iter;
      });
}


inline auto first() {
   return sequence_manipulator([](sequence<auto> s) {
         auto i = s.begin();
         if (i == s.end()) {
            throw std::range_error("First cannot be computed on empty sequence.");
         }
         return *i;
      });
}


inline auto last_or_default() {
   return sequence_manipulator([](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;

         S result = {};
         for (const S &s_value : s) {
            result = s_value;
         }

         return std::move(result);
      });
}


template<class T>
inline auto last_or_default(T &&t) {
   return sequence_manipulator([t=std::forward<T>(t)](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;
         static_assert(std::is_convertible<T, S>::value, "Unable to convert default value type T to sequence value type S.");

         S result{t};
         for (const S &s_value : s) {
            result = s_value;
         }

         return std::move(result);
      });
}


inline auto last() {
   return sequence_manipulator([](sequence<auto> s) {
         auto i = s.begin();
         auto e = s.end();
         if (i == e) {
            throw std::range_error("Last cannot be computed on empty sequence.");
         }

         auto result = *i;
         for (++i; i != e; ++i) {
            result = *i;
         }
         return std::move(result);
      });
}


inline auto element_at_or_default(std::size_t n) {
   return sequence_manipulator([n](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;

         auto i = s.begin();
         auto e = s.end();

         for (; i != e && n-- > 0; ++i) {}

         return (i == e) ? S{} : *i;
      });
}


template<class T>
inline auto element_at_or_default(std::size_t n, T &&t) {
   return sequence_manipulator([n, t=std::forward<T>(t)](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;
         static_assert(std::is_convertible<T, S>::value, "Unable to convert default value type T to sequence value type S.");

         auto i = s.begin();
         auto e = s.end();

         for (; i != e && n-- > 0; ++i) {}

         return (i == e) ? S{t} : *i;
      });
}


inline auto element_at(std::size_t n) {
   return sequence_manipulator([n](sequence<auto> s) mutable {
         auto i = s.begin();
         auto e = s.end();

         for (; i != e && n-- > 0; ++i) {}

         if (i == e) {
            throw std::range_error("Element at index cannot be retrieved because there aren't enough elements in the sequence.");
         }

         return *i;
      });
}


inline auto single_or_default() {
   return sequence_manipulator([](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;

         auto e = s.end();
         auto i = s.begin();
         if (i == e) {
            return S{};
         }
         
         S candidate{*i++};
         return (i == e) ? candidate : S{};
      });
}


template<class T>
inline auto single_or_default(T &&t) {
   return sequence_manipulator([t=std::forward<T>(t)](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;

         auto e = s.end();
         auto i = s.begin();
         if (i == e) {
            return S{t};
         }
         
         S candidate{*i++};
         return (i == e) ? candidate : S{t};
      });
}


inline auto single() {
   return sequence_manipulator([](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;

         auto e = s.end();
         auto i = s.begin();
         if (i == e) {
            throw std::range_error("Cannot retrieve single result from empty sequence.");
         }
         
         S candidate{*i++};
         if (i != e) {
            throw std::range_error("More than one element present in sequence.");
         }

         return std::move(candidate);
      });
}

#endif
