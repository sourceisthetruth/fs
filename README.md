# In-Memory File System

The c++ implementation of an In-Memory FS for a single user.
The FS is represented with a trie data structure, where each directory or file is a node.
The root directory is the root node of the trie, and all other nodes can have one parent, and 0 to multiple children.
The absolute path from root is stored in each node, while the relative file names from the parent directory are used
as the keys of the children map in each parent node. 

### Open Source Libraries

- We use googletest http://google.github.io/googletest/primer.html
  for our C++ unit tests.
  Download here
    ```
    https://github.com/google/googletest/releases/tag/release-1.12.1
    ```
- Install cmake https://google.github.io/googletest/quickstart-cmake.html
    ```
  brew install cmake #
    ```
    For non-mac systems, use other installer such as "apt-get install".

## Unit Test

- Add unit test target to CMakeLists.txt and link test executable with gtest_main

    ```
  add_executable(gUnitTests ../fs_impl_test.cc)
  add_library(fs_impl SHARED ../fs_impl.h ../fs_read_impl.cc ../fs_write_impl.cc)
  
  target_link_libraries(gUnitTests fs_impl gtest gtest_main)
  add_test(gUnitTests gUnitTests)
  ```
- Run unit test with the googletest framework
  ```
  cd gtest
  sudo cmake CMakeLists.txt
  sudo make
  ./gUnitTests
  ```

## Run Interactive Prompt via CLI
In the top dir
```
g++ -std=c++11 -o out fs_read_impl.cc fs_write_impl.cc fs_service.cc && ./out
```

## FS Commands

cd [directory_name] 
- Change the current working directory. The working directory begins at '/'. You may
  traverse to a child directory or the parent.
- If the working directory is already at root, changing directory to parent is no op.
- Return error if directory doesn't exist or given input is a file.

pwd
- Get the current working directory. Returns the current working directory's path from
  the root to console.
- Append "/" for directory. For example, "/school/homework/" means homework is a dir,
whereas "/school/homework" means homework is a file.

mkdir [directory_name]
- Create a new directory. Return error if a file or directory with the same name exists under the parent.
- Implemented with Extension:
  - Create a new directory with both relative paths and absolute paths:
    - If path param starts with "/", traversal starts from root
    - Otherwise, traversal starts from the working directory
    - Automatically create any intermediate directories on the path that don’t exist yet.

ls [directory_name]
- Get the directory contents: Returns the children of the current working directory.
- Return error for invalid or non-existing paths.
- If directory name is not specified, list all contents of the working directory.
- Implemented with Extension:
  - List directory contents with both relative paths and absolute paths:
    - If path param starts with "/", traversal starts from root
    - Otherwise, traversal starts from the working directory

rm [file/dir_name]
- Remove a directory/file. The target must be among the current working directory’s
  children.
- If the target directory is a parent, all subdirs of the target directory will be removed too.
- Return error for non-existing directories/files.

touch [file_name]
- Create a new file: Creates a new empty file in the current working directory.
- Return error if a file or a directory with the same name already exists.

write [file_name] [file_content]
- Write file contents: Writes the specified contents to a file in the current working
  directory. Files have to be created previously.
- Return error if file doesn't exist or path points to a directory.
- Implemented with Extension:
  - If path param starts with "/", traversal starts from root
  - Otherwise, traversal starts from the current directory

cat [file_name]
- Get file contents: Returns the content of a file in the current working directory.
- Return error if file doesn't exist or path points to a directory.
- Implemented with Extension:
  - If path param starts with "/", traversal starts from root
  - Otherwise, traversal starts from the working directory

mv [source_file_name] [dest_file_name]
- Move a file: Move an existing file in the current working directory to a new location (in
  the same directory).
- Override the dest file if it already exists.
- Return error if file doesn't exist or path points to a directory.
- No op if source path is the same as destination. 

find [file/dir_name]
- Find a file/directory: Given a filename, find all the files and directories within the current
    working directory that have exactly that name.
- Return the list in sorted order (empty if nothing is found).
 
Return "command not found" to not supported commands.
