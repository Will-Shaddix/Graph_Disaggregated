
#include <stdio.h>
#include <tuple>

using namespace std;

int main(){

    const uint32_t x = 32;
    //tuple<uint32_t, uint32_t, uint32_t> my_tuple = make_tuple(x,x,x);
    uint32_t my_tuple = x;
    //printf("old tuple value = 32, new value = %u\n", get<0>(my_tuple));
    printf("my tuple is: %d", my_tuple);

    return 1;
}