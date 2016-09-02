
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
	static int ProximoEstado;

	Estado = ProximoEstado;
	switch(Estado)
	{
		/*
		 * Maquina de Estado
		 * Estado 0: Ocioso;
		 * Estado 1: Recepcao;
		 * Estado 2: Byte de Escape
		*/

		case 0:
			if(!strcmp(&byte,&Flag))
			{
				//printf("\n INICIO DA MENSAGEM"\n);
				ProximoEstado = 1;
			}
			break;

		case 1:
			if(!strcmp(&byte,&Flag_Escape))
			{
				ProximoEstado = 2;// Estado so mudara na proxima chamada

			}
			else if(!strcmp(&byte,&Flag))// fim
			{

				ProximoEstado = 0;
			}
			else
			{
				buffer[n_bytes]=byte;
				n_bytes++;
			}
			break;

		case 2:
			byte ^= 0x20;
			if(!strcmp(&byte,&Flag))
			{
				ProximoEstado = 0;
				// chamar checksum?
				// salvar menasgem
			}
			else
			{
				buffer[n_bytes]=byte;
				n_bytes++;
				ProximoEstado = 1;
			}
			break;

		default:
			break;

	}
	if(!ProximoEstado && n_bytes)
	{
		return true;
	}
	else
	{
		return false;
	}
}
int Framing::receive(char *BufferRecepcao)
{


	int i;
	int TamanhoMensagem = strlen(BufferRecepcao);
	if (TamanhoMensagem > max_bytes || TamanhoMensagem < min_bytes)
	{
		printf( "Quadro invalido");
	}
	for(i=0;TamanhoMensagem;i++)
	{
		 if(handle(BufferRecepcao[i]))
		 {
			 printf( "Fim do quadro");
			 /*fazer checksum
			  * if(sucesso)
			  * Salvar Buffer
			  * else
			  * Descartar mensagem , memset(buffer,0,sizeof(buffer));

			 */
			 break;
		 }
	}

    return TamanhoMensagem;

}

