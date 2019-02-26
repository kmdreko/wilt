////////////////////////////////////////////////////////////////////////////////
// FILE: narraytests.cpp
// DATE: 2018-11-14
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: Tests for the NArray class

////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Trevor Wilson
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions :
// 
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cassert>

#include "../wilt-narray/narray.hpp"
#include "tests.h"

class NoDefault
{
public:
  NoDefault() = delete;
};

class TrackDefault
{
public:
  TrackDefault() { ++count; }
  static int count;
  static void reset() { count = 0; }
};
int TrackDefault::count = 0;

class TrackCopy
{
public:
  TrackCopy() {}
  TrackCopy(const TrackCopy&) { ++count; }
  static int count;
  static void reset() { count = 0; }
};
int TrackCopy::count = 0;

void testDefaultConstructorHasSizeZero()
{
  { // T=int N=1
    wilt::NArray<int, 1> a;

    assert(a.size() == 0);
    assert(a.sizes() == wilt::Point<1>({ 0 }));
  }

  { // T=int N=2
    wilt::NArray<int, 2> a;

    assert(a.size() == 0);
    assert(a.sizes() == wilt::Point<2>({ 0, 0 }));
  }

  { // T=int N=5
    wilt::NArray<int, 5> a;

    assert(a.size() == 0);
    assert(a.sizes() == wilt::Point<5>({ 0, 0, 0, 0, 0 }));
  }

  { // T=NoDefault N=1
    wilt::NArray<NoDefault, 1> a;

    assert(a.size() == 0);
    assert(a.sizes() == wilt::Point<1>({ 0 }));
  }

  { // T=TrackDefault N=1
    TrackDefault::reset();

    wilt::NArray<TrackDefault, 1> a;

    assert(a.size() == 0);
    assert(a.sizes() == wilt::Point<1>({ 0 }));
    assert(TrackDefault::count == 0);
  }
}

void testSizedConstructorHasConstructedSize()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 3 });

    assert(a.size() == 3);
    assert(a.sizes() == wilt::Point<1>({ 3 }));
  }

  { // T=int N=2
    wilt::NArray<int, 2> a({ 3, 2 });

    assert(a.size() == 6);
    assert(a.sizes() == wilt::Point<2>({ 3, 2 }));
  }

  { // T=int N=5
    wilt::NArray<int, 5> a({ 3, 2, 5, 1, 7 });

    assert(a.size() == 210);
    assert(a.sizes() == wilt::Point<5>({ 3, 2, 5, 1, 7 }));
  }

  { // T=TrackDefault N=1
    TrackDefault::reset();

    wilt::NArray<TrackDefault, 1> a({ 3 });

    assert(a.size() == 3);
    assert(a.sizes() == wilt::Point<1>({ 3 }));
    assert(TrackDefault::count == 3);
  }
}

void testSizedWithDefaultConstructorHasConstructedSize()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 3 }, 1);

    assert(a.size() == 3);
    assert(a.sizes() == wilt::Point<1>({ 3 }));
  }

  { // T=int N=2
    wilt::NArray<int, 2> a({ 3, 2 }, 1);

    assert(a.size() == 6);
    assert(a.sizes() == wilt::Point<2>({ 3, 2 }));
  }

  { // T=int N=5
    wilt::NArray<int, 5> a({ 3, 2, 5, 1, 7 }, 1);

    assert(a.size() == 210);
    assert(a.sizes() == wilt::Point<5>({ 3, 2, 5, 1, 7 }));
  }

  { // T=TrackCopy N=1
    TrackCopy::reset();

    wilt::NArray<TrackCopy, 1> a({ 3 }, TrackCopy{});

    assert(a.size() == 3);
    assert(a.sizes() == wilt::Point<1>({ 3 }));
    assert(TrackCopy::count == 3);
  }
}

void testCopyConstructorSharesData()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 3 }, 0);
    wilt::NArray<int, 1> b(a);
    b[1] = 1;

    assert(a.size() == b.size());
    assert(a.sizes() == b.sizes());
    assert(a[1] == 1);
    assert(&a[0] == &b[0]);
  }
}

void testConstNArrayConstructor()
{
  { // T=TrackDefault
    wilt::NArray<TrackDefault, 1> a({ 3 });
    TrackDefault::reset();
    wilt::NArray<const TrackDefault, 1> b(a);

    assert(TrackDefault::count == 0);
  }

  { // T=TrackCopy
    wilt::NArray<TrackCopy, 1> a({ 3 });
    TrackCopy::reset();
    wilt::NArray<const TrackCopy, 1> b(a);

    assert(TrackCopy::count == 0);
  }

  { // (const T) -> (T) : doesn't compile
    //wilt::NArray<const int, 1> a({ 3 });
    //wilt::NArray<int, 1> b(a);
  }

  { // (const T).clone() -> (T) : does compile
    wilt::NArray<const int, 1> a({ 3 });
    wilt::NArray<int, 1> b(a.clone());
  }
}

