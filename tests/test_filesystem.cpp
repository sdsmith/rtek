#include <gtest/gtest.h>

#include "core/platform/filesystem.h"
#include "core/platform/unicode.h"
#include "core/types.h"
#include "tests/common.h"
#include <fmt/core.h>
#include <sds/string.h>
#include <filesystem>
#include <string>

using namespace rk;
using namespace sds;
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

        m_test_dir = wd.string() + RK_PATH_SEPARATOR_STR "rteklib_test";
    }

    virtual void TearDown()
    {
        stdfs::remove_all(m_test_dir);
        stdfs::current_path(m_original_wd);
    }

    std::string const& get_test_dir() { return m_test_dir; }

private:
    stdfs::path m_original_wd;
    std::string m_test_dir;
};

// TODO(sdsmith): @perf: tests that don't modify the file system shouldn't use the fixture that
// prepares the filesystem (FilesystemTest).

TEST_F(FilesystemTest, path_is_windows_extended_path)
{
    fs::Path path;

    path = fs::Path("");
    EXPECT_FALSE(path.is_windows_extended_path());
    path = fs::Path("a\\b\\c\\");
    EXPECT_FALSE(path.is_windows_extended_path());
    path = fs::Path("\\\\\\\\");
    EXPECT_FALSE(path.is_windows_extended_path());
    path = fs::Path("\\\\?");
    EXPECT_FALSE(path.is_windows_extended_path());
    path = fs::Path("\\\\\\?\\");
    EXPECT_FALSE(path.is_windows_extended_path());
    path = fs::Path("\\a\\b\\\\?\\");
    EXPECT_FALSE(path.is_windows_extended_path());

    path = fs::Path("\\\\?\\");
    EXPECT_TRUE(path.is_windows_extended_path());
    path = fs::Path("\\\\?\\a\\b\\c\\");
    EXPECT_TRUE(path.is_windows_extended_path());
    path = fs::Path("\\\\?\\UNC\\");
    EXPECT_TRUE(path.is_windows_extended_path());
    path = fs::Path("\\\\?\\UNC\\a\\b\\c\\");
    EXPECT_TRUE(path.is_windows_extended_path());
}

TEST_F(FilesystemTest, path_is_windows_unc_path)
{
    fs::Path path;

    path = fs::Path("");
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path("a\\b\\c\\");
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path("\\\\\\\\");
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path("\\\\?");
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path("\\\\\\?\\");
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path("\\a\\b\\\\?\\");
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path("\\\\?\\UNC");
    EXPECT_FALSE(path.is_windows_unc_path());
    path = fs::Path("\\\\?\\a\\b\\c\\");
    EXPECT_FALSE(path.is_windows_unc_path());

    path = fs::Path("\\\\?\\UNC\\");
    EXPECT_TRUE(path.is_windows_unc_path());
    path = fs::Path("\\\\?\\UNC\\a\\b\\c\\");
    EXPECT_TRUE(path.is_windows_unc_path());
}

TEST_F(FilesystemTest, create_directory)
{
    std::string path =
        fmt::format("{}" RK_PATH_SEPARATOR_STR "{}", get_test_dir().c_str(), "hello_world");
    bool exists = false;

    RK_EXPECT_OK(fs::directory_exists(path.c_str(), exists));
    EXPECT_FALSE(exists);
    RK_EXPECT_OK(fs::create_directory(path.c_str()));
    RK_EXPECT_OK(fs::directory_exists(path.c_str(), exists));
    EXPECT_TRUE(exists);
}

TEST_F(FilesystemTest, path_normalize)
{
    fs::Path path;

#if SDS_OS_WINDOWS
    path = fs::Path("C:\\hello\\world");
    RK_EXPECT_OK(fs::path_normalize(path));
    EXPECT_STREQ(path.data(), "C:\\hello\\world");

    path = fs::Path("C:/hello\\world");
    RK_EXPECT_OK(fs::path_normalize(path));
    EXPECT_STREQ(path.data(), "C:\\hello\\world");

    path = fs::Path("C:/hello/world");
    RK_EXPECT_OK(fs::path_normalize(path));
    EXPECT_STREQ(path.data(), "C:\\hello\\world");

#elif SDS_OS_LINUX
    path = fs::Path("/hello/world");
    RK_EXPECT_OK(fs::path_normalize(path));
    EXPECT_STREQ(path.data(), "/hello/world");

    path = fs::Path("/hello\\ world");
    RK_EXPECT_OK(fs::path_normalize(path));
    EXPECT_STREQ(path.data(), "/hello\\ world");
#endif
}

