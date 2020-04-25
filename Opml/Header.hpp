#pragma once

#include "ExpansionState.hpp"
#include "XmlLite.hpp"

namespace winrt::Kirkov::Data::Opml::implementation
{
    class OpmlHeader final 
        : public implements<OpmlHeader, IOpmlHeader, IStringable>
        , public XmlPersist
    {
        std::wstring _title;
        DateTime _created;
        DateTime _modified;
        std::wstring _ownername;
        std::wstring _owneremail;
        std::wstring _ownerid;
        Uri _documenation{nullptr};
        std::optional<uint32_t> _scrollstate;
        Rect _window;
        IOpmlExpansionState _expansionState{ make<OpmlExpansionState>() };

    public:
        OpmlHeader() = default;
        OpmlHeader(IXmlReader* const reader);

        void SaveXml(IXmlWriter* const writer) const override;
        void LoadXml(IXmlReader* const reader) override;

        hstring Title();
        void Title(hstring const& value);

        DateTime Created();
        void Created(DateTime const& value);

        DateTime Modified();
        void Modified(DateTime const& value);

        hstring OwnerName();
        void OwnerName(hstring const& value);

        hstring OwnerEmail();
        void OwnerEmail(hstring const& value);

        hstring OwnerId();
        void OwnerId(hstring const& value);

        Uri Documentation();
        void Documentation(Uri const& value);

        uint32_t ScrollState();
        void ScrollState(uint32_t const value);

        Rect Window();
        void Window(Rect const& value);

        IOpmlExpansionState ExpansionState();

        hstring ToString() const;

    private:
        void AcceptValue(std::wstring_view const name, std::wstring& value);
        static bool IsKnownTag(std::wstring_view const name) noexcept;
    };
}
