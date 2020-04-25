#pragma once

#include "OpmlOutline.g.h"
#include "XmlLite.hpp"

namespace winrt::Kirkov::Data::Opml::implementation
{
    class OpmlOutline final 
        : public OpmlOutlineT<OpmlOutline>
        , public XmlPersist
    {
        IOpmlAttributes _attributes;
        IOpmlOutlines _outlines;

    public:
        OpmlOutline();
        OpmlOutline(hstring const& text);

        void SaveXml(IXmlWriter* const writer, bool const body) const;
        void SaveXml(IXmlWriter* const writer) const override;
        void LoadXml(IXmlReader* const reader) override;

        hstring ToString();
        hstring Text();
        void Text(hstring const& value);
        hstring Type();
        void Type(hstring const& value);
        bool IsComment();
        void IsComment(bool const value);
        bool IsBreakPoint();
        void IsBreakPoint(bool const value);
        DateTime Created();
        void Created(DateTime const& value);
        IOpmlAttributes Attributes();
        IOpmlOutlines Outlines();
    };
}

namespace winrt::Kirkov::Data::Opml::factory_implementation
{
    struct OpmlOutline final : OpmlOutlineT<OpmlOutline, implementation::OpmlOutline>
    {
    };
}
