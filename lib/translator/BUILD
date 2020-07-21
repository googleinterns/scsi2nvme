cc_library(
  name = "read_capacity_10_lib",
  srcs = ["read_capacity_10.cc"],
  hdrs = ["read_capacity_10.h"],
  deps = [
      "//third_party/spdk:nvme_lib",
      "//lib:scsi_lib",
      ":common",
      "@com_google_absl//absl/types:span",
  ],
  visibility = ["//visibility:public"],
)

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
    ":read_capacity_10_lib",
    ":inquiry_lib",
    ":request_sense_lib",
    "@com_google_absl//absl/types:span",
    ":common",
  ],
  visibility = ["//visibility:public"]
)

cc_library(
  name = "request_sense_lib",
  hdrs = ["request_sense.h"],
  srcs = ["request_sense.cc"],
  deps = [
      "//third_party/spdk:nvme_lib",
      "//lib:scsi_lib",
      ":common",
      "@com_google_absl//absl/base",
  ],
  visibility = ["//visibility:public"],
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
