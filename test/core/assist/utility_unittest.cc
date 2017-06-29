
#include <gtest/gtest.h>
#include "core/assist/utility.h"

TEST(HexStrTest, NormalInput)
{
    EXPECT_EQ("61 62 63 ", fh::core::assist::utility::Hex_str("abc", 3));
}
