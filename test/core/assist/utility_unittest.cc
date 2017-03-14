
#include "core/assist/utility.h"
#include "gtest/gtest.h"

TEST(HexStrTest, NormalInput)
{
    EXPECT_EQ("61 62 63 ", fh::core::assist::utility::Hex_str("abc", 3));
}
