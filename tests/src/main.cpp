#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "compage.h"
#include "utils.h"

int main(int argc, char *argv[]){

    if(argc > 1){
        if(argc > 2){
            // generate config file
            if(strcmp(argv[1], "default") == 0)
                return compage_createDefaultConfig(argv[2]);
        }

        // use config file
        compage_initFromConfig(argv[1]);
        compage_doPthreads();
        pause();
        return 0;
    }

    //compage_debugSections();

    pause();
    return 0;
}
