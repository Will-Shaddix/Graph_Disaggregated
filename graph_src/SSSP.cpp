#include <stdio.h>
#include <stdlib.h>
#include <deque>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>


int main(int argc, char** argv){

    char data[64];
    int num_bytes = 64;
    
    char* mapped_area; //could replace char with data type/tuple?

    int buffer_size = 2048;
    int buffer_addr = open("buffer_addr", O_RDWR | O_CREAT, S_IRWXU);
    //printf("Hello World, file descriptor = %d\n", buffer_addr);

    mapped_area = (char*)mmap((void *)0xabcde000, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, buffer_addr, 0);  //Map shared should mean we only have to do this once



    int x = read(buffer_addr, data, num_bytes);
    printf("Data: %s\n", data);



    // FILE* file_stream = fdopen(buffer_addr, "w+");

    // fwrite((void*)"Goodbye", 1, 8, file_stream); 
    // fclose(file_stream);


    
    // x = read(buffer_addr, data, num_bytes);
    printf("Mapped_area: %p\n", mapped_area);

    strcpy(mapped_area, "Nice meetin you!\n"); 


    x = read(buffer_addr, data, num_bytes);
    printf("Data: %s\n", data);
    

}