#include "Pch.hpp"

static hstring GetThisPath()
{
    wchar_t path[_MAX_PATH + 1];
    GetModuleFileName(nullptr, path, _countof(path));

    PathResolve(path, nullptr, PRF_REQUIREABSOLUTE);

    PathRemoveFileSpec(path);

    return { path };
}

void Compare(IOpmlExpansionState const& lhs, IOpmlExpansionState const& rhs)
{
    REQUIRE(lhs.Size() == rhs.Size());
    
    for (uint32_t i = 0; i < lhs.Size(); ++i)
    {
        REQUIRE(lhs.GetAt(i) == rhs.GetAt(i));
    }

    REQUIRE(lhs.ToString() == rhs.ToString());
}

void Compare(IOpmlAttributes const& lhs, IOpmlAttributes const& rhs)
{
    REQUIRE(lhs.Size() == rhs.Size());

    for (auto&& kvp : lhs)
    {
        REQUIRE(kvp.Value() == rhs.Lookup(kvp.Key()));
    }

    REQUIRE(lhs.ToString() == rhs.ToString());
}

void Compare(Windows::Foundation::Uri const& lhs, Windows::Foundation::Uri const& rhs)
{
    if (lhs)
    {
        REQUIRE(rhs);
        REQUIRE(lhs.ToString() == rhs.ToString());
    }
    else
    {
        REQUIRE(!rhs);
    }
}

void Compare(IOpmlHeader const& lhs, IOpmlHeader const& rhs)
{
    REQUIRE(lhs.Title() == rhs.Title());
    REQUIRE(lhs.Created() == rhs.Created());
    REQUIRE(lhs.Modified() == rhs.Modified());
    REQUIRE(lhs.OwnerName() == rhs.OwnerName());
    REQUIRE(lhs.OwnerEmail() == rhs.OwnerEmail());
    REQUIRE(lhs.OwnerId() == rhs.OwnerId());
    Compare(lhs.Documentation(), rhs.Documentation());
    REQUIRE(lhs.ScrollState() == rhs.ScrollState());
    REQUIRE(lhs.Window() == rhs.Window());
    Compare(lhs.ExpansionState(), rhs.ExpansionState());
    REQUIRE(lhs.ToString() == rhs.ToString());
}

void Compare(IOpmlOutline const& lhs, IOpmlOutline const& rhs);

void Compare(IOpmlOutlines const& lhs, IOpmlOutlines const& rhs)
{
    REQUIRE(lhs.Size() == rhs.Size());

    for (uint32_t i = 0; i < lhs.Size(); ++i)
    {
        Compare(lhs.GetAt(i), rhs.GetAt(i));
    }

    REQUIRE(lhs.ToString() == rhs.ToString());
}

void Compare(IOpmlOutline const& lhs, IOpmlOutline const& rhs)
{
    Compare(lhs.Attributes(), rhs.Attributes());
    Compare(lhs.Outlines(), rhs.Outlines());
    REQUIRE(lhs.ToString() == rhs.ToString());
}

void Compare(IOpmlDocument const& lhs, IOpmlDocument const& rhs)
{
    REQUIRE(lhs.Version() == rhs.Version());
    Compare(lhs.Header(), rhs.Header());
    Compare(lhs.Body(), rhs.Body());
    REQUIRE(lhs.ToString() == rhs.ToString());
}

IAsyncAction LoadSaveCompare(StorageFile const& filein)
{
    auto document = co_await OpmlDocument::LoadFromFileAsync(filein);

    auto folder = co_await filein.GetParentAsync();

    std::wstring outname = filein.Name().data(); outname += L".out";

    StorageFile fileout = co_await folder.CreateFileAsync(outname.data(), CreationCollisionOption::ReplaceExisting);

    co_await document.SaveToFileAsync(fileout);

    auto document2 = co_await OpmlDocument::LoadFromFileAsync(fileout);

    Compare(document, document2);

    co_return;
}

IAsyncAction LoadSaveCompareAll()
{
    StorageFolder thisfolder = co_await StorageFolder::GetFolderFromPathAsync(GetThisPath());
    StorageFolder datafolder = co_await thisfolder.GetFolderAsync(L"Data"sv);

    QueryOptions queryOptions(CommonFileQuery::OrderBySearchRank, { L"*" });
    queryOptions.UserSearchFilter(L"*.opml");
    StorageFileQueryResult queryResult = datafolder.CreateFileQueryWithOptions(queryOptions);

    IVectorView<StorageFile> files = co_await queryResult.GetFilesAsync();
    for (auto&& file : files)
    {
        co_await LoadSaveCompare(file);
    }

    co_return;
}

TEST_CASE("LoadSaveCompareAll", "[document]")
{
    LoadSaveCompareAll().get();
}
