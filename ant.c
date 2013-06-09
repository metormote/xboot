/************************************************************************/
/* XBoot Extensible AVR Bootloader                                      */
/*                                                                      */
/* ANT Module                                      .                    */
/*                                                                      */
/* ant.c                                          .                     */
/*                                                                      */
/* Erik Rosen <erik.rosen@iop.io>.........                              */
/*                                                                      */
/* Copyright (c) 2013 Erik Rosen....                                    */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files(the "Software"), to deal in the Software without restriction,  */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/

#include <util/delay.h>
#include "ant.h"

static void ant_send(uint8_t cmd, uint8_t* data, uint8_t size);
static void ant_receive(uint8_t *cmd, uint8_t* data, uint8_t *size);

void ant_init(uint16_t device_no)
{
  uint8_t cmd, len, i;
  uint8_t msg[16];
  for(i=0;i<16;i++) msg[i]=0;
  
  ANT_UART_PORT.DIRSET = (1 << ANT_UART_TX_PIN);
  ANT_UART_DEVICE.BAUDCTRLA = (ANT_UART_BSEL_VALUE & USART_BSEL_gm);
  ANT_UART_DEVICE.BAUDCTRLB = ((ANT_UART_BSCALE_VALUE << USART_BSCALE_gp) & USART_BSCALE_gm) | ((ANT_UART_BSEL_VALUE >> 8) & ~USART_BSCALE_gm);
  #if ANT_UART_CLK2X
  ANT_UART_DEVICE.CTRLB = USART_RXEN_bm | USART_CLK2X_bm | USART_TXEN_bm;
  #else
  ANT_UART_DEVICE.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
  #endif // UART_CLK2X
  
  
  ANT_SLEEP_PORT.DIRSET = (1 << ANT_SLEEP_PIN);
  ANT_SLEEP_PORT.OUTCLR = (1 << ANT_SLEEP_PIN);
  
  ANT_SUSPEND_PORT.DIRSET = (1 << ANT_SUSPEND_PIN);
  ANT_SUSPEND_PORT.OUTSET = (1 << ANT_SUSPEND_PIN);
  
  ANT_RESET_PORT.DIRSET = (1 << ANT_RESET_PIN);
  ANT_RESET_PORT.OUTSET = (1 << ANT_RESET_PIN);
  
  ant_receive(&cmd, msg, &len);
  
  //enable crystal message
  msg[0]=0x00;// filler byte
  ant_send(0x6D, msg, 1);

  //read channel response
  ant_receive(&cmd, msg, &len);
  
  //assign channel
  msg[0]=0x00; //channel no
  msg[1]=0x00;  //channel type 0x00=slave 0x10=master
  msg[2]=0x0;  //network number
  msg[3]=0x0;  //extended assignment
  ant_send(0x42, msg, 4);
  
  //read assign channel response
  ant_receive(&cmd, msg, &len);
  
  //set channel id
  msg[0]=0x00; //channel no
  //msg[1]=(uint8_t)device_no;  //device no lsb
  //msg[2]=(uint8_t)(device_no >> 8);  //device no msb
  msg[1]=0x0;  //device no lsb
  msg[2]=0x0;  //device no msb
  msg[3]=0x0;  //device type
  msg[4]=0x0;  //trans type
  ant_send(0x51, msg, 5);
  
  //read set channel id response
  ant_receive(&cmd, msg, &len);
  
  //set channel freq
  msg[0]=0x00;  //channel no
  msg[1]=0x58;  //freq 2488MHz
  ant_send(0x45, msg, 2);
  
  //read set channel freq response
  ant_receive(&cmd, msg, &len);
  
  //set tx power
  msg[0]=0x00;  //channel no
  msg[1]=0x03;  //power 3=0dB
  ant_send(0x60, msg, 2);
  
  //read set tx power response
  ant_receive(&cmd, msg, &len);
  
  //set channel period
  msg[0]=0x00; //channel no
  msg[1]=0x00;  //period lsb
  msg[2]=0x10;  //period msb
  ant_send(0x43, msg, 3);
  
  //read set period response
  ant_receive(&cmd, msg, &len);
  
  //set channel search timeout
  msg[0]=0x00; //channel no
  msg[1]=0x20;  //timeout 32*2.5 sec
  ant_send(0x44, msg, 2);
  
  //read set channel timeout response
  ant_receive(&cmd, msg, &len);
  
  //set channel low prio search timeout
  msg[0]=0x00; //channel no
  msg[1]=0x00;  //no low prio search
  ant_send(0x63, msg, 2);
  
  //read set channel timeout response
  ant_receive(&cmd, msg, &len);
  
  //open channel
  msg[0]=0x00; //channel no
  ant_send(0x4B, msg, 1);
  
  //read open channel response
  ant_receive(&cmd, msg, &len);
}


