cc_library(
    name = "nvme_internal",
    hdrs = ["nvme_internal.h"],
    textual_hdrs = ["@system_deps//:linux"],
)

cc_library(
  name = "nvme_driver",
  hdrs = ["nvme_driver.h"],
  deps = [
      ":nvme_internal",
  ],
  visibility = ["//visibility:public"],
)
