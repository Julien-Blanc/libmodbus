#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "modbus.h"

#include "default_mapping.h"

int default_set_single_register(int address, void *tab_registers, int max_regs,
                                int data)
{
   uint16_t * regs = (uint16_t *)tab_registers;
   if (address >= max_regs)
      return -1;
   regs[address] = data;
   return 0;
}

int default_set_multiple_registers(int address, int nb,
                                   void *tab_registers, int max_regs,
                                   const uint8_t *data)
{
   int i, j;
   uint16_t * regs = (uint16_t *)tab_registers;
   if (address + nb > max_regs)
      return -1;
   for (i = address, j = 0; i < address + nb; i++, j += 2) {
      regs[i] = (data[j] << 8) + data[j + 1];
   }
   return 0;

}

int default_set_multiple_io_status(int address, int nb,
                                   void *coils, int max_coil,
                                   const uint8_t *data)
{
   if (address + nb > max_coil)
      return -1;
   modbus_set_bits_from_bytes(coils, address, nb, data);
   return 0;
}

int default_set_io_status(int address, void *coils, int max_coil,
                           const uint16_t data)
{
   if (address >= max_coil)
      return -1;
   ((uint8_t *)coils)[address] = (data) ? ON : OFF;
   return 0;
}

int default_response_registers(int address, int nb,
                           void *tab_registers, int max_regs,
                           uint8_t *rsp, int offset)
{
   int i;
   uint16_t *regs = (uint16_t *)tab_registers;
   if ((address + nb) > max_regs)
      return -1;
   else {
      for (i = address; i < address + nb; i++) {
         rsp[offset++] = regs[i] >> 8;
         rsp[offset++] = regs[i] & 0xFF;
      }
   }
   return offset;
}


int default_response_io_status(int address, int nb,
                               uint8_t *tab_io_status, int max_io_status,
                               uint8_t *rsp, int offset)
{
    int shift = 0;
    /* Instead of byte (not allowed in Win32) */
    int one_byte = 0;
    int i;
    if (address + nb > max_io_status)
       return -1;

    for (i = address; i < address+nb; i++) {
        one_byte |= tab_io_status[i] << shift;
        if (shift == 7) {
            /* Byte is full */
            rsp[offset++] = one_byte;
            one_byte = shift = 0;
        } else {
            shift++;
        }
    }

    if (shift != 0)
        rsp[offset++] = one_byte;

    return offset;
}

/*
   Default Init functions
*/
void * default_init_bits(int nb_bits)
{
   /* Negative number raises a POSIX error */
   return (uint8_t *) malloc(nb_bits * sizeof(uint8_t));
}


void * default_init_registers(int nb_registers)
{
   return (uint16_t *) malloc(nb_registers * sizeof(uint16_t));
}

/*
   Default free.
*/
void default_free(void * addr)
{
   free(addr);
}


