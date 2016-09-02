
#include "Framing.h"
#include "Serial.h"
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

using namespace std;
char Flag = 0x7e;
char Flag_Escape = 0x7d;

void Framing::send(char *buffer, int bytes)
{
	porta->write(buffer,bytes);
}

bool Framing::handle(char byte)
{
	switch(estado)
	{
		/*
		 * Maquina de Estado
		 * Estado 0: Detectada Mensagem;
		 * Estado 1: Recepcao de Bytes;
		 * Estado 2: Byte de Escape
		 * Estado 3: Fim de Recepcao
		 */

		case 0:
			if(!strcmp(&byte,&Flag))
			{
				//printf("\n INICIO DA MENSAGEM"\n);
				n_bytes++;
				estado = 1;
			}
			break;

		case 1:
			if(!strcmp(&byte,&Flag_Escape))
			{
				estado = 2;
			}
			else if(!strcmp(&byte,&Flag))// fim
			{
				n_bytes++;
				estado = 3;
			}
			else
			{
				n_bytes++;
			}
			break;

		case 2:
			if(!strcmp(&byte,&Flag_Escape))
			{
				byte ^= 0x20;
				n_bytes++;
				estado = 1;
			}
			break;
		case 3:
			estado = 0;
			// chamar checksum?
			// salvar menasgem
			break;

		default:
			break;

	}
	if (estado != 3)
	{
		return true;
	}
	else
	{
		return false;
	}

}
int Framing::receive(char *buffer)
{


    	int i,j;
    	int TamanhoMensagem = strlen(buffer);
    	for(i=0;TamanhoMensagem;i++)
    	{
    		 if(handle(buffer[i]))
    		 {
    			 printf( "Fim do quadro");
    			 break;
    		 }
    	}


    //FILE *received_file;
    //rpmaçdp
    //received_file = fopen(argv[3], "w");
/*


    if (strcmp(buffer, Flag) == 0)
    {
        cout << "ashow 7E"
        //handle(Flag);
    }
    if (strcmp(buffer, Flag2) == 0)
    {
        cout << "ashow 7D"
        //handle(Flag);
    }
    numero_bytes++;
*/
    //fclose(received_file);
    return TamanhoMensagem;

}

/*
bool Framing::handle(char byte)
{
    int Estado;

     switch(Estado)
            {
                case 0:
                    break;

                case 1:
                    break;

                case 2:
                    break;

                case 3:

                    // return true se quadro completo ( novo 7E)
                    break;

                default:
                    break;
            }
}
*/
