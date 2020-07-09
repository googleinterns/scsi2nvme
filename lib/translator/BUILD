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
  name = "status_lib",
  hdrs = ["status.h"],
  srcs = ["status.cc"],
  deps = [
      "//third_party/spdk_defs:nvme_defs_lib",
      "//lib:scsi_defs_lib",
      "//lib/translator:common",
      "@com_google_absl//absl/base",
  ],
  visibility = ["//visibility:public"],
)
