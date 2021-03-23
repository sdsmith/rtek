#pragma once

namespace rk
{
/**
 * \brief UTF-8 aware getenv. Returns 0 if the variable is not set.
 *
 * This function is not thread safe or reenterable as defined by the standard library.
 *
 * \see std::getenv
 */
char* getenv(const char* key) noexcept;

/**
 * \brief Same as std::system but cmd is UTF-8.
 *
 * \see std::system
 */
int system(const char* cmd) noexcept;

/**
 * \brief Set environment variable
 *
 * \param overwrite If true any existing value is always overwritten, otherwise, if the variable
 * exists it remains unchanged.
 *
 * \param key Name. UTF-8 on Windows.
 * \param value Value. UTF-8 on Windows.
 * \return zero on success, else nonzero
 */
bool setenv(const char* key, const char* value, bool overwrite) noexcept;

/**
 * \brief Remove environment variable
 *
 * \param key Envvar name. Is UTF-8 on Windows
 * \return zero on success, else nonzero
 */
bool unsetenv(const char* key) noexcept;

/**
 * \brief Adds or changes an environment variable
 *
 * \param string Format KEY=VALUE. MAY become part of the environment, hence changes to the value
 * MAY change the environment. For portability it is hence recommended NOT to change it. UTF-8 on
 * Windows. \return zero on success, else nonzero
 */
bool putenv(char* string) noexcept;

}; // namespace rk