void ant_deinit(void)
{
  uint8_t cmd, len, i;
  uint8_t msg[16];
  for(i=0;i<16;i++) msg[i]=0;
  
  //close channel
  msg[0]=0x00; //channel no
  ant_send(0x4C, msg, 1);
    
  //read close channel response
  ant_receive(&cmd, msg, &len);
  
  //unassign channel
  msg[0]=0x00; //channel no
  ant_send(0x41, msg, 1);
  
  //read unassign channel response
  ant_receive(&cmd, msg, &len);
  
  ANT_UART_DEVICE.CTRLB = 0;
  ANT_UART_DEVICE.BAUDCTRLA = 0;
  ANT_UART_DEVICE.BAUDCTRLB = 0;
  ANT_UART_PORT.DIRCLR = (1 << ANT_UART_TX_PIN);
  
  ANT_RESET_PORT.DIRCLR = (1 << ANT_RESET_PIN);
  ANT_RESET_PORT.OUTCLR = (1 << ANT_RESET_PIN);
}


static void ant_send(uint8_t cmd, uint8_t* data, uint8_t size)
{
  uint8_t msg[size+4];
  uint8_t cs = 0;
  uint8_t *p  = msg;
  
  msg[0]=0xA4;
  msg[1]=size;
  msg[2]=cmd;
  for (int i=0;i<size;i++)
  {
    msg[i+3]=data[i];
  }
  //calc checksum
  for (int i=0;i<size+3;i++)
  {
    cs ^= msg[i];
  }
  msg[size+3]=cs;
  
  // put data...
  for (int i=0;i<size+4;i++)
  {
    // wait for rts to be deasserted...
    while(ANT_RTS_PORT.IN & (1 << ANT_RTS_PIN)) {}
    ant_uart_send_char_blocking(*p);
    p++;
  }
}


