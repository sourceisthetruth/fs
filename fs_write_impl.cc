#include "fs_impl.h"

using namespace std;

/* Implementation of functions in this file adds/deletes nodes of the trie data structure */

// Create a new directory. The current working directory is the parent.
// Return Error if a file or directory with the same name exists under the parent.
// Extension:
// Create a new directory with both relative paths and absolute paths:
// 1. if path param starts with "/", traversal starts from root
// 2. if path param doesn't start with "/", traversal starts from the working directory
// 3. automatically create any intermediate directories on the path that don’t exist yet.
// 4. trie traversal has a runtime complexity of O(n) for n subdirs
void FileSystem::mkdir(string path) {
    bool created = false;
    File* traverse = currDir;
    vector<string> subdirs = split(path, '/');
    int i = 0;
    if (subdirs[0] == "") {
        traverse = root;
        i = 1;
    }
    for (; i < subdirs.size(); i++) {
        string subdir = subdirs[i];
        if (subdir == "..") {
            if (traverse->parent) {
                traverse = traverse->parent;
                continue;
            } else {
                throw invalid_argument("Invalid path: " + path);
            }
        }
        if (traverse->children.find(subdir) != traverse->children.end()) {
            traverse = traverse->children[subdir];
            continue;
        }

        File* newDir = new File();
        newDir->isDir = true;
        newDir->name = traverse->name + subdir + "/";
        traverse->children[subdir] = newDir;
        newDir->parent = traverse;
        traverse = newDir;
        created = true;
    }
    if (!created) throw invalid_argument("File/Directory exists: " + path);
}

// Remove a directory or a file. The target must be among the current working directory’s children.
// If the target directory is a parent, all subdirs of the target directory will be removed too.
// TODO(mianl): Implement with absolute vs relative path extension
void FileSystem::rm(string path) {
    File* traverse = currDir;

    if (traverse->children.find(path) == traverse->children.end()) {
        throw invalid_argument("No such file or directory: " + path);
    }
    traverse->children.erase(path);
}

// Create a new file: Creates a new empty file in the current working directory.
// Return Error if a file or directory with the same name already exists.
// TODO(mianl): Implement with absolute vs relative path extension
void FileSystem::touch(string path) {
    if (currDir->children.find(path) != currDir->children.end())
        throw invalid_argument("File/Directory exists: " + path);
    File* newFile = new File();
    newFile->isDir = false;
    newFile->name = currDir->name + path;
    newFile->parent = currDir;
    newFile->content = "";
    currDir->children[path] = newFile;
}

// Write file contents: Appends the specified content to a file in the current working
// directory. Return Error if the file doesn't already exist.
// Extension:
// 1. if path param starts with "/", traversal starts from root
// 2. if path param doesn't start with "/", traversal starts from the working directory
// 3. trie traversal has a runtime complexity of O(n) for n subdirs
void FileSystem::write(string path, string content) {
    File* traverse = currDir;
    int i = 0;
    vector<string> subdirs = split(path, '/');
    if (subdirs[0] == "") {
        traverse = root;
        i = 1;
    }
    for (; i < subdirs.size(); i++) {
        string subdir = subdirs[i];
        if (subdir == "..") {
            if (traverse->parent) {
                traverse = traverse->parent;
                continue;
            } else {
                throw invalid_argument("Invalid path: " + path);
            }
        }

        if (traverse->children.find(subdir) == traverse->children.end()) {
            throw invalid_argument("File not found: " + path);
        }
        traverse = traverse->children[subdir];
    }
    if (!traverse->isDir) {
        traverse->content += content;
    } else {
        throw invalid_argument("Not a file: " + path);
    }
}

// Move a file: Move an existing file in the current working directory to a new location in
// the same directory. Override the dest file if it already exists.
// TODO(mianl): Implement with absolute vs relative path extension
void FileSystem::mv(string from, string to) {
    if (currDir->children.find(from) == currDir->children.end()) throw invalid_argument("File not found: " + from);
    if (from == to) return;

    File* move = currDir->children[from];
    if (move->isDir) throw invalid_argument("Not a file: " + from);
    move->name = currDir->name + to;
    currDir->children.erase(from);
    currDir->children[to] = move;
}


/************************ util functions **************************
TODO(mianl): move util functions to a separate file as this section grows larger
*/

// Helper function to split a string by a delimiter
vector<string> FileSystem::split(string s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> res;
    while (getline(ss, item, delim)) {
        res.push_back(item);
    }
    return res;
}
