
#include "Framing.h"
#include "Serial.h"
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;
char Flag = 0x7e;
char Flag_Escape = 0x7d;
typedef unsigned short  crc;

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
			if(byte == Flag)
			{
				//printf("\n INICIO DA MENSAGEM"\n);
				ProximoEstado = 1;
			}
			break;

		case 1:
			if(byte==Flag_Escape)
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
			if(byte == Flag)
			{
				ProximoEstado = 0;
				// chamar checksum?
				// salvar menasgem
			}
			else
			{
				byte ^= 0x20;
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

	int TamanhoMensagem = strlen(BufferRecepcao);
	if (TamanhoMensagem > max_bytes || TamanhoMensagem < min_bytes)
	{
		printf( "Quadro invalido");
	}
	for(int i=0;TamanhoMensagem;i++)
	{
		 if(handle(BufferRecepcao[i]))
		 {
			 printf( "Fim do quadro");
			 /*
			 if(check_crc(buffer,TamanhoMensagem))
			 {
				 //salvar
			 }
			 else
			 {
				 printf( "ERRO CRC");
				 memset(buffer,0,sizeof(unsigned char)*max_bytes);
				 free(buffer);

			 }
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
bool Framing::check_crc(unsigned char *buffer, int len) // checa o resultado do crc na recepcao
{
	if ( pppfcs16(buffer, len + 2) == PPPGOODFCS16 )
		return true;
	else return false;
}
void Framing::gen_crc(unsigned char * buffer, int len) //Adiciona crc a mensagem
{
	u16 trialfcs;

	/* add on output */
	trialfcs = pppfcs16(buffer, len );
	trialfcs ^= 0xffff;                 	/* complement */
	buffer[len] = (trialfcs & 0x00ff);      /* least significant byte first */
	buffer[len+1] = ((trialfcs >> 8) & 0x00ff);

}
u16 Framing::pppfcs16( unsigned char * cp, int len) // Fast Frame Check Sequence 16 bits
{
	crc fcs = PPPINITFCS16; // Initial value

	assert(sizeof (u16) == 2);
	assert(((u16) -1) > 0);
	while (len--)
	   fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
	return (fcs);
}


