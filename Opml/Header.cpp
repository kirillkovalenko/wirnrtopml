#include "Pch.hpp"
#include "Header.hpp"
#include "DateTime.hpp"

namespace winrt::Kirkov::Data::Opml::implementation
{
    // A <head> contains zero or more optional elements
    OpmlHeader::OpmlHeader(IXmlReader* reader)
        : OpmlHeader()
    {
        LoadXml(reader);
    }

    void OpmlHeader::LoadXml(IXmlReader* const reader)
    {
        // <head> is already parsed by the caller
        unsigned level = 1;

        std::wstring_view name;
        std::wstring value;

        XmlNodeType nodeType{};

        while (S_OK == reader->Read(&nodeType))
        {
            if (XmlNodeType_Element == nodeType)
            {
                check_empty_prefix(reader);

                name = get_local_name(reader);

                if (!IsKnownTag(name))
                {
                    throw std::invalid_argument("name");
                }

                ++level;

                value.clear();
            }
            else if (XmlNodeType_Text == nodeType)
            {
                const auto val = get_value(reader);
                value.append(val);
            }
            else if (XmlNodeType_EndElement == nodeType)
            {
                if (--level == 0)
                {
                    // opml uses {top, left, bottom, right} model but winrt has {x, y, width, height} model
                    _window.Width -= _window.X; // convert right to width 
                    _window.Height -= _window.Y; // convert bottom to height

                    return;
                }

                AcceptValue(name, value);
            }
        }
    }

    void OpmlHeader::SaveXml(IXmlWriter* const writer) const
    {
        check_hresult(writer->WriteStartElement(nullptr, L"head", nullptr));

        write_element(writer, L"title", _title);
        write_element(writer, L"dateCreated", _created);
        write_element(writer, L"dateModified", _modified);
        write_element(writer, L"ownerName", _ownername);
        write_element(writer, L"ownerEmail", _owneremail);
        write_element(writer, L"ownerId", _ownerid);
        write_element(writer, L"docs", _documenation ? _documenation.ToString() : hstring());
        write_element(writer, L"expansionState", _expansionState.ToString());
        write_element(writer, L"vertScrollState", _scrollstate);

        // opml uses {top, left, bottom, right} model but winrt has {x, y, width, height} model
        if (_window != Rect())
        {
            write_element(writer, L"windowLeft", static_cast<int32_t>(_window.X));
            write_element(writer, L"windowTop", static_cast<int32_t>(_window.Y));
            write_element(writer, L"windowBottom", static_cast<int32_t>(_window.Y + _window.Height));
            write_element(writer, L"windowRight", static_cast<int32_t>(_window.X + _window.Width));
        }

        check_hresult(writer->WriteFullEndElement());
    }

