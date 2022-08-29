#include "fs_impl.h"

using namespace std;

/* User prompt for using the in memory file system
   TODO(mianl): Make fs impl thread-safe, and spawn a new thread for each user prompt.
*/
int main() {
    FileSystem fs;

    while(1) {
        string input;
        while(getline(std::cin, input)) {
            vector<string> commands = fs.split(input, ' ');
            if (commands.size() == 0) continue;
            string command = commands[0];
            string param = ".";

            if (command == "mkdir") {
                if (commands.size() != 2) {
                    cout << "SYNOPSIS: mkdir [directory_name]" << endl;
                    continue;
                }
                param = commands[1];
                try {
                    fs.mkdir(param);
                } catch (const std::invalid_argument& e) {
                    cout << e.what() << endl;
                }
            }
            else if (command == "rm") {
                if (commands.size() != 2) {
                    cout << "SYNOPSIS: rm [file/dir_name]" << endl;
                    continue;
                }
                param = commands[1];
                try {
                    fs.rm(param);
                } catch (const std::invalid_argument& e) {
                    cout << e.what() << endl;
                }
            }
            else if (command == "write") {
                if (commands.size() != 3) {
                    cout << "SYNOPSIS: write [file_name] [file_content]" << endl;
                    continue;
                }
                try {
                    fs.write(commands[1], commands[2]);
                } catch (const std::invalid_argument& e) {
                    cout << e.what() << endl;
                }
            }
            else if (command == "mv") {
                if (commands.size() != 3) {
                    cout << "SYNOPSIS: mv [source_file_name] [dest_file_name]" << endl;
                    continue;
                }
                try {
                    fs.mv(commands[1], commands[2]);
                } catch (const std::invalid_argument& e) {
                    cout << e.what() << endl;
                }
            }
            else if (command == "touch") {
                if (commands.size() != 2) {
                    cout << "SYNOPSIS: touch [file_name]" << endl;
                    continue;
                }
                param = commands[1];
                try {
                    fs.touch(param);
                } catch (const std::invalid_argument& e) {
                    cout << e.what() << endl;
                }
            }
            else if (command == "ls") {
                if (commands.size() > 2) {
                    cout << "SYNOPSIS: " << endl;
                    cout << "   ls: list current working directory contents " << endl;
                    cout << "   ls [directory_name]: list contents for directory " << endl;
                    continue;
                }
                if (commands.size() == 2) param = commands[1];
                try {
                    for(string file : fs.ls(param)) {
                        cout << file << endl;
                    }
                } catch (const std::invalid_argument& e) {
                    cout << e.what() << endl;
                }
            }
            else if (command == "cd") {
                if (commands.size() != 2) {
                    cout << "SYNOPSIS: cd [directory_name]" << endl;
                    continue;
                }
                param = commands[1];
                try {
                    fs.cd(param);
                } catch (const std::invalid_argument& e) {
                    cout << e.what() << endl;
                }
            }
            else if (command == "pwd") {
                if (commands.size() != 1) {
                    cout << "SYNOPSIS: pwd – return working directory name" << endl;
                    continue;
                }
                try {
                    cout << fs.pwd() << endl;
                } catch (const std::invalid_argument& e) {
                    cout << e.what() << endl;
                }
            }
            else if (command == "find") {
                if (commands.size() != 2) {
                    cout << "SYNOPSIS: find [file/dir_name]" << endl;
                    continue;
                }
                param = commands[1];
                try {
                    for(string file : fs.find(param)) {
                        cout << file << endl;
                    }
                } catch (const std::invalid_argument& e) {
                    cout << e.what() << endl;
                }
            }
            else if (command == "cat") {
                if (commands.size() != 2) {
                    cout << "SYNOPSIS: cat [file_name]" << endl;
                    continue;
                }
                param = commands[1];
                try {
                    cout << fs.cat(param) << endl;
                } catch (const std::invalid_argument& e) {
                    cout << e.what() << endl;
                }
            } else {
                cout << "command not found: " << command << endl;
            }
        }
    }

    return 0;
}
