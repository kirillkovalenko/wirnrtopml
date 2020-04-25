#include "Pch.hpp"

TEST_CASE("EmptyOutline", "[outline]")
{
    OpmlOutline outline;

    SECTION("Text proprerty works") 
    {
        REQUIRE(outline.Text() == L"");

        outline.Text(L"Meow");
        REQUIRE(outline.Text() == L"Meow");
    }

    SECTION("Type proprerty works")
    {
        REQUIRE(outline.Type() == L"");

        outline.Type(L"Black");
        REQUIRE(outline.Type() == L"Black");
    }

    SECTION("IsComment proprerty works")
    {
        REQUIRE(outline.IsComment() == false);
        
        outline.IsComment(true);
        REQUIRE(outline.IsComment() == true);
        
        outline.IsComment(false);
        REQUIRE(outline.IsComment() == false);
    }

    SECTION("IsBreakPoint proprerty works")
    {
        REQUIRE(outline.IsBreakPoint() == false);
        
        outline.IsBreakPoint(true);
        REQUIRE(outline.IsBreakPoint() == true);

        outline.IsBreakPoint(false);
        REQUIRE(outline.IsBreakPoint() == false);
    }

    SECTION("Created proprerty works")
    {
        const auto created = outline.Created();
        REQUIRE(created != DateTime{});
    }

    SECTION("Attributes property works")
    {
        const auto attributes = outline.Attributes();
        REQUIRE(attributes.Size() == 1);
    }

    SECTION("Outlines property works")
    {
        const auto outlines = outline.Outlines();
        REQUIRE(outlines.Size() == 0);
    }
}