
#include "Framing.h"
#include "Serial.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <cstdio>
#include <ctime>

#define msgTypeACK 0;
#define msgTypePayload 1;
#define numSeq0 0;
#define numSeq1 1;
#define ACK_0 0;
#define ACK_1 1;
#define TypeACK 0;
#define TypePayload 1;

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
				std::cout << "Inicio Da Mensagem!" << std::endl;
				n_bytes = 0;
				Estado =1;
			}
			break;

		case 1:
			if(n_bytes > max_bytes)
			{
				std::cout << "Overflow: " << this->n_bytes << " bytes" << std::endl;
				Estado = 0;
			}
			else
			{

				if(!strcmp(&byte,&Flag))// fim
				{
					std::cout << "Fim Da Mensagem" << std::endl;
					Estado = 0;
					return true;
				}
				else if(byte==Flag_Escape)
				{
					Estado = 2;

				}
				else
				{
					buffer[n_bytes]=byte;
					n_bytes++;
				}
			}
			break;

		case 2:
			if(byte == Flag)
			{
				std::cout << "Fim Da Mensagem" << std::endl;
				Estado = 0;
				return true;
			}
			else
			{
				byte ^= 0x20;
				buffer[n_bytes]=byte;
				n_bytes++;
				Estado = 1;
			}
			break;

		default:
			break;

	}
}
int Framing::receive(char *BufferRecepcao)
{

	int TamanhoMensagem = strlen(BufferRecepcao);
	bool ControleMaquina = false;
	if (TamanhoMensagem > max_bytes || TamanhoMensagem < min_bytes)
	{
		std::cout << "Quadro Invalido" << std::endl;
	}
	for(int i=0;i<TamanhoMensagem && !ControleMaquina;i++)
	{
		 ControleMaquina = handle(BufferRecepcao[i]);

	}

	if(this->check_crc(buffer,n_bytes))
	 {
		memcpy(buffer, this->buffer,TamanhoMensagem);
		std::cout << "Data received: " << buffer  << std::endl;
		}
	 else
	 {
		std::cout << "CRC does not match!: " << buffer  << std::endl;
	 }

	return n_bytes;
}

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
			 *fazer checksum
			  * if(sucesso)
			  * Salvar Buffer
			  * else
			  * Descartar mensagem , memset(buffer,0,sizeof(buffer));

			 break;
		 }
	}

    return TamanhoMensagem;


}
*/
bool Framing::check_crc(char *buffer, int len) // checa o resultado do crc na recepcao
{
	if ( pppfcs16(buffer, len + 2) == PPPGOODFCS16 )
		return true;
	else return false;
}
char * Framing::gen_crc(char * buffer, int len) //Adiciona crc a mensagem
{
	u16 trialfcs;

	/* add on output */
	trialfcs = pppfcs16(buffer, len );
	trialfcs ^= 0xffff;                 	/* complement */
	buffer[len] = (trialfcs & 0x00ff);      /* least significant byte first */
	buffer[len+1] = ((trialfcs >> 8) & 0x00ff);
	return buffer;

}
u16 Framing::pppfcs16( char * cp, int len) // Fast Frame Check Sequence 16 bits
{
	crc fcs = PPPINITFCS16; // Initial value

	//assert(sizeof (u16) == 2);
	//assert(((u16) -1) > 0);
	//u16 fcs=PPINITFCS16
	assert(sizeof (u16) == 2);
	assert(((u16) -1) > 0);
	while (len--)
	   fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
	return (fcs);
}

