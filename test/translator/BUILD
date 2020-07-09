cc_test(
  name = "common_tests",
  srcs = [ "common_test.cc" ],
  deps = [
    "//lib/translator:common",
    "//lib:scsi_defs_lib",
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
  name = "read_tests",
  srcs = [ "read_test.cc" ],
  deps = [
    "//lib/translator:read_lib",
    "//lib:scsi_defs_lib",
    "//third_party/spdk_defs:nvme_defs_lib",
    "//lib/translator:common",
    "@googletest//:gtest_main"
  ]
)
