#include "Pch.hpp"
#include "Outlines.hpp"
#include "Outline.hpp"

namespace winrt::Kirkov::Data::Opml::implementation
{
    void OpmlOutlines::LoadXml(IXmlReader* const reader)
    {
        hresult hr;

        XmlNodeType nodeType{};

        while (S_OK == (hr = reader->Read(&nodeType)))
        {
            if (XmlNodeType_Element == nodeType)
            {
                check_empty_prefix(reader);

                const auto name = get_local_name(reader);

                if (L"outline"sv == name)
                {
                    auto outline = make_self<OpmlOutline>();
                    outline->LoadXml(reader);
                    _outlines.push_back(outline.as<IOpmlOutline>());
                }
                else
                {
                    throw std::invalid_argument("name");
                }
            }
            else if (XmlNodeType_EndElement == nodeType)
            {
                return;
            }
        }

        check_hresult(hr);
    }

    void OpmlOutlines::SaveXml(IXmlWriter* const writer) const
    {
        for (auto&& outline : _outlines)
        {
            get_self<OpmlOutline>(outline)->SaveXml(writer, false);
        }
    }

    hstring OpmlOutlines::ToString() const
    {
        std::wstringstream ss;

        for (auto&& outline : _outlines)
        {
            ss << outline.ToString().data() << std::endl;
        }

        return ss.str().data();
    }
}