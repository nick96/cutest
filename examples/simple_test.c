#include <limits.h> /* Needed for INT_MIN and INT_MAX */

#include "cutest.h"

test(add_shall_return_0_if_both_inputs_are_0)
{
  assert_eq(0, add(0, 0));
}

test(add_shall_add_the_two_inputs_and_return_their_sum)
{
  assert_eq(1, add(0, 1));
  assert_eq(1, add(1, 0));
  assert_eq(2, add(1, 1));
  assert_eq(3, add(2, 1));
  assert_eq(3, add(1, 2));
}

test(add_shall_be_able_to_add_minimum_values_for_integers)
{
  assert_eq(((long)INT_MIN + (long)INT_MIN), add(INT_MIN, INT_MIN));
}

test(add_shall_be_able_To_add_maximum_values_for_integers)
{
  assert_eq(((long)INT_MAX + (long)INT_MAX), add(INT_MAX, INT_MAX));
}
