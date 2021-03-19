#include <gtest/gtest.h>

#include "core/platform/filesystem.h"
#include "core/platform/platform.h"
#include "core/platform/unicode.h"
#include "core/types.h"
#include "tests/common.h"
#include <fmt/core.h>
#include <filesystem>

using namespace rk;
namespace stdfs = std::filesystem;

// TODO(sdsmith): Check path sizes after each function is called. make sure it's being update
// correctly!

class FilesystemTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        m_original_wd = stdfs::current_path();
        stdfs::current_path(stdfs::temp_directory_path());
        stdfs::create_directory("rteklib_test");
        stdfs::path wd = stdfs::current_path();

#if RK_OS == RK_OS_WINDOWS
        m_test_dir = wd.wstring() + UC(RK_PATH_SEPARATOR_STR "rteklib_test");
#else
        m_test_dir = wd.string() + UC(RK_PATH_SEPARATOR_STR "rteklib_test");
#endif
    }

    virtual void TearDown()
    {
        stdfs::remove_all(m_test_dir);
        stdfs::current_path(m_original_wd);
    }

    ustring const& get_test_dir() { return m_test_dir; }

private:
    stdfs::path m_original_wd;
    ustring m_test_dir;
};

// TODO(sdsmith): @perf: tests that don't modify the file system shouldn't use the fixture that
// prepares the filesystem (FilesystemTest).

TEST_F(FilesystemTest, path_is_windows_extended_path)
{
    fs::Path path;

    path = fs::Path(UC(""));
    EXPECT_FALSE(path.is_windows_extended_path());
    path = fs::Path(UC("a\\b\\c\\"));
    EXPECT_FALSE(path.is_windows_extended_path());
    path = fs::Path(UC("\\\\\\\\"));
    EXPECT_FALSE(path.is_windows_extended_path());
    path = fs::Path(UC("\\\\?"));
    EXPECT_FALSE(path.is_windows_extended_path());
    path = fs::Path(UC("\\\\\\?\\"));
    EXPECT_FALSE(path.is_windows_extended_path());
    path = fs::Path(UC("\\a\\b\\\\?\\"));
    EXPECT_FALSE(path.is_windows_extended_path());

    path = fs::Path(UC("\\\\?\\"));
    EXPECT_TRUE(path.is_windows_extended_path());
    path = fs::Path(UC("\\\\?\\a\\b\\c\\"));
    EXPECT_TRUE(path.is_windows_extended_path());
    path = fs::Path(UC("\\\\?\\UNC\\"));
    EXPECT_TRUE(path.is_windows_extended_path());
    path = fs::Path(UC("\\\\?\\UNC\\a\\b\\c\\"));
    EXPECT_TRUE(path.is_windows_extended_path());
}

TEST_F(FilesystemTest, path_is_windows_unc_path)
{
    fs::Path path;

    path = fs::Path(UC(""));
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path(UC("a\\b\\c\\"));
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path(UC("\\\\\\\\"));
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path(UC("\\\\?"));
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path(UC("\\\\\\?\\"));
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path(UC("\\a\\b\\\\?\\"));
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path(UC("\\\\?\\UNC"));
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path(UC("\\\\?\\a\\b\\c\\"));
    EXPECT_FALSE(path.is_windows_unc_path());

    path = fs::Path(UC("\\\\?\\UNC\\"));
    EXPECT_TRUE(path.is_windows_unc_path());
    path = fs::Path(UC("\\\\?\\UNC\\a\\b\\c\\"));
    EXPECT_TRUE(path.is_windows_unc_path());
}

TEST_F(FilesystemTest, create_directory)
{
    ustring path =
        fmt::format(UC("{}" RK_PATH_SEPARATOR_STR "{}"), get_test_dir().c_str(), UC("hello_world"));
    EXPECT_FALSE(fs::directory_exists(path.c_str()));
    RK_EXPECT_OK(fs::create_directory(path.c_str()));
    EXPECT_TRUE(fs::directory_exists(path.c_str()));
}

