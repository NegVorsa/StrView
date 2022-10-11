#include <catch2/catch_test_macros.hpp>
//
#define STR_VIEW_IMPLEMENTATION
#include "StrView.h"


TEST_CASE("StrView_is_equal", "[StrView]") {
  StrView foo = StrView_from_cstr("Foo");
  StrView empty  = StrView_from_cstr("");

  SECTION("\"\" eq \"\"") {
    REQUIRE(StrView_is_equal(empty, empty));
  }
  SECTION("\"Foo\" eq \"\"") {
    REQUIRE_FALSE(StrView_is_equal(foo, empty));
  }
  SECTION("\"Foo\" eq \"Foo\"") {
    REQUIRE(StrView_is_equal(foo, StrView_from_cstr("Foo")));
  }
  SECTION(" \"Foo\" eq \"foo\"") {
    REQUIRE(!StrView_is_equal(foo, StrView_from_cstr("foo")));
  }
}

TEST_CASE("StrView_starts_with", "[StrView]") {
  StrView foobar = StrView_from_cstr("FooBar");
  StrView empty  = StrView_from_cstr("");

  SECTION("\"FooBar\" starts with \"\"") {
    REQUIRE(StrView_starts_with(foobar, empty));
  }
  SECTION("\"FooBar\" starts with \"Foo\"") {
    REQUIRE(StrView_starts_with(foobar, StrView_from_cstr("Foo")));
  }
  SECTION("\"FooBar\" starts with \"Bar\"") {
    REQUIRE_FALSE(StrView_starts_with(foobar, StrView_from_cstr("Bar")));
  }
}

TEST_CASE("StrView_ends_with", "[StrView]") {
  StrView foobar = StrView_from_cstr("FooBar");

  SECTION("\"FooBar\" ends with \"\"") {
    REQUIRE(StrView_ends_with(foobar, StrView_from_cstr("")));
  }
  SECTION("\"FooBar\" ends with \"Foo\"") {
    REQUIRE_FALSE(StrView_ends_with(foobar, StrView_from_cstr("Foo")));
  }
  SECTION("\"FooBar\" ends with \"Bar\"") {
    REQUIRE(StrView_ends_with(foobar, StrView_from_cstr("Bar")));
  }
}

TEST_CASE("StrView_contains", "[StrView]") {
  StrView empty  = StrView_from_cstr("");

  SECTION("\"\" contains \"\"") {
    REQUIRE(StrView_contains(empty, empty));
  }
  SECTION("\"Foo\" contains \"\"") {
    REQUIRE(StrView_contains(StrView_from_cstr("Foo"), empty));
  }
  SECTION("\"\" contains \"x\"") {
    REQUIRE_FALSE(StrView_contains(empty, StrView_from_cstr("x")));
  }
}

TEST_CASE("StrView_skip_if", "[StrView]") {
  SECTION("skip space in \"  Foo\"") {
    StrView foo = StrView_skip_if(StrView_from_cstr("  Foo"), isspace);
    REQUIRE(StrView_is_equal(foo, StrView_from_cstr("Foo")));
  }
}

TEST_CASE("StrView_trim", "[StrView]") {
  SECTION("trim in \"  Foo  \"") {
    StrView foo = StrView_trim(StrView_from_cstr("  Foo  "));
    REQUIRE(StrView_is_equal(foo, StrView_from_cstr("Foo")));
  }
}

TEST_CASE("StrView_substr", "[StrView]") {
  StrView foobar = StrView_from_cstr("FooBar");
  StrView bar    = StrView_from_cstr("Bar");
  StrView empty  = StrView_from_cstr("");
  
  SECTION("\"\" substr(3, -1) is \"\"") {
    StrView sub = StrView_substr(empty, 3, -1);
    REQUIRE(StrView_is_equal(sub, empty));
  }
  SECTION("\"FooBar\" substr(3, 0) is \"\"") {
    StrView sub = StrView_substr(foobar, 3, 0);
    REQUIRE(StrView_is_equal(sub, empty));
  }
  SECTION("\"FooBar\" substr(3, 3) is \"Bar\"") {
    StrView sub = StrView_substr(foobar, 3, 3);
    REQUIRE(StrView_is_equal(sub, bar));
  }
  SECTION("\"FooBar\" substr(3, 10) is \"Bar\"") {
    StrView sub = StrView_substr(foobar, 3, 10);
    REQUIRE(StrView_is_equal(sub, bar));
  }
  SECTION("\"FooBar\" substr(3, -1) is \"Bar\"") {
    StrView sub = StrView_substr(foobar, 3, -1);
    REQUIRE(StrView_is_equal(sub, bar));
  }
  SECTION("\"FooBar\" substr(-3, 3) is \"Bar\"") {
    StrView sub = StrView_substr(foobar, -3, 3);
    REQUIRE(StrView_is_equal(sub, bar));
  }
  SECTION("\"FooBar\" substr(-3, 0) is \"\"") {
    StrView sub = StrView_substr(foobar, -3, 0);
    REQUIRE(StrView_is_equal(sub, empty));
  }
  SECTION("\"FooBar\" substr(-3, -1) is \"Bar\"") {
    StrView sub = StrView_substr(foobar, -3, -1);
    REQUIRE(StrView_is_equal(sub, bar));
  }
  SECTION("\"FooBar\" substr(-10, 3) is \"\"") {
    StrView sub = StrView_substr(foobar, -10, 3);
    REQUIRE(StrView_is_equal(sub, empty));
  }
}

