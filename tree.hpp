#pragma once

#include <exception>
#include <istream>
#include <memory>
#include <optional>
#include <ostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "utils.hpp"

namespace Shiba::Perm {

enum class TraversalOrder {
    PreOrder,
    InOrder,
    PostOrder
};

const static char pDelimiter = '.';
const static std::regex pSegmentRegex("^[a-z]{1}[a-zA-Z_-]{0,}$");

bool SegmentIsValid(const std::string &segment) {
    return std::regex_match(segment, pSegmentRegex);
}

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

    void
    Append(const std::string &segment) {
        if (SegmentIsValid(segment)) {
            lazyString += '.';
            lazyString += segment;
            permission.push_back(segment);
        }
    }

    std::string String() const {
        return lazyString;
    }

    std::vector<std::string> Permission() const {
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

    std::vector<std::string> permission;
    std::string lazyString;
};

class PTree {
  public:
    PTree(std::string data, std::optional<std::reference_wrapper<PTree>> parent = std::nullopt, std::size_t depth = 0)
    : parent(parent)
    , data(data)
    , depth(depth) {
    }

    ~PTree() {
    }

    bool HasIndex(const std::string &data) const {
        for (const auto &node : children)
            if (node.data == data)
                return true;
        return false;
    }

    std::size_t GetIndex(const std::string &data) const {
        for (auto it = children.cbegin(); it != children.cend(); it++)
            if (it->data == data)
                return it - children.cbegin();
        throw std::out_of_range("Couldn't find index");
    }

    std::vector<PTree>::iterator Get(const std::string &data) {
        auto idx = std::find_if(children.begin(), children.end(), [&data](PTree &node) {
            return node.data == data;
        });

        if (idx == children.end())
            throw std::out_of_range("Couldn't find iterator");

        return idx;
    }

    std::vector<PTree>::const_iterator Get(const std::string &data) const {
        auto idx = std::find_if(children.cbegin(), children.cend(), [&data](const PTree &node) {
            return node.data == data;
        });

        if (idx == children.cend())
            throw std::out_of_range("Couldn't find iterator");

        return idx;
    }

    bool HasChildren() const {
        return !children.empty();
    }

    void Insert(const std::string &data) {
        if (!HasIndex(data))
            children.push_back(PTree(data, *this, depth + 1));
    }

    void Insert(const std::vector<std::string> &dataVec) {
        Insert(dataVec.cbegin(), dataVec.cend());
    }

    void Insert(std::vector<std::string>::const_iterator it, decltype(it) end) {
        if (it == end)
            return;

        try {
            auto targetIt = Get(*it);
            targetIt->Insert(it + 1, end);
        } catch (std::out_of_range &) {
            children.push_back(PTree(*it, *this, depth + 1));
            (children.end() - 1)->Insert(it + 1, end);
        }
    }

    template<TraversalOrder order = TraversalOrder::PreOrder, typename Callable>
    void Traverse(Callable callback) {
        InternalTraverse<Callable, PTree *, order>(callback, this);
    }

    template<TraversalOrder order = TraversalOrder::PreOrder, typename Callable>
    void Traverse(Callable callback) const {
        const_cast<PTree *>(this)->InternalTraverse<Callable, const PTree *, order>(callback, this);
    }

    const std::string &Identifier() const {
        return data;
    }

    std::size_t Depth() const {
        return depth;
    }

    std::vector<std::string> GetFullBranch() {
        std::vector<std::string> vec;

        std::optional<std::reference_wrapper<PTree>> current = *this;
        while (current.has_value()) {
            vec.push_back(current->get().data);
            current = current->get().parent;
        }

        std::reverse(vec.begin(), vec.end());
        return vec;
    }

    LinearPermission GetPermission() {
        return LinearPermission(GetFullBranch());
    }

    friend std::ostream &operator<<(std::ostream &stream, const PTree &tree) {
        tree.Traverse([](const Shiba::Perm::PTree &node) {
            for (std::size_t i = 0; i != node.Depth(); i++)
                std::cout << ' ';
            std::cout << node.Identifier() << std::endl;
        });

        return stream;
    }

  private:
    std::optional<std::reference_wrapper<PTree>> parent;
    std::vector<PTree> children;
    std::string data;
    std::size_t depth;

    template<typename Callable, typename ThisType, TraversalOrder order = TraversalOrder::PreOrder>
    void InternalTraverse(Callable callback, ThisType th) {
        static_assert(order == TraversalOrder::PreOrder || order == TraversalOrder::PostOrder);

        if constexpr (order == TraversalOrder::PreOrder) {
            callback(*th);
            for (auto &child : children)
                child.InternalTraverse<Callable, ThisType, order>(callback, &child);

        } else if constexpr (order == TraversalOrder::PostOrder) {
            for (auto &child : children)
                child.InternalTraverse<Callable, ThisType, order>(callback, &child);

            callback(*th);
        }
    }
};

} // namespace Shiba::Perm