TEST_F(FilesystemTest, path_normalize)
{
    fs::Path path;

#if RK_OS == RK_OS_WINDOWS
    path = fs::Path(UC("C:\\hello\\world"));
    RK_EXPECT_OK(fs::path_normalize(path));
    EXPECT_STREQ(path.data(), UC("C:\\hello\\world"));

    path = fs::Path(UC("C:/hello\\world"));
    RK_EXPECT_OK(fs::path_normalize(path));
    EXPECT_STREQ(path.data(), UC("C:\\hello\\world"));

    path = fs::Path(UC("C:/hello/world"));
    RK_EXPECT_OK(fs::path_normalize(path));
    EXPECT_STREQ(path.data(), UC("C:\\hello\\world"));

#elif RK_OS == RK_OS_LINUX
    path = fs::Path(UC("/hello/world"));
    RK_EXPECT_OK(fs::path_normalize(path));
    EXPECT_STREQ(path.data(), UC("/hello/world"));

    path = fs::Path(UC("/hello\\ world"));
    RK_EXPECT_OK(fs::path_normalize(path));
    EXPECT_STREQ(path.data(), UC("/hello\\ world"));
#endif
}

TEST_F(FilesystemTest, path_add_tailing_slash)
{
    // No ending slash
    fs::Path path(UC("hello"));
    RK_EXPECT_OK(fs::path_add_trailing_slash(path));
    EXPECT_STREQ(path.data(), UC("hello" RK_PATH_SEPARATOR_STR));
    path = fs::Path(UC("/example/path"));
    RK_EXPECT_OK(fs::path_add_trailing_slash(path));
    EXPECT_STREQ(path.data(), UC("/example/path" RK_PATH_SEPARATOR_STR));

    // Ending slash
    path = fs::Path(UC("/example/unix/slash/"));
    RK_EXPECT_OK(fs::path_add_trailing_slash(path));
    EXPECT_STREQ(path.data(), UC("/example/unix/slash/"));
    path = fs::Path(UC("/example/windows/slash\\"));
    RK_EXPECT_OK(fs::path_add_trailing_slash(path));
    EXPECT_STREQ(path.data(), UC("/example/windows/slash\\"));

    // Root
    path = fs::Path(UC("/"));
    RK_EXPECT_OK(fs::path_add_trailing_slash(path));
    EXPECT_STREQ(path.data(), UC("/"));

    // Empty string
    path = fs::Path(UC(""));
    EXPECT_STREQ(path.data(), UC(""));
}

TEST_F(FilesystemTest, path_remove_tailing_slash)
{
    // TODO(sdsmith): convert to path_remove_trailing_slash
    // No ending slash
    fs::Path path(UC("hello" RK_PATH_SEPARATOR_STR));
    RK_EXPECT_OK(fs::path_remove_trailing_slash(path));
    EXPECT_STREQ(path.data(), UC("hello"));
    path = fs::Path(UC("/example/path" RK_PATH_SEPARATOR_STR));
    RK_EXPECT_OK(fs::path_remove_trailing_slash(path));
    EXPECT_STREQ(path.data(), UC("/example/path"));

    // Ending slash
    path = fs::Path(UC("/example/unix/slash/"));
    RK_EXPECT_OK(fs::path_remove_trailing_slash(path));
    EXPECT_STREQ(path.data(), UC("/example/unix/slash"));
    path = fs::Path(UC("/example/windows/slash\\"));
    RK_EXPECT_OK(fs::path_remove_trailing_slash(path));
    EXPECT_STREQ(path.data(), UC("/example/windows/slash"));

#if RK_OS == RK_OS_LINUX
    // Root
    path = fs::Path(UC("/"));
    RK_EXPECT_OK(fs::path_remove_trailing_slash(path));
    EXPECT_STREQ(path.data(), UC("/"));
#endif

    // Empty string
    path = fs::Path(UC(""));
    EXPECT_STREQ(path.data(), UC(""));
}

