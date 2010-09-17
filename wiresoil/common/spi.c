#include "pic24_all.h"
#include "pic24_ports.h"
#include "vdip.h"
#include <stdio.h>

/***********************************************************
 *  Pin definitions for SPI
 **********************************************************/

#define PORT_SDI   _LATB2	// SDI (on VDIP1) is RB6
#define PORT_SCLK  _LATB1	// SCLK is RB5
#define PORT_SDO   _RB3		// SDO (on VDIP1) is RB7
#define PORT_CS    _LATB6	// CS is RB2


/***********************************************************
 * Constants and variables for SPI
 **********************************************************/

#define DIR_SPIWRITE 0
#define DIR_SPIREAD  1


//**********************************************************
/**
 * @brief Uses a nop to create a very short delay.
 */
//**********************************************************
#define spiDelay() \
 asm("nop");\
 asm("nop");


//**********************************************************
/**
 * @brief Transfer data to/from the VDIP
 * @param[in] int The direction of the transfer
 * @param[out] char* The data to send or a buffer to fill
 */
//**********************************************************
int SPI_Xfer(int spiDirection, char *pSpiData)
{
	unsigned char retData,
				  bitData;

	// Clock 1 - Start State
	PORT_SDI = 1;
	PORT_CS = 1;

	spiDelay();
	PORT_SCLK = 1;
	spiDelay();
	PORT_SCLK = 0;

	// Clock 2 - Direction
	PORT_SDI = spiDirection;

	spiDelay();
	PORT_SCLK = 1;
	spiDelay();
	PORT_SCLK = 0;

	// Clock 3 - Address
	PORT_SDI = 0;

	spiDelay();
    PORT_SCLK = 1;
	spiDelay();
	PORT_SCLK = 0;

	// Clocks 4..11 - Data Phase
	bitData = 0x80;
	switch(spiDirection)
	{
		// read operation
		case DIR_SPIREAD:
			retData = 0;
			spiDelay();
			retData |= PORT_SDO?0x80:0;
			PORT_SCLK = 1;
			spiDelay();
			PORT_SCLK = 0;

			spiDelay();
			retData |= PORT_SDO?0x40:0;
			PORT_SCLK = 1;
			spiDelay();
			PORT_SCLK = 0;

			spiDelay();
			retData |= PORT_SDO?0x20:0;
			PORT_SCLK = 1;
			spiDelay();
			PORT_SCLK = 0;

			spiDelay();
			retData |= PORT_SDO?0x10:0;
			PORT_SCLK = 1;
			spiDelay();
			PORT_SCLK = 0;

			spiDelay();
			retData |= PORT_SDO?0x08:0;
			PORT_SCLK = 1;
			spiDelay();
			PORT_SCLK = 0;

			spiDelay();
			retData |= PORT_SDO?0x04:0;
			PORT_SCLK = 1;
			spiDelay();
			PORT_SCLK = 0;

			spiDelay();
			retData |= PORT_SDO?0x02:0;
			PORT_SCLK = 1;
			spiDelay();
			PORT_SCLK = 0;

			spiDelay();
			retData |= PORT_SDO?0x01:0;
			PORT_SCLK = 1;
			spiDelay();
			PORT_SCLK = 0;

			*pSpiData = retData;
			break;

		// write operation
		case DIR_SPIWRITE:
			retData = *pSpiData;

			while (bitData)
			{
				PORT_SDI = (retData & bitData)?1:0;
				spiDelay();
				PORT_SCLK = 1;
				spiDelay();
				PORT_SCLK = 0;
				bitData >>= 1;
			}
			break;
	}

	spiDelay();
	bitData = PORT_SDO;			//0 = new data read/data recieved
								//1 = old data read/data not received
	if(bitData == 1)
	{
		DELAY_US(10);
	}

	PORT_SCLK = 1;
	spiDelay();
	PORT_SCLK = 0;

	// CS goes low to disable SPI communications
	PORT_CS = 0;
	spiDelay();

	// Clock 13 - CS low
	spiDelay();
	PORT_SCLK = 1;
	spiDelay();
	PORT_SCLK = 0;

    return bitData;
}


//**********************************************************
/**
 * @brief Initialize the pins for the SPI interface
 */
//**********************************************************
void SPI_Init(void)
{

	// Set up digital pins
	CONFIG_RB1_AS_DIG_OUTPUT();
	CONFIG_RB2_AS_DIG_OUTPUT();
	CONFIG_RB3_AS_DIG_INPUT();
	CONFIG_RB6_AS_DIG_OUTPUT();

	// Configure initial pin states
	PORT_SDI = 0;
	PORT_SCLK = 0;
	PORT_CS = 0;
}


//**********************************************************
/**
 * @brief Wait till one character is read and then return it
 * @return char The received character
 */
//**********************************************************
char SPI_ReadWait(void)
{
	char spiData;
	
	while (SPI_Xfer(DIR_SPIREAD, &spiData)){
		printf("%c",spiData);
	}

    // If the new line isn't added, then the
    // characters just overwrite each other, and
    // since a space is the last character before
    // the EOS, none of the output shows up.
    if(spiData == 0x0d)
    {
        spiData = '\n';
    }

    return spiData;
}


//**********************************************************
/**
 * @brief Non-blocking read of one character from SPI bus
 * @return char The received character
 */
//**********************************************************
char SPI_Read(char *pSpiData)
{
	return SPI_Xfer(DIR_SPIREAD, pSpiData);
}

//**********************************************************
/**
 * @brief Blocking write of character to SPI bus
 * @param[in] char The byte to be transmitted
 */
//**********************************************************
void SPI_Write(char spiData)
{
	while(SPI_Xfer(DIR_SPIWRITE, &spiData));
}


//**********************************************************
/**
 * @brief Send a string of characters to the SPI bus
 * @param[in] const char* The string to send
 */
//**********************************************************
void SPI_WriteStr(const char *spiData)
{
    while(*spiData)
    {
        SPI_Write(*(spiData++));
    }    
    
	// Carriage Return - every command needs one.
	SPI_Write(0x0d);
}
