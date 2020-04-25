#include "Pch.hpp"

static const auto data =
{
    L"Mon, 31 Oct 2005 19:23:00 GMT"sv,
    L"Wed, 02 Oct 2002 15:00:00 -0200"sv,
    L"Wed, 02 Oct 2002 15:00:00 +0200"sv,
};

TEST_CASE("TestOpmlDateTime")
{
    for (auto&& i : data)
    {
        const auto datetime1 = OpmlDateTime::Parse(i);
        REQUIRE(datetime1 != DateTime{});

        const auto string = OpmlDateTime::ToString(datetime1);
        REQUIRE(!string.empty());

        const auto datetime2 = OpmlDateTime::Parse(string);
        REQUIRE(datetime1.time_since_epoch().count() == datetime2.time_since_epoch().count());
    }
}
