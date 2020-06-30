cc_test(
    name = "hello_world_test",
    srcs = ["hello_world_test.cc"],
    deps = [
        "//lib:hello_world_lib",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "scsi_defs_test",
    srcs = ["scsi_defs_test.cc"],
    deps = [
        "//lib:scsi_defs_lib",
        "@googletest//:gtest_main",
        "@com_google_absl//absl/base",
    ],
)

cc_test(
    name = "nvme_defs_test",
    srcs = ["nvme_defs_test.cc"],
    deps = [
        "//third_party/spdk_defs:nvme_defs_lib",
        "@googletest//:gtest_main",
        "@com_google_absl//absl/base",
    ],
)