TEST_F(FilesystemTest, path_add_tailing_separator)
{
    // No ending slash
    fs::Path path("hello");
    RK_EXPECT_OK(fs::path_add_trailing_separator(path));
    EXPECT_STREQ(path.data(), "hello" RK_PATH_SEPARATOR_STR);
    path = fs::Path("/example/path");
    RK_EXPECT_OK(fs::path_add_trailing_separator(path));
    EXPECT_STREQ(path.data(), "/example/path" RK_PATH_SEPARATOR_STR);

    // Ending slash
    path = fs::Path("/example/unix/slash/");
    RK_EXPECT_OK(fs::path_add_trailing_separator(path));
    EXPECT_STREQ(path.data(), "/example/unix/slash/");
    path = fs::Path("/example/windows/slash\\");
    RK_EXPECT_OK(fs::path_add_trailing_separator(path));
    EXPECT_STREQ(path.data(), "/example/windows/slash\\");

    // Root
    path = fs::Path("/");
    RK_EXPECT_OK(fs::path_add_trailing_separator(path));
    EXPECT_STREQ(path.data(), "/");

    // Empty string
    path = fs::Path("");
    EXPECT_STREQ(path.data(), "");
}

TEST_F(FilesystemTest, path_remove_tailing_separator)
{
    // TODO(sdsmith): convert to path_remove_trailing_separator
    // No ending slash
    fs::Path path("hello" RK_PATH_SEPARATOR_STR);
    RK_EXPECT_OK(fs::path_remove_trailing_separator(path));
    EXPECT_STREQ(path.data(), "hello");
    path = fs::Path("/example/path" RK_PATH_SEPARATOR_STR);
    RK_EXPECT_OK(fs::path_remove_trailing_separator(path));
    EXPECT_STREQ(path.data(), "/example/path");

    // Ending slash
    path = fs::Path("/example/unix/slash/");
    RK_EXPECT_OK(fs::path_remove_trailing_separator(path));
    EXPECT_STREQ(path.data(), "/example/unix/slash");
    path = fs::Path("/example/windows/slash\\");
    RK_EXPECT_OK(fs::path_remove_trailing_separator(path));
    EXPECT_STREQ(path.data(), "/example/windows/slash");

#if SDS_OS_LINUX
    // Root
    path = fs::Path("/");
    RK_EXPECT_OK(fs::path_remove_trailing_separator(path));
    EXPECT_STREQ(path.data(), "/");
#endif

    // Empty string
    path = fs::Path("");
    EXPECT_STREQ(path.data(), "");
}

TEST_F(FilesystemTest, path_has_tailing_separator)
{
    EXPECT_FALSE(fs::path_has_trailing_separator(""));
    EXPECT_TRUE(fs::path_has_trailing_separator(RK_PATH_SEPARATOR_STR));
    EXPECT_TRUE(fs::path_has_trailing_separator("hello" RK_PATH_SEPARATOR_STR));
    EXPECT_FALSE(fs::path_has_trailing_separator("hello"));
    EXPECT_FALSE(fs::path_has_trailing_separator("hello" RK_PATH_SEPARATOR_STR "world"));
    EXPECT_TRUE(fs::path_has_trailing_separator("hello" RK_PATH_SEPARATOR_STR
                                                "world" RK_PATH_SEPARATOR_STR));

    EXPECT_FALSE(fs::Path("").has_trailing_separator());
    EXPECT_TRUE(fs::Path(RK_PATH_SEPARATOR_STR).has_trailing_separator());
    EXPECT_TRUE(fs::Path("hello" RK_PATH_SEPARATOR_STR).has_trailing_separator());
    EXPECT_FALSE(fs::Path("hello").has_trailing_separator());
    EXPECT_FALSE(fs::Path("hello" RK_PATH_SEPARATOR_STR "world").has_trailing_separator());
    EXPECT_TRUE(fs::Path("hello" RK_PATH_SEPARATOR_STR "world" RK_PATH_SEPARATOR_STR)
                    .has_trailing_separator());
}

