cc_library(
  name = "common",
  hdrs = ["common.h"],
  srcs = ["common.cc"],
  deps = [
    "//third_party/spdk_defs:nvme_defs_lib",
    "//lib:scsi_defs_lib",
    "@com_google_absl//absl/types:span",
    "@com_google_absl//absl/base",
    "@com_google_absl//absl/strings",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "translation",
  hdrs = ["translation.h"],
  srcs = ["translation.cc"],
  deps = [
    "@com_google_absl//absl/types:span",
    "//lib/translator:common",
    "//lib/translator:read_lib",
  ],
  visibility = ["//visibility:public"]
)

cc_library(
  name = "read_lib",
  hdrs = ["read.h"],
  srcs = ["read.cc"],
  deps = [
      "//lib/translator:common",
      "//lib:scsi_defs_lib",
      "//third_party/spdk_defs:nvme_defs_lib",
      "@googletest//:gtest_main",
      "@com_google_absl//absl/types:span",
      "@com_google_absl//absl/base",
          ],
  visibility = ["//visibility:public"],
)
