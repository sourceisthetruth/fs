#include "fs_impl.h"

#include "gtest/gtest.h"

/* Test fs_impl classes */
namespace {

// Tests creating directory with absolute paths
// The read functions cd(string) and pwd() are tested here too
TEST(FileSystem, TestMkdirAbsolutePath) {
    FileSystem fs;
    fs.mkdir("/a/b/c");

    vector<string> dirs = fs.ls(".");
    EXPECT_EQ(1, dirs.size());
    EXPECT_EQ("a", dirs[0]);
    EXPECT_EQ("/", fs.pwd());

    fs.cd("a");
    dirs = fs.ls(".");
    EXPECT_EQ(1, dirs.size());
    EXPECT_EQ("b", dirs[0]);
    EXPECT_EQ("/a/", fs.pwd());

    fs.cd("b");
    dirs = fs.ls(".");
    EXPECT_EQ(1, dirs.size());
    EXPECT_EQ("c", dirs[0]);
    EXPECT_EQ("/a/b/", fs.pwd());
}

// Tests creating directory from the working directory, including parent ".."
// The read functions cd(string) and pwd are tested here too
TEST(FileSystem, TestMkdirRelativePath) {
    FileSystem fs;
    fs.mkdir("school");
    fs.cd("school");
    EXPECT_EQ("/school/", fs.pwd());

    fs.mkdir("homework");
    fs.cd("homework");
    EXPECT_EQ("/school/homework/", fs.pwd());

    fs.mkdir("../../lunch");
    fs.mkdir("math");
    fs.mkdir("spanish");
    fs.mkdir("history");
    vector<string> dirs = fs.ls(".");
    EXPECT_EQ(3, dirs.size());
    EXPECT_EQ("history", dirs[0]);
    EXPECT_EQ("math", dirs[1]);
    EXPECT_EQ("spanish", dirs[2]);

    fs.cd("../");
    fs.mkdir("cheatsheet");
    dirs = fs.ls(".");
    EXPECT_EQ(2, dirs.size());
    EXPECT_EQ("cheatsheet", dirs[0]);
    EXPECT_EQ("homework", dirs[1]);

    fs.cd("../");
    dirs = fs.ls(".");
    EXPECT_EQ(2, dirs.size());
    EXPECT_EQ("lunch", dirs[0]);
    EXPECT_EQ("school", dirs[1]);
    EXPECT_EQ("/", fs.pwd());
}

// Tests mkdir should fail with invalid path
// rm and cd should fail because dir doesn't exist
TEST(FileSystem, TestMkdirInvalidPath) {
    FileSystem fs;

    try {
        fs.mkdir("../a");
        FAIL() << "Expected exception because already reached root";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("Invalid path: ../a"));
    }

    try {
        fs.rm("a");
        FAIL() << "Expected exception because dir doesn't exist";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("No such file or directory: a"));
    }

    try {
        fs.cd("a");
        FAIL() << "Expected exception because dir doesn't exist";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("Directory not found: a"));
    }
    fs.cd("../");
    fs.touch("file");
    try {
        fs.cd("file");
        FAIL() << "Expected exception because it's not a directory";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("Not a directory: file"));
    }
}

// Tests touch, write (with absolute paths), and remove
TEST(FileSystem, TestTouchWriteRm) {
    FileSystem fs;
    fs.mkdir("/a/b/c");
    fs.cd("a");
    fs.touch("newfile");

    vector<string> dirs = fs.ls(".");
    EXPECT_EQ(2, dirs.size());
    EXPECT_EQ("b", dirs[0]);
    EXPECT_EQ("newfile", dirs[1]);
    EXPECT_EQ("", fs.cat("newfile"));

    fs.write("/a/newfile", "Newcontent");
    EXPECT_EQ("Newcontent", fs.cat("newfile"));

    fs.write("newfile", "Morecontent");
    EXPECT_EQ("NewcontentMorecontent", fs.cat("newfile"));

    fs.rm("b");
    dirs = fs.ls(".");
    EXPECT_EQ(1, dirs.size());
    EXPECT_EQ("newfile", dirs[0]);
    EXPECT_EQ("/a/", fs.pwd());
}

// Tests touch, write (with relative paths), and move
TEST(FileSystem, TestTouchWriteMv) {
    FileSystem fs;
    fs.mkdir("/a/b/c");
    fs.cd("a");
    fs.touch("newfile");

    fs.cd("b");
    fs.write("../newfile",  "Newcontent");
    fs.cd("c");
    fs.write("../../newfile",  "Morecontent");

    fs.cd("../");
    vector<string> dirs = fs.ls(".");
    EXPECT_EQ(1, dirs.size());
    EXPECT_EQ("c", dirs[0]);
    EXPECT_EQ("/a/b/", fs.pwd());

    fs.cd("../");
    EXPECT_EQ("NewcontentMorecontent", fs.cat("newfile"));

    fs.mv("newfile", "v2");
    EXPECT_EQ("NewcontentMorecontent", fs.cat("v2"));
    dirs = fs.ls(".");
    EXPECT_EQ(2, dirs.size());
    EXPECT_EQ("b", dirs[0]);
    EXPECT_EQ("v2", dirs[1]);
    EXPECT_EQ("/a/", fs.pwd());
}

