#include <iostream>

#include "tree.hpp"

int main(int, char **) {
    Shiba::Perm::LinearPermission perm("foo.bar.baz");
    //std::cout << perm << std::endl;
    for (auto vec = perm.Permission(); const auto &str : vec) {
        //std::cout << str << std::endl;
    }

    Shiba::Perm::LinearPermission p0("foo");
    Shiba::Perm::LinearPermission p1("foo.bar");
    Shiba::Perm::LinearPermission p2("foo.baz");
    Shiba::Perm::LinearPermission p3("foo.bar.qux");

    Shiba::Perm::TreeNode tree("root");
    tree.Append(p0);
    tree.Append(p1);
    tree.Append(p2);
    tree.Append(p3);
    std::cout << tree << std::endl;

    std::cout << std::boolalpha << tree.ContainsLinear(Shiba::Perm::LinearPermission("root.foo.qux"));

    return 0;
}