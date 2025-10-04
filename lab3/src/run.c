#include <unistd.h>
int main(void) {
    char *const args = {NULL};

    int pid = fork();


    if ( pid == 0 ) {
		execv("sequential_min_max", args);
	}

    wait( 2 );

    return 0;
}