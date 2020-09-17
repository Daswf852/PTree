#pragma once

#include <exception>
#include <istream>
#include <memory>
#include <optional>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "utils.hpp"

namespace Shiba::Perm {

const static char pDelimiter = '.';
const static std::regex pSegmentRegex("^[a-z]{1}[a-zA-Z_-]{0,}$");

except(PermissionException, std::exception);
except(BadSegment, PermissionException);

class LinearPermission {
  public:
    LinearPermission(const std::string &pstr)
    : lazyString(pstr) {
        std::vector<std::string> tempStore;

        std::istringstream iss(pstr);
        std::string current;
        while (std::getline(iss, current, pDelimiter)) {
            if (std::regex_match(current, pSegmentRegex))
                tempStore.push_back(current);
            else
                throw BadSegment();
        }

        permission = std::move(tempStore);
    }

    LinearPermission(const std::vector<std::string> &vec) {
        if (vec.size() == 0)
            throw std::out_of_range("Tried to construct LinearPermission with 0 permissions in the passed vector, fuck you");

        lazyString = "";

        for (const auto &str : vec) {
            if (std::regex_match(str, pSegmentRegex))
                permission.push_back(str);
            else
                throw BadSegment();
        }
        RecalculateLazyString();
    }

    LinearPermission(const LinearPermission &other)
    : permission(other.permission) {
        RecalculateLazyString();
    }

    LinearPermission &operator=(const LinearPermission &other) {
        permission = other.permission;
        RecalculateLazyString();
        return *this;
    }

    void Append(const std::string &segment) {
        std::unique_lock<std::mutex> lock(mutex);

        if (std::regex_match(segment, pSegmentRegex)) {
            lazyString += '.';
            lazyString += segment;
            permission.push_back(segment);
        }
    }

    std::string String() const {
        std::unique_lock<std::mutex> lock(mutex);

        return lazyString;
    }

    std::vector<std::string> Permission() const {
        std::unique_lock<std::mutex> lock(mutex);

        return permission;
    }

    void PopBack() {
        permission.pop_back();
        RecalculateLazyString();
    }

    void PopFront() {
        permission.erase(permission.begin());
        RecalculateLazyString();
    }

    friend std::ostream &operator<<(std::ostream &stream, const LinearPermission &perm) {
        stream << perm.String();
        return stream;
    }

  private:
    void RecalculateLazyString() {
        for (const auto &str : permission) {
            lazyString += str;
            lazyString += '.';
        }

        lazyString.pop_back();
    }

    mutable std::mutex mutex;
    std::vector<std::string> permission;
    std::string lazyString;
};

class TreeNode {
  public:
    TreeNode(const std::string &identifier, std::optional<std::reference_wrapper<const TreeNode>> parent = std::nullopt)
    : identifier(identifier)
    , parent(parent) {
    }

    ~TreeNode() = default;

    void Append(const LinearPermission &perm) {
        Append(perm.Permission());
    }

    LinearPermission GetLinear() const {
        std::unique_lock<std::mutex> lock(nodeMutex);
        std::vector<std::string> identifiers;

        std::reference_wrapper<const TreeNode> ref = *this;
        identifiers.push_back(ref.get().identifier);
        while (ref.get().parent != std::nullopt) {
            ref = ref.get().parent->get();
            identifiers.push_back(ref.get().identifier);
        }

        std::reverse(identifiers.begin(), identifiers.end());

        return LinearPermission(identifiers);
    }

    bool ContainsLinear(const LinearPermission &perm) {
        std::unique_lock<std::mutex> lock(nodeMutex);

        std::string ident = perm.Permission().at(0);
        if (ident != identifier)
            return false;

        auto newPerm = perm;
        newPerm.PopFront();

        if (newPerm.Permission().size() == 0)
            return true;

        ident = newPerm.Permission().at(0);

        if (children.size() == 0) {
            return (ident == identifier);
        } else {
            for (const auto &child : children) {
                if (child->identifier == ident) {
                    return child->ContainsLinear(newPerm);
                }
            }
        }

        return false;
    }

    friend std::ostream &operator<<(std::ostream &stream, const TreeNode &node) {
        if (node.children.size() == 0) {
            auto perm = node.GetLinear();

            stream << perm << std::endl;
        } else {
            for (const auto &child : node.children) {
                stream << *child;
            }
        }

        return stream;
    }

  private:
    std::size_t GetChildIndex(const std::string &identifier) const { //not thread safe
        for (std::size_t idx = 0; const auto &child : children) {
            if (child->identifier == identifier)
                return idx;
            ++idx;
        }
        throw std::out_of_range("");
    }

    void Append(std::vector<std::string> vec) {
        if (vec.size() == 0)
            return;

        std::unique_lock<std::mutex> lock(nodeMutex);

        std::size_t existingIndex = 0;
        std::string ident = vec.at(0);

        try {
            existingIndex = GetChildIndex(ident);
        } catch (std::out_of_range &) {
            children.push_back(std::make_unique<TreeNode>(ident, *this));
            existingIndex = children.size() - 1;
        }

        vec.erase(vec.begin());

        if (vec.size() > 0)
            children.at(existingIndex)->Append(vec);
    }

    std::string identifier;
    std::vector<std::unique_ptr<TreeNode>> children;
    std::optional<std::reference_wrapper<const TreeNode>> parent;

    mutable std::mutex nodeMutex;
};

} // namespace Shiba::Perm