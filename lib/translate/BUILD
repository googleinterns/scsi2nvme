cc_library(
  name = "translation_lib",
  srcs = ["inquiry.cc"],
  hdrs = ["inquiry.h", "common.h"],
  deps = [
      "//third_party/spdk_defs:nvme_defs_lib",
      "//lib:scsi_defs_lib",
      "@com_google_absl//absl/base",
      "@com_google_absl//absl/types:span"
  ],
  visibility = ["//visibility:public"],
)