bool Framing::arq_tx(char * buffer, int len, int estado)
{
	// Pacote recebido é o ponto de partida para validar o processo de arq			
	// receive(buffer); 
	// testa se o frame recebido é um pacote válido (marcus)
	// testa se o frama recebido está com o crc correto (marcus) e retorna apenas o byte de informação.
	//-----------------------
	// testa se o frame recebido é um ack (ronaldo) - OK
	// testa se o ack recebido é o correto (ronaldo) - OK
	// passa para o proximo estado e desliga o timer (ronaldo) - OK
	clock_t start;
	double timeout = 3; //aguarda 3 segundos até confirmar que o ack foi perdido
	static int PE  = estado;
 	int ret_TX     = 3; // retransmissões máximas
 	int iRet       = 0 //contador de retransmissão. R
	char ack0      = '0';
	char ack1      = '1';
	switch(PE)
	{
		case 0:
			//liga o time pois o pacote foi enviado neste instante
			if(iRet < ret_TX){
				start = clock();
				this->send(buffer, len);
				PE = 1;
				iRet++;
			}else{
				// estouro numero de retrnsmissão. O que fazer?			
			}
		break;
		case 1:
			while((( std::clock() - start ) / (double) CLOCKS_PER_SEC) < timeout){
	
				 if(strlen(info) == strlen(char)){
    				 	if(info == ack1){
				 	        //byte ACK1 reconhecido
                        			PE = 2; // irá para o estado 2 para enviar próximo frame.
                     			}else{
                        			//byte ACK1 não reconhecido. Retransmissão.
                        			PE = 0; //volta para o estado 0 para reenvio.
                     			}
                       		}else{
			 		PE = 0;//ack não foi reconhecido. ir para estado 0 de renstramissão
				}
		    	}
            		//  Estouro do timeout. ir para estado de retransmissão 
            		PE = 0;

					
					
			//espera o ack0
		break;
	} 

        case 2:
            //envio do proximo frame.

        break;
	return false;
//retorna mensagem true;
}
int Framing::insertStuffByte(char * buffer, int len){
	int i,ii;
	for(i = 0,ii = 0; i <= len; i++, ii++){
		if(buffer[i] == Flag){
			for(ii = len; ii > i; ii--){
				buffer[ii] = buffer[ii - 1];
			}
			buffer[i]   = Flag_Escape;
			buffer[i + 1] = 0x5e; //Flag XOR 0x20
			
		}else if(buffer[i] == Flag_Escape){
			for(ii = len;ii > i; ii--){
				buffer[ii] = buffer[ii - 1];
			}			
			buffer[i]   = Flag_Escape;
			buffer[i + 1] = 0x5d; //Flag_Escape XOR 0x20
		}else{
		}
	}
	return strlen(buffer);
}
int Framing::insertFlagFrame(char * buffer, int len){
	int i;	
	for (i = len; i > 0; i--){
		buffer[i] = buffer[i - 1];
	}
	buffer[0] = Flag;
	buffer[len + 1] = Flag;
	return strlen(buffer);
}

int Framing::insertControlFrame(char * buffer, int len, int typeMsg, int seqNum, int ackNum){
//insere tipo da mensagem
//insere numero de sequencia
//insere numero ack
// byte control sera o segundo byte do frame.
//EXEMPLO : tipo payload, sequencia 0, ack 0. 
// campo 8 do bytecontrol = Tipo da mensagem.
// campo 7 do bytecontrol = numero de seq.
// campo 6 do byte control = numero ack.
// campo 5 ~ 1 = reservado.
	char bytecontrol = 0x00;
	int i;	
	if(typeMsg == typeACK){
		bytecontrol |= ( typeACK << 7)
	}else{
		bytecontrol |= ( typePayload << 7)
	}
	
	if(seqNum == numSeq0){
		bytecontrol |= (numSeq0 << 6)
	}else{
		bytecontrol |= (numSeq1 << 6)
	}
	
	if(ackNum == ACK_0){
		bytecontrol |= (ACK_0 << 5)
	}else{
		bytecontrol |= (ACK_1 << 5)
	}
	
	for (i = len; i > 0; i--){
		buffer[i] = buffer[i - 1];
	}
	buffer[0] = bytecontrol;
	return strlen(buffer);
	
}
bool Framing::mountFrame(char * buffer, int len,int typeMsg, int seqNum, int ackNum){
	// A função mount frame precisa receber o ponteiro do payload(dados), respectivo tamanho, o tipo de mensagem, numero
	// sequencia e ack.
	int l; 
	buffer = gen_crc(buffer, len);
	l = strlen(buffer);
	l = insertStuffByte(buffer, l);
	l = insertControlByte(buffer, l, typeMsg, seqNum, ackNum);
	l = insertFlagFrame(buffer, l);


}
