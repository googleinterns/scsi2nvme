cc_library(
  name = "common",
  hdrs = ["common.h"],
  srcs = ["common.cc"],
  deps = [
    "//lib:scsi_lib",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "translation",
  hdrs = ["translation.h"],
  srcs = ["translation.cc"],
  deps = [
    "//third_party/spdk:nvme_lib",
    ":common",
    ":inquiry_lib",
    ":read_lib",
    ":read_capacity_10_lib",
    ":request_sense_lib",
    ":verify_lib",
    ":report_luns_lib",
    ":write_lib"
  ],
  visibility = ["//visibility:public"]
)

cc_library(
  name = "inquiry_lib",
  srcs = ["inquiry.cc"],
  hdrs = ["inquiry.h"],
  deps = [
      "//third_party/spdk:nvme_lib",
      ":common",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "read_capacity_10_lib",
  srcs = ["read_capacity_10.cc"],
  hdrs = ["read_capacity_10.h"],
  deps = [
      "//third_party/spdk:nvme_lib",
      ":common",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "report_luns_lib",
  hdrs = ["report_luns.h"],
  srcs = ["report_luns.cc"],
  deps = [
    "//third_party/spdk:nvme_lib",
    ":common",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "status_lib",
  hdrs = ["status.h"],
  srcs = ["status.cc"],
  deps = [
      "//third_party/spdk:nvme_lib",
      ":common",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "request_sense_lib",
  hdrs = ["request_sense.h"],
  srcs = ["request_sense.cc"],
  deps = [
      "//third_party/spdk:nvme_lib",
      ":common",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "read_lib",
  hdrs = ["read.h"],
  srcs = ["read.cc"],
  deps = [
      "//third_party/spdk:nvme_lib",
      ":common",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "verify_lib",
  hdrs = ["verify.h"],
  srcs = ["verify.cc"],
  deps = [
      "//third_party/spdk:nvme_lib",
      ":common",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "write_lib",
  hdrs = ["write.h"],
  srcs = ["write.cc"],
  deps = [
    //:common",
    "//lib:scsi_lib",
    "//third_party/spdk:nvme_lib",
    "@com_google_absl//absl/base",
  ],
  visibility = ["//visibility:public"],
)
