#include "iconventional.h"

/*
https://www.npmjs.com/package/conventional-changelog-writer

*/

using namespace StandardRelease;

IConventionalCommit::IConventionalCommit()
    : m_valid(false)
    , m_error()
    , m_commits()
    , m_semver()
{
}

IConventionalCommit::~IConventionalCommit() {}

void IConventionalCommit::setVersion(const SemVer &semver) {
    m_semver = semver;
}

SemVer IConventionalCommit::version() const {
    return m_semver;
}

void IConventionalCommit::setValid(bool valid) {
    m_valid = valid;
}

void IConventionalCommit::setError(const Error error)
{
    m_error = error;
}

void IConventionalCommit::setCommits(const IConventionalCommit::Commits commits) {
    m_commits = commits;
}

IConventionalCommit::Commits IConventionalCommit::commits() const {
    return m_commits;
}

bool IConventionalCommit::isValid() const {
    return m_valid;
}

Error IConventionalCommit::error() const {
    return m_error;
}

