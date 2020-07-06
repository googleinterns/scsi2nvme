// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lib/translator/common.h"
#include "lib/translator/logsense.h"
#include "gtest/gtest.h"

namespace {
    TEST(TranslateSupportedLogPages, success) {
        uint8_t buf[100];
        absl::Span<uint8_t> span = absl::MakeSpan(buf);
        translator::TranslateSupportedLogPages(span);
        const scsi_defs::SupportedLogPages result = *reinterpret_cast<const scsi_defs::SupportedLogPages *>(&buf);
        const scsi_defs::PageCode *result_list = reinterpret_cast<const scsi_defs::PageCode *>(&buf[sizeof(scsi_defs::SupportedLogPages)]);

        ASSERT_EQ(result.page_len, 0x4);
        ASSERT_EQ(result_list[0], scsi_defs::PageCode::kSupportedLogPages);
        ASSERT_EQ(result_list[1], scsi_defs::PageCode::kTemperature);
        ASSERT_EQ(result_list[2], scsi_defs::PageCode::kSolidStateMedia);
        ASSERT_EQ(result_list[3], scsi_defs::PageCode::kInformationalExceptions);
    };

}  // namespace