    void OpmlHeader::AcceptValue(std::wstring_view const name, std::wstring& value)
    {
        if (L"title"sv == name)
        {
            if (_title.empty())
            {
                _title = std::move(value);
            }
            else
            {
                throw std::invalid_argument("title");
            }
        }
        else if (L"dateCreated"sv == name)
        {
            _created = OpmlDateTime::Parse(value.data());
        }
        else if (L"dateModified"sv == name)
        {
            _modified = OpmlDateTime::Parse(value.data());
        }
        else if (L"ownerName"sv == name)
        {
            if (_ownername.empty())
            {
                _ownername = std::move(value);
            }
            else
            {
                throw std::invalid_argument("ownerName");
            }
        }
        else if (L"ownerEmail"sv == name)
        {
            if (_owneremail.empty())
            {
                _owneremail = std::move(value);
            }
            else
            {
                throw std::invalid_argument("ownerEmail");
            }
        }
        else if (L"ownerId"sv == name)
        {
            if (_ownerid.empty())
            {
                _ownerid = std::move(value);
            }
            else
            {
                throw std::invalid_argument("ownerId");
            }
        }
        else if (L"docs"sv == name)
        {
            if (_documenation)
            {
                throw std::invalid_argument("docs");
            }
            else
            {
                _documenation = Uri(value);
            }
        }
        else if (L"expansionState"sv == name)
        {
            if (_expansionState.Size() == 0)
            {
                std::wstringstream ss(value);

                for (uint32_t i{}; ss >> i;)
                {
                    _expansionState.Append(i);

                    if (ss.peek() == L',' || ss.peek() == L' ')
                    {
                        ss.ignore();
                    }
                }
            }
            else
            {
                throw std::invalid_argument("expansionState");
            }
        }
        else if (L"vertScrollState"sv == name)
        {
            if (_scrollstate.has_value())
            {
                throw std::invalid_argument("vertScrollState");
            }
            else
            {
                _scrollstate = std::stoul(value, nullptr, 10);
            }
        }
        else if (L"windowTop"sv == name)
        {
            if (_window.Y == 0.0)
            {
                _window.Y = static_cast<float>(std::stoi(value, nullptr, 10));
            }
            else
            {
                throw std::invalid_argument("windowTop");
            }
        }
        else if (L"windowLeft"sv == name)
        {
            if (_window.X == 0.0)
            {
                _window.X = static_cast<float>(std::stoi(value, nullptr, 10));
            }
            else
            {
                throw std::invalid_argument("windowLeft");
            }
        }
        else if (L"windowBottom"sv == name)
        {
            if (_window.Height == 0.0)
            {
                // opml uses {top, left, bottom, right} model but winrt has {x, y, width, height} model
                _window.Height = static_cast<float>(std::stoi(value, nullptr, 10)); // temporary keep bottom as height
            }
            else
            {
                throw std::invalid_argument("windowBottom");
            }
        }
        else if (L"windowRight"sv == name)
        {
            if (_window.Width == 0.0)
            {
                // opml uses {top, left, bottom, right} model but winrt has {x, y, width, height} model
                _window.Width = static_cast<float>(std::stoi(value, nullptr, 10)); // temporary keep right as width
            }
            else
            {
                throw std::invalid_argument("windowRight");
            }
        }
        else
        {
            throw std::invalid_argument("name");
        }
    }

    bool OpmlHeader::IsKnownTag(std::wstring_view const name) noexcept
    {
        return L"title"sv == name ||
            L"dateCreated"sv == name ||
            L"dateModified"sv == name ||
            L"ownerName"sv == name ||
            L"ownerEmail"sv == name ||
            L"ownerId"sv == name ||
            L"docs"sv == name ||
            L"expansionState"sv == name ||
            L"vertScrollState"sv == name ||
            L"windowTop"sv == name ||
            L"windowLeft"sv == name ||
            L"windowBottom"sv == name ||
            L"windowRight"sv == name;
    }

    hstring OpmlHeader::Title()
    {
        return _title.data();
    }

    void OpmlHeader::Title(hstring const& value)
    {
        _title = value;
    }

    DateTime OpmlHeader::Created()
    {
        return _created;
    }

    void OpmlHeader::Created(DateTime const& value)
    {
        _created = value;
    }

    DateTime OpmlHeader::Modified()
    {
        return _modified;
    }

    void OpmlHeader::Modified(DateTime const& value)
    {
        _modified = value;
    }

    hstring OpmlHeader::OwnerName()
    {
        return _ownername.data();
    }

    void OpmlHeader::OwnerName(hstring const& value)
    {
        _ownername = value;
    }

    hstring OpmlHeader::OwnerEmail()
    {
        return _owneremail.data();
    }

    void OpmlHeader::OwnerEmail(hstring const& value)
    {
        _owneremail = value;
    }

    hstring OpmlHeader::OwnerId()
    {
        return _ownerid.data();
    }

    void OpmlHeader::OwnerId(hstring const& value)
    {
        _ownerid = value;
    }

    Uri OpmlHeader::Documentation()
    {
        return _documenation;
    }

    void OpmlHeader::Documentation(Uri const& value)
    {
        _documenation = value;
    }

    uint32_t OpmlHeader::ScrollState()
    {
        return _scrollstate.value_or(0);
    }

    void OpmlHeader::ScrollState(uint32_t const value)
    {
        _scrollstate = value;
    }

    Rect OpmlHeader::Window()
    {
        return _window;
    }

    void OpmlHeader::Window(Rect const& value)
    {
        _window = value;
    }

    IOpmlExpansionState OpmlHeader::ExpansionState()
    {
        return _expansionState;
    }

    hstring OpmlHeader::ToString() const
    {
        return ToXmlString(true);
    }
}
