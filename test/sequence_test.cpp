#include "../include/sequence.h"
#include <gtest/gtest.h>
#include <iostream>


namespace {

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


TEST(from, produces_identical_sequence) {
   // Given
   std::vector<short> expected = { 1, 2, 3, 9, 8, 7 };
   auto target = sequence<short>::from(expected);

   // When
   bool actual = std::equal(target.begin(), target.end(), expected.begin());

   // Then
   ASSERT_TRUE(actual);
}


TEST(select_many, extracts_single_value_to_many) {
   // Given
   const char expected[] = "hello world";
   std::vector<std::string> strings = { "hello", " ", "world" };
   auto string_seq = sequence<std::string>::from(strings);
   auto target = string_seq.select_many([](std::string s) { return sequence<char>::from(s); });

   // When
   bool actual = std::equal(target.begin(), target.end(), std::begin(expected));

   // Then
   ASSERT_TRUE(actual);
}


TEST(take, returns_all_elements_when_n_is_greater_than_number_of_elements) {
  // Given
  auto target = sequence<int>::from({1, 2, 3, 4, 5, 6}).take(20);

  // When
  std::size_t actual = target.count();

  // Then
  ASSERT_EQ(6, actual);
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


TEST(symmetric_differency, properly_performs_set_difference) {
   // Given
   auto l = sequence<int>::range(7, 15);
   auto r = sequence<int>::range(0, 10);
   auto expected = { 0, 1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14 };

   // When
   auto actual = l.symmetric_difference(r);

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

}


int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