// Tests invalid scenarios should fail write
// mv should fail too if file doesn't exist
TEST(FileSystem, TestWriteInvalid) {
    FileSystem fs;
    fs.mkdir("/a/b/c");
    fs.cd("a");

    // Verify write failure
    try {
        fs.write("../../newfile", "");
        FAIL() << "Expected exception because already reached root";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("Invalid path: ../../newfile"));
    }
    try {
        fs.write("/newfile", "");
        FAIL() << "Expected exception because file doesn't exist";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("File not found: /newfile"));
    }

    // Verify mv failure
    try {
        fs.mv("b", "newfile");
        FAIL() << "Expected exception because it's not a file";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("Not a file: b"));
    }
    try {
        fs.mv("newfile", "v2");
        FAIL() << "Expected exception because file doesn't exist";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("File not found: newfile"));
    }
}

// Tests mkdir and touch should fail if file/directory already exists.
TEST(FileSystem, TestCreateInvalid) {
    FileSystem fs;
    fs.mkdir("/a/b/c");
    try {
        fs.mkdir("a");
        FAIL() << "Expected exception because directory exists";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("File/Directory exists: a"));
    }

    try {
        fs.touch("a");
        FAIL() << "Expected exception because directory exists";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("File/Directory exists: a"));
    }

    fs.cd("a");
    fs.touch("dup");
    try {
        fs.mkdir("/a/dup");
        FAIL() << "Expected exception because file exists";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("Invalid path: /a/dup"));
    }
}

// Tests ls with absolute paths and relative paths. Children include both dirs and files.
TEST(FileSystem, TestLsValid) {
    FileSystem fs;
    fs.mkdir("/a/b/c");
    fs.cd("a");
    fs.touch("file");

    // ls relative paths
    vector<string> dirs = fs.ls("b");
    EXPECT_EQ(1, dirs.size());
    EXPECT_EQ("c", dirs[0]);

    dirs = fs.ls("b/c");
    EXPECT_EQ(0, dirs.size());

    dirs = fs.ls("file");
    EXPECT_EQ(1, dirs.size());
    EXPECT_EQ("file", dirs[0]);

    fs.cd("../");
    fs.mkdir("d/e/f");

    // ls absolute paths
    dirs = fs.ls("/");
    EXPECT_EQ(2, dirs.size());
    EXPECT_EQ("a", dirs[0]);
    EXPECT_EQ("d", dirs[1]);

    dirs = fs.ls("/a");
    EXPECT_EQ(2, dirs.size());
    EXPECT_EQ("b", dirs[0]);
    EXPECT_EQ("file", dirs[1]);

    dirs = fs.ls("/d/e");
    EXPECT_EQ(1, dirs.size());
    EXPECT_EQ("f", dirs[0]);
}

// Invalid scenarios should fail ls
// find should return empty if not found
TEST(FileSystem, TestLsInvalidPath) {
    FileSystem fs;
    fs.mkdir("/a/b/c");
    fs.cd("a");
    fs.touch("target");

    vector<string> found = fs.find("b");
    EXPECT_EQ(1, found.size());
    EXPECT_EQ("/a/b/", found[0]);
    found = fs.find("c");
    EXPECT_EQ(1, found.size());
    EXPECT_EQ("/a/b/c/", found[0]);

    fs.rm("b");
    found = fs.find("b");
    EXPECT_TRUE(found.empty());
    found = fs.find("c");
    EXPECT_TRUE(found.empty());

    try {
        fs.ls("b");
        FAIL() << "Expected exception because dir doesn't exist";
    }
    catch (invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("No such file or directory: b"));
    }
    try {
        fs.ls("b/c");
        FAIL() << "Expected exception because dir doesn't exist";
    }
    catch (invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("No such file or directory: b/c"));
    }

    try {
        fs.ls("/a/b/");
        FAIL() << "Expected exception because dir doesn't exist";
    }
    catch (invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("No such file or directory: /a/b/"));
    }
    try {
        fs.ls("/a/b/c");
        FAIL() << "Expected exception because dir doesn't exist";
    }
    catch (invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("No such file or directory: /a/b/c"));
    }
}

// Tests cat with absolute paths and relative paths
// The read function find(string) is tested here too
TEST(FileSystem, TestCatValid) {
    FileSystem fs;
    fs.mkdir("/a/b/c");
    fs.touch("target");
    fs.cd("a");
    fs.touch("target");

    fs.cd("../");
    fs.mkdir("d/e/target");
    fs.cd("d");
    fs.touch("target");

    vector<string> found = fs.find("target");
    EXPECT_EQ(2, found.size());
    EXPECT_EQ("/d/target", found[0]);
    EXPECT_EQ("/d/e/target/", found[1]);

    fs.cd("../");
    found = fs.find("target");
    EXPECT_EQ(4, found.size());
    EXPECT_EQ("/target", found[0]);
    EXPECT_EQ("/a/target", found[1]);
    EXPECT_EQ("/d/target", found[2]);
    EXPECT_EQ("/d/e/target/", found[3]);

    fs.cd("d");
    fs.write("/target", "content1");
    fs.write("/a/target", "content2");
    fs.write("/d/target", "content3");

    EXPECT_EQ("content1", fs.cat("/target"));
    EXPECT_EQ("content2", fs.cat("/a/target"));
    EXPECT_EQ("content3", fs.cat("/d/target"));
}

TEST(FileSystem, TestCatInvalid) {
    FileSystem fs;
    fs.mkdir("/a/b/c");
    fs.mkdir("mydir");
    fs.cd("a");

    try {
        fs.cat("../../newfile");
        FAIL() << "Expected exception because already reached root";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("Invalid path: ../../newfile"));
    }
    try {
        fs.cat("/newfile");
        FAIL() << "Expected exception because file doesn't exist";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("File not found: /newfile"));
    }
    try {
        fs.cat("../mydir");
        FAIL() << "Expected exception because it's a directory";
    }
    catch(invalid_argument const & err) {
        EXPECT_EQ(err.what(), string("Not a file: ../mydir"));
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
}  // namespace