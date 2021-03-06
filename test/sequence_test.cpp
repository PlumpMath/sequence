#include <iostream>
#include <random>
#include "../include/sequence.h"
#include <gtest/gtest.h>


namespace {

using namespace sequencing;

std::mt19937 twister;
auto random_int = [] (auto a, auto b) {
   std::uniform_int_distribution<std::common_type_t<decltype(a), decltype(b)>> generate{a, b};
   return generate(twister);
};

void seed_rng() {
   int seed = ::testing::GTEST_FLAG(random_seed);
   if (seed == 0) {
      std::random_device generate_seed;
      // This is the acceptable range from Google.
      while (!((0 < seed) && (seed < 100000))) {
         seed = generate_seed();
      }
   }
   std::cout << "To reproduce results, specify '--gtest_random_seed=" << seed << "' on the command line." << std::endl;
   twister.seed(seed);
}


struct A { std::string a; };
struct B { std::string a; int b; };
struct C { std::string a; int c; };
struct D { std::string a; int b; int c; };

inline bool operator==(const A &l, const A &r) { return l.a == r.a; }
inline bool operator==(const B &l, const B &r) { return l.a == r.a && l.b == r.b; }
inline bool operator==(const C &l, const C &r) { return l.a == r.a && l.c == r.c; }
inline bool operator==(const D &l, const D &r) { return l.a == r.a && l.b == r.b && l.c == r.c; }

struct combine {
   inline D operator()(const B &l, const C &r) const {
      return { l.a, l.b, r.c };
   }
};


inline std::ostream& operator<<(std::ostream &os, const A &a) {
   return os << "A{'" << a.a << '}';
}


inline std::ostream& operator<<(std::ostream &os, const B &b) {
   return os << "B{'" << b.a << "', " << b.b << '}';
}


inline std::ostream& operator<<(std::ostream &os, const C &c) {
   return os << "C{'" << c.a << "', " << c.c << '}';
}


inline std::ostream& operator<<(std::ostream &os, const D &d) {
   return os << "D{'" << d.a << "', " << d.b << ", " << d.c << '}';
}


TEST(any, finds_at_least_one_value_that_holds_true) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   auto target = range(start, finish);

   // When
   bool actual = std::move(target) | any([](int x) { return x == 1; });

   // Then
   ASSERT_TRUE(actual);
}


TEST(any, cannot_find_a_value_holding_true) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   auto target = range(start, finish);

   // When
   bool actual = target | any([](int x) { return x == 100; });

   // Then
   ASSERT_FALSE(actual);
}


TEST(any, is_false_on_empty_sequence) {
   // Given
   auto target = sequence<int>{};

   // When
   bool actual = target | any([](int x) { return x == 100; });

   // Then
   ASSERT_FALSE(actual);
}


TEST(all, finds_that_all_values_are_true) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   auto target = range(start, finish);

   // When
   bool actual = target | all([](int x) { return x < 100; });

   // Then
   ASSERT_TRUE(actual);
}


TEST(all, finds_at_least_one_value_is_not_true) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   auto target = range(start, finish);

   // When
   bool actual = std::move(target) | all([](int x) { return x != 2; });

   // Then
   ASSERT_FALSE(actual);
}


TEST(all, is_true_on_empty_sequence) {
   // Given
   auto target = sequence<int>{};

   // When
   bool actual = target | all([](int x) { return x == 100; });

   // Then
   ASSERT_TRUE(actual);
}


TEST(none, finds_at_least_one_value_that_holds_true) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   auto target = range(start, finish);

   // When
   bool actual = std::move(target) | none([](int x) { return x == 1; });

   // Then
   ASSERT_FALSE(actual);
}


TEST(none, cannot_find_a_value_holding_true) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   auto target = range(start, finish);

   // When
   bool actual = target | none([](int x) { return x == 100; });

   // Then
   ASSERT_TRUE(actual);
}


TEST(none, is_true_on_empty_sequence) {
   // Given
   auto target = sequence<int>{};

   // When
   bool actual = target | none([](int x) { return x == 100; });

   // Then
   ASSERT_TRUE(actual);
}


