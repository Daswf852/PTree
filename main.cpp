#include <iostream>

#include "tree.hpp"

int main(int, char **) {
    Shiba::Perm::PNode fullPermissions;
    fullPermissions.InsertLP("foo.bar");
    fullPermissions.InsertLP("foo.baz");
    fullPermissions.InsertLP("foo.qux");

    std::cout << "Full permission tree:" << std::endl;
    std::cout << fullPermissions << std::endl;

    Shiba::Perm::PNode userPermissions;
    userPermissions.InsertLP("foo");
    std::cout << "User permissions before assisted removal:" << std::endl;
    std::cout << userPermissions << std::endl;

    userPermissions.Get("foo").AssistedRemove("bar", fullPermissions);
    std::cout << "After removal:" << std::endl;
    std::cout << userPermissions << std::endl;

    return 0;
}