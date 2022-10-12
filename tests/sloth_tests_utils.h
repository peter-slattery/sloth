/* date = October 11th 2022 10:34 pm */

#ifndef SLOTH_TESTS_UTILS_H
#define SLOTH_TESTS_UTILS_H

#define sloth_test_assert(c) if (!(c)) { do{ *((volatile int*)0) = 0xFFFF; }while(0); }
#define sloth_r32_abs(v) ((v) < 0 ? -(v) : (v))
#define sloth_r32_equals(a,b) (sloth_r32_abs((a) - (b)) < 0.001f)

Sloth_U32
sloth_test_string_len(char* s)
{
  char* at = s;
  while (*at != 0) at++;
  return (Sloth_U32)(at - s);
}

Sloth_Bool
sloth_test_strings_equal(char* a, char* b)
{
  Sloth_U32 a_len = sloth_test_string_len(a);
  Sloth_U32 b_len = sloth_test_string_len(b);
  if (a_len != b_len) return false;
  for (Sloth_U32 i = 0; i < a_len; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

// Naive string sizing
Sloth_V2
sloth_test_get_text_size(Sloth_Widget* widget)
{
  Sloth_V2 result = {
    .x = widget->text_len * 14,
    .y = 14,
  };
  return result;
}

#endif //SLOTH_TESTS_UTILS_H
