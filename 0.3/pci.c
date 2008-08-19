/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * 
 *
 * $Id: pci.c,v 1.2 2006-07-12 14:11:31 serpilliere Exp $
 *
 */


#include "vars.h"
#include "pci.h"

#include "utils.h"


#include "pci_data.h"

/*
#define rr0d_outb outb
#define rr0d_outw outw
#define rr0d_outl outl

#define rr0d_inb inb
#define rr0d_inw inw
#define rr0d_inl inl
*/

/*******************************/
#define PCI_CONF_TYPE_NONE 0
#define PCI_CONF_TYPE_1    1
#define PCI_CONF_TYPE_2    2

static unsigned char pci_conf_type = PCI_CONF_TYPE_1;//PCI_CONF_TYPE_NONE;


#define PCI_CONF1_ADDRESS(bus, dev, fn, reg) \
	(0x80000000 | (bus << 16) | (dev << 11) | (fn << 8) | (reg & ~3))

#define PCI_CONF2_ADDRESS(dev, reg)	(unsigned short)(0xC000 | (dev << 8) | reg)

int pci_conf_read(unsigned bus, unsigned dev, unsigned fn, unsigned reg, unsigned len, unsigned int *value)
{
  int result;

  if (!value || (bus > 255) || (dev > 31) || (fn > 7) || (reg > 255))
    return -1;

  result = -1;
  switch(pci_conf_type) {
  case PCI_CONF_TYPE_1:
    rr0d_outl(PCI_CONF1_ADDRESS(bus, dev, fn, reg), 0xCF8);
    switch(len) {
    case 1:  *value = rr0d_inb(0xCFC + (reg & 3)); result = 0; break;
    case 2:  *value = rr0d_inw(0xCFC + (reg & 2)); result = 0; break;
    case 4:  *value = rr0d_inl(0xCFC); result = 0; break;
    }
    break;
  case PCI_CONF_TYPE_2:
    rr0d_outb(0xF0 | (fn << 1), 0xCF8);
    rr0d_outb((unsigned char)bus, 0xCFA);

    switch(len) {
    case 1:  *value = rr0d_inb(PCI_CONF2_ADDRESS(dev, reg)); result = 0; break;
    case 2:  *value = rr0d_inw(PCI_CONF2_ADDRESS(dev, reg)); result = 0; break;
    case 4:  *value = rr0d_inl(PCI_CONF2_ADDRESS(dev, reg)); result = 0; break;
    }
    rr0d_outb(0, 0xCF8);
    break;
  }
  return result;
}

int pci_conf_write(unsigned bus, unsigned dev, unsigned fn, unsigned reg, unsigned len, unsigned int value)
{
  int result;

  if ((bus > 255) || (dev > 31) || (fn > 7) || (reg > 255))
    return -1;

  result = -1;
  switch(pci_conf_type) {
  case PCI_CONF_TYPE_1:
    rr0d_outl(PCI_CONF1_ADDRESS(bus, dev, fn, reg), 0xCF8);
    switch(len) {
    case 1:  rr0d_outb((unsigned char)value, 0xCFC + (reg & 3)); result = 0; break;
    case 2:  rr0d_outw((unsigned short)value, 0xCFC + (reg & 2)); result = 0; break;
    case 4:  rr0d_outl(value, 0xCFC); result = 0; break;
    }
    break;
  case PCI_CONF_TYPE_2:
    rr0d_outb(0xF0 | (fn << 1), 0xCF8);
    rr0d_outb((unsigned char)bus, 0xCFA);

    switch(len) {
    case 1: rr0d_outb((unsigned char)value, PCI_CONF2_ADDRESS(dev, reg)); result = 0; break;
    case 2: rr0d_outw((unsigned short)value, PCI_CONF2_ADDRESS(dev, reg)); result = 0; break;
    case 4: rr0d_outl(value, PCI_CONF2_ADDRESS(dev, reg)); result = 0; break;
    }
    rr0d_outb(0, 0xCF8);
    break;
  }
  return result;
}




unsigned int pci_get_name(unsigned int v, unsigned int d, unsigned char *out_char)
{
  unsigned int i, ii;
  pci_desc* vendor;
  pci_desc* device;
  unsigned int precision = 0;

 

  for(i=0;i< sizeof(tab_vendor)/sizeof(pci_desc);i++)
    {
      vendor=&tab_vendor[i];
      if (vendor->id != v)
	continue;

      //printf("%s\n", vendor->name);
      strcpy(out_char, vendor->name);
      strcat(out_char, " ");
      precision=1;
      if (vendor->list == NULL)
	continue;

      for(ii=0;;ii++)
	{
	  device = &(vendor->list[ii]);
	  if (device->id == 0)
	    break;


	  if (device->id == d)
	    //printf("%s\n", device->name);
	    strcat(out_char, device->name);
	}
      
      
      precision=2;
      return precision;
    }


  return precision;
}

