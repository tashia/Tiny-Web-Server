#include "GlobalHelpFunctions.h"
#include "TinyHttpServer.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout<<"Please provide the port numbers you want to moniter: \n";
        return 0;
    }
    HttpServer* pHSs[argc-1];
    for (int i=1; i<argc; i++)
    {
        pHSs[i-1] = new HttpServer(StringToInt(argv[i]));
        pHSs[i-1]->start();
    }
    while(1);
}
