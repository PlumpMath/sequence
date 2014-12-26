#ifndef SEQUENCE_ELEMENT_ACCESS_H__
#define SEQUENCE_ELEMENT_ACCESS_H__

#ifndef SEQUENCING_SEQUENCE_H__
#error This file is meant to be included from sequence.h
#endif


template<class T>
inline auto first_or_default(T &&t) {
   using std::begin;
   using std::end;
   using std::forward;

   return sequence_manipulator([t=forward<T>(t)](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;
         static_assert(std::is_convertible<T, S>::value, "Unable to convert default value type T to sequence value type S.");

         auto iter = begin(s);
         if (iter == end(s)) {
            return S{t};
         }
         return *iter;
      });
}


inline auto first_or_default() {
   using std::begin;
   using std::end;

   return sequence_manipulator([](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;

         auto iter = begin(s);
         if (iter == end(s)) {
            return S{};
         }
         return *iter;
      });
}


inline auto first() {
   using std::begin;
   using std::end;

   return sequence_manipulator([](sequence<auto> s) {
         auto i = begin(s);
         if (i == end(s)) {
            throw std::range_error("First cannot be computed on empty sequence.");
         }
         return *i;
      });
}


inline auto last_or_default() {
   using std::move;

   return sequence_manipulator([](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;

         S result = {};
         for (const S &s_value : s) {
            result = s_value;
         }

         return move(result);
      });
}


template<class T>
inline auto last_or_default(T &&t) {
   using std::forward;
   using std::move;

   return sequence_manipulator([t=forward<T>(t)](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;
         static_assert(std::is_convertible<T, S>::value, "Unable to convert default value type T to sequence value type S.");

         S result{t};
         for (const S &s_value : s) {
            result = s_value;
         }

         return move(result);
      });
}


inline auto last() {
   using std::begin;
   using std::end;
   using std::move;

   return sequence_manipulator([](sequence<auto> s) {
         auto i = begin(s);
         auto e = end(s);
         if (i == e) {
            throw std::range_error("Last cannot be computed on empty sequence.");
         }

         auto result = *i;
         for (++i; i != e; ++i) {
            result = *i;
         }
         return move(result);
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
   using std::begin;
   using std::end;
   using std::forward;

   return sequence_manipulator([n, t=forward<T>(t)](sequence<auto> s) mutable {
         typedef typename decltype(s)::value_type S;
         static_assert(std::is_convertible<T, S>::value, "Unable to convert default value type T to sequence value type S.");

         auto i = begin(s);
         auto e = end(s);

         for (; i != e && n-- > 0; ++i) {}

         return (i == e) ? S{t} : *i;
      });
}


inline auto element_at(std::size_t n) {
   using std::begin;
   using std::end;

   return sequence_manipulator([n](sequence<auto> s) mutable {
         auto i = begin(s);
         auto e = end(s);

         for (; i != e && n-- > 0; ++i) {}

         if (i == e) {
            throw std::range_error("Element at index cannot be retrieved because there aren't enough elements in the sequence.");
         }

         return *i;
      });
}


inline auto single_or_default() {
   using std::begin;
   using std::end;

   return sequence_manipulator([](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;

         auto e = end(s);
         auto i = begin(s);
         if (i == e) {
            return S{};
         }
         
         S candidate{*i++};
         return (i == e) ? candidate : S{};
      });
}


template<class T>
inline auto single_or_default(T &&t) {
   using std::begin;
   using std::end;
   using std::forward;

   return sequence_manipulator([t=forward<T>(t)](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;

         auto e = end(s);
         auto i = begin(s);
         if (i == e) {
            return S{t};
         }
         
         S candidate{*i++};
         return (i == e) ? candidate : S{t};
      });
}


inline auto single() {
   using std::begin;
   using std::end;
   using std::move;

   return sequence_manipulator([](sequence<auto> s) {
         typedef typename decltype(s)::value_type S;

         auto e = end(s);
         auto i = begin(s);
         if (i == e) {
            throw std::range_error("Cannot retrieve single result from empty sequence.");
         }
         
         S candidate{*i++};
         if (i != e) {
            throw std::range_error("More than one element present in sequence.");
         }

         return move(candidate);
      });
}

#endif
