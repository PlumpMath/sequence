#include "../include/sequence.h"
#include <gtest/gtest.h>
#include <iostream>


namespace {


struct A { std::string a; };

inline bool operator==(const A &l, const A &r) { return l.a == r.a; }


TEST(any, finds_at_least_one_value_that_holds_true) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   auto target = sequence<int>::range(start, finish);

   // When
   bool actual = target.any([](int x) { return x == 1; });

   // Then
   ASSERT_TRUE(actual);
}


TEST(any, cannot_find_a_value_holding_true) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   auto target = sequence<int>::range(start, finish);

   // When
   bool actual = target.any([](int x) { return x == 100; });

   // Then
   ASSERT_FALSE(actual);
}


TEST(all, finds_that_all_values_are_true) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   auto target = sequence<int>::range(start, finish);

   // When
   bool actual = target.all([](int x) { return x < 100; });

   // Then
   ASSERT_TRUE(actual);
}


TEST(all, finds_at_least_one_value_is_not_true) {
   // Given
   const int start = 10;
   const int finish = 0;
   RecordProperty("start", start);
   RecordProperty("finish", finish);
   auto target = sequence<int>::range(start, finish);

   // When
   bool actual = target.all([](int x) { return x != 2; });

   // Then
   ASSERT_FALSE(actual);
}