TEST_F(FilesystemTest, path_add_extension)
{
    uchar const* exe0 = UC("exe");
    uchar const* exe1 = UC(".exe");
    uchar const* junk0 = UC("asdfghj");
    uchar const* junk1 = UC(".asdfghj");

    // File
    fs::Path path(UC("abc"));
    RK_EXPECT_OK(fs::path_add_extension(path, exe0));
    EXPECT_STREQ(path.data(), UC("abc.exe"));
    path = fs::Path(UC("abc"));
    RK_EXPECT_OK(fs::path_add_extension(path, exe1));
    EXPECT_STREQ(path.data(), UC("abc.exe"));
    path = fs::Path(UC("abc"));
    RK_EXPECT_OK(fs::path_add_extension(path, junk0));
    EXPECT_STREQ(path.data(), UC("abc.asdfghj"));
    path = fs::Path(UC("abc"));
    RK_EXPECT_OK(fs::path_add_extension(path, junk1));
    EXPECT_STREQ(path.data(), UC("abc.asdfghj"));

    // An actual path
    path = fs::Path(UC("/test/path/b"));
    RK_EXPECT_OK(fs::path_add_extension(path, exe0));
    EXPECT_STREQ(path.data(), UC("/test/path/b.exe"));
    path = fs::Path(UC("test/path/zxcf"));
    RK_EXPECT_OK(fs::path_add_extension(path, exe1));
    EXPECT_STREQ(path.data(), UC("test/path/zxcf.exe"));
}

TEST_F(FilesystemTest, path_append)
{
    fs::Path path;

    path = fs::Path(UC("hello"));
    RK_EXPECT_OK(fs::path_append(path, UC("world")));
    EXPECT_STREQ(path.data(), UC("hello" RK_PATH_SEPARATOR_STR "world"));

    path = fs::Path(UC("hello/"));
    RK_EXPECT_OK(fs::path_append(path, UC("world")));
    EXPECT_STREQ(path.data(), UC("hello" RK_PATH_SEPARATOR_STR "world"));

#if RK_OS == RK_OS_WINDOWS
    path = fs::Path(UC("hello\\"));
    RK_EXPECT_OK(fs::path_append(path, UC("world")));
    EXPECT_STREQ(path.data(), UC("hello\\world"));
    path = fs::Path(UC("hello"));
    RK_EXPECT_OK(fs::path_append(path, UC("\\world")));
    EXPECT_STREQ(path.data(), UC("hello\\world"));
    path = fs::Path(UC("hello\\"));
    RK_EXPECT_OK(fs::path_append(path, UC("\\world")));
    EXPECT_STREQ(path.data(), UC("hello\\world"));
#else
    path = fs::Path(UC("hello"));
    RK_EXPECT_OK(fs::path_append(path, UC("/world")));
    EXPECT_STREQ(path.data(), UC("hello/world"));
    path = fs::Path(UC("hello/"));
    RK_EXPECT_OK(fs::path_append(path, UC("/world")));
    EXPECT_STREQ(path.data(), UC("hello/world"));
#endif
}

TEST_F(FilesystemTest, path_remove_dup_separators)
{
    fs::Path path;

#define EXPECT_RM_DUP_SEP(X, Y)                         \
    path = fs::Path(X);                                 \
    RK_EXPECT_OK(fs::path_remove_dup_separators(path)); \
    EXPECT_STREQ(path.data(), Y);                       \
    EXPECT_EQ(path.size(), platform::ustrlen(Y));

#if RK_OS == RK_OS_WINDOWS
    EXPECT_RM_DUP_SEP(UC("a"), UC("a"));
    EXPECT_RM_DUP_SEP(UC("a\\"), UC("a\\"));
    EXPECT_RM_DUP_SEP(UC("a\\\\"), UC("a\\"));
    EXPECT_RM_DUP_SEP(UC("a\\\\\\"), UC("a\\"));
    EXPECT_RM_DUP_SEP(UC("a\\b"), UC("a\\b"));
    EXPECT_RM_DUP_SEP(UC("a\\\\\\b"), UC("a\\b"));

    // Windows allows two path separators as a path prefix
    EXPECT_RM_DUP_SEP(UC("\\\\"), UC("\\\\"));
    EXPECT_RM_DUP_SEP(UC("\\\\\\"), UC("\\\\"));
    EXPECT_RM_DUP_SEP(UC("\\\\a\\\\\\b"), UC("\\\\a\\b"));

#elif RK_OS == RK_OS_LINUX
    // TODO(sdsmith): other test cases

    // Linux allows two path separators as a path prefix
    EXPECT_RM_DUP_SEP(UC("//"), UC("//"));
    EXPECT_RM_DUP_SEP(UC("///"), UC("//"));
    EXPECT_RM_DUP_SEP(UC("//a///b"), UC("//a/b"));
#endif
}

