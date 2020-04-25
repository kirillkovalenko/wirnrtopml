#pragma once

namespace winrt::Kirkov::Data::Opml::implementation
{
    template<typename T = uint8_t>
    class hgloballock final
    {
        HGLOBAL const _handle;
        LPVOID const _pointer;

    public:
        using pointer = T*;
        using const_pointer = const T*;
        using size_type = uint32_t;

    public:
        explicit hgloballock(HGLOBAL handle) noexcept
            : _handle(handle)
            , _pointer(GlobalLock(handle))
        {
            WINRT_ASSERT(_pointer != nullptr);
        }

        ~hgloballock() noexcept
        {
            if (_pointer != nullptr)
            {
                GlobalUnlock(_handle);
                WINRT_ASSERT(ERROR_SUCCESS == GetLastError());
            }
        }

        explicit operator bool() const noexcept
        {
            return _pointer != nullptr;
        }

        size_type size() const noexcept
        {
            return static_cast<size_type>(GlobalSize(_handle) / sizeof(T));
        }

        pointer data() noexcept
        {
            return static_cast<pointer>(_pointer);
        }

        const_pointer data() const noexcept
        {
            return static_cast<const_pointer>(_pointer);
        }

    private:
        hgloballock(hgloballock const&) = delete;
        hgloballock(hgloballock&&) = delete;
        hgloballock const& operator=(hgloballock const&) = delete;
        hgloballock& operator=(hgloballock&&) = delete;
    };

    class __declspec(novtable) XmlPersist
    {
    protected:
        inline static void check_empty_prefix(IXmlReader* const reader)
        {
            WINRT_ASSERT(reader != nullptr);
            LPCWSTR wszPrefix{}; UINT cchPrefix{ UINT_MAX };
            check_hresult(reader->GetPrefix(&wszPrefix, &cchPrefix));
            if (0 != cchPrefix)
            {
                throw std::invalid_argument("prefix");
            }
        }

        inline static std::wstring_view get_local_name(IXmlReader* const reader)
        {
            WINRT_ASSERT(reader != nullptr);
            LPCWSTR wszLocalName{}; UINT cchLocalName{};
            check_hresult(reader->GetLocalName(&wszLocalName, &cchLocalName));
            return { wszLocalName, cchLocalName };
        }

        inline static std::wstring_view get_value(IXmlReader* const reader)
        {
            WINRT_ASSERT(reader != nullptr);
            PCWSTR wszValue{}; UINT cchValue{};
            check_hresult(reader->GetValue(&wszValue, &cchValue));
            return { wszValue, cchValue };
        }

        inline static void write_element(IXmlWriter* const writer, wchar_t const* name, wchar_t const* text)
        {
            check_hresult(writer->WriteStartElement(nullptr, name, nullptr));
            check_hresult(writer->WriteString(text));
            check_hresult(writer->WriteFullEndElement());
        }

        inline static void write_element(IXmlWriter* const writer, wchar_t const* name, std::wstring const& value)
        {
            if (!value.empty())
            {
                write_element(writer, name, value.data());
            }
        }

        inline static void write_element(IXmlWriter* const writer, wchar_t const* name, hstring const& value)
        {
            if (!value.empty())
            {
                write_element(writer, name, value.data());
            }
        }

        inline static void write_element(IXmlWriter* const writer, wchar_t* const name, DateTime value)
        {
            if (value != DateTime())
            {
                auto string = OpmlDateTime::ToString(value);
                write_element(writer, name, string);
            }
        }

        inline static void write_element(IXmlWriter* const writer, wchar_t* const name, std::optional<uint32_t> const& value)
        {
            if (value.has_value())
            {
                auto string = std::to_wstring(*value);
                write_element(writer, name, string);
            }
        }

        inline static void write_element(IXmlWriter* const writer, wchar_t* const name, int32_t const value)
        {
            auto string = std::to_wstring(value);
            write_element(writer, name, string);
        }

    public:
        virtual void SaveXml(IXmlWriter* const writer) const = 0;
        virtual void LoadXml(IXmlReader* const reader) = 0;

    protected:
        hstring ToXmlString(bool const withIndent) const
        {
            hresult hr;

            com_ptr<IXmlWriter> writer;
            hr = CreateXmlWriter(__uuidof(writer), writer.put_void(), nullptr);
            check_hresult(hr);

            com_ptr<IStream> ostream;
            hr = CreateStreamOnHGlobal(nullptr, true, ostream.put());
            check_hresult(hr);

            com_ptr<IXmlWriterOutput> output;
            hr = CreateXmlWriterOutputWithEncodingName(ostream.get(), nullptr, L"UTF-16", output.put());
            check_hresult(hr);

            hr = writer->SetOutput(output.get());
            check_hresult(hr);

            hr = writer->SetProperty(XmlWriterProperty_Indent, withIndent);
            check_hresult(hr);

            SaveXml(writer.get());

            hr = writer->Flush();
            check_hresult(hr);

            HGLOBAL hglobal{};
            hr = GetHGlobalFromStream(ostream.get(), &hglobal);
            check_hresult(hr);

            hgloballock<wchar_t> memory(hglobal);
            check_bool(!!memory);

            hstring result{ memory.data(), memory.size() };

            return result;
        }
    };
}