void list_controller(void)
{


  int i;
  unsigned int ctrl_bus;
  unsigned int ctrl_dev;
  unsigned int ctrl_fn;

  int result;
  unsigned int tmp;
  unsigned int vendor;
  unsigned int device;
  unsigned char pci_data[0x40];
  unsigned char debug[256];

 
  for (ctrl_bus = 0;ctrl_bus<255;ctrl_bus++)
    for (ctrl_dev = 0;ctrl_dev<31;ctrl_dev++)
      for (ctrl_fn = 0;ctrl_fn<7;ctrl_fn++)
	{	  
	  result = pci_conf_read(ctrl_bus, ctrl_dev, ctrl_fn, PCI_vendor_id  ,2, &vendor  );
	  result = pci_conf_read(ctrl_bus, ctrl_dev, ctrl_fn, PCI_device_id  ,2, &device  );
	  
	  if ((vendor == 0xFFFF || device == 0xFFFF ) ||
	      (vendor == 0x0    && device == 0x0))
	    continue;
	  Sprintf(debug, "%.4X %.4X\n", vendor, device);
	  Out_Debug_String(debug);
	  pci_get_name(vendor, device, debug);
	  strcat(debug, "\n");
	  Out_Debug_String(debug);

	  for (i=0;i<0x40;i++)
	    {
	      result = pci_conf_read(ctrl_bus, ctrl_dev, ctrl_fn, i,1, &tmp);
	      pci_data[i] = (unsigned char)(tmp&0xff);
#ifdef PCI_DEBUG
	      Sprintf(debug, "%.2X ", tmp&0xFF);
	      Out_Debug_String(debug);
	      if (i%16==15)
		Out_Debug_String("\n");
#endif

	    }
	  


	}



}

/*If a display is found, give back its reg adress*/
unsigned int pci_detect_display(void)
{


  int i;
  unsigned int ctrl_bus;
  unsigned int ctrl_dev;
  unsigned int ctrl_fn;

  unsigned int header_type_tmp;

  int result;
  unsigned int tmp;
  unsigned int vendor;
  unsigned int device;
  unsigned char pci_data[0x40];
  pci_info* p_pci_info;

  unsigned char debug[256];
  unsigned int reg1_address;

  Out_Debug_String("Start pci scan\n");

 
  for (ctrl_bus = 0;ctrl_bus<255;ctrl_bus++)
    for (ctrl_dev = 0;ctrl_dev<31;ctrl_dev++)
      for (ctrl_fn = 0;ctrl_fn<7;ctrl_fn++)
	{	  
	  result = pci_conf_read(ctrl_bus, ctrl_dev, ctrl_fn, PCI_vendor_id  ,2, &vendor  );
	  result = pci_conf_read(ctrl_bus, ctrl_dev, ctrl_fn, PCI_device_id  ,2, &device  );
	  
	  if ((vendor == 0xFFFF || device == 0xFFFF ) ||
	      (vendor == 0x0    && device == 0x0))
	    continue;

	  Sprintf(debug, "%.4X %.4X\n", vendor, device);
	  Out_Debug_String(debug);


	  result = pci_conf_read(ctrl_bus, ctrl_dev, ctrl_fn, PCI_header_type,1, &header_type_tmp);

	  header_type_tmp&=0x7F;

	  Sprintf(debug, "Header type %.2X\n", header_type_tmp);
	  Out_Debug_String(debug);

	  for (i=0;i<0x40;i++)
	    {
	      result = pci_conf_read(ctrl_bus, ctrl_dev, ctrl_fn, i,1, &tmp);
	      pci_data[i] = (unsigned char)(tmp&0xff);
#ifdef PCI_DEBUG
	      Sprintf(debug, "%.2X ", tmp&0xFF);
	      Out_Debug_String(debug);
	      if (i%16==15)
		Out_Debug_String("\n");
#endif
	    }

	  p_pci_info = (pci_info*)pci_data;


	  //If its not a display device
	  if (p_pci_info->class_base != 3)
	    continue;

	  if (header_type_tmp == 0)
	    {
	      reg1_address = p_pci_info->u.h0.base_registers[1];
	    }
	  else if (header_type_tmp == 1)
	    {
	      reg1_address = p_pci_info->u.h1.base_registers[1];
	    }
	  else
	    {
		Out_Debug_String("Strange Header...\n");
		continue;
	    }


	  if (reg1_address & PCI_BASE_ADDRESS_SPACE)
	    {
		Out_Debug_String("Device with io...\n");
		continue;
	    }

	  reg1_address &= ~PCI_BASE_ADDRESS_SPACE_IO;
	  reg1_address &= PCI_BASE_ADDRESS_MEM_MASK;

	  pci_get_name(vendor, device, debug);

	  Out_Debug_String("____________\n");
	  Out_Debug_String(debug);
	  Out_Debug_String("\n____________\n");

	  Sprintf(debug, "Good one, returning %.8X\n", reg1_address);
	  Out_Debug_String(debug);

	  return reg1_address;

	}

  Out_Debug_String("Pci scan: failed\n");

  return 0;

}

