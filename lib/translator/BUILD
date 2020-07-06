cc_library(
  name = "common",
  hdrs = ["common.h"],
  srcs = ["common.cc"],
  deps = [ ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "logsense_lib",
  hdrs = ["logsense.h"],
  srcs = ["logsense.cc"],
  deps = [
      "//lib/translator:common",
      "//third_party/spdk_defs:nvme_defs_lib",
      "//lib:scsi_defs_lib",
      "@com_google_absl//absl/base",
      "@com_google_absl//absl/types:span",
      "@com_google_absl//absl/strings",
  ],
  visibility = ["//visibility:public"],
)