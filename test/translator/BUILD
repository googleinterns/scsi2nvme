cc_test(
  name = "common_tests",
  srcs = [ "common_test.cc" ],
  deps = [
    "//lib/translator:common",
    "//lib:scsi_lib",
    "@googletest//:gtest_main"
  ]
)

cc_test(
  name = "translation_tests",
  srcs = [ "translation_test.cc" ],
  deps = [
    "//lib/translator:translation",
    "@googletest//:gtest_main"
  ]
)

cc_test(
  name = "status_test",
  srcs = [ "status_test.cc" ],
  deps = [
    "//lib/translator:status_lib",
    "@googletest//:gtest_main"
  ]
)

cc_test(
  name = "test_unit_ready_tests",
  srcs = [ "test_unit_ready_test.cc" ],
  deps = [
    "//lib/translator:test_unit_ready_lib",
    "@googletest//:gtest_main"
  ]
)
