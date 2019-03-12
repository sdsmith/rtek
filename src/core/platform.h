#if defined(_WIN32) || defined(_WIN64)
#elif defined(__CYGWIN__) && !defined(_WIN32)
#   warning Cygwin is not officially supported
#else
#   error Unsupported platform
#endif

#if defined(_MSC_VER)
//  Compiling with MSVC
#   define RTK_FUNC_NAME __FUNCTION__
#else
#   define RTK_FUNC_NAME __FUNC__
#endif