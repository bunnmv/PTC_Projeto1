#ifndef FRAMING_H_
#define FRAMING_H_

#include "Serial.h"
#include "crc16.h"
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>



class Framing : public Serial
{

public:
    //Framing(Serial &s):serial(s){}// passagem parametro por referencia
	Framing(){};
	char * gen_crc(char * buf, int len);
	int insertFlagFrame(char *buffer,int len);
	int insertStuffByte(char *buffer,int len);
	
	// monta o frame:
	bool mountFrame(char *buffer,int len,int typeMsg, int seqNum, int ackNum);
	bool arq_tx(char *buffer, int len,int estado);
	Framing(int bytes_min, int bytes_max){
		//porta = &dev;
		min_bytes = bytes_min;
		max_bytes = bytes_max;
		buffer = (char*)(malloc(bytes_max));
		n_bytes = 0;
		Estado = 0;
	};
	void send(char * buffer, int bytes);
       int receive(char * BufferRecepcao);
    ~Framing(){}


private:
	int min_bytes, max_bytes;
	Serial *porta;
	char * buffer; // deve ser dimensionado na instanciação

	// bytes recebidos pela MEF até o momento
	int n_bytes;

	// estado atual da MEF
	int Estado;

	// aqui se implementa a máquina de estados de recepção
	// retorna true se reconheceu um quadro completo
	bool handle(char byte);

	// verifica o CRC do conteúdo contido em "buffer". Os dois últimos
	// bytes desse buffer contém o valor de CRC
	bool check_crc(char * buffer, int len);

	// gera o valor de CRC dos bytes contidos em buffer. O valor de CRC
	// é escrito em buffer[len] e buffer[len+1]

	int insertControlByte(char * buffer, int len, int typeMsg, int seqNum, int ackNum);

	// calcula o valor de CRC dos bytes contidos em "cp".
	// "fcs" deve ter o valor PPPINITFCS16
	// O resultado é o valor de CRC (16 bits)
	// OBS: adaptado da RFC 1662 (enquadramento no PPP)
	u16 pppfcs16(char * cp, int len);

	bool arq_tx(char *buffer,int len);
	
	bool arq_rx(char* buffer);
};


#endif /* FRAMING_H_ */
