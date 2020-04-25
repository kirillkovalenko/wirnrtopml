#pragma once

#include "XmlLite.hpp"

namespace winrt::Kirkov::Data::Opml::implementation
{
    class OpmlAttributes final
        : public implements<OpmlAttributes, IOpmlAttributes, IMap<hstring, hstring>, IMapView<hstring, hstring>, IIterable<IKeyValuePair<hstring, hstring>>, IStringable>
        , public winrt::map_base<OpmlAttributes, hstring, hstring>
        , public XmlPersist
    {
        std::map<hstring, hstring> _attributes;

    public:
        OpmlAttributes() = default;

        void SaveXml(IXmlWriter* const writer) const override;
        void LoadXml(IXmlReader* const reader) override;

        hstring ToString() const;

        auto& get_container() const noexcept
        {
            return _attributes;
        }

        auto& get_container() noexcept
        {
            return _attributes;
        }
    };
}