TEST(first, provides_first_value_in_the_sequence) {
   // Given
   int expected = std::rand() % 5;
   RecordProperty("expected", expected);
   auto target = sequence<int>::range(expected, 10);

   // When
   int actual = target.first();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(first, throws_when_the_sequence_is_empty) {
   // Given
   auto target = sequence<int>::empty_sequence();

   // When
   ASSERT_THROW(target.first(), std::range_error);
}


TEST(first_or_default, returns_first_value_in_the_sequence_when_the_sequence_is_not_empty) {
   // Given
   int expected = std::rand() % 3;
   RecordProperty("expected", expected);
   auto target = sequence<int>::range(expected, 10);

   // When
   int actual = target.first_or_default();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(first_or_default, returns_default_constructed_value_when_the_sequence_is_empty) {
   // Given
   int expected = int();
   auto target = sequence<int>::empty_sequence();;

   // When
   int actual = target.first_or_default();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(first_or_default, returns_provided_default_value_when_the_sequence_is_empty) {
   // Given
   int expected = std::rand() % 87;
   RecordProperty("expected", expected);
   auto target = sequence<int>::empty_sequence();;

   // When
   int actual = target.first_or_default(expected);

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(last, provides_last_value_in_the_sequence) {
   // Given
   int expected = (std::rand() % 7) + 1;
   RecordProperty("expected", expected);
   auto target = sequence<int>::range(0, expected + 1);

   // When
   int actual = target.last();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(last, throws_when_the_sequence_is_empty) {
   // Given
   auto target = sequence<int>::empty_sequence();

   // When
   ASSERT_THROW(target.last(), std::range_error);
}


TEST(last_or_default, returns_last_value_in_the_sequence_when_the_sequence_is_not_empty) {
   // Given
   int expected = (std::rand() % 7) + 3;
   RecordProperty("expected", expected);
   auto target = sequence<int>::range(2, expected + 1);

   // When
   int actual = target.last_or_default();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(last_or_default, returns_default_constructed_value_when_the_sequence_is_empty) {
   // Given
   int expected = int();
   auto target = sequence<int>::empty_sequence();;

   // When
   int actual = target.last_or_default();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(last_or_default, returns_provided_default_value_when_the_sequence_is_empty) {
   // Given
   int expected = (std::rand() % 37) + 3;
   RecordProperty("expected", expected);
   auto target = sequence<int>::empty_sequence();;

   // When
   int actual = target.last_or_default(expected);

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(element_at, returns_the_element_at_the_given_index) {
   // Given
   auto target = sequence<int>::from({ 3, 4, 8 });

   // When
   int actual = target.element_at(1);

   // Then
   ASSERT_EQ(4, actual);
}


TEST(element_at, throws_range_error_when_there_is_no_element_at_the_index) {
   // Given
   auto target = sequence<int>::from({ 3, 4, 8 });

   // When
   ASSERT_THROW(target.element_at(4), std::range_error);
}


TEST(element_at_or_default, returns_the_element_at_the_given_index) {
   // Given
   auto target = sequence<int>::from({ 3, 4, 8 });

   // When
   int actual = target.element_at_or_default(1);

   // Then
   ASSERT_EQ(4, actual);
}


TEST(element_at_or_default, returns_default_value_when_there_is_no_element_at_the_index) {
   // Given
   auto target = sequence<int>::from({ 3, 4, 8 });

   // When
   auto actual = target.element_at_or_default(4);

   // Then
   ASSERT_EQ(int(), actual);
}


TEST(element_at_or_default, returns_provided_default_value_when_there_is_no_element_at_the_index) {
   // Given
   auto target = sequence<int>::from({ 3, 4, 8 });

   // When
   auto actual = target.element_at_or_default(4, 15);

   // Then
   ASSERT_EQ(15, actual);
}


TEST(contains, returns_false_on_empty_sequence) {
   // Given
   auto target = sequence<char>::empty_sequence();

   // When
   bool actual = target.contains('n');

   // Then
   ASSERT_FALSE(actual);
}


TEST(contains, returns_false_when_sequence_does_not_have_item) {
   // Given
   auto target = sequence<char>::from("abc");

   // When
   bool actual = target.contains('n');

   // Then
   ASSERT_FALSE(actual);
}


TEST(contains, returns_true_when_sequence_has_item) {
   // Given
   auto target = sequence<char>::from("abc");

   // When
   bool actual = target.contains('c');

   // Then
   ASSERT_TRUE(actual);
}


TEST(count, is_0_when_sequence_is_empty) {
   // Given
   auto target = sequence<std::string>::empty_sequence();

   // When
   std::size_t actual = target.count();

   // Then
   ASSERT_EQ(0, actual);
}


TEST(count, reflects_number_of_elements_in_sequence) {
   // Given
   std::vector<std::string> vec = { "hello", " ", "world" };
   auto target = sequence<std::string>::from(vec);

   // When
   std::size_t actual = target.count();

   // Then
   ASSERT_EQ(3, actual);
}


TEST(count, only_counts_the_elements_matching_the_predicate) {
   // Given
   std::vector<std::string> vec = { "hello", " ", "world", "foo", "bar" };
   auto target = sequence<std::string>::from(vec);

   // When
   std::size_t actual = target.count([](const std::string &s) { return s.size() == 3; });

   // Then
   ASSERT_EQ(2, actual);
}


TEST(zip_with, produces_a_sequence_with_same_number_of_elements_as_pairs) {
   // Given
   std::vector<std::string> svec = { "hello", " ", "world" };
   std::vector<int> ivec = { 1, 2, 3 };
   auto sseq = sequence<std::string>::from(svec);
   auto iseq = sequence<int>::from(ivec);

   // When
   auto target = iseq.zip_with(sseq);

   // Then
   ASSERT_EQ(3, target.count());
}


TEST(zip_with, produces_a_sequence_with_same_elements_as_pairs) {
   // Given
   std::vector<std::string> svec = { "foo", "baz" };
   std::vector<int> ivec = { 10, -3 };
   auto sseq = sequence<std::string>::from(svec);
   auto iseq = sequence<int>::from(ivec);
   std::vector<std::pair<int, std::string>> expected = { { 10, "foo" }, { -3, "baz" } };

   // When
   auto target = iseq.zip_with(sseq);

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), target.begin()));
}


TEST(pairwise, reduces_sequence_by_half) {
   // Given
   auto s = sequence<double>::range(-1.0, 1.0, 0.5);

   // When
   auto target = s.pairwise();

   // Then
   ASSERT_EQ(2, target.count());
}


TEST(pairwise, produces_pairs_of_the_generating_sequence) {
   // Given
   auto s = sequence<char>::from("beer");
   std::vector<std::pair<char, char>> expected = { { 'b', 'e' }, { 'e', 'r' } };

   // When
   auto target = s.pairwise();

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), target.begin()));
}


TEST(pairwise, truncates_remaining_element_by_default) {
   // Given
   auto s = sequence<char>::from("beers");
   std::vector<std::pair<char, char>> expected = { { 'b', 'e' }, { 'e', 'r' } };

   // When
   auto target = s.pairwise();

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), target.begin()));
}


