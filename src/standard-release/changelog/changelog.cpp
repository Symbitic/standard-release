/*
 * TODO:
 *     + The 'hash' in a commit may be empty.
 */
#include "changelog.h"
#include "cmark.h"
#include <chrono>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <regex>

struct StandardRelease::ChangelogPrivate
{
    cmark_node *root;
    cmark_node *sibling;

    ChangelogPrivate()
    {
        root = cmark_node_new(CMARK_NODE_DOCUMENT);
    }

    ~ChangelogPrivate()
    {
        if (root != nullptr) {
            cmark_node_free(root);
        }
    }
};

// Find the node to insert new content after (`cmark_node_insert_after`).
static cmark_node *findInsertNode(cmark_node *root)
{
    cmark_node *node = cmark_node_first_child(root);
    cmark_node *sibling = nullptr;

    while (true) {
        node = cmark_node_next(node);
        const std::string ntype = cmark_node_get_type_string(node);
        if (ntype == "heading" || ntype == "NONE") {
            break;
        } else {
            sibling = node;
        }
    }

    return sibling;
}

static bool childrenAdded(cmark_node *node)
{
    cmark_node *first = cmark_node_first_child(node);
    cmark_node *last = cmark_node_last_child(node);
    if (first == last) {
        return false;
    }
    return true;
}

static bool hasItems(cmark_node *node)
{
    cmark_node *item = cmark_node_first_child(node);
    return cmark_node_get_type(item) == CMARK_NODE_NONE ? false : true;
}

static cmark_node *addSection(cmark_node *root, cmark_node *list, const char *name, int level)
{
    cmark_node *header = cmark_node_new(CMARK_NODE_HEADING);
    cmark_node_set_heading_level(header, level);

    cmark_node *txt = cmark_node_new(CMARK_NODE_TEXT);
    cmark_node_set_literal(txt, name);
    cmark_node_append_child(header, txt);

    cmark_node_insert_after(root, header);
    cmark_node_insert_after(header, list);

    return list;
}

static void addItem(cmark_node *list, const std::string scope, const std::string subject,
                    const std::string hash, const std::string url)
{
    cmark_node *item = cmark_node_new(CMARK_NODE_ITEM);
    cmark_node *p = cmark_node_new(CMARK_NODE_PARAGRAPH);

    if (!scope.empty()) {
        cmark_node *strong = cmark_node_new(CMARK_NODE_STRONG);
        cmark_node *strong_text = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node *space = cmark_node_new(CMARK_NODE_TEXT);

        const std::string str = scope + ":";
        cmark_node_set_literal(strong_text, str.c_str());

        cmark_node_set_literal(space, " ");

        cmark_node_append_child(strong, strong_text);
        cmark_node_append_child(p, strong);
        cmark_node_append_child(p, space);
    }

    cmark_node *subject_txt = cmark_node_new(CMARK_NODE_TEXT);
    cmark_node_set_literal(subject_txt, subject.c_str());
    cmark_node_append_child(p, subject_txt);

    if (!hash.empty()) {
        cmark_node *begin = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node_set_literal(begin, " (");

        cmark_node *link = cmark_node_new(CMARK_NODE_LINK);
        cmark_node *link_txt = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node_set_literal(link_txt, hash.c_str());

        std::ostringstream linkStream;
        linkStream << url << "/commit/" << hash;
        cmark_node_set_url(link, linkStream.str().c_str());
        cmark_node_append_child(link, link_txt);

        cmark_node *end = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node_set_literal(end, ")");

        cmark_node_append_child(p, begin);
        cmark_node_append_child(p, link);
        cmark_node_append_child(p, end);
    }

    cmark_node_append_child(item, p);
    cmark_node_append_child(list, item);
}

using namespace StandardRelease;

enum HeaderType
{
    Release,
    Section
};

static int getHeaderLevel(const SemVer next, const SemVer old, HeaderType headerType)
{
    const auto incrementType = next.incrementType(old);
    int level = 0;

    switch (incrementType) {
        case SemVer::Major:
        case SemVer::Minor:
            level = 2;
            break;
        default:
            level = 3;
            break;
    }

    // Section headers should always be one level smaller than Releases.
    if (headerType == HeaderType::Section) {
        level++;
    }

    return level;
}

