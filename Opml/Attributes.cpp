#include "Pch.hpp"
#include "Attributes.hpp"

namespace winrt::Kirkov::Data::Opml::implementation
{
    void OpmlAttributes::SaveXml(IXmlWriter* const writer) const
    {
        hresult hr;

        for (const auto& [key, value] : _attributes)
        {
            hr = writer->WriteAttributeString(nullptr, key.data(), nullptr, value.data());
            check_hresult(hr);
        }
    }

    void OpmlAttributes::LoadXml(IXmlReader* const reader)
    {
        _attributes.clear();

        for (hresult hr = reader->MoveToFirstAttribute(); S_OK == hr; hr = reader->MoveToNextAttribute())
        {
            check_empty_prefix(reader);

            hstring name{ get_local_name(reader) };

            hstring value{ get_value(reader) };

            _attributes[name] = value;
        }
    }

    hstring OpmlAttributes::ToString() const
    {
        std::wstringstream ss;

        for (const auto& [key, value] : _attributes)
        {
            ss << key.data() << L"=" << value.data() << L";";
        }

        return ss.str().data();
    }
}