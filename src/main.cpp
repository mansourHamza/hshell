#include "shell.h"

#include <unistd.h>

#include <cstdlib>
#include <iostream>

using namespace std;

int main() {
    vector<string> history;

    while (true) {
        print_prompt();

        string line;
        if (!getline(cin, line)) {
            // ctrl-d makes getline fail, just quit
            cout << endl;
            break;
        }

        line = trim(line);
        if (line.empty()) {
            continue;
        }

        history.push_back(line);

        size_t pipe_pos = line.find('|');
        if (pipe_pos != string::npos) {
            if (line.find('|', pipe_pos + 1) != string::npos) {
                cerr << "hshell: only one pipe supported" << endl;
                continue;
            }
            if (line.find('>') != string::npos) {
                cerr << "hshell: pipe and redirection cannot be combined" << endl;
                continue;
            }

            string left_str = trim(line.substr(0, pipe_pos));
            string right_str = trim(line.substr(pipe_pos + 1));
            if (left_str.empty() || right_str.empty()) {
                cerr << "hshell: malformed pipe, expected: cmd1 | cmd2" << endl;
                continue;
            }

            vector<string> left = split(left_str);
            vector<string> right = split(right_str);
            run_pipe(left, right);
            continue;
        }

        size_t redir_pos = line.find('>');
        if (redir_pos != string::npos) {
            if (line.find('>', redir_pos + 1) != string::npos) {
                cerr << "hshell: only one redirection supported" << endl;
                continue;
            }

            string left_str = trim(line.substr(0, redir_pos));
            string file_str = trim(line.substr(redir_pos + 1));
            if (left_str.empty() || file_str.empty()) {
                cerr << "hshell: malformed redirection, expected: cmd > file" << endl;
                continue;
            }

            vector<string> left = split(left_str);
            vector<string> file_parts = split(file_str);
            if (file_parts.size() != 1) {
                cerr << "hshell: redirection needs a single filename" << endl;
                continue;
            }

            run_with_redirect(left, file_parts[0]);
            continue;
        }

        vector<string> args = split(line);
        if (args.empty()) {
            continue;
        }

        string cmd = args[0];

        if (cmd == "exit") {
            break;
        }

        if (cmd == "cd") {
            // cd must run in the shell process itself.
            // If it runs in a child process, the directory change disappears
            // when that child exits.
            if (args.size() == 1) {
                const char *home = getenv("HOME");
                if (home == nullptr) {
                    cerr << "hshell: cd: HOME not set" << endl;
                } else if (chdir(home) != 0) {
                    perror("hshell: cd failed");
                }
            } else if (args.size() == 2) {
                if (chdir(args[1].c_str()) != 0) {
                    perror("hshell: cd failed");
                }
            } else {
                cerr << "hshell: cd: too many arguments" << endl;
            }
            continue;
        }

        if (cmd == "pwd") {
            char buf[4096];
            if (getcwd(buf, sizeof(buf)) != nullptr) {
                cout << buf << endl;
            } else {
                perror("hshell: pwd failed");
            }
            continue;
        }

        if (cmd == "history") {
            for (size_t i = 0; i < history.size(); i++) {
                cout << (i + 1) << "  " << history[i] << endl;
            }
            continue;
        }

        run_external(args);
    }

    return 0;
}