void testCloneConstructsNewArrayCorrectly()
{
  { // T=int N=3
    wilt::NArray<int, 3> a({ 3, 4, 5 }, 1);
    wilt::NArray<int, 3> b = a.flipY().transpose();
    wilt::NArray<int, 3> c = b.clone();
    b[0][0][0] = 5;

    assert(c.size() == a.size());
    assert(c.sizes() == wilt::Point<3>({ 4, 3, 5 }));
    assert(c.steps() == wilt::Point<3>({ 15, 5, 1 }));
    for (auto v : c)
      assert(v == 1);
  }

  { // makes exactly N copies
    wilt::NArray<TrackCopy, 1> a({ 3 });
    TrackCopy::reset();
    wilt::NArray<TrackCopy, 1> b = a.clone();

    assert(TrackCopy::count == 3);
  }

  { // does not default construct any elements
    wilt::NArray<TrackDefault, 1> a({ 3 });
    TrackDefault::reset();
    wilt::NArray<TrackDefault, 1> b = a.clone();

    assert(TrackDefault::count == 0);
  }
}

void testAsCondensedCreatesCorrectNArray()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 12 });

    wilt::NArray<int, 1> b = a.asCondensed();
    assert(b.sizes() == wilt::Point<1>({ 12 }));
    assert(b.steps() == wilt::Point<1>({ 1 }));

    wilt::NArray<int, 1> c = a.flipX().asCondensed();
    assert(c.sizes() == wilt::Point<1>({ 12 }));
    assert(c.steps() == wilt::Point<1>({ -1 }));
  }

  { // T=int N=1
    wilt::NArray<int, 3> a;

    wilt::NArray<int, 3> b = a.asCondensed();
    assert(b.sizes() == wilt::Point<3>({ 0, 0, 0 }));
    assert(b.steps() == wilt::Point<3>({ 0, 0, 0 }));
  }

  { // T=int N=3
    wilt::NArray<int, 3> a({ 2, 3, 4 });

    wilt::NArray<int, 3> b = a.asCondensed();
    assert(b.sizes() == wilt::Point<3>({ 1, 1, 24 }));
    assert(b.steps() == wilt::Point<3>({ 24, 24, 1 }));

    wilt::NArray<int, 3> e = a.flipX().flipY().flipZ().asCondensed();
    assert(e.sizes() == wilt::Point<3>({ 1, 1, 24 }));
    assert(e.steps() == wilt::Point<3>({ 24, 24, -1 }));

    wilt::NArray<int, 3> c = a.flipX().asCondensed();
    assert(c.sizes() == wilt::Point<3>({ 1, 2, 12 }));
    assert(c.steps() == wilt::Point<3>({ 24, -12, 1 }));

    wilt::NArray<int, 3> d = a.subarray({ 0, 0, 0 }, { 1, 2, 3 }).asCondensed();
    assert(d.sizes() == wilt::Point<3>({ 1, 2, 3 }));
    assert(d.steps() == wilt::Point<3>({ 12, 4, 1 }));
  }
}

void testSkipNCreatesCorrectNArray()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 5 });

    wilt::NArray<int, 1> b = a.skipX(2);
    assert(b.sizes() == wilt::Point<1>({ 3 }));
    assert(b.steps() == wilt::Point<1>({ 2 }));

    wilt::NArray<int, 1> c = a.skipX(2, 1);
    assert(c.sizes() == wilt::Point<1>({ 2 }));
    assert(c.steps() == wilt::Point<1>({ 2 }));

    wilt::NArray<int, 1> d = a.skipX(1);
    assert(d.sizes() == wilt::Point<1>({ 5 }));
    assert(d.steps() == wilt::Point<1>({ 1 }));

    wilt::NArray<int, 1> e = a.skipX(1, 2);
    assert(e.sizes() == wilt::Point<1>({ 3 }));
    assert(e.steps() == wilt::Point<1>({ 1 }));
  }

  { // T=int N=3
    wilt::NArray<int, 3> a({ 3, 3, 3 });

    wilt::NArray<int, 3> b = a.skipY(2).skipZ(2);
    assert(b.sizes() == wilt::Point<3>({ 3, 2, 2 }));
    assert(b.steps() == wilt::Point<3>({ 9, 6, 2 }));
  }
}

