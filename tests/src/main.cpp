#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"
extern "C"{
#include "compage.h"
}

int main(int argc, const char *argv[]){
    return compage_main(argc, argv);
}
