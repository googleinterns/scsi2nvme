// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lib/hello_world.h"

#include "gtest/gtest.h"

namespace {

/*
    Tests the HelloWorld class to return the expected string
*/
TEST(HelloWorldClass, ShouldReturnHelloWorldString) {
  HelloWorld *hw = new HelloWorld();
  std::string expected = "Hello World!";
  std::string actual = hw->get_hello_world_string();
  EXPECT_EQ(expected, actual);
}
}  // namespace
