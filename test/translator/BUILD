cc_test(
    name = "inquiry_test",
    srcs = ["inquiry_test.cc"],
    deps = [
        "//lib:scsi_defs_lib",
        "//third_party/spdk_defs:nvme_defs_lib",
        "@googletest//:gtest_main",
        "@com_google_absl//absl/base",
        "//lib/translator:inquiry_lib"
    ],
)

cc_test(
  name = "translator_tests",
  srcs = [ "common_test.cc" ],
  deps = [
    "//lib/translator:common",
    "@googletest//:gtest_main"
  ]
)
