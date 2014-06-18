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
   ASSERT_THROW(target.max(), std::domain_error);
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
   ASSERT_THROW(target.min(), std::domain_error);
}

}


int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