TEST_F(FilesystemTest, path_canonicalize)
{
    fs::Path path;

#if RK_OS == RK_OS_WINDOWS
    RK_EXPECT_OK(fs::path_canonicalize(UC("C:\\name_1\\.\\name_2\\..\\name_3"), path));
    EXPECT_STREQ(path.data(), UC("C:\\name_1\\name_3"));
    RK_EXPECT_OK(fs::path_canonicalize(UC("C:\\name_1\\..\\name_2\\.\\name_3"), path));
    EXPECT_STREQ(path.data(), UC("C:\\name_2\\name_3"));
    RK_EXPECT_OK(fs::path_canonicalize(UC("C:\\name_1\\name_2\\.\\name_3\\..\\name_4"), path));
    EXPECT_STREQ(path.data(), UC("C:\\name_1\\name_2\\name_4"));
    RK_EXPECT_OK(
        fs::path_canonicalize(UC("C:\\name_1\\.\\name_2\\.\\name_3\\..\\name_4\\.."), path));
    EXPECT_STREQ(path.data(), UC("C:\\name_1\\name_2"));
    RK_EXPECT_OK(fs::path_canonicalize(UC("C:\\name_1\\*..."), path));
    EXPECT_STREQ(path.data(), UC("C:\\name_1\\*."));
    RK_EXPECT_OK(fs::path_canonicalize(UC("C:\\.."), path));
    EXPECT_STREQ(path.data(), UC("C:\\"));
#else
    RK_EXPECT_OK(fs::path_canonicalize(UC("/name_1/./name_2/../name_3"), path));
    EXPECT_STREQ(path.data(), UC("/name_1/name_3"));
    RK_EXPECT_OK(fs::path_canonicalize(UC("/name_1/../name_2/./name_3"), path));
    EXPECT_STREQ(path.data(), UC("/name_2/name_3"));
    RK_EXPECT_OK(fs::path_canonicalize(UC("/name_1/name_2/./name_3/../name_4"), path));
    EXPECT_STREQ(path.data(), UC("/name_1/name_2/name_4"));
    RK_EXPECT_OK(fs::path_canonicalize(UC("/name_1/./name_2/./name_3/../name_4/.."), path));
    EXPECT_STREQ(path.data(), UC("/name_1/name_2"));
    RK_EXPECT_OK(fs::path_canonicalize(UC("/name_1/*..."), path));
    EXPECT_STREQ(path.data(), UC("/name_1/*."));
    RK_EXPECT_OK(fs::path_canonicalize(UC("/.."), path));
    EXPECT_STREQ(path.data(), UC("/"));
#endif
}

TEST_F(FilesystemTest, path_clean)
{
    fs::Path path;

    path = fs::Path(UC(""));
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), UC(""));

#if RK_OS == RK_OS_WINDOWS
    path = fs::Path(UC("/abc/.\\..\\hello/"));
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), UC("\\hello"));

    path = fs::Path(UC("deep///blue\\"));
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), UC("deep\\blue"));

    path = fs::Path(UC("deep///blue\\.."));
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), UC("deep"));

    path = fs::Path(UC("deep///blue\\\\./.\\..\\ocean\\.\\"));
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), UC("deep\\ocean"));

#elif RK_OS == RK_OS_LINUX
    path = fs::Path(UC(""));
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), UC(""));

    path = fs::Path(UC(""));
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), UC(""));

    path = fs::Path(UC(""));
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), UC(""));

#endif
}

TEST_F(FilesystemTest, path_combine)
{
    // TODO(sdsmith):
}

