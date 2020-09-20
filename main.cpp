#include <ios>
#include <iostream>

#include "tree.hpp"

namespace Perm = Shiba::Perm;

#define Test(tree, str) \
    std::cout << str << ": " << std::boolalpha << tree.ContainsLP(str) << std::endl;

int main(int, char **) {
    std::vector<std::string> toInsert{
        "special.placeholder",
        "special.chanOP",
        "core.utils.echo",
        "core.mgmt.restart",
    };

    Perm::PNode rootTree("root");
    for (const auto &str : toInsert)
        rootTree.InsertLP(str);

    std::cout << rootTree;

    for (const auto &str : toInsert)
        Test(rootTree, str);

    Test(rootTree, "root");
    Test(rootTree, "root.special");
    Test(rootTree, "root.core");
    Test(rootTree, "root.core.utils");
    Test(rootTree, "root.core.mgmt");

    std::cout << std::endl;

    rootTree.Traverse([](Perm::PNode &node) {
        if (node.HasChildren())
            return;

        for (const auto &str : node.GetFullBranch())
            std::cout << str << '.';

        std::cout << std::endl;
    });

    return 0;
}