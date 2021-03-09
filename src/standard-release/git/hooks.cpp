#include "hooks.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>

const auto commitMsg = R"(#!/bin/bash

myexe=$(which standard-release 2> /dev/null)
if [ -z "$myexe" ]
then
    echo "standard-release not found in PATH." > /dev/stderr
    exit 1
fi
)";

using namespace StandardRelease;

GitHooks::GitHooks()
    : m_error()
{
}

GitHooks::Result GitHooks::install(const std::string &dirname)
{
    return NotAGitRepo;
}
