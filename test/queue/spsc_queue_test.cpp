#include <csics/queue/SPSCQueue.hpp>
// right now, instantation testing
struct HeaderTest {
    int a;
    float b;
};

struct DataTest {
    double x;
    char y;
};

namespace csics::queue {
template class SPSCQueueBlockAdapter<HeaderTest, DataTest>;
template class SPSCQueueBlockAdapterRange<HeaderTest, DataTest>;
};


// For now nothing, in the future maybe use gtest
int main() {}
