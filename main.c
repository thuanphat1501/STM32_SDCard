
#include "main.h"
#include "fatfs.h"
#include <string.h>
#include <stdio.h>
#include "fatfs_sd.h"
#include "ff.h"
#include "delay.h"
#include "spi.h"
#include "uart.h"
#include "gpio.h"
#ifdef __GNUC__
 /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
 #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
 #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


FATFS fs;  // file system
FIL fil; // File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;

#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE];  // to store strings..

int i=0;

int bufsize (char *buf)
{
	int i=0;
	while (*buf++ != '\0') i++;
	return i;
}

void clear_buffer (void)
{
	for (int i=0; i<BUFFER_SIZE; i++) buffer[i] = '\0';
}

void send_uart(char *string) {
    uint8_t len = strlen(string);
    for (uint8_t i = 0; i < len; i++) {
        USART_SendData(USART1, string[i]); 
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); 
    }
}

int main(void)
{
  GPIO_Config();
	SPI_Config();
	UART_Config();
  MX_FATFS_Init();
 
  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
  Delay_ms (500);
  fresult = f_mount(&fs, "/", 1);
  	if (fresult != FR_OK){
  		printf ("ERROR with code:%d!!! in mounting SD CARD...\n\n",fresult);
  	}
  	else{
  		send_uart("SD CARD mounted successfully...\n\n");


  	/*************** Card capacity details ********************/

  	/* Check free space */
  	f_getfree("", &fre_clust, &pfs);

  	total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
  	sprintf (buffer, "SD CARD Total Size: \t%lu\n",total);
  	send_uart(buffer);
  	clear_buffer();
  	free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
  	sprintf (buffer, "SD CARD Free Space: \t%lu\n\n",free_space);
  	send_uart(buffer);
  	clear_buffer();



  	/************* The following operation is using PUTS and GETS *********************/

  	/* Open file to write/ create a file if it doesn't exist */
      fresult = f_open(&fil, "file1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

  	/* Writing text */
  	f_puts("This data is from the FILE1.txt. And it was written using ...f_puts... ", &fil);

  	/* Close file */
  	fresult = f_close(&fil);

  	if (fresult == FR_OK)send_uart ("File1.txt created and the data is written \n");

  	/* Open file to read */
  	fresult = f_open(&fil, "file1.txt", FA_READ);

  	/* Read string from the file */
  	f_gets(buffer, f_size(&fil), &fil);

  	send_uart("File1.txt is opened and it contains the data as shown below\n");
  	send_uart(buffer);
  	send_uart("\n\n");

  	/* Close file */
  	f_close(&fil);

  	clear_buffer();




  	/**************** The following operation is using f_write and f_read **************************/

  	/* Create second file with read write access and open it */
  	fresult = f_open(&fil, "file2.txt", FA_CREATE_ALWAYS | FA_WRITE);

  	/* Writing text */
  	strcpy (buffer, "This is File2.txt, written using ...f_write... and it says Hello from Controllerstech\n");

  	fresult = f_write(&fil, buffer, bufsize(buffer), &bw);

  	send_uart ("File2.txt created and data is written\n");

  	/* Close file */
  	f_close(&fil);



  	// clearing buffer to show that result obtained is from the file
  	clear_buffer();

  	/* Open second file to read */
  	fresult = f_open(&fil, "file2.txt", FA_READ);
  	if (fresult == FR_OK)send_uart ("file2.txt is open and the data is shown below\n");

  	/* Read data from the file
  	 * Please see the function details for the arguments */
  	f_read (&fil, buffer, f_size(&fil), &br);
  	send_uart(buffer);
  	send_uart("\n\n");

  	/* Close file */
  	f_close(&fil);

  	clear_buffer();


  	/*********************UPDATING an existing file ***************************/

  	/* Open the file with write access */
  	fresult = f_open(&fil, "file2.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);

  	/* Move to offset to the end of the file */
  	fresult = f_lseek(&fil, f_size(&fil));

  	if (fresult == FR_OK)send_uart ("About to update the file2.txt\n");

  	/* write the string to the file */
  	fresult = f_puts("This is updated data and it should be in the end", &fil);

  	f_close (&fil);

  	clear_buffer();

  	/* Open to read the file */
  	fresult = f_open (&fil, "file2.txt", FA_READ);

  	/* Read string from the file */
  	fresult = f_read (&fil, buffer, f_size(&fil), &br);
  	if (fresult == FR_OK)send_uart ("Below is the data from updated file2.txt\n");
  	send_uart(buffer);
  	send_uart("\n\n");

  	/* Close file */
  	f_close(&fil);

  	clear_buffer();


  	/*************************REMOVING FILES FROM THE DIRECTORY ****************************/

  	fresult = f_unlink("/file1.txt");
  	if (fresult == FR_OK) send_uart("file1.txt removed successfully...\n");

  	fresult = f_unlink("/file2.txt");
  	if (fresult == FR_OK) send_uart("file2.txt removed successfully...\n");

  	/* Unmount SDCARD */
  	fresult = f_mount(NULL, "/", 1);
  	if (fresult == FR_OK) send_uart ("SD CARD UNMOUNTED successfully...\n");
  	}
  
  while (1)
  {
    
  }

}

PUTCHAR_PROTOTYPE
{
	USART_SendData(USART1,(uint8_t)ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
 return ch;
}