TEST_F(FilesystemTest, path_add_extension)
{
    char const* exe0 = "exe";
    char const* exe1 = ".exe";
    char const* junk0 = "asdfghj";
    char const* junk1 = ".asdfghj";

    // File
    fs::Path path("abc");
    RK_EXPECT_OK(fs::path_add_extension(path, exe0));
    EXPECT_STREQ(path.data(), "abc.exe");
    path = fs::Path("abc");
    RK_EXPECT_OK(fs::path_add_extension(path, exe1));
    EXPECT_STREQ(path.data(), "abc.exe");
    path = fs::Path("abc");
    RK_EXPECT_OK(fs::path_add_extension(path, junk0));
    EXPECT_STREQ(path.data(), "abc.asdfghj");
    path = fs::Path("abc");
    RK_EXPECT_OK(fs::path_add_extension(path, junk1));
    EXPECT_STREQ(path.data(), "abc.asdfghj");

    // An actual path
    path = fs::Path("/test/path/b");
    RK_EXPECT_OK(fs::path_add_extension(path, exe0));
    EXPECT_STREQ(path.data(), "/test/path/b.exe");
    path = fs::Path("test/path/zxcf");
    RK_EXPECT_OK(fs::path_add_extension(path, exe1));
    EXPECT_STREQ(path.data(), "test/path/zxcf.exe");
}

TEST_F(FilesystemTest, path_find_extension)
{
    char const* path;

    path = "";
    EXPECT_EQ(fs::path_find_extension(path), nullptr);
    path = RK_PATH_SEPARATOR_STR;
    EXPECT_EQ(fs::path_find_extension(path), nullptr);
    path = "hello" RK_PATH_SEPARATOR_STR;
    EXPECT_EQ(fs::path_find_extension(path), nullptr);
    path = "hello" RK_PATH_SEPARATOR_STR "world";
    EXPECT_EQ(fs::path_find_extension(path), nullptr);

    // File ending in period
    path = "hello.";
    EXPECT_EQ(fs::path_find_extension(path), nullptr);
    path = "hello.world.";
    EXPECT_EQ(fs::path_find_extension(path), nullptr);

    // Hidden files
    path = ".hello.";
    EXPECT_EQ(fs::path_find_extension(path), nullptr);
    path = ".hello";
    EXPECT_EQ(fs::path_find_extension(path), nullptr);
    path = ".hello.et";
    EXPECT_EQ(fs::path_find_extension(path), path + 6);
    path = "abc" RK_PATH_SEPARATOR_STR ".hello.";
    EXPECT_EQ(fs::path_find_extension(path), nullptr);
    path = "abc" RK_PATH_SEPARATOR_STR ".hello";
    EXPECT_EQ(fs::path_find_extension(path), nullptr);
    path = "abc" RK_PATH_SEPARATOR_STR ".hello.et";
    EXPECT_EQ(fs::path_find_extension(path), path + 10);

    path = "hello.world";
    EXPECT_EQ(fs::path_find_extension(path), path + 5);
    path = "hello.world.me";
    EXPECT_EQ(fs::path_find_extension(path), path + 11);
    path = "abc" RK_PATH_SEPARATOR_STR "hello.world.me";
    EXPECT_EQ(fs::path_find_extension(path), path + 15);
}
TEST_F(FilesystemTest, path_find_file_name)
{
    char const* path;
    fs::Path fs_path;

#define EXPECT_FIND_FILE_NAME(X, Ptr_Offset)                     \
    path = X;                                                    \
    EXPECT_EQ(fs::path_find_file_name(path), path + Ptr_Offset); \
    fs_path = fs::Path(path);                                    \
    EXPECT_EQ(fs::path_find_file_name(fs_path), fs_path.data() + Ptr_Offset)

#define EXPECT_FIND_FILE_NAME_NULL(X)                  \
    path = X;                                          \
    EXPECT_EQ(fs::path_find_file_name(path), nullptr); \
    fs_path = fs::Path(path);                          \
    EXPECT_EQ(fs::path_find_file_name(fs_path), nullptr)

    EXPECT_FIND_FILE_NAME_NULL("");
    EXPECT_FIND_FILE_NAME_NULL(RK_PATH_SEPARATOR_STR);
    EXPECT_FIND_FILE_NAME_NULL("hello" RK_PATH_SEPARATOR_STR);
    EXPECT_FIND_FILE_NAME("hello" RK_PATH_SEPARATOR_STR "world", 6);

    // File ending in period
    EXPECT_FIND_FILE_NAME("hello.", 0);
    EXPECT_FIND_FILE_NAME("hello.world.", 0);

    // Hidden files
    EXPECT_FIND_FILE_NAME(".hello.", 0);
    EXPECT_FIND_FILE_NAME(".hello", 0);
    EXPECT_FIND_FILE_NAME(".hello.et", 0);
    EXPECT_FIND_FILE_NAME("abc" RK_PATH_SEPARATOR_STR ".hello.", 4);
    EXPECT_FIND_FILE_NAME("abc" RK_PATH_SEPARATOR_STR ".hello", 4);
    EXPECT_FIND_FILE_NAME("abc" RK_PATH_SEPARATOR_STR ".hello.et", 4);

    EXPECT_FIND_FILE_NAME("hello.world", 0);
    EXPECT_FIND_FILE_NAME("hello.world.me", 0);
    EXPECT_FIND_FILE_NAME("abc" RK_PATH_SEPARATOR_STR "hello.world.me", 4);

#undef EXPECT_FIND_FILE_NAME_NULL
#undef EXPECT_FIND_FILE_NAME
}