void testReshapeCreatesCorrectNArray()
{
  { // T=int N=2
    wilt::NArray<int, 2> a({ 14, 14 });

    wilt::NArray<int, 4> b = a.subarray({ 1, 1 }, { 12, 12 }).reshape<4>({ 4, 3, 4, 3 });
    assert(b.sizes() == wilt::Point<4>({ 4, 3, 4, 3 }));
    assert(b.steps() == wilt::Point<4>({ 42, 14, 3, 1 }));

    wilt::NArray<int, 2> c = a.reshape<2>({ 98, 2 });
    assert(c.sizes() == wilt::Point<2>({ 98, 2 }));
    assert(c.steps() == wilt::Point<2>({ 2, 1 }));

    wilt::NArray<int, 5> d = a.reshape<5>({ 1, 98, 1, 2, 1 });
    assert(d.sizes() == wilt::Point<5>({ 1, 98, 1, 2, 1 }));
    assert(d.steps() == wilt::Point<5>({ 196, 2, 2, 1, 1 }));

    wilt::NArray<int, 2> e = a.flipX().flipY().reshape<2>({ 98, 2 });
    assert(e.sizes() == wilt::Point<2>({ 98, 2 }));
    assert(e.steps() == wilt::Point<2>({ -2, -1 }));
  }
}

void testRepeatCreatesCorrectNArray()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 1 }, 5);

    wilt::NArray<int, 2> b = a.repeat(100);
    assert(b.size() == 100);
    assert(b.sizes() == wilt::Point<2>({ 1, 100 }));
    assert(b.steps() == wilt::Point<2>({ 1, 0 }));
    assert(&b.at({ 0, 0 }) == &b.at({ 0, 99 }));

    wilt::NArray<int, 1> c = a.repeat(100).sliceX(0);
    assert(c.size() == 100);
    assert(c.sizes() == wilt::Point<1>({ 100 }));
    assert(c.steps() == wilt::Point<1>({ 0 }));
    assert(&c.at({ 0 }) == &c.at({ 99 }));

    wilt::NArray<int, 3> d = a.repeat(100).reshape<3>({ 1, 5, 20 }).transpose();
    assert(d.size() == 100);
    assert(d.sizes() == wilt::Point<3>({ 5, 1, 20 }));
    assert(d.steps() == wilt::Point<3>({ 0, 1, 0 }));
    assert(&d.at({ 0, 0, 0 }) == &d.at({ 4, 0, 19 }));
  }
}

void testWindowCreatesCorrectNArray()
{
  { // T=int N=2
    wilt::NArray<int, 2> a({ 10, 10 }, 5);

    wilt::NArray<int, 4> b = a.windowX(3).windowY(3);
    assert(b.size() == 576);
    assert(b.sizes() == wilt::Point<4>({ 8, 8, 3, 3 }));
    assert(b.steps() == wilt::Point<4>({ 10, 1, 10, 1 }));
    assert(&b.at({ 2, 0, 0, 0 }) == &b.at({ 0, 0, 2, 0 }));
    assert(&b.at({ 0, 2, 0, 0 }) == &b.at({ 0, 0, 0, 2 }));
    assert(&b.at({ 7, 7, 0, 0 }) == &b.at({ 5, 5, 2, 2 }));
  }
}

void testSubarraysIteratesCorrectly()
{
  {
    int count;
    wilt::NArray<int, 3> a({ 2, 3, 4 }, 5);

    count = 0;
    for (auto arr : a.subarrays<2>()) {
      assert((std::is_same<decltype(arr), wilt::NArray<int, 2>>::value));
      assert(arr.size() == 12);
      assert(arr.sizes() == wilt::Point<2>({ 3, 4 }));
      count++;
    }
    assert(count == 2);

    count = 0;
    for (auto arr : a.subarrays<1>()) {
      assert((std::is_same<decltype(arr), wilt::NArray<int, 1>>::value));
      assert(arr.size() == 4);
      assert(arr.sizes() == wilt::Point<1>({ 4 }));
      count++;
    }
    assert(count == 6);

    count = 0;
    for (auto arr : a.subarrays<0>()) {
      assert((std::is_same<decltype(arr), int>::value));
      assert(arr == 5);
      count++;
    }
    assert(count == 24);
  }
}

void testCompressCreatesCorrectNArray()
{
  { // T=int N=3
    wilt::NArray<int, 3> a({ 2, 3, 4 }, 5);

    wilt::NArray<int, 1> b = a.compress<1>([](wilt::NArray<int, 2> m) { return m[0][0] + 1; });
    assert(b.size() == 2);
    for (auto v : b)
      assert(v == 6);

    wilt::NArray<int, 2> c = a.compress<2>([](wilt::NArray<int, 1> m) { return m[0] + 1; });
    assert(c.size() == 6);
    for (auto v : c)
      assert(v == 6);
  }
}

