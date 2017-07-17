#include <gtest/gtest.h>
#include <utils.hpp>


using std::string;
using std::vector;


class UtilsTest : public testing::Test {
  public:
    virtual void SetUp() override {}

    virtual void TearDown() override {}
};


TEST_F(UtilsTest, splitString_0) {
  string s = "one,two, three,four\nfive, six";
  vector<string> v = splitString(s, ',');

  ASSERT_EQ(5, v.size());
  ASSERT_EQ("one", v[0]);
  ASSERT_EQ("two", v[1]);
  ASSERT_EQ(" three", v[2]);
  ASSERT_EQ("four\nfive", v[3]);
  ASSERT_EQ(" six", v[4]);
}