TEST_CASE("StrView_find", "[StrView]") {
  StrView foo = StrView_from_cstr("Foo");
  StrView empty  = StrView_from_cstr("");

  SECTION("pos in \"\" of \"\"") {
    REQUIRE(StrView_find(empty, empty) == 0);
  }
  SECTION("pos in \"\" of \"x\"") {
    REQUIRE(StrView_find(empty, StrView_from_cstr("x")) < 0);
  }
  SECTION("pos in \"Foo\" of \"\"") {
    REQUIRE(StrView_find(foo, empty) == 0);
  }
  SECTION("pos in \"Foo\" of \"F\"") {
    REQUIRE(StrView_find(foo, StrView_from_cstr("F")) == 0);
  }
  SECTION("pos in \"Foo\" of \"o\"") {
    REQUIRE(StrView_find(foo, StrView_from_cstr("o")) == 1);
  }
  SECTION("pos in \"Foo\" of \"oo\"") {
    REQUIRE(StrView_find(foo, StrView_from_cstr("oo")) == 1);
  }
  SECTION("pos in \"Foo\" of \"x\"") {
    REQUIRE(StrView_find(foo, StrView_from_cstr("x")) < 0);
  }
  SECTION("pos in \"Foo\" of \"xx\"") {
    REQUIRE(StrView_find(foo, StrView_from_cstr("xx")) < 0);
  }
  SECTION("pos in \"Foo\" of \"Foox\"") {
    REQUIRE(StrView_find(foo, StrView_from_cstr("Foox")) < 0);
  }
}

TEST_CASE("StrView_rfind", "[StrView]") {
  StrView foo = StrView_from_cstr("Foo");
  StrView empty  = StrView_from_cstr("");

  SECTION("pos in \"\" of \"\"") {
    REQUIRE(StrView_rfind(empty, empty) == 0);
  }
  SECTION("pos in \"\" of \"x\"") {
    REQUIRE(StrView_rfind(empty, StrView_from_cstr("x")) < 0);
  }
  SECTION("pos in \"Foo\" of \"\"") {
    REQUIRE(StrView_rfind(foo, empty) == 3);
  }
  SECTION("pos in \"Foo\" of \"F\"") {
    REQUIRE(StrView_rfind(foo, StrView_from_cstr("F")) == 0);
  }
  SECTION("pos in \"Foo\" of \"o\"") {
    REQUIRE(StrView_rfind(foo, StrView_from_cstr("o")) == 2);
  }
  SECTION("pos in \"Foo\" of \"oo\"") {
    REQUIRE(StrView_rfind(foo, StrView_from_cstr("oo")) == 1);
  }
  SECTION("pos in \"Fooo\" of \"oo\"") {
    REQUIRE(StrView_rfind(StrView_from_cstr("Fooo"), StrView_from_cstr("oo")) == 2);
  }
  SECTION("pos in \"Foo\" of \"x\"") {
    REQUIRE(StrView_rfind(foo, StrView_from_cstr("x")) < 0);
  }
  SECTION("pos in \"Foo\" of \"xx\"") {
    REQUIRE(StrView_rfind(foo, StrView_from_cstr("xx")) < 0);
  }
  SECTION("pos in \"Foo\" of \"Foox\"") {
    REQUIRE(StrView_rfind(foo, StrView_from_cstr("Foox")) < 0);
  }
}

TEST_CASE("StrView_index_of", "[StrView]") {
  StrView foo = StrView_from_cstr("Foo");

  SECTION("in \"Foo\" index of \'o\'") {
    REQUIRE(StrView_index_of(foo, 'o') == 1);
  }
  SECTION("in \"Foo\" index of \'x\'") {
    REQUIRE(StrView_index_of(foo, 'x') < 0);
  }
}

