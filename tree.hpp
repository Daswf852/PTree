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

    void Append(const std::string &segment) {
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

class PNode {
  public:
    PNode()
    : root(*this)
    , parent(std::nullopt)
    , data("root")
    , depth(0) {}

    PNode(std::string data)
    : root(*this)
    , parent(std::nullopt)
    , data(data)
    , depth(0) {}

    PNode(const std::string &data, PNode &root, PNode &parent, std::size_t depth)
    : root(root)
    , parent(parent)
    , data(data)
    , depth(depth) {}

    ~PNode() {
    }

    ///////////////////
    // Trivial funcs //
    ///////////////////

    bool HasChildren() const {
        return !children.empty();
    }

    const std::string &Identifier() const {
        return data;
    }

    std::size_t Depth() const {
        return depth;
    }

    PNode &Root() {
        return root;
    }

    void InsertParents() {
        for (auto &child : children) {
            child.parent = *this;
            child.InsertParents();
        }
    }

    const PNode &Get(const std::string &data) const {
        auto it = std::find_if(children.begin(), children.end(), [&data](const PNode &node) {
            return node.data == data;
        });

        if (it == children.cend())
            throw std::out_of_range("Couldn't find iterator");

        return *it;
    }

    PNode &Get(const std::string &data) {
        try {
            return const_cast<PNode &>(const_cast<const PNode *>(this)->Get(data));
        } catch (std::out_of_range &) {
            std::rethrow_exception(std::current_exception());
        }

        throw std::runtime_error("how the fuck");
    }

    bool Contains(const std::string &) const {
        try {
            Get(data);
            return true;
        } catch (std::out_of_range &) {
            return false;
        }
    }

    bool Contains(std::vector<std::string>::const_iterator it, decltype(it) end) const {
        if (it == end)
            return !HasChildren(); //if the node has children, we can't match a wildcard check

        try {
            return Get(*it).Contains(it + 1, end);
        } catch (std::out_of_range &) { //catch from Get
            return !HasChildren();      //if the permission continues but the node doesn't, the node is a wildcard
        }
    }

    PNode &Insert(const std::string &data) {
        if (!Contains(data)) {
            children.push_back(std::move(PNode(data, root, *this, depth + 1)));
            root.InsertParents();
            return *(children.end() - 1);
        } else {
            return Get(data);
        }
    }

    ///////////////////
    // Derived funcs //
    ///////////////////

    const PNode &Get(std::vector<std::string>::const_iterator it, decltype(it) end) const {
        if (it == end)
            return *this;

        try {
            return Get(*it).Get(it + 1, end);
        } catch (std::out_of_range &) {
            std::rethrow_exception(std::current_exception());
        }
    }

    PNode &Get(std::vector<std::string>::const_iterator it, decltype(it) end) {
        try {
            return const_cast<PNode &>(const_cast<const PNode *>(this)->Get(it, end));
        } catch (std::out_of_range &) {
            std::rethrow_exception(std::current_exception());
        }
    }

    const PNode &Get(const std::vector<std::string> &vec) const {
        try {
            return Get(vec.cbegin(), vec.cend());
        } catch (std::out_of_range &) {
            std::rethrow_exception(std::current_exception());
        }
    }

    PNode &Get(const std::vector<std::string> &vec) {
        try {
            return const_cast<PNode &>(const_cast<const PNode *>(this)->Get(vec.cbegin(), vec.cend()));
        } catch (std::out_of_range &) {
            std::rethrow_exception(std::current_exception());
        }
    }

    const PNode &GetLP(const std::string &perm) const {
        try {
            return Get(LinearPermission(perm).Permission());
        } catch (std::out_of_range &) {
            std::rethrow_exception(std::current_exception());
        }
    }

    PNode &GetLP(const std::string &perm) {
        try {
            return const_cast<PNode &>(const_cast<const PNode *>(this)->GetLP(perm));
        } catch (std::out_of_range &) {
            std::rethrow_exception(std::current_exception());
        }
    }

    bool Contains(const std::vector<std::string> &vec) const {
        return Contains(vec.cbegin(), vec.cend());
    }

    bool Contains(const LinearPermission &perm) const {
        return Contains(perm.Permission());
    }

    bool ContainsLP(const std::string &perm) const {
        return Contains(LinearPermission(perm));
    }

    void Insert(std::vector<std::string>::const_iterator it, decltype(it) end) {
        if (it == end)
            return;

        try {
            Get(*it).Insert(it + 1, end);
        } catch (std::out_of_range &) {
            Insert(*it);
            (children.end() - 1)->Insert(it + 1, end);
        }
    }

    void Insert(const std::vector<std::string> &dataVec) {
        Insert(dataVec.cbegin(), dataVec.cend());
    }

    void Insert(const LinearPermission &perm) {
        Insert(perm.Permission());
    }

    void InsertLP(const std::string &perm) {
        Insert(LinearPermission(perm));
    }

    ///////////////////////
    // Misc helper funcs //
    ///////////////////////

    template<TraversalOrder order = TraversalOrder::PreOrder, typename Callable>
    void Traverse(Callable callback) {
        InternalTraverse<Callable, PNode *, order>(callback, this);
    }

    template<TraversalOrder order = TraversalOrder::PreOrder, typename Callable>
    void Traverse(Callable callback) const {
        const_cast<PNode *>(this)->InternalTraverse<Callable, const PNode *, order>(callback, this);
    }

    std::vector<std::string> GetFullBranch() const {
        std::vector<std::string> vec;
        vec.push_back(data);

        std::optional<std::reference_wrapper<PNode>> current = parent;
        while (current != std::nullopt) {
            vec.push_back(current->get().data);
            current = current->get().parent;
        }

        std::reverse(vec.begin(), vec.end());
        return vec;
    }

    LinearPermission GetPermission() {
        return LinearPermission(GetFullBranch());
    }

    friend std::ostream &operator<<(std::ostream &stream, const PNode &tree) {
        tree.Traverse<TraversalOrder::PreOrder>([](const Shiba::Perm::PNode &node) {
            for (std::size_t i = 0; i != node.Depth(); i++)
                std::cout << ' ';
            std::cout << node.Identifier() << std::endl;
        });

        return stream;
    }

  private:
    PNode &root;
    std::optional<std::reference_wrapper<PNode>> parent;
    std::vector<PNode> children;
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