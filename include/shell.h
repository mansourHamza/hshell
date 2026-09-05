#pragma once

#include <string>
#include <vector>

std::string trim(const std::string &s);
std::vector<std::string> split(const std::string &s);

std::string get_display_dir();
void print_prompt();

int run_external(std::vector<std::string> args);
int run_with_redirect(std::vector<std::string> args, std::string filename);
int run_pipe(std::vector<std::string> left, std::vector<std::string> right);