TEST_CASE("StrView_find_first_of", "[StrView]") {
  StrView foobar = StrView_from_cstr("FooBar");

  SECTION("in \"FooBar\" first of \"\"") {
    REQUIRE(StrView_find_first_of(foobar, StrView_from_cstr("")) < 0);
  }
  SECTION("in \"FooBar\" first of \"xyz\"") {
    REQUIRE(StrView_find_first_of(foobar, StrView_from_cstr("xyz")) < 0);
  }
  SECTION("in \"FooBar\" of \"rao\"") {
    REQUIRE(StrView_find_first_of(foobar, StrView_from_cstr("rao")) == 1);
  }
}

TEST_CASE("StrView_find_first_not_of", "[StrView]") {
  StrView foobar = StrView_from_cstr("FooBar");

  SECTION("in \"FooBar\" first not of \"\"") {
    REQUIRE(StrView_find_first_not_of(foobar, StrView_from_cstr("")) == 0);
  }
  SECTION("in \"FooBar\" first not of \"Fxy\"") {
    REQUIRE(StrView_find_first_not_of(foobar, StrView_from_cstr("Fxy")) == 1);
  }
  SECTION("in \"FooBar\" first not of \"FoBar\"") {
    REQUIRE(StrView_find_first_not_of(foobar, StrView_from_cstr("FoBar")) < 0);
  }
}

TEST_CASE("StrView_find_last_of", "[StrView]") {
  StrView foobar = StrView_from_cstr("FooBar");

  SECTION("in \"FooBar\" last of \"\"") {
    REQUIRE(StrView_find_last_of(foobar, StrView_from_cstr("")) < 0);
  }
  SECTION("in \"FooBar\" last of \"xyz\"") {
    REQUIRE(StrView_find_last_of(foobar, StrView_from_cstr("xyz")) < 0);
  }
  SECTION("in \"FooBar\" last of \"rao\"") {
    REQUIRE(StrView_find_last_of(foobar, StrView_from_cstr("rao")) == 5);
  }
}

TEST_CASE("StrView_find_last_not_of", "[StrView]") {
  StrView foobar = StrView_from_cstr("FooBar");

  SECTION("in \"FooBar\" last not of \"\"") {
    REQUIRE(StrView_find_last_not_of(foobar, StrView_from_cstr("")) == 5);
  }
  SECTION("in \"FooBar\" last not of \"xyz\"") {
    REQUIRE(StrView_find_last_not_of(foobar, StrView_from_cstr("xyz"))  == 5);
  }
  SECTION("in \"FooBar\" last not of \"rao\"") {
    REQUIRE(StrView_find_last_not_of(foobar, StrView_from_cstr("rao")) == 3);
  }
}

TEST_CASE("UTF8View_get_decode", "[UTF8View]") {
  SECTION("U+00200000") {
    const uint8_t data[] = {0xF8, 0x88, 0x80, 0x80, 0x80};
    UTF8View s8 = {data, sizeof(data)};
    struct CodeResult cr = UTF8View_get_codepoint(s8);
    REQUIRE(cr.codepoint == 0x200000);
    REQUIRE(cr.size == 5);
  }
  SECTION("U+04000000") {
    const uint8_t data[] = {0xFC, 0x84, 0x80, 0x80, 0x80, 0x80};
    UTF8View s8 = {data, sizeof(data)};
    struct CodeResult cr = UTF8View_get_codepoint(s8);
    REQUIRE(cr.codepoint == 0x4000000);
    REQUIRE(cr.size == 6);
  }
  SECTION("U+001FFFFF") {
    const uint8_t data[] = {0xF7, 0xBF, 0xBF, 0xBF};
    UTF8View s8 = {data, sizeof(data)};
    struct CodeResult cr = UTF8View_get_codepoint(s8);
    REQUIRE(cr.codepoint == 0x1FFFFF);
    REQUIRE(cr.size == 4);
  }
  SECTION("U+03FFFFFF") {
    const uint8_t data[] = {0xFB, 0xBF, 0xBF, 0xBF, 0xBF};
    UTF8View s8 = {data, sizeof(data)};
    struct CodeResult cr = UTF8View_get_codepoint(s8);
    REQUIRE(cr.codepoint == 0x3FFFFFF);
    REQUIRE(cr.size == 5);
  }
  SECTION("U+7FFFFFFF") {
    const uint8_t data[] = {0xFD, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF};
    UTF8View s8 = {data, sizeof(data)};
    struct CodeResult cr = UTF8View_get_codepoint(s8);
    REQUIRE(cr.codepoint == 0x7FFFFFFF);
    REQUIRE(cr.size == 6);
  }
}

