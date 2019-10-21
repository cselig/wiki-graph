#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <chrono> 

using namespace std::chrono;
using namespace std;

int bfs(int src, int dst, int arr[], int graph_len)
{
    auto start = high_resolution_clock::now();

    queue<int> q;
    int breadth, offset, ind, num_links, link_start_ind, to_visit_offset, qsize;

    q.push(src);

    // mark start node with parent = -1
    arr[src / 4] = -1;

    breadth = 1;
    while (!q.empty()) {
        printf("breadth, qsize: %d, %lu\n", breadth, q.size());

        qsize = q.size();
        for (int i = 0; i < qsize; i++) {
            offset = q.front();
            q.pop();
            ind = offset / 4;
            num_links = arr[ind + 1];
            link_start_ind = ind + 4;
            for (int j = link_start_ind; j < link_start_ind + num_links; j++) {
                to_visit_offset = arr[j];
                if (arr[to_visit_offset / 4] == 0) {
                    // mark parent
                    arr[to_visit_offset / 4] = offset;
                    if (to_visit_offset == dst) {
                        auto stop = high_resolution_clock::now();
                        auto duration = duration_cast<microseconds>(stop - start);
                        printf("Path of length %d found in %.4f seconds\n", (breadth + 1), duration.count() / 1000000.0);
                        return to_visit_offset;
                    }
                    // add to queue
                    q.push(to_visit_offset);
                }
            }
        }
        breadth++;        
    }

    return -1;
}


void trace_path(int arr[], int offset)
{
    printf("tracing path...\n");

    printf("%d, ", offset);
    while (arr[offset / 4] != -1) {
        offset = arr[offset / 4];
        printf("%d, ", offset);
    }
    printf("\n");
}





int main(void) {
    auto start = high_resolution_clock::now();

    FILE* fileptr;
    int* buffer;
    long filelen;

    fileptr = fopen("./wikidata/indexbi.bin", "rb");  // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file

    buffer = (int *)malloc((filelen) * sizeof(int));
    fread(buffer, filelen, 1, fileptr);
    fclose(fileptr);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    printf("file loaded in %.4f seconds\n", duration.count() / 1000000.0);

    int offset = bfs(586209616, 515901088, buffer, filelen / 4);
    // long path: 586209616, 515901088

    if (offset == -1) {
        printf("no path found\n");
        return 0;
    }

    trace_path(buffer, offset);

    return 0;
}
