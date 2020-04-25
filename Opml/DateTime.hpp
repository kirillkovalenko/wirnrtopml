#pragma once

#include "OpmlDateTime.g.h"

namespace winrt::Kirkov::Data::Opml::implementation
{
    struct OpmlDateTime final
    {
        OpmlDateTime() = default;
        static Windows::Foundation::DateTime Parse(hstring const& value);
        static hstring ToString(Windows::Foundation::DateTime const& value);
    };
}
namespace winrt::Kirkov::Data::Opml::factory_implementation
{
    struct OpmlDateTime final : OpmlDateTimeT<OpmlDateTime, implementation::OpmlDateTime>
    {
    };
}
