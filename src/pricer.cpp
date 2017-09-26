#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


using namespace std;
int main(int argc, char **argv)
{
    int opt,c;
    while ((opt = getopt(argc,argv,"c")) != EOF)
        switch(opt)
        {
            case 'c': c = 1; cout << "c is enabled" << c << endl; break;
            //case '': cout << "nothing is enabled" << endl; break;
            case '?': fprintf(stderr, "usuage is -c: <value> \n"); exit(1);
            //default: cout << endl;
        }

    cout << "A FAIRE" << endl;
    return 0;
}