TEST_F(FilesystemTest, path_append)
{
    fs::Path path;

    path = fs::Path("hello");
    RK_EXPECT_OK(fs::path_append(path, "world"));
    EXPECT_STREQ(path.data(), "hello" RK_PATH_SEPARATOR_STR "world");

    path = fs::Path("hello/");
    RK_EXPECT_OK(fs::path_append(path, "world"));
    EXPECT_STREQ(path.data(), "hello" RK_PATH_SEPARATOR_STR "world");

#if SDS_OS_WINDOWS
    path = fs::Path("hello\\");
    RK_EXPECT_OK(fs::path_append(path, "world"));
    EXPECT_STREQ(path.data(), "hello\\world");
    path = fs::Path("hello");
    RK_EXPECT_OK(fs::path_append(path, "\\world"));
    EXPECT_STREQ(path.data(), "hello\\world");
    path = fs::Path("hello\\");
    RK_EXPECT_OK(fs::path_append(path, "\\world"));
    EXPECT_STREQ(path.data(), "hello\\world");
#else
    path = fs::Path("hello");
    RK_EXPECT_OK(fs::path_append(path, "/world"));
    EXPECT_STREQ(path.data(), "hello/world");
    path = fs::Path("hello/");
    RK_EXPECT_OK(fs::path_append(path, "/world"));
    EXPECT_STREQ(path.data(), "hello/world");
#endif
}

TEST_F(FilesystemTest, path_remove_dup_separators)
{
    fs::Path path;

#define EXPECT_RM_DUP_SEP(X, Y)                         \
    path = fs::Path(X);                                 \
    RK_EXPECT_OK(fs::path_remove_dup_separators(path)); \
    EXPECT_STREQ(path.data(), Y);                       \
    EXPECT_EQ(path.size(), sds::str_size(Y));

#if SDS_OS_WINDOWS
    EXPECT_RM_DUP_SEP("a", "a");
    EXPECT_RM_DUP_SEP("a\\", "a\\");
    EXPECT_RM_DUP_SEP("a\\\\", "a\\");
    EXPECT_RM_DUP_SEP("a\\\\\\", "a\\");
    EXPECT_RM_DUP_SEP("a\\b", "a\\b");
    EXPECT_RM_DUP_SEP("a\\\\\\b", "a\\b");

    // Windows allows two path separators as a path prefix
    EXPECT_RM_DUP_SEP("\\\\", "\\\\");
    EXPECT_RM_DUP_SEP("\\\\\\", "\\\\");
    EXPECT_RM_DUP_SEP("\\\\a\\\\\\b", "\\\\a\\b");

#elif SDS_OS_LINUX
    // TODO(sdsmith): other test cases

    // Linux allows two path separators as a path prefix
    EXPECT_RM_DUP_SEP("//", "//");
    EXPECT_RM_DUP_SEP("///", "//");
    EXPECT_RM_DUP_SEP("//a///b", "//a/b");
#endif

#undef EXPECT_RM_DUP_SEP
}

