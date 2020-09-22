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

    //std::cout << rootTree;

    /*for (const auto &str : toInsert)
        Test(rootTree, str);

    Test(rootTree, "root");
    Test(rootTree, "root.special");
    Test(rootTree, "root.core");
    Test(rootTree, "root.core.utils");
    Test(rootTree, "root.core.mgmt");

    std::cout << std::endl;*/

    /*rootTree.Traverse([](Perm::PNode &node) {
        if (node.HasChildren())
            return;

        for (const auto &str : node.GetFullBranch())
            std::cout << str << '.';

        std::cout << std::endl;
    });*/

    //rootTree.FillParentsOfChildren();
    std::cout << Perm::LinearPermission(rootTree.GetLP("").GetFullBranch()).String() << std::endl;
    std::cout << Perm::LinearPermission(rootTree.GetLP("core").GetFullBranch()).String() << std::endl;
    std::cout << Perm::LinearPermission(rootTree.GetLP("core.utils").GetFullBranch()).String() << std::endl;
    std::cout << Perm::LinearPermission(rootTree.GetLP("core.utils.echo").GetFullBranch()).String() << std::endl;

    /*for (const auto &str : Perm::LinearPermission("special.placeholder").Permission())
        std::cout << str << '.';
    std::cout << std::endl;

    std::cout << ref.Identifier() << '.';
    for (std::optional<std::reference_wrapper<Perm::PNode>> current = ref.parent; current != std::nullopt; current = current->get().parent)
        std::cout << current->get().Identifier() << '.';

    std::cout << std::endl;*/

    return 0;
}