#include "Framing.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <fstream>
    using std::ofstream;
#include <iostream>
#include <vector>
    using std::cout;
    using std::endl;


int main(){
	ofstream stream;
	stream.open("/home/ronaldo/Documentos/PTC/workspace/protocol/myurl.txt");
	const char * path = "/dev/ttyUSB0";
   char * data = (char*)malloc(19);
    data[0] = 'O';
    data[1] = 'l';
    data[2] = 'a';
    data[3] = 'a';
    data[4] = 'M';
    data[5] = 0x7e;
    data[6] = 'N';
    data[7] = 'D';
    data[8] = 'O';

    	//Serial serial(path,9600);
   	 Framing testa(8,1024);
   	int idx = strlen(data);
	testa.mountFrame(data,idx,0,0,);
	stream.write((char*)data,15);
	stream.close();




  //  testa.send(data1,11);
   //: testa.receive(data1);

return 0;
}