TEST_F(FilesystemTest, path_common_prefix)
{
    fs::Path path;

#if RK_OS == RK_OS_WINDOWS
    RK_EXPECT_OK(fs::path_common_prefix(UC("\\a\\b\\c\\d"), UC("\\a\\b\\c\\d"), path));
    EXPECT_STREQ(path.data(), UC("\\a\\b\\c\\d"));
    RK_EXPECT_OK(fs::path_common_prefix(UC("\\a\\b\\c\\d"), UC("\\a\\b\\c\\d\\e\\f"), path));
    EXPECT_STREQ(path.data(), UC("\\a\\b\\c\\d"));

    RK_EXPECT_OK(fs::path_common_prefix(UC("\\a\\b\\c\\d\\"), UC("\\a\\b\\c\\d\\e\\f"), path));
    EXPECT_STREQ(path.data(), UC("\\a\\b\\c\\d\\"));
    RK_EXPECT_OK(fs::path_common_prefix(UC("\\a\\b\\c\\d\\"), UC("\\a\\b\\c\\d\\"), path));
    EXPECT_STREQ(path.data(), UC("\\a\\b\\c\\d\\"));

    RK_EXPECT_OK(fs::path_common_prefix(UC("C:\\apple\\back\\cmd\\dock"),
                                        UC("C:\\apple\\back\\cmd\\dock\\epslon\\folder"), path));
    EXPECT_STREQ(path.data(), UC("C:\\apple\\back\\cmd\\dock"));

#elif RK_OS == RK_OS_LINUX
    RK_EXPECT_OK(fs::path_common_prefix(UC("/a/b/c/d"), UC("/a/b/c/d"), path));
    EXPECT_STREQ(path.data(), UC("/a/b/c/d"));
    RK_EXPECT_OK(fs::path_common_prefix(UC("/a/b/c/d"), UC("/a/b/c/d/e/f"), path));
    EXPECT_STREQ(path.data(), UC("/a/b/c/d"));

    RK_EXPECT_OK(fs::path_common_prefix(UC("/a/b/c/d/"), UC("/a/b/c/d/e/f"), path));
    EXPECT_STREQ(path.data(), UC("/a/b/c/d"));
    RK_EXPECT_OK(fs::path_common_prefix(UC("/a/b/c/d/"), UC("/a/b/c/d/"), path));
    EXPECT_STREQ(path.data(), UC("/a/b/c/d/"));

    RK_EXPECT_OK(fs::path_common_prefix(UC("C:/apple/back/cmd/dock"),
                                        UC("C:/apple/back/cmd/dock/epslon/folder"), path));
    EXPECT_STREQ(path.data(), UC("C:/apple/back/cmd/dock"));
#endif
}

TEST_F(FilesystemTest, path_common_prefix_length)
{
    s32 len = 0;

#if RK_OS == RK_OS_WINDOWS
    RK_EXPECT_OK(fs::path_common_prefix_length(UC("\\a\\b\\c\\d"), UC("\\a\\b\\c\\d"), len));
    EXPECT_EQ(len, platform::ustrlen(UC("\\a\\b\\c\\d")));
    RK_EXPECT_OK(fs::path_common_prefix_length(UC("\\a\\b\\c\\d"), UC("\\a\\b\\c\\d\\e\\f"), len));
    EXPECT_EQ(len, platform::ustrlen(UC("\\a\\b\\c\\d")));

    RK_EXPECT_OK(
        fs::path_common_prefix_length(UC("\\a\\b\\c\\d\\"), UC("\\a\\b\\c\\d\\e\\f"), len));
    EXPECT_EQ(len, platform::ustrlen(UC("\\a\\b\\c\\d\\")));
    RK_EXPECT_OK(fs::path_common_prefix_length(UC("\\a\\b\\c\\d\\"), UC("\\a\\b\\c\\d\\"), len));
    EXPECT_EQ(len, platform::ustrlen(UC("\\a\\b\\c\\d\\")));

    RK_EXPECT_OK(fs::path_common_prefix_length(
        UC("C:\\apple\\back\\cmd\\dock"), UC("C:\\apple\\back\\cmd\\dock\\epslon\\folder"), len));
    EXPECT_EQ(len, platform::ustrlen(UC("C:\\apple\\back\\cmd\\dock")));

#elif RK_OS == RK_OS_LINUX
    RK_EXPECT_OK(fs::path_common_prefix_length(UC("/a/b/c/d"), UC("/a/b/c/d"), len));
    EXPECT_EQ(len, platform::ustrlen(UC("/a/b/c/d")));
    RK_EXPECT_OK(fs::path_common_prefix_length(UC("/a/b/c/d"), UC("/a/b/c/d/e/f"), len));
    EXPECT_EQ(len, platform::ustrlen(UC("/a/b/c/d")));

    RK_EXPECT_OK(fs::path_common_prefix_length(UC("/a/b/c/d/"), UC("/a/b/c/d/e/f"), len));
    EXPECT_EQ(len, platform::ustrlen(UC("/a/b/c/d")));
    RK_EXPECT_OK(fs::path_common_prefix_length(UC("/a/b/c/d/"), UC("/a/b/c/d/"), len));
    EXPECT_EQ(len, platform::ustrlen(UC("/a/b/c/d/")));

    RK_EXPECT_OK(fs::path_common_prefix_length(UC("C:/apple/back/cmd/dock"),
                                               UC("C:/apple/back/cmd/dock/epslon/folder"), len));
    EXPECT_EQ(len, platform::ustrlen(UC("C:/apple/back/cmd/dock")));
#endif
}

