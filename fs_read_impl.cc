#include "fs_impl.h"

using namespace std;

/* Implementation of functions in this file does not mutate nodes during traversal */

// Change the current working directory to a child directory or the parent.
// If the working directory is already at root, changing directory to parent is a no op.
// Return Error if directory doesn't exist or given input is a file.
// TODO(mianl): Implement with absolute vs relative path extension
void FileSystem::cd(string path) {
    if (path == "../") {
        if (currDir->parent) currDir = currDir->parent;
        return;
    }
    File* traverse = currDir;
    if (traverse->children.find(path) == traverse->children.end()) {
        throw invalid_argument("Directory not found: " + path);
    } else {
        if (traverse->children[path]->isDir) currDir = traverse->children[path];
        else  {
            throw invalid_argument("Not a directory: " + path);
        }
    }
}

// Get the current working directory. Returns the current working directory's path from the root.
string FileSystem::pwd() {
    return currDir->name;
}

// Get the directory contents: Returns the children of the current working directory.
// Extension:
// List directory contents with both relative paths and absolute paths:
// 1. if path param starts with "/", traversal starts from root
// 2. if path param doesn't start with "/", traversal starts from the working directory
// 3. if path points to a file, list the filename
// 4. O(n+m) for n subdirs and m files
vector<string> FileSystem::ls(string path) {
    vector<string> files;
    File* traverse = currDir;
    int i = 0;
    if (path != ".") {
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
                throw invalid_argument("No such file or directory: " + path);
            }
            traverse = traverse->children[subdir];
        }

        if (!traverse->isDir) {
            files.push_back(subdirs.back());
            return files;
        }
    }
    // c++ map is a treemap: keys should be sorted and the returned file list will be in alphabetic order.
    for (auto iter = traverse->children.begin(); iter != traverse->children.end(); iter++) {
        files.push_back(iter->first);
    }
    return files;
}

// Find a file/directory: Given a filename, find all the files and directories within the current
// working directory that have exactly that name.
// Implemented with BFS and return a list of absolute paths in sorted order (empty if nothing is found).
vector<string> FileSystem::find(string filename) {
    vector<string> files;
    queue<File*> q;
    q.push(currDir);
    while (!q.empty()) {
        File* traverse = q.front();
        q.pop();
        if (traverse->children.find(filename) != traverse->children.end()) {
            files.push_back(traverse->children[filename]->name);
        }
        for (auto iter = traverse->children.begin(); iter != traverse->children.end(); iter++) {
            q.push(iter->second);
        }
    }
    return files;
}

// Get file contents: Returns the content of a file in the current working directory.
// Extension:
// 1. if path param starts with "/", traversal starts from root
// 2. if path param doesn't start with "/", traversal starts from the working directory
// 3. O(n) for n subdirs
string FileSystem::cat(string path) {
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

    if (!traverse->isDir) return traverse->content;
    throw invalid_argument("Not a file: " + path);
}


