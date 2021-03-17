#pragma once

#include "core/status.h"
#include <string>

namespace rk::unicode
{
Status wide_to_ansi(std::wstring const& ws, std::string& s) noexcept;
Status ansi_to_wide(std::string const& s, std::wstring& ws) noexcept;
Status wide_to_utf8(std::wstring const& ws, std::string& s) noexcept;
Status utf8_to_wide(std::string const& s, std::wstring& ws) noexcept;
} // namespace rk::unicode