TEST_F(FilesystemTest, path_is_descendant)
{
    bool is_descendant = false;

    // TODO(sdsmith): normalize paths

#define EXPECT_IS_DESCENDANT(A, B)                             \
    RK_EXPECT_OK(fs::path_is_descendant(A, B, is_descendant)); \
    EXPECT_TRUE(is_descendant) << "Expect: " << A << " descendant of " << B;
#define EXPECT_NOT_DESCENDANT(A, B)                            \
    RK_EXPECT_OK(fs::path_is_descendant(A, B, is_descendant)); \
    EXPECT_FALSE(is_descendant) << "Expect: " << A << " not descendant of " << B;

#if RK_OS == RK_OS_WINDOWS
    EXPECT_NOT_DESCENDANT(UC("\\a\\b\\c"), UC("\\a\\b\\c"));
    EXPECT_NOT_DESCENDANT(UC("\\a\\b\\c"), UC(""));
    EXPECT_NOT_DESCENDANT(UC(""), UC("\\a\\b\\c"));
    EXPECT_NOT_DESCENDANT(UC(""), UC(""));
    EXPECT_NOT_DESCENDANT(UC("\\a\\b\\c"), UC("\\")); // slash as 'root' is only valid on linux

    EXPECT_IS_DESCENDANT(UC("\\a\\b\\c"), UC("\\a\\b\\"));
    EXPECT_IS_DESCENDANT(UC("\\a\\b\\c"), UC("\\a\\b"));
    EXPECT_IS_DESCENDANT(UC("\\a\\b\\c"), UC("\\a\\"));
    EXPECT_IS_DESCENDANT(UC("\\a\\b\\c"), UC("\\a"));

    EXPECT_NOT_DESCENDANT(UC("\\a\\b\\"), UC("\\a\\b\\c"));
    EXPECT_NOT_DESCENDANT(UC("\\a\\b"), UC("\\a\\b\\c"));
    EXPECT_NOT_DESCENDANT(UC("\\a\\"), UC("\\a\\b\\c"));
    EXPECT_NOT_DESCENDANT(UC("\\a"), UC("\\a\\b\\c"));
    EXPECT_NOT_DESCENDANT(UC("\\"), UC("\\a\\b\\c"));

#elif RK_OS == RK_OS_LINUX
    EXPECT_NOT_DESCENDANT(UC("/a/b/c"), UC("/a/b/c"));
    EXPECT_NOT_DESCENDANT(UC("/a/b/c"), UC(""));
    EXPECT_NOT_DESCENDANT(UC(""), UC("/a/b/c"));
    EXPECT_NOT_DESCENDANT(UC(""), UC(""));

    EXPECT_IS_DESCENDANT(UC("/a/b/c"), UC("/a/b/"));
    EXPECT_IS_DESCENDANT(UC("/a/b/c"), UC("/a/b"));
    EXPECT_IS_DESCENDANT(UC("/a/b/c"), UC("/a/"));
    EXPECT_IS_DESCENDANT(UC("/a/b/c"), UC("/a"));
    EXPECT_IS_DESCENDANT(UC("/a/b/c"), UC("/"));

    EXPECT_NOT_DESCENDANT(UC("/a/b/"), UC("/a/b/c"));
    EXPECT_NOT_DESCENDANT(UC("/a/b"), UC("/a/b/c"));
    EXPECT_NOT_DESCENDANT(UC("/a/"), UC("/a/b/c"));
    EXPECT_NOT_DESCENDANT(UC("/a"), UC("/a/b/c"));
    EXPECT_NOT_DESCENDANT(UC("/"), UC("/a/b/c"));
#endif
}
