#include "shell.h"

#include <fcntl.h>
#include <limits.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace std;

string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t");
    if (start == string::npos) {
        return "";
    }
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

vector<string> split(const string &s) {
    vector<string> parts;
    stringstream ss(s);
    string word;
    while (ss >> word) {
        parts.push_back(word);
    }
    return parts;
}

string get_display_dir() {
    char buf[4096];
    if (getcwd(buf, sizeof(buf)) == nullptr) {
        return "?";
    }
    string dir = buf;
    const char *home = getenv("HOME");
    if (home != nullptr) {
        string h = home;
        if (dir == h) {
            return "~";
        }
        if (dir.size() > h.size() && dir.substr(0, h.size()) == h &&
            dir[h.size()] == '/') {
            return "~" + dir.substr(h.size());
        }
    }
    return dir;
}

void print_prompt() {
    cout << "hshell " << get_display_dir() << " > " << flush;
}

// execvp wants char*, not string, so build that array here
static vector<char *> make_argv(vector<string> &args) {
    vector<char *> argv;
    for (size_t i = 0; i < args.size(); i++) {
        argv.push_back(const_cast<char *>(args[i].c_str()));
    }
    argv.push_back(nullptr);
    return argv;
}

int run_external(vector<string> args) {
    if (args.empty()) {
        return 0;
    }

    vector<char *> argv = make_argv(args);

    pid_t pid = fork();
    if (pid < 0) {
        perror("hshell: fork failed");
        return 1;
    }

    if (pid == 0) {
        execvp(argv[0], argv.data());
        // if we get here exec failed
        perror("hshell");
        exit(1);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    return 0;
}

int run_with_redirect(vector<string> args, string filename) {
    if (args.empty() || filename.empty()) {
        return 1;
    }

    vector<char *> argv = make_argv(args);

    pid_t pid = fork();
    if (pid < 0) {
        perror("hshell: fork failed");
        return 1;
    }

    if (pid == 0) {
        // point stdout at the file
        int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("hshell: open failed");
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);

        execvp(argv[0], argv.data());
        perror("hshell");
        exit(1);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    return 0;
}

int run_pipe(vector<string> left, vector<string> right) {
    if (left.empty() || right.empty()) {
        return 1;
    }

    int fds[2];
    if (pipe(fds) < 0) {
        perror("hshell: pipe failed");
        return 1;
    }

    vector<char *> left_argv = make_argv(left);
    vector<char *> right_argv = make_argv(right);

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("hshell: fork failed");
        close(fds[0]);
        close(fds[1]);
        return 1;
    }

    if (pid1 == 0) {
        // left side writes into the pipe
        dup2(fds[1], STDOUT_FILENO);
        close(fds[0]);
        close(fds[1]);

        execvp(left_argv[0], left_argv.data());
        perror("hshell");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("hshell: fork failed");
        close(fds[0]);
        close(fds[1]);
        return 1;
    }

    if (pid2 == 0) {
        // right side reads from the pipe
        dup2(fds[0], STDIN_FILENO);
        close(fds[0]);
        close(fds[1]);

        execvp(right_argv[0], right_argv.data());
        perror("hshell");
        exit(1);
    }

    // parent doesn't need the pipe
    close(fds[0]);
    close(fds[1]);

    int status = 0;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    return 0;
}
