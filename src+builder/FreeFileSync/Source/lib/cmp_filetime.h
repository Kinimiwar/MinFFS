// **************************************************************************
// * This file is part of the FreeFileSync project. It is distributed under *
// * GNU General Public License: http://www.gnu.org/licenses/gpl-3.0        *
// * Copyright (C) Zenju (zenju AT gmx DOT de) - All Rights Reserved        *
// **************************************************************************

#ifndef CMP_FILETIME_H_032180451675845
#define CMP_FILETIME_H_032180451675845

#include <ctime>
#include <algorithm>

namespace zen
{
//---------------------------------------------------------------------------------------------------------------
inline
bool sameFileTime(std::int64_t lhs, std::int64_t rhs, int tolerance, unsigned int optTimeShiftHours)
{
    if (tolerance < 0) //= unlimited tolerance by convention!
        return true;

    if (lhs < rhs)
        std::swap(lhs, rhs);

    if (lhs - rhs <= tolerance)
        return true;

    if (optTimeShiftHours > 0)
    {
        const int shiftSec = static_cast<int>(optTimeShiftHours) * 3600;
        if (rhs <= std::numeric_limits<std::int64_t>::max() - shiftSec) //protect against integer overflow!
        {
            const std::int64_t low  = std::min(rhs + shiftSec, lhs);
            const std::int64_t high = std::max(rhs + shiftSec, lhs);

            if (high - low <= tolerance)
                return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------

enum class TimeResult
{
    EQUAL,
    LEFT_NEWER,
    RIGHT_NEWER,
    LEFT_INVALID,
    RIGHT_INVALID
};


inline
TimeResult compareFileTime(std::int64_t lhs, std::int64_t rhs, int tolerance, unsigned int optTimeShiftHours)
{
#if defined _MSC_VER && _MSC_VER < 1900
#error function scope static initialization is not yet thread-safe!
#endif

    //number of seconds since Jan 1st 1970 + 1 year (needn't be too precise)
    static const std::int64_t oneYearFromNow = std::time(nullptr) + 365 * 24 * 3600;

    if (sameFileTime(lhs, rhs, tolerance, optTimeShiftHours)) //last write time may differ by up to 2 seconds (NTFS vs FAT32)
        return TimeResult::EQUAL;

    //check for erroneous dates
    if (lhs < 0 || lhs > oneYearFromNow) //earlier than Jan 1st 1970 or more than one year in future
        return TimeResult::LEFT_INVALID;

    if (rhs < 0 || rhs > oneYearFromNow)
        return TimeResult::RIGHT_INVALID;

    //regular time comparison
    if (lhs < rhs)
        return TimeResult::RIGHT_NEWER;
    else
        return TimeResult::LEFT_NEWER;
}
}

#endif //CMP_FILETIME_H_032180451675845
