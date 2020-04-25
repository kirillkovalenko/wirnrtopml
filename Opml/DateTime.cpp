#include "Pch.hpp"

#include "DateTime.hpp"
#include "OpmlDateTime.g.cpp"

// http://dev.opml.org/spec2.html:
// All date-times conform to the Date and Time Specification of RFC 822, 
// with the exception that the year may be expressed with two characters 
// or four characters (four preferred).

// RFC822 datetime specification:
//    date-time   =  [ day "," ] date time
//    day         =  "Mon"  | "Tue" |  "Wed"  | "Thu" |  "Fri"  | "Sat" |  "Sun"
//    date        =  1*2DIGIT month 2DIGIT
//    month       =  "Jan"  |  "Feb" |  "Mar"  |  "Apr"
//                |  "May"  |  "Jun" |  "Jul"  |  "Aug"
//                |  "Sep"  |  "Oct" |  "Nov"  |  "Dec"
//    time        =  hour zone
//    hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT]
//    zone        =  "UT"  | "GMT"
//                |  "EST" | "EDT"
//                |  "CST" | "CDT"
//                |  "MST" | "MDT"
//                |  "PST" | "PDT"
//                | 1ALPHA | ( ("+" | "-") 4DIGIT )

namespace Rfc822
{
    struct DateTimeInfo final : SYSTEMTIME
    {
        int32_t wTzOffset{};

        DateTimeInfo() noexcept
            : SYSTEMTIME{}
        {
        }
    };

    static const std::array<std::wstring_view, 7> WeekDays =
    {
        L"Sun"sv, L"Mon"sv, L"Tue"sv, L"Wed"sv, L"Thu"sv, L"Fri"sv, L"Sat"sv
    };

    static const std::array<std::wstring_view, 12> Months =
    {
        L"Jan"sv, L"Feb"sv, L"Mar"sv, L"Apr"sv, L"May"sv, L"Jun"sv,
        L"Jul"sv, L"Aug"sv, L"Sep"sv, L"Oct"sv, L"Nov"sv, L"Dec"sv
    };

    struct TimeZoneInfo final
    {
        std::wstring_view name;
        int32_t offset{};
    };

    // https://developercommunity.visualstudio.com/content/problem/265542/error-c2078-too-many-initializers-when-using-stdar-1.html
    static const std::array<TimeZoneInfo, 15> TimeZones =
    {
        {
            { L"UT"sv, 0 },
            { L"GMT"sv, 0 },
            { L"EST"sv, -18000 },
            { L"EDT"sv, -14400 },
            { L"CST"sv, -21600 },
            { L"CDT"sv, -18000 },
            { L"MST"sv, -25200 },
            { L"MDT"sv, -21600 },
            { L"PST"sv, -28800 },
            { L"PDT"sv, -25200 },
            { L"Z"sv, 0 },
            { L"A"sv, -3600 },
            { L"M"sv, -43200 },
            { L"N"sv, +3600 },
            { L"Y"sv, +43200 }
        }
    };

    static uint16_t Get2Digit(const wchar_t*& string, const bool optional2 = false)
    {
        uint16_t result{};

        if (isdigit(*string))
        {
            result = static_cast<uint16_t>(*string - L'0');
            ++string;
        }
        else
        {
            throw std::invalid_argument("integer");
        }

        if (isdigit(*string))
        {
            result *= 10;
            result += static_cast<uint16_t>(*string - L'0');
            ++string;
        }
        else if (!optional2)
        {
            throw std::invalid_argument("integer");
        }

        return result;
    }

    static int32_t Get4Digit(const wchar_t* & string, const bool optional2 = false)
    {
        int32_t result{};

        for (size_t i = 0; i < 4; ++i)
        {
            if (isdigit(*string))
            {
                result *= 10;
                result += static_cast<int32_t>(*string - L'0');
                ++string;
            }
            else
            {
                if (optional2 && i == 2)
                {
                    return result;
                }
                else
                {
                    throw std::invalid_argument("integer");
                }
            }
        }

        return result;
    }

    static inline void GotoNextToken(const wchar_t*& string) noexcept
    {
        while (*string == L' ') ++string;
    }

    static inline bool niequal(std::wstring_view const& lhs, const wchar_t* rhs) noexcept
    {
        return 0 == _wcsnicmp(rhs, lhs.data(), lhs.size());
    }

    static inline bool niequal(const wchar_t* lhs, std::wstring_view const& rhs) noexcept
    {
        return 0 == _wcsnicmp(lhs, rhs.data(), rhs.size());
    }