TEST_F(FilesystemTest, path_canonicalize)
{
    fs::Path path;

#if SDS_OS_WINDOWS
    RK_EXPECT_OK(fs::path_canonicalize("C:\\name_1\\.\\name_2\\..\\name_3", path));
    EXPECT_STREQ(path.data(), "C:\\name_1\\name_3");
    RK_EXPECT_OK(fs::path_canonicalize("C:\\name_1\\..\\name_2\\.\\name_3", path));
    EXPECT_STREQ(path.data(), "C:\\name_2\\name_3");
    RK_EXPECT_OK(fs::path_canonicalize("C:\\name_1\\name_2\\.\\name_3\\..\\name_4", path));
    EXPECT_STREQ(path.data(), "C:\\name_1\\name_2\\name_4");
    RK_EXPECT_OK(fs::path_canonicalize("C:\\name_1\\.\\name_2\\.\\name_3\\..\\name_4\\..", path));
    EXPECT_STREQ(path.data(), "C:\\name_1\\name_2");
    RK_EXPECT_OK(fs::path_canonicalize("C:\\name_1\\*...", path));
    EXPECT_STREQ(path.data(), "C:\\name_1\\*.");
    RK_EXPECT_OK(fs::path_canonicalize("C:\\..", path));
    EXPECT_STREQ(path.data(), "C:\\");
#else
    RK_EXPECT_OK(fs::path_canonicalize("/name_1/./name_2/../name_3", path));
    EXPECT_STREQ(path.data(), "/name_1/name_3");
    RK_EXPECT_OK(fs::path_canonicalize("/name_1/../name_2/./name_3", path));
    EXPECT_STREQ(path.data(), "/name_2/name_3");
    RK_EXPECT_OK(fs::path_canonicalize("/name_1/name_2/./name_3/../name_4", path));
    EXPECT_STREQ(path.data(), "/name_1/name_2/name_4");
    RK_EXPECT_OK(fs::path_canonicalize("/name_1/./name_2/./name_3/../name_4/..", path));
    EXPECT_STREQ(path.data(), "/name_1/name_2");
    RK_EXPECT_OK(fs::path_canonicalize("/name_1/*...", path));
    EXPECT_STREQ(path.data(), "/name_1/*.");
    RK_EXPECT_OK(fs::path_canonicalize("/..", path));
    EXPECT_STREQ(path.data(), "/");
#endif
}

TEST_F(FilesystemTest, path_clean)
{
    fs::Path path;

    path = fs::Path("");
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), "");

#if SDS_OS_WINDOWS
    path = fs::Path("/abc/.\\..\\hello/");
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), "\\hello");

    path = fs::Path("deep///blue\\");
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), "deep\\blue");

    path = fs::Path("deep///blue\\..");
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), "deep");

    path = fs::Path("deep///blue\\\\./.\\..\\ocean\\.\\");
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), "deep\\ocean");

#elif SDS_OS_LINUX
    path = fs::Path("");
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), "");

    path = fs::Path("");
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), "");

    path = fs::Path("");
    RK_EXPECT_OK(fs::path_clean(path));
    EXPECT_STREQ(path.data(), "");

#endif
}

TEST_F(FilesystemTest, path_combine)
{
    // TODO(sdsmith):
}

