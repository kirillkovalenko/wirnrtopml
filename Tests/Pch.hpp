#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <ctime>
using namespace std::literals;

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Search.h>
#include <winrt/Kirkov.Data.Opml.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Search;
using namespace Kirkov::Data::Opml;

#include "Catch2.hpp"