static void ant_receive(uint8_t *cmd, uint8_t* data, uint8_t *size)
{
  uint8_t *p  = data;
  uint8_t c, i = 0;
  
  // get data...
  for (;;)
  {
    // wait for rx complete
    while (!ant_uart_char_received()) {}
    c = ant_uart_cur_char();
    
    switch(i) {
      case 0:
        if(c!=0xA4)
          continue;
        break;
      case 1:
        *size = c;
        break;
      case 2:
        *cmd=c;
        break;
      default:
        if(i==(*size)+3) {
          //cs=c;
          return;
        }
        else {
          *p++ = c;
        }
    }
    i++;
  }
}

  
  void ant_load_firmware(void)
{
  uint8_t cmd, len, last;
  uint8_t data[16], buffer[SPM_PAGESIZE], fw_command[6];
  uint8_t *ptr, *seg;
  uint16_t l, crc, crc2;
  uint32_t adr;
  
  // read last block
  Flash_ReadFlashPage(buffer, APP_SECTION_START + APP_SECTION_SIZE - SPM_PAGESIZE);
  // check for install command
  if (buffer[SPM_PAGESIZE-6] == 'X' && buffer[SPM_PAGESIZE-5] == 'B' &&
    buffer[SPM_PAGESIZE-4] == 'I' && buffer[SPM_PAGESIZE-3] == 'A')
  {
    //store the firmware upgrade command
    for(int i=0;i<6;i++) {
      fw_command[i]=buffer[SPM_PAGESIZE-1-i];
    }
    crc = (buffer[SPM_PAGESIZE-2] << 8) | buffer[SPM_PAGESIZE-1];
    
    for(;;) {
      adr = APP_SECTION_START;
      seg = buffer;
      ptr = seg;
      last=0;
    
      ant_init(crc);
    
      while(!last) {
        for(;;) {
          ant_receive(&cmd, data, &len);
          switch(cmd) {
            case 0x50:
              //check if its the first frame in the burst
              if((data[0] & 0xE0)==0) {
                ptr=seg;
                //check if its the last segment
                if(data[1] & 0x80) {
                  last=1;
                }
                l=(data[3] << 8) | data[2];
              }
              else {
                for(int i=1;i<len;i++) {
                  *ptr=data[i];
                  ptr++;
                }
              }
          
              //check if its the last frame in the burst
              if(data[0] & 0x80) {
                //set extra data to 0xff
                for(int i=0;i<(l % 8);i++) {
                  *(--ptr)=0xff;
                }
                seg=ptr;
                goto burst_done;
              }
              break;
            case 0x40:
              if(data[1]==1) {
                switch(data[2]) {
                  //search timeout
                  case 1:
                  //channel closed
                  case 7:
                  //rx fail go to search
                  case 8:
                    goto rx_done;
                    break;
                }
              }
          }
        }
      
        burst_done:
      
        if(seg-buffer>=SPM_PAGESIZE || last) {
          #ifdef USE_LED
          LED_PORT.OUTTGL = (1 << LED_PIN);
          #endif // USE_LED
        
          //fill out the last segment with 0xff
          if(last) {
            for(int i=ptr-buffer;i<SPM_PAGESIZE;i++) {
              *ptr=0xff;
              ptr++;
            }
          }
          // if it's the last page, restore the firmware upgrade command
          if (adr >= APP_SECTION_START + APP_SECTION_SIZE - SPM_PAGESIZE)
          {
            for(int i=0;i<6;i++) {
              buffer[SPM_PAGESIZE-1-i]=fw_command[i];
            }
          }
          Flash_ProgramPage(adr, buffer, 1);
          adr+=SPM_PAGESIZE;
          seg = buffer;
        }
      }
    
      for(int i=0;i<SPM_PAGESIZE;i++) {
        buffer[i]=0xff;
      }
      for (; adr < APP_SECTION_START + APP_SECTION_SIZE; adr += SPM_PAGESIZE)
      {
        // if it's the last page, restore the firmware upgrade command
        if (adr >= APP_SECTION_START + APP_SECTION_SIZE - SPM_PAGESIZE)
        {
          for(int i=0;i<6;i++) {
            buffer[SPM_PAGESIZE-1-i]=fw_command[i];
          }
        }
        Flash_ProgramPage(adr, buffer, 1);
      }
    
      rx_done:
    
      ant_deinit();
    
      #ifdef USE_LED
      LED_PORT.OUTSET = (1 << LED_PIN);
      #endif // USE_LED
    
      // skip last 6 bytes as they are the install command
      crc2 = crc16_block(APP_SECTION_START, APP_SECTION_SIZE - 6);
    
      // crc last 6 bytes as empty
      for (int i = 0; i < 6; i++)
        crc2 = _crc16_update(crc2, 0xff);
    
      if (crc == crc2)
      {
        for (int i = SPM_PAGESIZE-6; i < SPM_PAGESIZE; i++)
            buffer[i] = 0xff;
        Flash_ProgramPage(APP_SECTION_START + APP_SECTION_SIZE - SPM_PAGESIZE, buffer, 1);
      
        break;
      }
    }
  }
}