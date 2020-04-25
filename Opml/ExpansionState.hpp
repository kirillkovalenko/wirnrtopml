#pragma once

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Kirkov::Data::Opml;

namespace winrt::Kirkov::Data::Opml::implementation
{
    class OpmlExpansionState final
        : public implements<OpmlExpansionState, IOpmlExpansionState, IVector<uint32_t>, IVectorView<uint32_t>, IIterable<uint32_t>, IStringable>
        , public vector_base<OpmlExpansionState, uint32_t>
    {
        std::vector<uint32_t> _state;

    public:
        OpmlExpansionState() = default;

        hstring ToString() const;

        auto& get_container() const noexcept
        {
            return _state;
        }

        auto& get_container() noexcept
        {
            return _state;
        }
    };
}