static cmark_node *generateReleaseHeader(cmark_node *root, const SemVer next, const SemVer old,
                                         const std::string url, const std::string timeStr)
{
    cmark_node *header = cmark_node_new(CMARK_NODE_HEADING);
    cmark_node *paragraph = cmark_node_new(CMARK_NODE_PARAGRAPH);

    // Add <a name=""></a> reference.
    {
        cmark_node *inlineHtml = cmark_node_new(CMARK_NODE_HTML_INLINE);
        const std::string inlineString = "<a name=\"" + next.str() + "\"></a>";

        cmark_node_set_literal(inlineHtml, inlineString.c_str());

        cmark_node_append_child(paragraph, inlineHtml);
        cmark_node_insert_after(root, paragraph);
    }

    // Add [version](/compare/) (date)
    {
        cmark_node *link = cmark_node_new(CMARK_NODE_LINK);
        cmark_node *link_txt = cmark_node_new(CMARK_NODE_TEXT);

        const int level = getHeaderLevel(next, old, HeaderType::Release);
        cmark_node_set_heading_level(header, level);

        cmark_node_set_literal(link_txt, next.str().c_str());

        std::ostringstream linkStream;
        linkStream << url << "/compare/v" << old << "...v" << next;
        cmark_node_set_url(link, linkStream.str().c_str());
        cmark_node_append_child(link, link_txt);

        cmark_node *time = cmark_node_new(CMARK_NODE_TEXT);
        std::ostringstream timeStream;
        timeStream << " (" << timeStr << ")";
        cmark_node_set_literal(time, timeStream.str().c_str());

        cmark_node_append_child(header, link);
        cmark_node_append_child(header, time);
        cmark_node_insert_after(paragraph, header);
    }

    return header;
}

Changelog::Changelog()
    : IChangelog()
    , d(new ChangelogPrivate)
{
}

Changelog::Changelog(const std::string filename)
    : IChangelog(filename)
    , d(new ChangelogPrivate)
{
}

Changelog::~Changelog()
{
    delete d;
}

void Changelog::read()
{
    std::error_code code;

    if (d->root != nullptr) {
        cmark_node_free(d->root);
    }
    const bool exists = std::filesystem::exists(filename(), code);
    if (exists) {
        readFile();
    } else {
        createFile();
    }
}

void Changelog::readFile()
{
    FILE *f = fopen(filename().c_str(), "r");
    if (f == nullptr) {
        setError(Error(Error::ErrorOpeningFile, filename()));
        return;
    }

    d->root = cmark_parse_file(f, CMARK_OPT_DEFAULT);
    fclose(f);

    if (d->root == nullptr) {
        // TODO: New error: Invalid CHANGELOG.
        setError(Error::InternalError);
        return;
    }

    d->sibling = findInsertNode(d->root);
    if (d->sibling == nullptr) {
        // TODO: New error: Invalid CHANGELOG.
        setError(Error::InternalError);
        return;
    }

    // cmark_node_insert_after

#if 0
    std::cout << cmark_node_get_type_string(node) << std::endl;
    cmark_node *n1 = cmark_node_first_child(node);
    std::cout << cmark_node_get_type_string(n1) << std::endl;
    std::cout << cmark_node_get_literal(n1) << std::endl;
#endif

    setError(Error::InternalError);
}

void Changelog::write()
{
    std::ofstream changelogFile;
    changelogFile.open(filename());
    changelogFile << content();
    changelogFile.close();
}

// std::string Changelog::content() const {}