TEST_F(FilesystemTest, path_common_prefix)
{
    fs::Path path;

#if SDS_OS_WINDOWS
    RK_EXPECT_OK(fs::path_common_prefix("\\a\\b\\c\\d", "\\a\\b\\c\\d", path));
    EXPECT_STREQ(path.data(), "\\a\\b\\c\\d");
    RK_EXPECT_OK(fs::path_common_prefix("\\a\\b\\c\\d", "\\a\\b\\c\\d\\e\\f", path));
    EXPECT_STREQ(path.data(), "\\a\\b\\c\\d");

    RK_EXPECT_OK(fs::path_common_prefix("\\a\\b\\c\\d\\", "\\a\\b\\c\\d\\e\\f", path));
    EXPECT_STREQ(path.data(), "\\a\\b\\c\\d\\");
    RK_EXPECT_OK(fs::path_common_prefix("\\a\\b\\c\\d\\", "\\a\\b\\c\\d\\", path));
    EXPECT_STREQ(path.data(), "\\a\\b\\c\\d\\");

    RK_EXPECT_OK(fs::path_common_prefix("C:\\apple\\back\\cmd\\dock",
                                        "C:\\apple\\back\\cmd\\dock\\epslon\\folder", path));
    EXPECT_STREQ(path.data(), "C:\\apple\\back\\cmd\\dock");

#elif SDS_OS_LINUX
    RK_EXPECT_OK(fs::path_common_prefix("/a/b/c/d", "/a/b/c/d", path));
    EXPECT_STREQ(path.data(), "/a/b/c/d");
    RK_EXPECT_OK(fs::path_common_prefix("/a/b/c/d", "/a/b/c/d/e/f", path));
    EXPECT_STREQ(path.data(), "/a/b/c/d");

    RK_EXPECT_OK(fs::path_common_prefix("/a/b/c/d/", "/a/b/c/d/e/f", path));
    EXPECT_STREQ(path.data(), "/a/b/c/d");
    RK_EXPECT_OK(fs::path_common_prefix("/a/b/c/d/", "/a/b/c/d/", path));
    EXPECT_STREQ(path.data(), "/a/b/c/d/");

    RK_EXPECT_OK(fs::path_common_prefix("C:/apple/back/cmd/dock",
                                        "C:/apple/back/cmd/dock/epslon/folder", path));
    EXPECT_STREQ(path.data(), "C:/apple/back/cmd/dock");
#endif
}