TEST_CASE("StrView_try_decode", "[StrView]") {
  SECTION("TODO name") {
    SECTION("ASCII") {
      StrView s = StrView_from_cstr("ASCII");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == 'A');
      REQUIRE(cr.size == 1);
    }
    SECTION("U+00000000") {
      const uint8_t s[] = {0};
      struct CodeResult cr = StrView_try_decode(StrView_from_raw(s, sizeof(s)));
      REQUIRE(cr.codepoint == 0);
      REQUIRE(cr.size == 1);
    }
    SECTION("U+00000080") {
      StrView s = StrView_from_cstr("\xC2\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == 0x80);
      REQUIRE(cr.size == 2);
    }
    SECTION("U+00000800") {
      StrView s = StrView_from_cstr("\xE0\xA0\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == 0x800);
      REQUIRE(cr.size == 3);
    }
    SECTION("U+00010000") {
      StrView s = StrView_from_cstr("\xF0\x90\x80\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == 0x10000);
      REQUIRE(cr.size == 4);
    }
    SECTION("U+00200000") {
      StrView s = StrView_from_cstr("\xF8\x88\x80\x80\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 5);
    }
    SECTION("U+04000000") {
      StrView s = StrView_from_cstr("\xFC\x84\x80\x80\x80\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 6);
    }
    SECTION("U+0000007F") {
      StrView s = StrView_from_cstr("\x7F");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == 0x7F);
      REQUIRE(cr.size == 1);
    }
    SECTION("U+000007FF") {
      StrView s = StrView_from_cstr("\xDF\xBF");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == 0x7FF);
      REQUIRE(cr.size == 2);
    }
    SECTION("U+0000FFFF") {
      StrView s = StrView_from_cstr("\xEF\xBF\xBF");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == 0xFFFF);
      REQUIRE(cr.size == 3);
    }
    SECTION("U+001FFFFF") {
      StrView s = StrView_from_cstr("\xF7\xBF\xBF\xBF");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 4);
    }
    SECTION("U+03FFFFFF") {
      StrView s = StrView_from_cstr("\xFB\xBF\xBF\xBF\xBF");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 5);
    }
    SECTION("U+7FFFFFFF") {
      StrView s = StrView_from_cstr("\xFD\xBF\xBF\xBF\xBF\xBF");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 6);
    }
  }
  SECTION("other boundary conditions") {
    SECTION("U+0000D7FF") {
      StrView s = StrView_from_cstr("\xED\x9F\xBF");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == 0xD7FF);
      REQUIRE(cr.size == 3);
    }
    SECTION("U+0000E000") {
      StrView s = StrView_from_cstr("\xEE\x80\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == 0xE000);
      REQUIRE(cr.size == 3);
    }
    // FIXME
    SECTION("U+0000FFFD") {
      StrView s = StrView_from_cstr("\xEF\xBF\xBD");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == 0xFFFD);
      REQUIRE(cr.size == 3);
    }
    SECTION("U+0010FFFF") {
      StrView s = StrView_from_cstr("\xF4\x8F\xBF\xBF");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == 0x10FFFF);
      REQUIRE(cr.size == 4);
    }
    SECTION("U+00110000") {
      StrView s = StrView_from_cstr("\xF4\x90\x80\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 4);
    }
  }
  
  SECTION("Malformed sequences") {
    SECTION("1 byte 0x80") {
      StrView s = StrView_from_cstr("\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 1);
    }
    SECTION("1 byte 0xBF") {
      StrView s = StrView_from_cstr("\xBF");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 1);
    }
    SECTION("2 bytes 0x80") {
      StrView s = StrView_from_cstr("\x80\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 1);
    }
    SECTION("3 bytes 0x80") {
      StrView s = StrView_from_cstr("\x80\x80\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 1);
    }
    SECTION("4 bytes 0x80") {
      StrView s = StrView_from_cstr("\x80\x80\x80\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 1);
    }
    SECTION("5 bytes 0x80") {
      StrView s = StrView_from_cstr("\x80\x80\x80\x80\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 1);
    }
    SECTION("6 bytes 0x80") {
      StrView s = StrView_from_cstr("\x80\x80\x80\x80\x80\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 1);
    }
    SECTION("7 bytes 0x80") {
      StrView s = StrView_from_cstr("\x80\x80\x80\x80\x80\x80\x80");
      struct CodeResult cr = StrView_try_decode(s);
      REQUIRE(cr.codepoint == CODEPOINT_INVALID);
      REQUIRE(cr.size == 1);
    }
  }
}
