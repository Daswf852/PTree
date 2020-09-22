#include <iostream>

#include "tree.hpp"

namespace Perm = Shiba::Perm;

int main(int, char **) {
    /*std::vector<std::string> toInsert{
        "special.placeholder",
        "special.chanOP",
        "core.utils.echo",
        "core.mgmt.restart",
        "core.mgmt.op",
    };

    Perm::PNode rootTree("root");
    for (const auto &str : toInsert)
        rootTree.InsertLP(str);*/

    Perm::PNode rootTree("root");
    auto &abc = rootTree.Insert("a").Insert("b").Insert("c");
    auto &ade = rootTree.Get("a").Insert("d").Insert("e");

    //rootTree.FillParentsOfChildren();
    std::cout << Perm::LinearPermission(rootTree.GetLP("a.b.c").GetFullBranch()).String() << std::endl;
    std::cout << Perm::LinearPermission(rootTree.GetLP("a.d.e").GetFullBranch()).String() << std::endl;

    return 0;
}