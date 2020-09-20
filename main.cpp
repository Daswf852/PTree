#include <iostream>

#include "tree.hpp"

int main(int, char **) {
    /*std::vector<std::vector<std::string>> TreeList{
        {"foo", "bar"},
        {"foo", "baz"},
        {"qux"},
        {"quz", "quuz"},
    };*/

    std::vector<std::vector<std::string>> TreeList{
        {"b", "a"},
        {"b", "d", "c"},
        {"b", "d", "e"},
        {"g", "i", "h"},
    };

    Shiba::Perm::PTree tree("f");

    for (const auto &vec : TreeList) {
        tree.Insert(vec);
    }

    std::cout << tree;

    std::cout << "Traversal, pre-order:" << std::endl;
    tree.Traverse<Shiba::Perm::TraversalOrder::PreOrder>([](const Shiba::Perm::PTree &node) { std::cout << node.Identifier(); });
    std::cout << std::endl;
    /*std::cout << "Traversal, in-order:" << std::endl;
    tree.Traverse<Shiba::Perm::TraversalOrder::InOrder>([](const Shiba::Perm::PTree &node) { std::cout << node.Identifier(); });
    std::cout << std::endl;*/
    std::cout << "Traversal, post-order:" << std::endl;
    tree.Traverse<Shiba::Perm::TraversalOrder::PostOrder>([](const Shiba::Perm::PTree &node) { std::cout << node.Identifier(); });
    std::cout << std::endl;

    return 0;
}