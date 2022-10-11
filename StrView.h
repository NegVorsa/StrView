/// @file StrView.h
/// @brief header only library for utf8 string
/// @author NegVorsa
/// @date 2022-09-10


#ifdef __cplusplus
extern "C" {
#endif

#ifndef INCLUDE_STR_VIEW_H
#define INCLUDE_STR_VIEW_H

#include <stddef.h>
#include <stdint.h>

typedef int       Bool;
typedef ptrdiff_t Ssize;
typedef size_t    Usize;


/// @brief string view to hold a sequence of 8bit
typedef struct {
  const uint8_t *data;
  Ssize size;
} StrView;

/// @brief string view to hold a valid utf8 sequence
typedef struct {
  const uint8_t *data;
  Ssize size;
} UTF8View;

/// @brief represent result of decoding utf8 sequence
struct CodeResult {
  int32_t codepoint;
  int size;
};

/// codepoint value to represent decoding failure 
#define CODEPOINT_INVALID (-1)
/// value to replace invalide utf8 byte looks like '?'
#define REPLACEMENT_MARCK (0xFFFD)

// usage as compiler flag
//  -DSTR_VIEW_LINK=static
// usage as preprosessor directive before include
//   #define STR_VIEW_LINK static
#ifndef STR_VIEW_LINK
#define STR_VIEW_LINK
#endif



STR_VIEW_LINK  StrView StrView_cast(UTF8View s8);


/// @brief make a string view that hold unspecified sequence
/// @param ptr pointer to a valid memory buffer
/// @param size Size in byte of the buffer
/// @return a string view of arbitrary bytes sequence.
/// @see StrView_from_cstr, UTF8View_from_strview
STR_VIEW_LINK  StrView StrView_from_raw(const void *ptr, Ssize size);


/// @brief make a string view from nul terminated string.
/// @pre a nul terminated string to get the string view size.
/// @param cstr nul terminated string.
/// @return first string view where all bytes are not nul.
/// @see StrView_from_raw, UTF8View_from_strview
///
/// example:
/// @code
///   const char *foobar = "foo\0bar";
///   StrView foo = StrView_cstr(foobar);
/// @endcode
STR_VIEW_LINK StrView StrView_from_cstr(const char *cstr);


/// @brief make utf8 view from string view
/// @param s string view to scan its bytes for valid utf8.
/// @return from s take all starting bytes that represent a valid utf8 sequence.
/// @see StrView_from_raw, StrView_from_cstr
STR_VIEW_LINK UTF8View UTF8View_from_strview(StrView s);


/// @brief count utf8 codepoint with no extra check.
/// @param s8 utf8 view
/// @return number of codepoint in the sequence.
STR_VIEW_LINK Ssize UTF8View_count_codepoint(UTF8View s8);


/// @brief count utf8 codepoint with checking.
/// @param s string view of arbitrary sequence.
/// @return number of codepoint and count of all invalid bytes in the sequence
STR_VIEW_LINK Ssize StrView_count_codepoint(StrView s);


/// @brief get first codepoint from utf8 string view.
/// @pre a valid UTF8 string view.
/// @param s8 a valid utf8 string view
/// @return result struct containing codepoint and its utf8 encoding size.
/// @see StrView_decode
STR_VIEW_LINK struct CodeResult UTF8View_get_codepoint(UTF8View s8);


/// @brief an utf8 decoder with extra check.
/// @param s string view to decode
/// @return if no error a result struct containing codepoint and its utf8 encoding size.
/// in case of invalid utf8 sequence, the value of codepoint is CODEPOINT_INVALID.
/// @see StrView_utf8_get_codepoint
STR_VIEW_LINK struct CodeResult StrView_try_decode(StrView s);



// TODO: doc

STR_VIEW_LINK     bool StrView_is_empty(StrView s);
STR_VIEW_LINK     bool StrView_is_valid_utf8(StrView s);
STR_VIEW_LINK      int StrView_compare(StrView s1, StrView s2);
STR_VIEW_LINK     bool StrView_is_equal(StrView s1, StrView s2);
STR_VIEW_LINK     bool StrView_starts_with(StrView s, StrView prefix);
STR_VIEW_LINK     bool StrView_ends_with(StrView s, StrView suffix);
STR_VIEW_LINK     bool StrView_contains(StrView s, StrView match);
STR_VIEW_LINK  StrView StrView_skip(StrView s, Ssize count);
STR_VIEW_LINK  StrView StrView_skip_if(StrView s, Bool (*predicate)(int c));
STR_VIEW_LINK  StrView StrView_rskip_if(StrView s, Bool (*predicate)(int c));
STR_VIEW_LINK  StrView StrView_trim(StrView s);
STR_VIEW_LINK  StrView StrView_substr(StrView s, Ssize pos, Ssize size);
STR_VIEW_LINK    Ssize StrView_find(StrView s, StrView match);
STR_VIEW_LINK    Ssize StrView_rfind(StrView s, StrView match);
STR_VIEW_LINK    Ssize StrView_index_of(StrView s, uint8_t c);
STR_VIEW_LINK    Ssize StrView_find_first_of(StrView s, StrView accept);
STR_VIEW_LINK    Ssize StrView_find_first_not_of(StrView s, StrView reject);
STR_VIEW_LINK    Ssize StrView_find_last_of(StrView s, StrView accept);
STR_VIEW_LINK    Ssize StrView_find_last_not_of(StrView s, StrView reject);


#endif // INCLUDE_STR_VIEW_H

//
//
//
//
//****************************
#ifdef STR_VIEW_IMPLEMENTATION
//****************************

#include <assert.h>
#include <string.h>
#include <ctype.h>

static struct CodeResult decode_lead_(uint8_t lead) {
  struct CodeResult cr;

  if (lead <= 0x7F) {
    cr.codepoint = lead;
    cr.size = 1;
  } else if ((lead & 0xE0) == 0xC0) {
    cr.codepoint = (lead & 0x1F);
    cr.size = 2;
  } else if ((lead & 0xF0) == 0xE0) {
    cr.codepoint = (lead & 0x0F);
    cr.size = 3;
  } else if ((lead & 0xF8) == 0xF0) {
    cr.codepoint = (lead & 0x07);
    cr.size = 4;
  } else if ((lead & 0xFC) == 0xF8) {
    cr.codepoint = (lead & 0x03);
    cr.size = 5;
  } else if ((lead & 0xFE) == 0xFC) {
    cr.codepoint = (lead & 0x01);
    cr.size = 6;
  } else {
    cr.codepoint = lead; 
    cr.size = 1;
  }

  return cr;
}

static struct CodeResult decode_tail_(CodeResult cr, StrView s) {
  static const struct {
    int size;
    struct {
      int32_t min, max;
    } codepoint_ranges;
    struct {
      uint8_t min, max;
    } bytes_ranges[4];
  } tbl[] = {
      {1, {0x000000, 0x00007F}, {{0x00, 0x7F}}},
      {2, {0x000080, 0x0007FF}, {{0xC2, 0xDF}, {0x80, 0xBF}}},
      {3, {0x000800, 0x000FFF}, {{0xE0, 0xE0}, {0xA0, 0xBF}, {0x80, 0xBF}}},
      {3, {0x001000, 0x00FFFF}, {{0xE1, 0xEF}, {0x80, 0xBF}, {0x80, 0xBF}}},
      {4, {0x010000, 0x03FFFF}, {{0xF0, 0xF0}, {0x90, 0xBF}, {0x80, 0xBF}, {0x80, 0xBF}}},
      {4, {0x040000, 0x0FFFFF}, {{0xF1, 0xF3}, {0x80, 0xBF}, {0x80, 0xBF}, {0x80, 0xBF}}},
      {4, {0x100000, 0x10FFFF}, {{0xF4, 0xF4}, {0x80, 0x8F}, {0x80, 0xBF}, {0x80, 0xBF}}}
    };

  if (cr.size > s.size) {
    cr.codepoint = CODEPOINT_INVALID;
    cr.size = s.size;

    return cr;
  }

  // make sure tail has all continuation bytes.
  for (int i = 1; i < cr.size; ++i) {
    if ((s.data[i] & 0xC0) != 0x80) {
      cr.codepoint = CODEPOINT_INVALID;
      cr.size = i;

      return cr;
    }
  }

  // updating codepoint value
  for (int i = 1; i < cr.size; ++i) {
    cr.codepoint = (cr.codepoint << 6) | (s.data[i] & 0x3F);
  }
  
  // check ranges
  bool is_valid = false;
  for (int i = 0; i != (sizeof(tbl) / sizeof(tbl[0])); ++i) {
    if (cr.size == tbl[i].size) {
      const int32_t c_min = tbl[i].codepoint_ranges.min;
      const int32_t c_max = tbl[i].codepoint_ranges.max;
      if (cr.codepoint >= c_min && cr.codepoint <= c_max) {
        is_valid = true;
        for (int b = 0; b < tbl[i].size; ++b) {
          const uint8_t b_min = tbl[i].bytes_ranges[b].min;
          const uint8_t b_max = tbl[i].bytes_ranges[b].max;
          const uint8_t value = s.data[b];
          is_valid = is_valid && value >= b_min && value <= b_max;
        }
        break;
      }
    }
  }
  if (!is_valid) {
    cr.codepoint = CODEPOINT_INVALID;
  }

  return cr;
}

StrView StrView_cast(UTF8View s8) {
  StrView s = {s8.data, s8.size};
  return s;
}

StrView StrView_from_raw(const void *ptr, Ssize size) {
  StrView s = {(const uint8_t*)ptr, size};
  return s;
}

StrView StrView_from_cstr(const char *s) {
  StrView cstr = {(const uint8_t*)s, (Ssize)strlen(s)};
  return cstr;
}

UTF8View UTF8View_from_strview(StrView s) {
  UTF8View s8 = {s.data, 0};
  
  struct CodeResult cr;
  while (cr = StrView_try_decode(s), cr.codepoint != CODEPOINT_INVALID) {
    s8.size += cr.size;
    s.data  += cr.size;
    s.size  -= cr.size;
  }

  // set size <= 0 for uncomplete decoding
  if (s.size != 0) {
    s8.size = - s8.size;
  }

  return s8;
}

struct CodeResult UTF8View_get_codepoint(UTF8View s8) {
  struct CodeResult cr = {CODEPOINT_INVALID, 0};

  assert(s8.size > 0);
  if (s8.size > 0) {
    cr = decode_lead_(s8.data[0]);
    assert(cr.size <= s8.size);

    for (int i = 1; i < cr.size; ++i) {
      cr.codepoint = (cr.codepoint << 6) | (s8.data[i] & 0x3F);
    }
 }

  return cr;
}

struct CodeResult StrView_try_decode(StrView s) {
  struct CodeResult cr = {CODEPOINT_INVALID, 0};
  
  if (s.size > 0) {
    cr = decode_lead_(s.data[0]);
    cr = decode_tail_(cr, s);
  }
  
  return cr;
}

Ssize UTF8View_count_codepoint(UTF8View s8) {
  Ssize count = 0;
  
  while (s8.size > 0) {
    struct CodeResult cr = UTF8View_get_codepoint(s8);
    s8.data += cr.size;
    s8.size -= cr.size;
    count++;
  }
  
  return count;
}

Ssize StrView_count_codepoint(StrView s) {
  Ssize count = 0;
  
  while (s.size > 0) {
    struct CodeResult cr = StrView_try_decode(s);
    Ssize size = (cr.codepoint != CODEPOINT_INVALID) ? cr.size : 1;

    s.data += size;
    s.size -= size;
    count += size;
  }
  
  return count;
}

bool StrView_is_empty(StrView s) {
  return s.size == 0;
}

bool StrView_is_valid_utf8(StrView s) {
  UTF8View s8 = UTF8View_from_strview(s);
  
  return s8.size == s.size;
}

int StrView_compare(StrView s1, StrView s2) {
  const Ssize min_size = s1.size < s2.size ? s1.size : s2.size;
  assert(min_size >= 0);// FIXME
  
  int result = memcmp(s1.data, s2.data, (Usize)min_size);
  return (result != 0) ? result : s1.size - s2.size;
}

bool StrView_is_equal(StrView s1, StrView s2) {
  assert(s1.size >= 0);

  if (s1.size == s2.size) return !memcmp(s1.data, s2.data, (Usize)s1.size);

  return false;
}

bool StrView_starts_with(StrView s, StrView prefix) {
  assert(prefix.size >= 0);

  if (s.size >= prefix.size)
    return memcmp(s.data, prefix.data, (Usize)prefix.size) == 0;

  return false;
}

bool StrView_ends_with(StrView s, StrView suffix) {
  assert(suffix.size >= 0);

  if (s.size >= suffix.size) {
    const Ssize pos = s.size - suffix.size;
    return memcmp(s.data + pos, suffix.data, (Usize)suffix.size) == 0;
  }

  return false;
}

bool StrView_contains(StrView s, StrView match) {
  return StrView_find(s, match) >= 0;
}

StrView StrView_skip(StrView s, Ssize count) {
  assert(count >= 0 && count <= s.size); // FIXME
    
  s.data += count;
  s.size -= count;
  
  return s;
}

StrView StrView_skip_if(StrView s, Bool (*predicate)(int c)) {
  Ssize i = 0;
  while( i < s.size && predicate(s.data[i])) {
    ++i;
  }
  s.data += i;
  s.size -= i;
  
  return s;
}

StrView StrView_rskip_if(StrView s, Bool (*predicate)(int c)) {
  Ssize i = 0;
  while(i < s.size && predicate(s.data[s.size - 1 - i])) {
    ++i;
  }
  s.size -= i;
  
  return s;
}

StrView StrView_trim(StrView s) {
  return StrView_rskip_if(StrView_skip_if(s, isspace), isspace);
}

StrView StrView_substr(StrView s, Ssize pos, Ssize size) {
  StrView sub = {NULL, 0};  

  if (pos < 0) pos += s.size;
  if (pos >= 0 && pos <= s.size) {
    const Ssize max_size = s.size - pos;
    sub.data = s.data + pos;
    sub.size = ( size >= 0 && size <= max_size) ? size : max_size;
  }

  return sub;
}

Ssize StrView_find(StrView s, StrView match) {
  assert(match.size >= 0);

  Ssize pos = 0;
  while (s.size - pos >= match.size) {
    if (memcmp(s.data + pos, match.data, (Usize)match.size) == 0) return pos;
    pos++;
  }

  return -1;
}

Ssize StrView_rfind(StrView s, StrView match) {
  assert(match.size >= 0);

  Ssize pos = s.size - match.size;
  while (pos >= 0) {
    if (memcmp(s.data + pos, match.data, (Usize)match.size) == 0) break;
    pos--;
  }

  return pos;
}

Ssize StrView_index_of(StrView s, uint8_t c) {
  for (Ssize i = 0; i < s.size; ++i) {
    if (s.data[i] == c) return i;
  }

  return -1;
}

Ssize StrView_find_first_of(StrView s, StrView accept) {
  uint8_t freq[256] = {0};

  for (Ssize i = 0; i < accept.size; ++i) {
    freq[accept.data[i]]++;
  }
  
  for (Ssize pos = 0; pos < s.size; ++pos) {
    if (freq[s.data[pos]] != 0) return pos;
  }
  
  return -1;
}

Ssize StrView_find_first_not_of(StrView s, StrView reject) {
  uint8_t freq[256] = {0};

  for (Ssize i = 0; i < reject.size; ++i) {
    freq[reject.data[i]]++;
  }
  
  for (Ssize pos = 0; pos < s.size; ++pos) {
    if (freq[s.data[pos]] == 0) return pos;
  }
  
  return -1;
}

Ssize StrView_find_last_of(StrView s, StrView accept) {
  uint8_t freq[256] = {0};

  for (Ssize i = 0; i < accept.size; ++i) {
    freq[accept.data[i]]++;
  }
  
  Ssize pos = s.size;
  while (--pos >= 0) {
    if (freq[s.data[pos]] != 0) break;
  }
  
  return pos;
}

Ssize StrView_find_last_not_of(StrView s, StrView reject) {
  uint8_t freq[256] = {0};

  for (Ssize i = 0; i < reject.size; ++i) {
    freq[reject.data[i]]++;
  }
  
  Ssize pos = s.size;
  while (--pos >= 0) {
    if (freq[s.data[pos]] == 0) break;
  }

  return pos;
}

#endif // STR_VIEW_IMPLEMENTATION


#ifdef __cplusplus
} // extern "C"
#endif
