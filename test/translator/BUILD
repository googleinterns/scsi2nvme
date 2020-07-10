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
  name = "read_tests",
  srcs = [ "read_test.cc" ],
  deps = [
    "//lib/translator:read_lib",
    "//lib:scsi_lib",
    "//third_party/spdk:nvme_lib",
    "//lib/translator:common",
    "@googletest//:gtest_main"
  ]
)

