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
    "//lib/translator:common"
  ],
  visibility = ["//visibility:public"]
)

cc_library(
  name = "test_unit_ready_lib",
  hdrs = ["test_unit_ready.h"],
  srcs = ["test_unit_ready.cc"],
  deps = [
      "//third_party/spdk:nvme_lib",
      "//lib:scsi_lib",
      "//lib/translator:common",
      "@com_google_absl//absl/base",
      "@com_google_absl//absl/types:span",
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
      "//lib/translator:common",
      "@com_google_absl//absl/base",
  ],
  visibility = ["//visibility:public"],
)