    static DateTimeInfo ParseDateTime(const wchar_t* string)
    {
        DateTimeInfo result;

        //
        // should either start with a digit or have [ day "," ] prefix
        //

        if (isalpha(*string))
        {
            auto i = std::find_if(std::begin(WeekDays), std::end(WeekDays), [=](auto&& day) { return niequal(string, day); });
            if (i == std::end(WeekDays))
            {
                throw std::invalid_argument("dayofweek");
            }
            else
            {
                string += i->size();
            }

            if (*string == L',')
            {
                // skip ',' and spaces
                ++string;
                GotoNextToken(string);
            }
            else
            {
                throw std::invalid_argument("token");
            }
        }

        //
        // parse date
        //

        // day of the month
        result.wDay = Get2Digit(string, true);
        if (result.wDay < 1 || result.wDay > 31)
        {
            throw std::invalid_argument("day");
        }

        // skip spaces
        GotoNextToken(string);

        // get month
        auto month = std::find_if(std::begin(Months), std::end(Months), [=](auto&& m) { return niequal(string, m); });
        if (month == std::end(Months))
        {
            throw std::invalid_argument("month");
        }
        else
        {
            auto index = std::distance(std::begin(Months), month);
            result.wMonth = static_cast<WORD>(index + 1); // wMonth is 1 based
            string += month->size();
        }

        // skip spaces
        GotoNextToken(string);

        // get year
        int32_t year = Get4Digit(string, true);
        if (year < 100)
        {
            // legacy (2 digit) date format 
            result.wYear = static_cast<WORD>(1900 + year);
        }
        else
        {
            // modern (4-digit) date format
            result.wYear += static_cast<WORD>(year);
        }

        // skip spaces
        GotoNextToken(string);

        //
        // time 
        //

        result.wHour = Get2Digit(string);
        if (result.wHour > 23)
        {
            throw std::invalid_argument("hours");
        }

        if (*string == L':')
        {
            ++string;
        }
        else
        {
            throw std::invalid_argument("separator");
        }

        result.wMinute = Get2Digit(string);
        if (result.wMinute > 59)
        {
            throw std::invalid_argument("minutes");
        }

        // check for optional seconds
        if (*string == L':')
        {
            ++string;
            result.wSecond = Get2Digit(string);
            if (result.wSecond > 59)
            {
                throw std::invalid_argument("seconds");
            }
        }

        // skip spaces
        GotoNextToken(string);

        //  check for the end of the string
        if (*string == L'\0')
        {
            throw std::invalid_argument("eol");
        }

        // 
        // time zone information
        //

        // try by name
        auto timezone = std::find_if(std::begin(TimeZones), std::end(TimeZones), [=](auto&& tz) { return niequal(string, tz.name); });
        if (timezone != std::end(TimeZones))
        {
            result.wTzOffset = timezone->offset;

            string += timezone->name.size();

            if (*string != L'\0')
            {
                throw std::invalid_argument("eol");
            }
            else
            {
                return result;
            }
        }

        // try by value
        if (*string != L'+' && *string != L'-')
        {
            throw std::invalid_argument("sign");
        }

        const bool isNegative = (*string == L'-');

        ++string; // skip sign

        int32_t offset = Get4Digit(string);

        offset = (offset / 100) * 60 + (offset % 100);
        result.wTzOffset = isNegative ? -offset : offset;

        // no more data expected
        if (*string != L'\0')
        {
            throw std::invalid_argument("datetime");
        }

        return result;
    }

    static std::wstring Format(SYSTEMTIME const& value)
    {
        std::wstringstream ss;

        ss << WeekDays.at(value.wDayOfWeek) << L","sv << L" "sv;

        ss << value.wDay << L" "sv;

        ss << Months.at(value.wMonth - 1) << L" "sv << value.wYear << L" "sv;

        ss << std::setfill(L'0') << std::setw(2) << value.wHour << L":"sv;
            
        ss << std::setfill(L'0') << std::setw(2) << value.wMinute << L":"sv;

        ss << std::setfill(L'0') << std::setw(2) << value.wSecond << L" "sv;

        ss << L"GMT"sv;

        return ss.str();
    }

    static Windows::Foundation::DateTime Parse(const wchar_t* value)
    {
        // parse the string actually
        auto datetime = Rfc822::ParseDateTime(value);

        // create timezone information
        TIME_ZONE_INFORMATION tzi{};
        tzi.Bias = -datetime.wTzOffset;

        // adjust timezone difference
        SYSTEMTIME universalTime{};
        check_bool(TzSpecificLocalTimeToSystemTime(&tzi, &datetime, &universalTime));

        // get result
        FILETIME ft{};
        check_bool(SystemTimeToFileTime(&universalTime, &ft));

        return clock::from_file_time(ft);
    }
}

namespace winrt::Kirkov::Data::Opml::implementation
{
    Windows::Foundation::DateTime OpmlDateTime::Parse(hstring const& value)
    {
        return Rfc822::Parse(value.data());
    }

    hstring OpmlDateTime::ToString(Windows::Foundation::DateTime const& value)
    {
        FILETIME filetime = clock::to_file_time(value);

        SYSTEMTIME systemTime{};
        check_bool(FileTimeToSystemTime(&filetime, &systemTime));

        return Rfc822::Format(systemTime).data();
    }
}
