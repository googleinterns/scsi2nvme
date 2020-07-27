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
    ":read_capacity_10_lib",
    ":request_sense_lib",
    ":status_lib",
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
  name = "status_lib",
  hdrs = ["status.h"],
  srcs = ["status.cc"],
  deps = [
      "//third_party/spdk:nvme_lib",
      ":common",
  ],
  visibility = ["//visibility:public"],
)