TEST_F(FilesystemTest, path_common_prefix_length)
{
    s32 len = 0;

#if SDS_OS_WINDOWS
    RK_EXPECT_OK(fs::path_common_prefix_length("\\a\\b\\c\\d", "\\a\\b\\c\\d", len));
    EXPECT_EQ(len, sds::str_size("\\a\\b\\c\\d"));
    RK_EXPECT_OK(fs::path_common_prefix_length("\\a\\b\\c\\d", "\\a\\b\\c\\d\\e\\f", len));
    EXPECT_EQ(len, sds::str_size("\\a\\b\\c\\d"));

    RK_EXPECT_OK(fs::path_common_prefix_length("\\a\\b\\c\\d\\", "\\a\\b\\c\\d\\e\\f", len));
    EXPECT_EQ(len, sds::str_size("\\a\\b\\c\\d\\"));
    RK_EXPECT_OK(fs::path_common_prefix_length("\\a\\b\\c\\d\\", "\\a\\b\\c\\d\\", len));
    EXPECT_EQ(len, sds::str_size("\\a\\b\\c\\d\\"));

    RK_EXPECT_OK(fs::path_common_prefix_length("C:\\apple\\back\\cmd\\dock",
                                               "C:\\apple\\back\\cmd\\dock\\epslon\\folder", len));
    EXPECT_EQ(len, sds::str_size("C:\\apple\\back\\cmd\\dock"));

#elif SDS_OS_LINUX
    RK_EXPECT_OK(fs::path_common_prefix_length("/a/b/c/d", "/a/b/c/d", len));
    EXPECT_EQ(len, sds::str_size("/a/b/c/d"));
    RK_EXPECT_OK(fs::path_common_prefix_length("/a/b/c/d", "/a/b/c/d/e/f", len));
    EXPECT_EQ(len, sds::str_size("/a/b/c/d"));

    RK_EXPECT_OK(fs::path_common_prefix_length("/a/b/c/d/", "/a/b/c/d/e/f", len));
    EXPECT_EQ(len, sds::str_size("/a/b/c/d"));
    RK_EXPECT_OK(fs::path_common_prefix_length("/a/b/c/d/", "/a/b/c/d/", len));
    EXPECT_EQ(len, sds::str_size("/a/b/c/d/"));

    RK_EXPECT_OK(fs::path_common_prefix_length("C:/apple/back/cmd/dock",
                                               "C:/apple/back/cmd/dock/epslon/folder", len));
    EXPECT_EQ(len, sds::str_size("C:/apple/back/cmd/dock"));
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

#if SDS_OS_WINDOWS
    EXPECT_NOT_DESCENDANT("\\a\\b\\c", "\\a\\b\\c");
    EXPECT_NOT_DESCENDANT("\\a\\b\\c", "");
    EXPECT_NOT_DESCENDANT("", "\\a\\b\\c");
    EXPECT_NOT_DESCENDANT("", "");
    EXPECT_NOT_DESCENDANT("\\a\\b\\c", "\\"); // slash as 'root' is only valid on linux

    EXPECT_IS_DESCENDANT("\\a\\b\\c", "\\a\\b\\");
    EXPECT_IS_DESCENDANT("\\a\\b\\c", "\\a\\b");
    EXPECT_IS_DESCENDANT("\\a\\b\\c", "\\a\\");
    EXPECT_IS_DESCENDANT("\\a\\b\\c", "\\a");

    EXPECT_NOT_DESCENDANT("\\a\\b\\", "\\a\\b\\c");
    EXPECT_NOT_DESCENDANT("\\a\\b", "\\a\\b\\c");
    EXPECT_NOT_DESCENDANT("\\a\\", "\\a\\b\\c");
    EXPECT_NOT_DESCENDANT("\\a", "\\a\\b\\c");
    EXPECT_NOT_DESCENDANT("\\", "\\a\\b\\c");

#elif SDS_OS_LINUX
    EXPECT_NOT_DESCENDANT("/a/b/c", "/a/b/c");
    EXPECT_NOT_DESCENDANT("/a/b/c", "");
    EXPECT_NOT_DESCENDANT("", "/a/b/c");
    EXPECT_NOT_DESCENDANT("", "");

    EXPECT_IS_DESCENDANT("/a/b/c", "/a/b/");
    EXPECT_IS_DESCENDANT("/a/b/c", "/a/b");
    EXPECT_IS_DESCENDANT("/a/b/c", "/a/");
    EXPECT_IS_DESCENDANT("/a/b/c", "/a");
    EXPECT_IS_DESCENDANT("/a/b/c", "/");

    EXPECT_NOT_DESCENDANT("/a/b/", "/a/b/c");
    EXPECT_NOT_DESCENDANT("/a/b", "/a/b/c");
    EXPECT_NOT_DESCENDANT("/a/", "/a/b/c");
    EXPECT_NOT_DESCENDANT("/a", "/a/b/c");
    EXPECT_NOT_DESCENDANT("/", "/a/b/c");
#endif

#undef EXPECT_NOT_DESCENDANT
#undef EXPECT_IS_DESCENDANT
}

TEST_F(FilesystemTest, path_component)
{
    char const* p = nullptr;
    std::optional<fs::Path_Component> component;

#define CHECK_COMPONENT(X)                                                                       \
    ASSERT_TRUE(component);                                                                      \
    EXPECT_TRUE(                                                                                 \
        unicode::ascii_cmp(component->name.data(), X, static_cast<s32>(component->name.size()))) \
        << "str1: " << component->name.data() << ", str2: " << (X);                              \
    component = component->next();

    // Empty
    p = nullptr;
    EXPECT_FALSE(fs::path_get_component(p));
    p = "";
    EXPECT_FALSE(fs::path_get_component(p));

    // All path separators
    p = "///////";
    EXPECT_FALSE(fs::path_get_component(p));

    p = "c:\\path1\\path2\\file.txt";
    component = fs::path_get_component(p);
    CHECK_COMPONENT("c:");
    CHECK_COMPONENT("path1");
    CHECK_COMPONENT("path2");
    CHECK_COMPONENT("file.txt");
    EXPECT_FALSE(component);

    p = "/path1/path2/file.txt";
    component = fs::path_get_component(p);
    CHECK_COMPONENT("path1");
    CHECK_COMPONENT("path2");
    CHECK_COMPONENT("file.txt");
    EXPECT_FALSE(component);

#undef CHECK_COMPONENT
}
