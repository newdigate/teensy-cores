#include <cassert>
#include <cstdint>
#include <cstdio>
static const int SIZE = 64;
static int avail(int head, int tail){ return head >= tail ? head - tail : SIZE + head - tail; }
static int availForWrite(int head, int tail){ return head >= tail ? SIZE - 1 - head + tail : tail - head - 1; }
int main(){
    assert(avail(0,0) == 0);
    assert(avail(5,0) == 5);
    assert(avail(0,60) == 4);
    assert(availForWrite(0,0) == SIZE - 1);
    assert(availForWrite(63,0) == 0);
    printf("test_ringbuffer OK\n");
    return 0;
}
