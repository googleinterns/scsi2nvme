cc_test(
    name = "scsi_test",
    srcs = ["scsi_test.cc"],
    deps = [
        "//lib:scsi_lib",
        "@googletest//:gtest_main",
        "@com_google_absl//absl/base",
    ],
)

cc_test(
    name = "nvme_test",
    srcs = ["nvme_test.cc"],
    deps = [
        "//third_party/spdk:nvme_lib",
        "@googletest//:gtest_main",
        "@com_google_absl//absl/base",
    ],
)

cc_test(
    name = "nvme_device_test",
    srcs = ["nvme_test.cc"],
    deps = [
        "//third_party/nvme_driver:nvme_driver",
        "//third_party/spdk:nvme_lib",
        "@googletest//:gtest_main",
        "@com_google_absl//absl/base",
    ],
)

