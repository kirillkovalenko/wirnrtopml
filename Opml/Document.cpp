#include "Pch.hpp"
#include "Document.hpp"
#include "OpmlDocument.g.cpp"

namespace winrt::Kirkov::Data::Opml::implementation
{
    OpmlDocument::OpmlDocument()
        : OpmlDocument(2)
    {
    }

    OpmlDocument::OpmlDocument(uint8_t documentVersion)
        : _version{ documentVersion }
        , _header{ make<OpmlHeader>() }
        , _body{ make<OpmlOutline>() }
    {
        if (_version != 1 && _version != 2)
        {
            throw std::invalid_argument("version");
        }
    }

    void OpmlDocument::SaveXml(IXmlWriter* const writer) const
    {
        hresult hr;

        hr = writer->SetProperty(XmlWriterProperty_Indent, TRUE);
        check_hresult(hr);

        hr = writer->WriteStartDocument(XmlStandalone_Omit);
        check_hresult(hr);

        hr = writer->WriteStartElement(nullptr, L"opml", nullptr);
        check_hresult(hr);

        const auto version = std::to_wstring(_version) + L".0";
        hr = writer->WriteAttributeString(nullptr, L"version", nullptr, version.data());
        check_hresult(hr);

        get_self<OpmlHeader>(_header)->SaveXml(writer);

        get_self<OpmlOutline>(_body)->SaveXml(writer, true);

        hr = writer->WriteEndElement();
        check_hresult(hr);

        hr = writer->WriteEndDocument();
        check_hresult(hr);

        hr = writer->Flush();
        check_hresult(hr);
    }

    void OpmlDocument::LoadXml(IXmlReader* const reader)
    {
        hresult hr;

        XmlNodeType nodeType{};

        while (S_OK == (hr = reader->Read(&nodeType)))
        {
            if (XmlNodeType_Element == nodeType)
            {
                check_empty_prefix(reader);

                const auto name = get_local_name(reader);

                if (name == L"opml"sv)
                {
                    break;
                }
                else
                {
                    throw std::invalid_argument("name");
                }
            }
        }

        check_hresult(hr);

        for (hr = reader->MoveToFirstAttribute(); S_OK == hr; hr = reader->MoveToNextAttribute())
        {
            check_empty_prefix(reader);

            const auto name = get_local_name(reader);
            if (name != L"version"sv)
            {
                throw std::invalid_argument("name");
            }

            // make copy to have a zero terminated value for scanf below
            std::wstring value{ get_value(reader) };

            uint32_t major{}, minor{};
            swscanf_s(value.data(), L"%u.%u", &major, &minor);
            if (major != 1 && major != 2)
            {
                throw std::invalid_argument("version");
            }
            else
            {
                _version = static_cast<uint8_t>(major);
            }
        }

        while (S_OK == (hr = reader->Read(&nodeType)))
        {
            if (XmlNodeType_Element == nodeType)
            {
                check_empty_prefix(reader);

                const auto name = get_local_name(reader);

                if (name == L"head"sv)
                {
                    _header = make<OpmlHeader>(reader);
                    break;
                }
                else
                {
                    throw std::invalid_argument("name");
                }
            }
        }

        check_hresult(hr);

        while (S_OK == (hr = reader->Read(&nodeType)))
        {
            if (XmlNodeType_Element == nodeType)
            {
                check_empty_prefix(reader);

                const auto name = get_local_name(reader);

                if (name == L"body"sv)
                {
                    auto body = make_self<OpmlOutline>();
                    body->LoadXml(reader);
                    _body = body.as<IOpmlOutline>();
                    break;
                }
                else
                {
                    throw std::invalid_argument("name");
                }
            }
        }

        check_hresult(hr);
    }

    IAsyncOperation<IOpmlDocument> OpmlDocument::LoadFromBufferAsync(IBuffer buffer)
    {
        throw hresult_not_implemented();
    }

    IAsyncOperation<IOpmlDocument> OpmlDocument::LoadFromUriAsync(Uri uri)
    {
        throw hresult_not_implemented();
    }

    IAsyncOperation<IOpmlDocument> OpmlDocument::LoadFromFileAsync(IStorageFile file)
    {
        auto rastream = co_await file.OpenAsync(FileAccessMode::Read);

        hresult hr;

        com_ptr<IStream> istream;
        hr = CreateStreamOverRandomAccessStream(
            reinterpret_cast<::IUnknown*>(winrt::get_abi(rastream)),
            __uuidof(istream), 
            istream.put_void());
        check_hresult(hr);

        com_ptr<IXmlReader> reader;
        hr = CreateXmlReader(__uuidof(reader), reader.put_void(), nullptr);
        check_hresult(hr);

        hr = reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit);
        check_hresult(hr);

        hr = reader->SetInput(istream.get());
        check_hresult(hr);

        auto document = make_self<OpmlDocument>();
        document->LoadXml(reader.get());

        co_return document.as<IOpmlDocument>();
    }

    IAsyncAction OpmlDocument::SaveToFileAsync(IStorageFile file)
    {
        auto rastream = co_await file.OpenAsync(FileAccessMode::ReadWrite);

        hresult hr;

        com_ptr<IStream> ostream;
        hr = CreateStreamOverRandomAccessStream(
            reinterpret_cast<::IUnknown*>(winrt::get_abi(rastream)),
            __uuidof(ostream),
            ostream.put_void());
        check_hresult(hr);

        com_ptr<IXmlWriter> writer;
        hr = CreateXmlWriter(__uuidof(writer), writer.put_void(), nullptr);
        check_hresult(hr);

        hr = writer->SetOutput(ostream.get());
        check_hresult(hr);

        SaveXml(writer.get());
    }

    hstring OpmlDocument::ToString() const
    {
        return ToXmlString(true);
    }

    uint8_t OpmlDocument::Version() const
    {
        return _version;
    }

    IOpmlHeader OpmlDocument::Header()
    {
        return _header;
    }

    IOpmlOutline OpmlDocument::Body()
    {
        return _body;
    }
}
