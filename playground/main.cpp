#include <fiber/sync/wait_group.hpp>

#include <iostream>

int main() {
    fiber::sync::WaitGroup wg;

    wg.Add(1);
    wg.Done();
    wg.Wait();

    return 0;
}