TEST(pairwise, captures_remainder_when_specified_to_do_so) {
   // Given
   auto s = sequence<char>::from("beers");
   std::vector<std::pair<char, char>> expected = { { 'b', 'e' }, { 'e', 'r' }, { 's', '\0' } };

   // When
   auto target = s.pairwise();

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), target.begin()));
}


TEST(max, provides_the_maximum_value) {
   // Given
   const int start = 0;
   const int finish = 10;
   RecordProperty("start", start);
   RecordProperty("finish", finish);

   // When
   int actual = sequence<int>::range(start, finish).max();

   // Then
   ASSERT_EQ(finish - 1, actual);
}


TEST(max, throws_domain_error_on_empty_sequence) {
   // Given
   auto target = sequence<int>::empty_sequence();

   // When
   ASSERT_THROW(target.max(), std::range_error);
}


TEST(min, provides_the_minimum_value) {
   // Given
   const int start = 0;
   const int finish = 10;
   RecordProperty("start", start);
   RecordProperty("finish", finish);

   // When
   int actual = sequence<int>::range(start, finish).min();

   // Then
   ASSERT_EQ(start, actual);
}


TEST(min, throws_domain_error_on_empty_sequence) {
   // Given
   auto target = sequence<int>::empty_sequence();

   // When
   ASSERT_THROW(target.min(), std::range_error);
}


#if 0
TEST(minmax, provides_pair_with_minimum_and_maximum_value) {
   // Given
   auto target = sequence<float>::range(7.0f, 5.0f, 0.25f);
   float expected_min = 5.25f;
   float expected_max = 7.00f;

   // When
   std::pair<float, float> actual = target.minmax();

   // Then
   ASSERT_FLOAT_EQ(expected_min, actual.first);
   ASSERT_FLOAT_EQ(expected_max, actual.second);
}


TEST(minmax, throws_range_error_on_empty_sequence) {
   // Given
   auto target = sequence<int>::empty_sequence();

   // When
   ASSERT_THROW(target.minmax(), std::range_error);
}
#endif


