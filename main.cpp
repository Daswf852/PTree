#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

#include "tree.hpp"

/// 2 FUNCTIONS BELOW ARE STOLEN FROM STACKOVERFLOW

int parseLine(char *line) {
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char *p = line;
    while (*p < '0' || *p > '9')
        p++;
    line[i - 3] = '\0';
    i = atoi(p);
    return i;
}

int getValue() { //Note: this value is in KB!
    FILE *file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL) {
        if (strncmp(line, "VmSize:", 7) == 0) {
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}

int main(int, char **) {
    Shiba::Perm::PNode root;
    std::reference_wrapper<Shiba::Perm::PNode> current = root;
    std::atomic_size_t inserts = 0;

    std::thread([&root, &inserts]() {
        int initialUsage = getValue();
        std::size_t prev = 0;

        for (;;) {
            int treeUsage = getValue() - initialUsage;

            double ratio = (double)treeUsage / (double)inserts;
            std::size_t ips = inserts - prev;
            prev = inserts;

            std::cout << "tree's mem usage in KB: " << treeUsage << ", elements: " << inserts << std::endl;
            std::cout << "B per element: " << ratio * 1000 << ", IPS: " << ips << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }).detach();

    for (;;) {
        current = current.get().Insert("");
        inserts++;
    }
    return 0;
}