void Changelog::createFile()
{
    cmark_node *sibling = nullptr;

    cmark_node *doc = cmark_node_new(CMARK_NODE_DOCUMENT);

    {
        cmark_node *header = cmark_node_new(CMARK_NODE_HEADING);
        cmark_node_set_heading_level(header, 1);

        cmark_node *txt = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node_set_literal(txt, "Changelog");
        cmark_node_append_child(header, txt);
        cmark_node_append_child(doc, header);
    }

    {
        cmark_node *p1 = cmark_node_new(CMARK_NODE_PARAGRAPH);
        cmark_node *p1_text = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node_set_literal(
                p1_text, "All notable changes to this project will be documented in this file.");
        cmark_node_append_child(p1, p1_text);
        cmark_node_append_child(doc, p1);
    }

    {
        cmark_node *p = cmark_node_new(CMARK_NODE_PARAGRAPH);
        cmark_node *txt1 = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node *link = cmark_node_new(CMARK_NODE_LINK);
        cmark_node *link_txt = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node *txt3 = cmark_node_new(CMARK_NODE_TEXT);

        cmark_node_set_literal(txt1, "See ");
        cmark_node_set_literal(link_txt, "Conventional Commits");
        cmark_node_set_url(link, "https://conventionalcommits.org");
        cmark_node_set_literal(txt3, " for commit guidelines.");

        cmark_node_append_child(link, link_txt);
        cmark_node_append_child(p, txt1);
        cmark_node_append_child(p, link);
        cmark_node_append_child(p, txt3);

        cmark_node_append_child(doc, p);
    }

    {
        cmark_node *p = cmark_node_new(CMARK_NODE_PARAGRAPH);
        cmark_node *txt1 = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node *link1 = cmark_node_new(CMARK_NODE_LINK);
        cmark_node *link1_txt = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node *link2 = cmark_node_new(CMARK_NODE_LINK);
        cmark_node *link2_txt = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node *txt2 = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node *txt3 = cmark_node_new(CMARK_NODE_TEXT);

        cmark_node_set_literal(txt1, "This format is based on ");
        cmark_node_set_literal(txt2, ", and this project adheres to ");
        cmark_node_set_literal(txt3, ".");
        cmark_node_set_literal(link1_txt, "Keep a Changelog");
        cmark_node_set_literal(link2_txt, "Semantic Versioning");

        cmark_node_set_url(link1, "https://keepachangelog.com/en/1.0.0/");
        cmark_node_set_url(link2, "https://semver.org/spec/v2.0.0.html");

        cmark_node_append_child(link1, link1_txt);
        cmark_node_append_child(link2, link2_txt);

        cmark_node_append_child(p, txt1);
        cmark_node_append_child(p, link1);
        cmark_node_append_child(p, txt2);
        cmark_node_append_child(p, link2);
        cmark_node_append_child(p, txt3);

        cmark_node_append_child(doc, p);
    }

    d->root = doc;
}

// TODO: Should extract url from origin be done here?
void Changelog::generate(const SemVer current, const SemVer old,
                         const IConventionalCommit::Commits commits, const std::string url)
{
    auto type = current.incrementType(old);
    char date[200];

    {
        // Extract time
        std::time_t t = std::time(0);
        std::tm *now = std::localtime(&t);
        int ret = strftime(date, sizeof(date), "%G-%m-%d", now);
        if (ret == 0) {
            setError(Error(Error::InternalError, "Invalid date format"));
            return;
        }
    }

    cmark_node *features_list = cmark_node_new(CMARK_NODE_LIST);
    cmark_node *bugs_list = cmark_node_new(CMARK_NODE_LIST);
    cmark_node *perf_list = cmark_node_new(CMARK_NODE_LIST);
    cmark_node *reverts_list = cmark_node_new(CMARK_NODE_LIST);

    cmark_node_set_list_tight(features_list, 1);
    cmark_node_set_list_tight(bugs_list, 1);
    cmark_node_set_list_tight(perf_list, 1);
    cmark_node_set_list_tight(reverts_list, 1);

    // std::cout << "---\n";

    for (const auto commit : commits) {
        if (commit.feature) {
            addItem(features_list, commit.scope, commit.subject, commit.hash, url);
        } else if (commit.bugfix) {
            addItem(bugs_list, commit.scope, commit.subject, commit.hash, url);
        } else if (commit.type == "perf") {
            addItem(perf_list, commit.scope, commit.subject, commit.hash, url);
        } else if (commit.type == "revert") {
            addItem(reverts_list, commit.scope, commit.subject, commit.hash, url);
        }
    }

    cmark_node *sibling = findInsertNode(d->root);

    sibling = generateReleaseHeader(sibling, current, old, url, date);

    const int level = getHeaderLevel(current, old, HeaderType::Section);

    // Add features.
    if (hasItems(features_list)) {
        sibling = addSection(sibling, features_list, "Features", level);
    } else {
        cmark_node_free(features_list);
    }

    // Add bug fixes.
    if (hasItems(bugs_list)) {
        sibling = addSection(sibling, bugs_list, "Bug Fixes", level);
    } else {
        cmark_node_free(bugs_list);
    }

    if (hasItems(perf_list)) {
        sibling = addSection(sibling, perf_list, "Performance Improvements", level);
    } else {
        cmark_node_free(perf_list);
    }

    if (hasItems(reverts_list)) {
        sibling = addSection(sibling, reverts_list, "Reverts", level);
    } else {
        cmark_node_free(reverts_list);
    }

    // TODO: Should "Build System", "Tests", "Docs", etc be included?

    char *data = cmark_render_commonmark(d->root, CMARK_OPT_DEFAULT, 0);
    setContent(data);
    delete data;

    // std::cout << content();

    // std::cout << "---\n";
}
