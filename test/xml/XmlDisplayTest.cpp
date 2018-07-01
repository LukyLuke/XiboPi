/*
 * CPP UnitTest for XmlDisplay parser
 * Copyright (C) 2018  Lukas Zurschmiede <lukas.zurschmiede@ranta.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */ 

#include "CppUTest/TestHarness.h"
#include "CppUTest/SimpleString.h"
#include "CppUTest/PlatformSpecificFunctions.h"
#include "CppUTest/TestMemoryAllocator.h"
#include "CppUTest/MemoryLeakDetector.h"

class XmlDisplayTest : public TestMemoryAllocator
{
public:
  virtual ~XmlDisplayTest() {}
  
  char* alloc_memory(size_t size, const char* file, int line) {
    return MemoryLeakWarningPlugin::getGlobalDetector()->allocMemory(getCurrentNewArrayAllocator(), size, file, line);
  }
  void free_memory(char* str, const char* file, int line) {
    MemoryLeakWarningPlugin::getGlobalDetector()->deallocMemory(getCurrentNewArrayAllocator(), str, file, line);
  }
  
};

TEST_GROUP(XmlDisplay) {
  void setup() {
    // Not yet
  }
  void teardown() {
    // Not yet
  }
};

TEST(XmlDisplay, FirstTest) {
  FAIL("Just fail");
}

TEST(XmlDisplay, SecondTest) {
  STRCMP_EQUAL("hello", "world");
  LONGS_EQUAL(1, 2);
  CHECK(false);
}
