cc_test(
    name = "inquiry_tests",
    srcs = ["inquiry_test.cc"],
    deps = [
        "//lib/translator:inquiry_lib",
        "@googletest//:gtest_main"
    ],
)

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
  name = "request_sense_tests",
  srcs = [ "request_sense_test.cc" ],
  deps = [
    "//lib/translator:request_sense_lib",
    "@googletest//:gtest_main"
  ]
)
