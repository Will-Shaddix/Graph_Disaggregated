#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <vector>
#include "graph_types.h"
#include <fstream>
#include <mutex>
#include <deque>

using namespace std;

struct update{

    uint32_t src_id;
    uint32_t dst_id;
    uint32_t dist;

};

struct sharedQueue{
    mutable mutex head_lock;
    mutable mutex tail_lock;
    mutable deque<update> queue;
};

// 

int main(int argc, char* argv[]){

    bool g_flag = true;
    int num_nodes = 512;
    int goal_node = 413;
    vector<vector<Edge> > EL(num_nodes);// Edge list
     // keeps track of which threads are done
    vector<Vertex> VL;
    for(int z =0; z < num_nodes; z++){ // initialize VL
        if(z ==0){
            VL.emplace_back(Vertex(1000, z)); //= Vertex(0, z);
        }
        else{
            VL.emplace_back(Vertex(1000, z));
            //VL[z] = Vertex(-1, z);
        }
    }

    

    ifstream graph_file;

    

    int num_threads = 0;
    if(argc == 1){

    }
    else if(argc == 4){
        num_threads = atoi(argv[1]);
        graph_file.open(argv[2]);
        goal_node = atoi(argv[3]);
    }
    else{
        return 0;
    }

    vector<thread> threadVector;
    vector<sharedQueue> threadQueues(num_threads); 
    vector<int> done(num_threads);

    done.assign(num_threads, 0);

    update initial;
    initial.dist = 0;
    initial.dst_id = 0;
    initial.src_id = 0;

    threadQueues[0].queue.emplace_back(initial); //should initialize
    

    auto thread_end=[&done, &g_flag](){
        while(g_flag){
            bool flag = true;
            for(int x: done){
                if(x != 1){
                    flag = false;
                }
            }
            //printf("flag = %d\n", flag);
            if(flag == true){
                g_flag = false;
            }
        }

    };

    // may need a helper thread to check when its all done
    auto thread_sssp=[&threadQueues, &threadVector, &done, &VL, &EL, &g_flag](int t_id, int n_thr, int vpt){
        // printf("thread id is: %d\n", t_id);

        while(g_flag){
    
            if(!threadQueues[t_id].queue.empty()){
                threadQueues[t_id].head_lock.lock();//Lock while accessing head

                update curr_update = threadQueues[t_id].queue.front();
                threadQueues[t_id].queue.pop_front();
                if(threadQueues[t_id].queue.empty()){
                    done[t_id] = 1;
                }
                else{
                    done[t_id] = 0;
                }

                threadQueues[t_id].head_lock.unlock();//unlock

                if (curr_update.dist < VL[curr_update.dst_id].dist){//we have a new distance
                    //printf("Updating: t_id:%d Current distance: %d  New Distance: %d Prev Edge: %d\n", t_id, VL[curr_update.dst_id].dist, curr_update.dist, curr_update.src_id);

                    VL[curr_update.dst_id].dist = curr_update.dist;
                    VL[curr_update.dst_id].prev_edge = curr_update.src_id;
                    //printf("this prev edge = %d at %d\n", VL[curr_update.dst_id].prev_edge, curr_update.dst_id);
                    
                    for(Edge my_edge : EL[curr_update.dst_id]){ // for each edge in neighborhood, this is yellow and pink section of sega pseudocode

                        threadQueues[my_edge.neighbor/vpt].tail_lock.lock();// need to check if empty, if so, also grab head lock
                        
                        update temp_up;
                        temp_up.dist = curr_update.dist + my_edge.weight;
                        temp_up.dst_id = my_edge.neighbor;
                        temp_up.src_id = curr_update.dst_id;
                        //to figure out which threads queue to update, we take the dst_id divided by (vertices per thread)
                        
                        threadQueues[my_edge.neighbor/vpt].queue.push_back(temp_up); //updates the frontier of the corresponding 

                        threadQueues[my_edge.neighbor/vpt].tail_lock.unlock();

                    }


                }



            }
            // else{

            // }
        }

        
        
    };


    int curr_src_id = -1;
    //uint32_t curr_edge_index[num_edge_binaries] = {0};
    //uint32_t curr_num_edges = 0;
    int max_dst_id = -1;
    int src_id, dst_id, weight;
    while(!graph_file.fail()) {
        // if (isWeighted) {
        //     graph_file >> src_id >> dst_id >> weight;
        // } else {
            graph_file >> src_id >> dst_id;
            weight = rand() % 32 + 1;
        //}
        // if (dst_id > max_dst_id) {
        //     max_dst_id = dst_id;
        // }
        if(src_id != dst_id){
            EL[src_id].push_back(Edge(weight, dst_id));
        }
    }
    //printf("test edge list[0]: %lu\n", EL[0][0].neighbor);


    for(int x = 0; x < num_threads; x++){
       
        threadVector.push_back(thread(thread_sssp, x, num_threads, num_nodes/num_threads));
    }

        threadVector.push_back(thread(thread_end));

    for(auto& t: threadVector){
        t.join();
    }




    // this part is just to print the path
    vector<int> path;
    path.emplace_back(goal_node);
    int prev = VL[goal_node].prev_edge;
    path.emplace_back(prev);

    while(prev != 0){
        prev = VL[prev].prev_edge;
        path.emplace_back(prev);
    }

    while(!path.empty()){
        printf("%d -> ", path.back());
        path.pop_back();
    }
    printf(" done!\n");

    graph_file.close();
}