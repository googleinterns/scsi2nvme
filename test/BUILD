cc_test(
    name = "tests",
    srcs = glob(["**/*.cc"]),
    deps = [
        "//lib:hello_world_lib",
        "//lib:scsi_defs_lib",
        "//third_party/spdk_defs:nvme_defs_lib",
        "@googletest//:gtest_main",
        "@com_google_absl//absl/base",
        "//lib/translate:inquiry_lib"
    ],
)