TEST(first, provides_first_value_in_the_sequence) {
   // Given
   int expected = random_int(0, 4);
   RecordProperty("expected", expected);
   auto target = range(expected, 10);

   // When
   int actual = target | first();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(first, throws_when_the_sequence_is_empty) {
   // Given
   auto target = sequence<int>();

   // When
   ASSERT_THROW(target | first(), std::range_error);
}


TEST(first_or_default, returns_first_value_in_the_sequence_when_the_sequence_is_not_empty) {
   // Given
   int expected = random_int(0, 2);
   RecordProperty("expected", expected);
   auto target = range(expected, 10);

   // When
   int actual = std::move(target) | first_or_default();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(first_or_default, returns_default_constructed_value_when_the_sequence_is_empty) {
   // Given
   int expected = int();
   auto target = sequence<int>();

   // When
   int actual = target | first_or_default();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(first_or_default, returns_provided_default_value_when_the_sequence_is_empty) {
   // Given
   int expected = random_int(0, 86);
   RecordProperty("expected", expected);
   auto target = sequence<int>();

   // When
   int actual = target | first_or_default(expected);

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(last, provides_last_value_in_the_sequence) {
   // Given
   int expected = random_int(1, 7);
   RecordProperty("expected", expected);
   auto target = range(0, expected + 1);

   // When
   int actual = target | last();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(last, throws_when_the_sequence_is_empty) {
   // Given
   auto target = sequence<int>();

   // When
   ASSERT_THROW(target | last(), std::range_error);
}


TEST(last_or_default, returns_last_value_in_the_sequence_when_the_sequence_is_not_empty) {
   // Given
   int expected = random_int(3, 9);
   RecordProperty("expected", expected);
   auto target = range(2, expected + 1);

   // When
   int actual = target | last_or_default();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(last_or_default, returns_default_constructed_value_when_the_sequence_is_empty) {
   // Given
   int expected = int();
   auto target = sequence<int>();

   // When
   int actual = target | last_or_default();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(last_or_default, returns_provided_default_value_when_the_sequence_is_empty) {
   // Given
   int expected = random_int(3, 39);
   RecordProperty("expected", expected);
   auto target = sequence<int>();

   // When
   int actual = target | last_or_default(expected);

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(element_at, returns_the_element_at_the_given_index) {
   // Given
   auto target = from({ 3, 4, 8 });

   // When
   int actual = target | element_at(1);

   // Then
   ASSERT_EQ(4, actual);
}


TEST(element_at, throws_range_error_when_there_is_no_element_at_the_index) {
   // Given
   auto target = from({ 3, 4, 8 });

   // When
   ASSERT_THROW(target | element_at(4), std::range_error);
}


TEST(element_at_or_default, returns_the_element_at_the_given_index) {
   // Given
   auto target = from({ 3, 4, 8 });

   // When
   int actual = target | element_at_or_default(1);

   // Then
   ASSERT_EQ(4, actual);
}


TEST(element_at_or_default, returns_default_value_when_there_is_no_element_at_the_index) {
   // Given
   auto target = from({ 3, 4, 8 });

   // When
   auto actual = target | element_at_or_default(4);

   // Then
   ASSERT_EQ(int(), actual);
}


TEST(element_at_or_default, returns_provided_default_value_when_there_is_no_element_at_the_index) {
   // Given
   auto target = from({ 3, 4, 8 });

   // When
   auto actual = target | element_at_or_default(4, 15);

   // Then
   ASSERT_EQ(15, actual);
}


TEST(single, returns_only_element) {
   // Given
   auto target = from({ 8 });

   // When
   auto actual = target | single();

   // Then
   ASSERT_EQ(8, actual);
}


TEST(single, throws_range_exception_when_sequence_is_empty) {
   // Given
   auto target = sequence<int>();

   // When
   ASSERT_THROW(target | single(), std::range_error);
}


TEST(single, throws_range_exception_when_sequence_has_multiple_elements) {
   // Given
   auto target = from({ 8, 7 });

   // When
   ASSERT_THROW(target | single(), std::range_error);
}


TEST(single_or_default, returns_only_element) {
   // Given
   auto target = from({ 8 });

   // When
   auto actual = target | single_or_default();

   // Then
   ASSERT_EQ(8, actual);
}


TEST(single_or_default, returns_default_value_when_sequence_is_empty) {
   // Given
   auto target = sequence<int>();

   // When
   auto actual = target | single_or_default();

   // Then
   ASSERT_EQ(int(), actual);
}


TEST(single_or_default, returns_provided_default_value_when_sequence_is_empty) {
   // Given
   auto target = sequence<int>();

   // When
   auto actual = target | single_or_default(5);

   // Then
   ASSERT_EQ(5, actual);
}


TEST(single_or_default, returns_default_value_when_sequence_has_multiple_elements) {
   // Given
   auto target = from({ 8, 7 });

   // When
   auto actual = target | single_or_default();

   // Then
   ASSERT_EQ(int(), actual);
}


TEST(single_or_default, returns_provided_default_value_when_sequence_has_multiple_elements) {
   // Given
   auto target = from({ 8, 7 });

   // When
   auto actual = target | single_or_default(883);

   // Then
   ASSERT_EQ(883, actual);
}


TEST(sort, returns_sorted_sequence) {
   // Given
   auto target = from({ 2, 3, 1, 4 });
   std::vector<int> expected = { 1, 2, 3, 4 };

   // When
   auto actual = target | sort();

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(sort, returns_sorted_sequence_using_provided_comparator) {
   // Given
   auto target = from({ 2, 3, 1, 4 });
   std::vector<int> expected = { 4, 3, 2, 1 };

   // When
   auto actual = target | sort(4, std::greater<int>());

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(reverse, returns_sequence_in_reverse_order) {
   // Given
   auto target = from({ 2, 3, 1, 4 });
   auto expected = { 4, 1, 3, 2 };

   // When
   auto actual = target | reverse(4);

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(contains, returns_false_on_empty_sequence) {
   // Given
   auto target = sequence<char>();

   // When
   bool actual = target | contains('n');

   // Then
   ASSERT_FALSE(actual);
}


TEST(contains, returns_false_when_sequence_does_not_have_item) {
   // Given
   auto target = from("abc");

   // When
   bool actual = target | contains('n');

   // Then
   ASSERT_FALSE(actual);
}


TEST(contains, returns_true_when_sequence_has_item) {
   // Given
   auto target = from("abc");

   // When
   bool actual = target | contains('c');

   // Then
   ASSERT_TRUE(actual);
}


TEST(count, is_0_when_sequence_is_empty) {
   // Given
   auto target = sequence<std::string>();
   size_t expected = 0;

   // When
   size_t actual = std::move(target) | count();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(count, reflects_number_of_elements_in_sequence) {
   // Given
   size_t expected = 3;
   std::vector<std::string> vec = { "hello", " ", "world" };
   auto target = from(vec);

   // When
   std::size_t actual = target | count();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(count, only_counts_the_elements_matching_the_predicate) {
   // Given
   size_t expected = 2;
   std::vector<std::string> vec = { "hello", " ", "world", "foo", "bar" };
   auto target = from(vec);

   // When
   std::size_t actual = target | count([](const std::string &s) { return s.size() == 3; });

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(zip_with, produces_a_sequence_with_same_number_of_elements_as_pairs) {
   // Given
   size_t expected = 3;
   std::vector<std::string> svec = { "hello", " ", "world" };
   std::vector<int> ivec = { 1, 2, 3 };
   auto sseq = from(svec);
   auto iseq = from(ivec);

   // When
   auto target = iseq | zip_with(std::move(sseq));

   // Then
   ASSERT_EQ(expected, target | count());
}


TEST(zip_with, produces_a_sequence_with_same_elements_as_pairs) {
   // Given
   std::vector<std::string> svec = { "foo", "baz" };
   std::vector<int> ivec = { 10, -3 };
   auto sseq = from(svec);
   auto iseq = from(ivec);
   std::vector<std::pair<int, std::string>> expected = { { 10, "foo" }, { -3, "baz" } };

   // When
   auto target = iseq | zip_with(std::move(sseq));

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), target.begin()));
}


TEST(pairwise, reduces_sequence_by_half) {
   // Given
   size_t expected = 2;
   auto s = range(-1.0, 1.0, 0.5);

   // When
   auto target = s | pairwise();

   // Then
   ASSERT_EQ(expected, target | count());
}


TEST(pairwise, produces_pairs_of_the_generating_sequence) {
   // Given
   auto s = from("beer");
   std::vector<std::pair<char, char>> expected = { { 'b', 'e' }, { 'e', 'r' } };

   // When
   auto target = s | pairwise();

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), target.begin()));
}


TEST(pairwise, truncates_remaining_element_by_default) {
   // Given
   auto s = from("beers");
   std::vector<std::pair<char, char>> expected = { { 'b', 'e' }, { 'e', 'r' } };

   // When
   auto target = s | pairwise();

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), target.begin()));
}


TEST(pairwise, captures_remainder_when_specified_to_do_so) {
   // Given
   auto s = from("beers");
   std::vector<std::pair<char, char>> expected = { { 'b', 'e' }, { 'e', 'r' }, { 's', '\0' } };

   // When
   auto target = s | pairwise(pairwise_capture::use_remainder);
         

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), target.begin()));
}


TEST(max, provides_the_maximum_value) {
   // Given
   int expected = 8;

   // When
   int actual = from({6, expected, 2}) | max();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(max, throws_domain_error_on_empty_sequence) {
   // Given
   auto target = sequence<int>();

   // When
   ASSERT_THROW(target | max(), std::range_error);
}


TEST(min, provides_the_minimum_value) {
   // Given
   int expected = 8;

   // When
   int actual = from({55, 22, 13, expected, 92}) | min();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(min, throws_domain_error_on_empty_sequence) {
   // Given
   auto target = sequence<int>();

   // When
   ASSERT_THROW(target | min(), std::range_error);
}


TEST(minmax, provides_pair_with_minimum_and_maximum_value) {
   // Given
   auto target = range(7.0f, 5.0f, 0.25f);
   float expected_min = 5.25f;
   float expected_max = 7.00f;

   // When
   std::pair<float, float> actual = target | minmax();

   // Then
   ASSERT_FLOAT_EQ(expected_min, actual.first);
   ASSERT_FLOAT_EQ(expected_max, actual.second);
}


TEST(minmax, throws_range_error_on_empty_sequence) {
   // Given
   auto target = sequence<int>();

   // When
   ASSERT_THROW(target | minmax(), std::range_error);
}


TEST(sum, provides_total_of_all_elements) {
   // Given
   std::vector<std::string> svec = { "Andrew", " ", "Ford" };
   auto target = from(svec);
   std::string expected = "Andrew Ford";

   // When
   auto actual = target | sum<std::string>();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(sum, provides_total_of_all_elements_beginning_with_initial_value) {
   // Given
   std::vector<std::string> svec = { " ", "Ford" };
   auto target = from(svec);
   std::string expected = "Andrew Ford";

   // When
   auto actual = target | sum(std::string("Andrew"));

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(sum, uses_provided_binary_operation) {
   // Given
   auto target = range(3, 0);
   std::uint16_t expected = 6;

   // When
   auto actual = target | sum(1, std::multiplies<std::uint16_t>());

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(sum, uses_provided_initial_value_and_binary_operation) {
   // Given
   auto target = range(3, 0);
   std::uint16_t expected = 12;

   // When
   auto actual = target | sum(2, std::multiplies<std::uint16_t>());

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(inner_product, calculates_correctly) {
   // Given
   auto dtarget = from({1.0, 2.0, 3.0});

   // When
   auto actual = dtarget | inner_product<double>(from({3, 2, 1}), 0.0);

   // Then
   ASSERT_DOUBLE_EQ(10.0, actual);
}


TEST(select, projects_into_new_type) {
   // Given
   std::vector<A> avec = { { "blammo" }, { "booyah" }, { "huzah" } };
   auto target = from(avec);
   std::vector<std::string> expected = { "blammo", "booyah", "huzah" };

   // When
   // Note: Explicitly setting the actual type rather than using auto to ensure
   //       that the resulting type is what we want (we'll get compiler errors
   //       if it isn't).
   sequence<std::string> actual = target | select([](const A &a) { return a.a; });

   // Then
   ASSERT_TRUE(std::equal(std::begin(expected), std::end(expected), actual.begin()));
}


TEST(select_many, extracts_single_value_to_many) {
   // Given
   std::string expected = "hello world";
   std::vector<std::string> strings = { "hello", " ", "world" };
   auto target = from(strings);

   // When
   auto actual = target | select_many([](std::string s) { return from(s); });

   // Then
   ASSERT_TRUE(std::equal(std::begin(expected), std::end(expected), actual.begin()));
}


TEST(where, filters_out_uninteresting_values) {
   // Given
   const int start = 0;
   const int finish = 10;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   int expected = start;

   // When
   auto target = range(start, finish)
                     | where([](int x) { return x % 2 == 0; });

   // Then
   for (int actual : target) {
      ASSERT_EQ(expected, actual);
      expected += 2;
   }
}


TEST(concat, appends_to_sequence) {
   // Given
   auto arg = range(3, 6);
   auto target = range(0, 3);
   std::vector<int> ivec = { 0, 1, 2, 3, 4, 5 };

   // When
   auto actual = target | concat(std::move(arg));

   // Then
   ASSERT_TRUE(std::equal(ivec.begin(), ivec.end(), actual.begin()));
}


TEST(take, returns_first_n_elements) {
  // Given
  std::vector<int> ivec = { 1, 2, 3 };
  auto target = from({1, 2, 3, 4, 5, 6});

  // When
  auto actual = target | take(3);

  // Then
  ASSERT_TRUE(std::equal(ivec.begin(), ivec.end(), actual.begin()));
}

TEST(take, returns_all_elements_when_n_is_greater_than_number_of_elements) {
  // Given
  size_t expected = 6;
  auto target = from({1, 2, 3, 4, 5, 6}) | take(20);

  // When
  std::size_t actual = target | count();

  // Then
  ASSERT_EQ(expected, actual);
}


TEST(take, is_empty_after_exhausting_all_elements) {
  // Given
  auto target = from({1, 2, 3, 4, 5, 6}) | take(0);

  // When
  bool actual = target.empty();

  // Then
  ASSERT_TRUE(actual);
}


TEST(take_while, stops_taking_elements_after_predicate_fails) {
  // Given
  std::vector<int> ivec = { 1, 2, 3 };
  auto target = from({1, 2, 3, 4, 5, 6});

  // When
  auto actual = target | take_while([](int x) { return (x % 7) < 4; });

  // Then
  ASSERT_TRUE(std::equal(ivec.begin(), ivec.end(), actual.begin()));
}


TEST(skip, skips_first_n_elements) {
   // Given
   std::string s = "foobar";
   auto target = from(s);

   // When
   auto actual = target | skip(3);

   // Then
   ASSERT_TRUE(std::equal(s.begin() + 3, s.end(), actual.begin()));
}


TEST(skip, produces_empty_sequence_when_there_are_not_enough_elements_to_skip) {
   // Given
   std::string s = "foobar";
   auto target = from(s);

   // When
   auto actual = target | skip(30);

   // Then
   ASSERT_TRUE(actual.empty());
}


TEST(skip_while, skips_until_predicate_fails) {
   // Given
   std::string s = "foobar";
   auto target = from(s);

   // When
   auto actual = target | skip_while([](char c) { return c != 'b'; });

   // Then
   ASSERT_TRUE(std::equal(s.begin() + 3, s.end(), actual.begin()));
}


TEST(page, returns_page_size_when_enough_elements_are_available) {
  // Given
  size_t expected = 2;
  auto target = from({1.0, 2.0, 3.0, 4.0, 5.0, 6.0}) | page(0, 2);

  // When
  std::size_t actual = target | count();

  // Then
  ASSERT_EQ(expected, actual);
}


TEST(page, returns_page_number_when_enough_elements_are_available) {
  // Given
  int expected = 3;
  auto target = from({1, 2, 3, 4, 5, 6}) | page(1, 2);

  // When
  int actual = target | first();

  // Then
  ASSERT_EQ(expected, actual);
}


TEST(page, returns_empty_sequence_when_page_number_is_too_large) {
  // Given
  auto target = from({1.0, 2.0, 3.0, 4.0, 5.0, 6.0}) | page(10, 2);

  // When
  bool actual = target.empty();

  // Then
  ASSERT_TRUE(actual);
}


TEST(page, returns_remaining_elements_on_last_page_with_smaller_than_page_size) {
  // Given
  size_t expected = 2;
  auto target = from({1, 2, 3, 4, 5, 6}) | page(1, 4);

  // When
  size_t actual = target | count();

  // Then
  ASSERT_EQ(expected, actual);
}


TEST(union_with, properly_performs_union) {
   // Given
   auto l = range(7, 15);
   auto r = range(0, 10);
   auto expected = range(0, 15);

   // When
   auto actual = union_with(std::move(l), std::move(r));

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(intersect_with, properly_performs_intersection) {
   // Given
   auto l = range(7, 15);
   auto r = range(0, 10);
   auto expected = { 7, 8, 9 };

   // When
   auto actual = intersect_with(std::move(l), std::move(r));

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(except, properly_performs_set_difference) {
   // Given
   auto l = range(0, 15);
   auto r = range(0, 15) | where([](int x) { return x % 2 == 1; });
   auto expected = { 0, 2, 4, 6, 8, 10, 12, 14 };

   // When
   auto actual = except(std::move(l), std::move(r));

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(symmetric_difference, properly_performs_set_difference) {
   // Given
   auto l = range(7, 15);
   auto r = range(0, 10);
   auto expected = { 0, 1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14 };

   // When
   auto actual = symmetric_difference(std::move(l), std::move(r));

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(join, produces_inner_join_of_two_sequences) {
   // Given
   auto b = {B{"foo", 3}, B{"bar", 5}};
   auto c = {C{"foo", 7}, C{"foo", 25}, C{"baz", 33}};
   std::vector<D> expected = { {"foo", 3, 7}, {"foo", 3, 25} };

   // When
   sequence<D> actual = join(from(b), [](const B &b) { return b.a; },
                             from(c), [](const C &c) { return c.a; },
                             combine(), 3);

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(from, produces_identical_sequence_as_container) {
   // Given
   std::vector<short> expected = { 1, 2, 3, 9, 8, 7 };

   // When
   auto actual = from(expected);

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(from, produces_identical_sequence_as_iterator_range) {
   // Given
   std::vector<short> expected = { 1, 2, 3, 9, 8, 7 };

   // When
   auto actual = from(std::begin(expected), std::end(expected));

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(from, produces_identical_sequence_as_initializer_list) {
   // Given
   std::vector<short> expected = { 1, 2, 3, 9, 8, 7 };

   // When
   auto actual = from({ 1, 2, 3, 9, 8, 7 });

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(generate, produces_a_sequence_with_n_elements) {
   // Given
   size_t x = random_int(0, 52);
   const size_t start = x;
   size_t n = random_int(3, 15);
   RecordProperty("start", x);
   RecordProperty("size", n);
   auto generator = [&x]() { return x++; };

   // When
   auto actual = generate(generator, n);

   // Then
   // Note: We need to test the entire sequence including end-state.
   auto aiter = actual.begin();
   for (size_t i = 0; i < n && aiter != actual.end(); ++i, ++aiter) {
      ASSERT_EQ(start + i, *aiter);
   }
   ASSERT_TRUE(actual.empty());
}


TEST(range, increments_across_start_to_finish) {
   // Given
   const int start = 0;
   const int finish = 10;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   int expected = start;

   // When
   auto target = range(start, finish);

   // Then
   for (int actual : target) {
      ASSERT_EQ(expected, actual);
      ++expected;
   }
}


TEST(range, increments_until_finishes_at_finish_value) {
   // Given
   const int start = 0;
   const int finish = 10;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   int actual = start;
   auto target = range(start, finish);

   // When
   for (int ignore : target) {
      (void)ignore;
      ++actual;
   }

   // Then
   ASSERT_EQ(finish, actual);
}


TEST(range, decrements_across_start_to_finish) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   int expected = start;

   // When
   auto target = range(start, finish);

   // Then
   for (int actual : target) {
      ASSERT_EQ(expected, actual);
      --expected;
   }
}


TEST(range, decrements_until_finishes_at_finish) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   int actual = start;
   auto target = range(start, finish);

   // When
   for (int ignore : target) {
      (void)ignore;
      --actual;
   }

   // Then
   ASSERT_EQ(finish, actual);
}


TEST(empty_sequence, does_not_iterate) {
   // Given
   size_t expected = 0;
   size_t invocations = 0;

   // When
   for (int ignored : sequence<int>()) {
      (void)ignored;
      ++invocations;
   }

   // Then
   ASSERT_EQ(expected, invocations);
}


TEST(empty_sequence, is_empty) {
   // Given
   auto target = sequence<float>();

   // When
   bool actual = target.empty();

   // Then
   ASSERT_TRUE(actual);
}


TEST(empty, true_when_sequence_is_empty) {
   // Given
   auto target = sequence<float>();

   // When
   bool actual = target | empty();

   // Then
   ASSERT_TRUE(actual);
}


TEST(empty, false_when_sequence_is_not_empty) {
   // Given
   auto target = from({1, 1, 2, 2, 3, 3, 4, 5});

   // When
   bool actual = target | empty();

   // Then
   ASSERT_FALSE(actual);
}

}


int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
   seed_rng();
   return RUN_ALL_TESTS();
}
