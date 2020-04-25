#pragma once

#include "XmlLite.hpp"

namespace winrt::Kirkov::Data::Opml::implementation
{
    class OpmlOutlines final
        : public implements<OpmlOutlines, IOpmlOutlines, IVector<IOpmlOutline>, IVectorView<IOpmlOutline>, IIterable<IOpmlOutline>, IStringable>
        , public vector_base<OpmlOutlines, IOpmlOutline>
        , public XmlPersist
    {
        std::vector<IOpmlOutline> _outlines;

    public:
        OpmlOutlines() = default;

        void SaveXml(IXmlWriter* const writer) const override;
        void LoadXml(IXmlReader* const reader) override;

        hstring ToString() const;

        auto& get_container() const noexcept
        {
            return _outlines;
        }

        auto& get_container() noexcept
        {
            return _outlines;
        }
    };

}