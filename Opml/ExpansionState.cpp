#include "Pch.hpp"
#include "ExpansionState.hpp"

namespace winrt::Kirkov::Data::Opml::implementation
{
    hstring OpmlExpansionState::ToString() const
    {
        std::wstringstream ss;

        for (size_t i = 0; i < _state.size(); ++i)
        {
            ss << _state[i];

            if (i != _state.size() - 1)
            {
                ss << L",";
            }
        }

        return ss.str().data();
    }
}
