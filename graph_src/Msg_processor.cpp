#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <vector>
#include "graph_types.h"
#include "graph_helper.h"
#include <fstream>
#include <mutex>
#include <deque>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

using namespace std;

int main(int argc, char* argv[]){

    int num_core_pairs = 2;
    vector<thread> threadVector;

    vector<Vertex> VL;


    // vector<sharedQueue> threadQueues(num_threads); 
    // vector<int> done(num_threads);


}