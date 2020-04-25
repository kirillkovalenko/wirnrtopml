#include "Pch.hpp"
#include "Outline.hpp"
#include "Outlines.hpp"
#include "OpmlOutline.g.cpp"
#include "Attributes.hpp"
#include "DateTime.hpp"

namespace winrt::Kirkov::Data::Opml::implementation
{
    OpmlOutline::OpmlOutline()
        : _attributes{make<OpmlAttributes>()}
        , _outlines{make<OpmlOutlines>()}
    {
        Created(DateTime::clock::now());
    }

    OpmlOutline::OpmlOutline(hstring const& text)
        : OpmlOutline()
    {
        Text(text);
    }

    void OpmlOutline::SaveXml(IXmlWriter* const writer) const
    {
        SaveXml(writer, false);
    }

    void OpmlOutline::LoadXml(IXmlReader* const reader)
    {
        // returns TRUE if the current element ends with />; otherwise, returns FALSE.
        // value should be saved before processing attributes
        const auto isEmpty = reader->IsEmptyElement();

        auto attributes = make_self< OpmlAttributes>();
        attributes->LoadXml(reader);
        _attributes = attributes.as<IOpmlAttributes>();

        if (isEmpty)
        {
            _outlines = make<OpmlOutlines>();
        }
        else
        {
            auto outlines = make_self<OpmlOutlines>();
            outlines->LoadXml(reader);
            _outlines = outlines.as<IOpmlOutlines>();
        }
    }

    void OpmlOutline::SaveXml(IXmlWriter* const writer, bool const body) const
    {
        check_hresult(writer->WriteStartElement(nullptr, body ? L"body" : L"outline", nullptr));
        get_self<OpmlAttributes>(_attributes)->SaveXml(writer);
        get_self<OpmlOutlines>(_outlines)->SaveXml(writer);
        check_hresult(writer->WriteEndElement());
    }

    hstring OpmlOutline::ToString()
    {
        return _attributes.ToString();
    }

    hstring OpmlOutline::Text()
    {
        hstring key(L"text"sv);

        if (_attributes.HasKey(key))
        {
            return _attributes.Lookup(key);
        }
        else
        {
            return {};
        }
    }

    void OpmlOutline::Text(hstring const& value)
    {
        if (value.empty())
        {
            throw std::invalid_argument("text");
        }
        else
        {
            _attributes.Insert(L"text"sv, value);
        }
    }

    hstring OpmlOutline::Type()
    {
        hstring key(L"type"sv);

        if (_attributes.HasKey(key))
        {
            return _attributes.Lookup(key);
        }
        else
        {
            return {};
        }
    }

    void OpmlOutline::Type(hstring const& value)
    {
        if (value.empty())
        {
            throw std::invalid_argument("type");
        }
        else
        {
            _attributes.Insert(L"type"sv, value);
        }
    }

    bool OpmlOutline::IsComment()
    {
        hstring key(L"isComment"sv);

        if (_attributes.HasKey(key))
        {
            const auto value = _attributes.Lookup(key);
            return value == L"true"sv;
        }
        else
        {
            return {};
        }
    }

    void OpmlOutline::IsComment(bool const value)
    {
        _attributes.Insert(L"isComment"sv, value ? L"true"sv : L"false"sv);
    }

    bool OpmlOutline::IsBreakPoint()
    {
        hstring key(L"isBreakpoint"sv);

        if (_attributes.HasKey(key))
        {
            const auto value = _attributes.Lookup(key);
            return value == L"true"sv;
        }
        else
        {
            return {};
        }
    }

    void OpmlOutline::IsBreakPoint(bool const value)
    {
        _attributes.Insert(L"isBreakpoint"sv, value ? L"true"sv : L"false"sv);
    }

    DateTime OpmlOutline::Created()
    {
        hstring key(L"created"sv);

        if (_attributes.HasKey(key))
        {
            const auto value = _attributes.Lookup(key);
            return OpmlDateTime::Parse(value.data());
        }
        else
        {
            return {};
        }
    }

    void OpmlOutline::Created(DateTime const& value)
    {
        auto string = OpmlDateTime::ToString(value);
        _attributes.Insert(L"created"sv, string);
    }

    IOpmlAttributes OpmlOutline::Attributes()
    {
        return _attributes;
    }

    IOpmlOutlines OpmlOutline::Outlines()
    {
        return _outlines;
    }
}
