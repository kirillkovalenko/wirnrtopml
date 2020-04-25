#pragma once

#include "OpmlDocument.g.h"
#include "Header.hpp"
#include "Outline.hpp"
#include "XmlLite.hpp"

namespace winrt::Kirkov::Data::Opml::implementation
{
    class OpmlDocument final 
        : public OpmlDocumentT<OpmlDocument>
        , public XmlPersist
    {
        uint8_t _version{ 1 };
        IOpmlHeader _header;
        IOpmlOutline _body;

    public:
        OpmlDocument();
        OpmlDocument(uint8_t documentVersion);

        void SaveXml(IXmlWriter* const writer) const override;
        void LoadXml(IXmlReader* const reader) override;

        static IAsyncOperation<IOpmlDocument> LoadFromBufferAsync(IBuffer buffer);
        static IAsyncOperation<IOpmlDocument> LoadFromUriAsync(Uri uri);
        static IAsyncOperation<IOpmlDocument> LoadFromFileAsync(IStorageFile file);

        IAsyncAction SaveToFileAsync(IStorageFile file);

        uint8_t Version() const;
        IOpmlHeader Header();
        IOpmlOutline Body();
        hstring ToString() const;
    };
}

namespace winrt::Kirkov::Data::Opml::factory_implementation
{
    struct OpmlDocument final : OpmlDocumentT<OpmlDocument, implementation::OpmlDocument>
    {
    };
}