TEST(sum, provides_total_of_all_elements) {
   // Given
   std::vector<std::string> svec = { "Andrew", " ", "Ford" };
   auto target = sequence<std::string>::from(svec);
   std::string expected = "Andrew Ford";

   // When
   auto actual = target.sum();

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(sum, provides_total_of_all_elements_beginning_with_initial_value) {
   // Given
   std::vector<std::string> svec = { " ", "Ford" };
   auto target = sequence<std::string>::from(svec);
   std::string expected = "Andrew Ford";

   // When
   auto actual = target.sum("Andrew");

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(sum, uses_provided_binary_operation) {
   // Given
   auto target = sequence<std::uint16_t>::range(3, 0);
   std::uint16_t expected = 6;

   // When
   auto actual = target.sum(1, std::multiplies<std::uint16_t>());

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(sum, uses_provided_initial_value_and_binary_operation) {
   // Given
   auto target = sequence<std::uint16_t>::range(3, 0);
   std::uint16_t expected = 12;

   // When
   auto actual = target.sum(2, std::multiplies<std::uint16_t>());

   // Then
   ASSERT_EQ(expected, actual);
}


TEST(inner_product, resolves_type_correctly) {
   // Given
   auto dtarget = sequence<double>::from({1.0, 2.0, 3.0});
   auto itarget = sequence<int>::from({3, 2, 1});

   // When
   auto actual = dtarget.inner_product(itarget);

   // Then
   ASSERT_TRUE((std::is_same<double, decltype(actual)>::value));
}


TEST(inner_product, calculates_correctly) {
   // Given
   auto dtarget = sequence<double>::from({1.0, 2.0, 3.0});
   auto itarget = sequence<int>::from({3, 2, 1});

   // When
   auto actual = dtarget.inner_product(itarget);

   // Then
   ASSERT_DOUBLE_EQ(10.0, actual);
}


TEST(select, projects_into_new_type) {
   // Given
   std::vector<A> avec = { { "blammo" }, { "booyah" }, { "huzah" } };
   auto target = sequence<A>::from(avec);
   std::vector<std::string> expected = { "blammo", "booyah", "huzah" };

   // When
   // Note: Explicitly setting the actual type rather than using auto to ensure
   //       that the resulting type is what we want (we'll get compiler errors
   //       if it isn't).
   sequence<std::string> actual = target.select([](const A &a) { return a.a; });

   // Then
   ASSERT_TRUE(std::equal(std::begin(expected), std::end(expected), actual.begin()));
}


TEST(select_many, extracts_single_value_to_many) {
   // Given
   std::string expected = "hello world";
   std::vector<std::string> strings = { "hello", " ", "world" };
   auto target = sequence<std::string>::from(strings);

   // When
   auto actual = target.select_many([](std::string s) { return sequence<char>::from(s); });

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
   auto target = sequence<int>::range(start, finish)
                     .where([](int x) { return x % 2 == 0; });

   // Then
   for (int actual : target) {
      ASSERT_EQ(expected, actual);
      expected += 2;
   }
}


TEST(concat, appends_to_sequence) {
   // Given
   auto arg = sequence<int>::range(3, 6);
   auto target = sequence<int>::range(0, 3);
   std::vector<int> ivec = { 0, 1, 2, 3, 4, 5 };

   // When
   auto actual = target.concat(arg);

   // Then
   ASSERT_TRUE(std::equal(ivec.begin(), ivec.end(), actual.begin()));
}


TEST(take, returns_first_n_elements) {
  // Given
  std::vector<int> ivec = { 1, 2, 3 };
  auto target = sequence<int>::from({1, 2, 3, 4, 5, 6});

  // When
  auto actual = target.take(3);

  // Then
  ASSERT_TRUE(std::equal(ivec.begin(), ivec.end(), actual.begin()));
}


TEST(take, returns_all_elements_when_n_is_greater_than_number_of_elements) {
  // Given
  auto target = sequence<int>::from({1, 2, 3, 4, 5, 6}).take(20);

  // When
  std::size_t actual = target.count();

  // Then
  ASSERT_EQ(6, actual);
}


TEST(take, is_empty_after_exhausting_all_elements) {
  // Given
  auto target = sequence<int>::from({1, 2, 3, 4, 5, 6}).take(0);

  // When
  bool actual = target.empty();

  // Then
  ASSERT_TRUE(actual);
}


TEST(take_while, stops_taking_elements_after_predicate_fails) {
  // Given
  std::vector<int> ivec = { 1, 2, 3 };
  auto target = sequence<int>::from({1, 2, 3, 4, 5, 6});

  // When
  auto actual = target.take_while([](int x) { return (x % 7) < 4; });

  // Then
  ASSERT_TRUE(std::equal(ivec.begin(), ivec.end(), actual.begin()));
}


TEST(skip, skips_first_n_elements) {
   // Given
   std::string s = "foobar";
   auto target = sequence<char>::from(s);

   // When
   auto actual = target.skip(3);

   // Then
   ASSERT_TRUE(std::equal(s.begin() + 3, s.end(), actual.begin()));
}


TEST(skip, produces_empty_sequence_when_there_are_not_enough_elements_to_skip) {
   // Given
   std::string s = "foobar";
   auto target = sequence<char>::from(s);

   // When
   auto actual = target.skip(30);

   // Then
   ASSERT_TRUE(actual.empty());
}


TEST(skip_while, skips_until_predicate_fails) {
   // Given
   std::string s = "foobar";
   auto target = sequence<char>::from(s);

   // When
   auto actual = target.skip_while([](char c) { return c != 'b'; });

   // Then
   ASSERT_TRUE(std::equal(s.begin() + 3, s.end(), actual.begin()));
}


TEST(page, returns_page_size_when_enough_elements_are_available) {
  // Given
  auto target = sequence<double>::from({1.0, 2.0, 3.0, 4.0, 5.0, 6.0}).page(0, 2);

  // When
  std::size_t actual = target.count();

  // Then
  ASSERT_EQ(2, actual);
}


TEST(page, returns_page_number_when_enough_elements_are_available) {
  // Given
  auto target = sequence<int>::from({1, 2, 3, 4, 5, 6}).page(1, 2);

  // When
  int actual = target.first();

  // Then
  ASSERT_EQ(3, actual);
}


TEST(page, returns_empty_sequence_when_page_number_is_too_large) {
  // Given
  auto target = sequence<double>::from({1.0, 2.0, 3.0, 4.0, 5.0, 6.0}).page(10, 2);

  // When
  bool actual = target.empty();

  // Then
  ASSERT_TRUE(actual);
}


TEST(page, returns_remaining_elements_on_last_page_with_smaller_than_page_size) {
  // Given
  auto target = sequence<int>::from({1, 2, 3, 4, 5, 6}).page(1, 4);

  // When
  std::size_t actual = target.count();

  // Then
  ASSERT_EQ(2, actual);
}


TEST(union_with, properly_performs_union) {
   // Given
   auto l = sequence<int>::range(7, 15);
   auto r = sequence<int>::range(0, 10);
   auto expected = sequence<int>::range(0, 15);

   // When
   auto actual = l.union_with(r);

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(intersect_with, properly_performs_intersection) {
   // Given
   auto l = sequence<int>::range(7, 15);
   auto r = sequence<int>::range(0, 10);
   auto expected = { 7, 8, 9 };

   // When
   auto actual = l.intersect_with(r);

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(except, properly_performs_set_difference) {
   // Given
   auto l = sequence<int>::range(0, 15);
   auto r = sequence<int>::range(0, 15).where([](int x) { return x % 2 == 1; });
   auto expected = { 0, 2, 4, 6, 8, 10, 12, 14 };

   // When
   auto actual = l.except(r);

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(symmetric_difference, properly_performs_set_difference) {
   // Given
   auto l = sequence<int>::range(7, 15);
   auto r = sequence<int>::range(0, 10);
   auto expected = { 0, 1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14 };

   // When
   auto actual = l.symmetric_difference(r);

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(from, produces_identical_sequence_as_container) {
   // Given
   std::vector<short> expected = { 1, 2, 3, 9, 8, 7 };

   // When
   auto actual = sequence<short>::from(expected);

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(from, produces_identical_sequence_as_iterator_range) {
   // Given
   std::vector<short> expected = { 1, 2, 3, 9, 8, 7 };

   // When
   auto actual = sequence<short>::from(std::begin(expected), std::end(expected));

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(from, produces_identical_sequence_as_initializer_list) {
   // Given
   std::vector<short> expected = { 1, 2, 3, 9, 8, 7 };

   // When
   auto actual = sequence<short>::from({ 1, 2, 3, 9, 8, 7 });

   // Then
   ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
}


TEST(generate, produces_a_sequence_with_n_elements) {
   // Given
   int x = std::rand() % 53;
   const int start = x;
   std::size_t n = (std::rand() % 13) + 3;
   RecordProperty("start", x);
   RecordProperty("size", n);
   auto generator = [&x]() { return x++; };

   // When
   auto actual = sequence<int>::generate(generator, n);

   // Then
   // Note: We need to test the entire sequence including end-state.
   auto aiter = actual.begin();
   for (std::size_t i = 0; i < n && aiter != actual.end(); ++i, ++aiter) {
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
   auto target = sequence<int>::range(start, finish);

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
   auto target = sequence<int>::range(start, finish);

   // When
   for (int ignore : target) {
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
   auto target = sequence<int>::range(start, finish);

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
   auto target = sequence<int>::range(start, finish);

   // When
   for (int ignore : target) {
      --actual;
   }

   // Then
   ASSERT_EQ(finish, actual);
}


TEST(empty_sequence, does_not_iterate) {
   // Given
   std::size_t invocations = 0;

   // When
   for (int ignored : sequence<int>::empty_sequence()) {
      ++invocations;
   }

   // Then
   ASSERT_EQ(0, invocations);
}


TEST(empty_sequence, is_empty) {
   // Given
   auto target = sequence<float>::empty_sequence();

   // When
   bool actual = target.empty();

   // Then
   ASSERT_TRUE(actual);
}

}


int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
