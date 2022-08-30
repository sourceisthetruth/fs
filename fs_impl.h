#ifndef FS_IMPL_H
#define FS_IMPL_H

#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

/* Implementation of an in memory linux style file system
*/
class FileSystem {
    struct File {
        map<string, File*> children;
        File* parent;
        bool isDir;
        // The absolute path from root
        string name;
        // If the node is a file, this field holds the file content.
        string content;
    };

    File* root;
    File* currDir;
  public:
    FileSystem() {
        root = new File();
        root->isDir = true;
        // The working directory begins at '/'.
        root->name = "/";
        currDir = root;
    }
    // Read functions: implementation of those functions does not mutate nodes
    void cd(string path);
    string pwd();
    vector<string> ls(string path);
    vector<string> find(string filename);
    string cat(string path);

    // Write functions: implementation of those functions mutates nodes
    void mkdir(string path);
    void rm(string path);
    void touch(string path);
    void write(string path, string content);
    void mv(string from, string to);

    // Util functions
    vector<string> split(string s, char delim);
};
#endif
