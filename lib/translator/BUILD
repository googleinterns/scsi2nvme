cc_library(
  name = "common",
  hdrs = ["common.h"],
  srcs = ["common.cc", "translation.cc"],
  deps = [
    "//third_party/spdk_defs:nvme_defs_lib",
    "//lib:scsi_defs_lib",
    "@com_google_absl//absl/types:span"
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "read",
  hdrs = ["read.h"],
  srcs = ["read.cc"],
  deps = [ 
          "//lib/translator:common",
        "//lib:scsi_defs_lib",
        "//third_party/spdk_defs:nvme_defs_lib",
        "@googletest//:gtest_main",
          "@com_google_absl//absl/types:span",
          ], 
  visibility = ["//visibility:public"],
)
