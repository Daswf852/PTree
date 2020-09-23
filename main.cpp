#include <iostream>

#include "tree.hpp"

int main(int, char **) {
    Shiba::Perm::PNode fullPermissions;
    fullPermissions.InsertLP("special.placeholder");
    fullPermissions.InsertLP("special.chanop");
    fullPermissions.InsertLP("core.echo");
    fullPermissions.InsertLP("core.ping");
    fullPermissions.InsertLP("core.mgmt.restart");
    fullPermissions.InsertLP("core.mgmt.pgive");
    fullPermissions.InsertLP("core.mgmt.ptake");
    fullPermissions.InsertLP("holo.addchan");

    std::string serialization = fullPermissions.Serialize();
    std::cout << serialization << std::endl;

    auto newNode = Shiba::Perm::PNode::FromSerialization(serialization);
    serialization = newNode.Serialize();
    std::cout << serialization << std::endl;

    return 0;
}