void testNArrayIteratorConstructor()
{
  { // T=int N=2 with smaller array
    int data[] = { 1, 2, 3 };

    wilt::NArray<int, 2> a({ 2, 2 }, std::begin(data), std::end(data));
    assert(a[0][0] == 1);
    assert(a[0][1] == 2);
    assert(a[1][0] == 3);
    assert(a[1][1] == 0);
  }

  { // T=int N=2 with larger array
    int data[] = { 1, 2, 3, 4, 5 };

    wilt::NArray<int, 2> a({ 2, 2 }, std::begin(data), std::end(data));
    assert(a[0][0] == 1);
    assert(a[0][1] == 2);
    assert(a[1][0] == 3);
    assert(a[1][1] == 4);
  }

  { // T=TrackDefault N=2 with smaller array
    TrackDefault data[3];

    TrackDefault::reset();
    wilt::NArray<TrackDefault, 2> a({ 2, 2 }, std::begin(data), std::end(data));
    assert(TrackDefault::count == 1);
  }

  { // T=TrackCopy N=2 with larger array
    TrackCopy data[5];

    TrackCopy::reset();
    wilt::NArray<TrackCopy, 2> a({ 2, 2 }, std::begin(data), std::end(data));
    assert(TrackCopy::count == 4);
  }
}

void testNArrayAddition()
{
  { // NArray<int, 2> + NArray<int, 2>
    wilt::NArray<int, 2> a({ 5, 5 }, 1);
    wilt::NArray<int, 2> b({ 5, 5 }, 2);

    auto c = a + b;
    assert((std::is_same<decltype(c), wilt::NArray<int, 2>>::value));
    assert(c.sizes() == wilt::Point<2>({ 5, 5 }));
    for (auto v : c)
      assert(v == 3);
  }

  { // NArray<int, 2> + int
    wilt::NArray<int, 2> a({ 5, 5 }, 1);

    auto c = a + 2;
    assert((std::is_same<decltype(c), wilt::NArray<int, 2>>::value));
    assert(c.sizes() == wilt::Point<2>({ 5, 5 }));
    for (auto v : c)
      assert(v == 3);
  }

  { // int + NArray<int, 2>
    wilt::NArray<int, 2> b({ 5, 5 }, 2);

    auto c = 1 + b;
    assert((std::is_same<decltype(c), wilt::NArray<int, 2>>::value));
    assert(c.sizes() == wilt::Point<2>({ 5, 5 }));
    for (auto v : c)
      assert(v == 3);
  }

  { // NArray<int, 2> + NArray<double, 2>
    wilt::NArray<int, 2> a({ 5, 5 }, 1);
    wilt::NArray<double, 2> b({ 5, 5 }, 2.25);

    auto c = a + b;
    assert((std::is_same<decltype(c), wilt::NArray<double, 2>>::value));
    assert(c.sizes() == wilt::Point<2>({ 5, 5 }));
    for (auto v : c)
      assert(v == 3.25);
  }

  { // NArray<int, 2> + double
    wilt::NArray<int, 2> a({ 5, 5 }, 1);

    auto c = a + 2.25;
    assert((std::is_same<decltype(c), wilt::NArray<double, 2>>::value));
    assert(c.sizes() == wilt::Point<2>({ 5, 5 }));
    for (auto v : c)
      assert(v == 3.25);
  }

  { // int + NArray<double, 2>
    wilt::NArray<double, 2> b({ 5, 5 }, 2.25);

    auto c = 1 + b;
    assert((std::is_same<decltype(c), wilt::NArray<double, 2>>::value));
    assert(c.sizes() == wilt::Point<2>({ 5, 5 }));
    for (auto v : c)
      assert(v == 3.25);
  }
}

void testMisc()
{
  { // test that window+skip can give the same result as a reshape+transpose
    auto arr = wilt::NArray<int, 2>({ 9, 16 }, 1);
    auto a = arr.reshape<4>({ 3, 3, 4, 4 }).transpose(1, 2);
    auto b = arr.windowX(3).windowY(4).skipX(3).skipY(4);
    assert(b.sizes() == a.sizes());
    assert(b.steps() == a.steps());
    assert(b == a);
  }
}

void testNArray()
{
  testDefaultConstructorHasSizeZero();
  testSizedConstructorHasConstructedSize();
  testSizedWithDefaultConstructorHasConstructedSize();
  testCopyConstructorSharesData();
  testConstNArrayConstructor();
  testCloneConstructsNewArrayCorrectly();
  testAsCondensedCreatesCorrectNArray();
  testSkipNCreatesCorrectNArray();
  testReshapeCreatesCorrectNArray();
  testRepeatCreatesCorrectNArray();
  testSubarraysIteratesCorrectly();
  testWindowCreatesCorrectNArray();
  testCompressCreatesCorrectNArray();
  testNArrayIteratorConstructor();

  testNArrayAddition();

  testMisc();
}
