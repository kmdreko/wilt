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
    assert(a.dims() == wilt::Point<1>({ 0 }));
  }

  { // T=int N=2
    wilt::NArray<int, 2> a;

    assert(a.size() == 0);
    assert(a.dims() == wilt::Point<2>({ 0, 0 }));
  }

  { // T=int N=5
    wilt::NArray<int, 5> a;

    assert(a.size() == 0);
    assert(a.dims() == wilt::Point<5>({ 0, 0, 0, 0, 0 }));
  }

  { // T=NoDefault N=1
    wilt::NArray<NoDefault, 1> a;

    assert(a.size() == 0);
    assert(a.dims() == wilt::Point<1>({ 0 }));
  }

  { // T=TrackDefault N=1
    TrackDefault::reset();

    wilt::NArray<TrackDefault, 1> a;

    assert(a.size() == 0);
    assert(a.dims() == wilt::Point<1>({ 0 }));
    assert(TrackDefault::count == 0);
  }
}

void testSizedConstructorHasConstructedSize()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 3 });

    assert(a.size() == 3);
    assert(a.dims() == wilt::Point<1>({ 3 }));
  }

  { // T=int N=2
    wilt::NArray<int, 2> a({ 3, 2 });

    assert(a.size() == 6);
    assert(a.dims() == wilt::Point<2>({ 3, 2 }));
  }

  { // T=int N=5
    wilt::NArray<int, 5> a({ 3, 2, 5, 1, 7 });

    assert(a.size() == 210);
    assert(a.dims() == wilt::Point<5>({ 3, 2, 5, 1, 7 }));
  }

  { // T=TrackDefault N=1
    TrackDefault::reset();

    wilt::NArray<TrackDefault, 1> a({ 3 });

    assert(a.size() == 3);
    assert(a.dims() == wilt::Point<1>({ 3 }));
    assert(TrackDefault::count == 3);
  }
}

void testSizedWithDefaultConstructorHasConstructedSize()
{
  { // T=int N=1
    wilt::NArray<int, 1> a({ 3 }, 1);

    assert(a.size() == 3);
    assert(a.dims() == wilt::Point<1>({ 3 }));
  }

  { // T=int N=2
    wilt::NArray<int, 2> a({ 3, 2 }, 1);

    assert(a.size() == 6);
    assert(a.dims() == wilt::Point<2>({ 3, 2 }));
  }

  { // T=int N=5
    wilt::NArray<int, 5> a({ 3, 2, 5, 1, 7 }, 1);

    assert(a.size() == 210);
    assert(a.dims() == wilt::Point<5>({ 3, 2, 5, 1, 7 }));
  }

  { // T=TrackCopy N=1
    TrackCopy::reset();

    wilt::NArray<TrackCopy, 1> a({ 3 }, TrackCopy{});

    assert(a.size() == 3);
    assert(a.dims() == wilt::Point<1>({ 3 }));
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
    assert(a.dims() == b.dims());
    assert(a[1] == 1);
    assert(&a[0] == &b[0]);
  }
}

void testNArray()
{
  testDefaultConstructorHasSizeZero();
  testSizedConstructorHasConstructedSize();
  testSizedWithDefaultConstructorHasConstructedSize();
  testCopyConstructorSharesData();
}