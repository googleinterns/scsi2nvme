cc_library(
  name = "inquiry_lib",
  srcs = ["inquiry.cc"],
  hdrs = ["inquiry.h"],
  deps = [
      "//third_party/spdk:nvme_lib",
      "//lib:scsi_lib",
      ":common",
      "@com_google_absl//absl/base",
      "@com_google_absl//absl/types:span",
      "@com_google_absl//absl/strings",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "common",
  hdrs = ["common.h"],
  srcs = ["common.cc"],
  deps = [
    "//third_party/spdk:nvme_lib",
    "//lib:scsi_lib",
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
    ":common",
    ":inquiry_lib"
  ],
  visibility = ["//visibility:public"]
)

cc_library(
  name = "status_lib",
  hdrs = ["status.h"],
  srcs = ["status.cc"],
  deps = [
      "//third_party/spdk:nvme_lib",
      "//lib:scsi_lib",
      ":common",
      "@com_google_absl//absl/base",
  ],
  visibility = ["//visibility:public"],
)
