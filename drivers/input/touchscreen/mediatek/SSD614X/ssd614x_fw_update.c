/* 
 * Copyright 2016 Solomon Systech Ltd. All rights reserved.
 *
 * Author: binkazhang@solomon-systech.com
 * Date: 2016.01.21
 */
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/input/mt.h>
#include <linux/kthread.h>

#include "ssd614x_fw_update_data.h"
#include "ssd614x.h"

#define SSD6141_FW			0x00
#define SSD6141_TMCREG		0x01
#define SSD6141_LUT			0x02
#define SSD6141_FDM			0x03
#define EFLASH_DCSW			0x04
#define SSD6141_RAM			0x05

#define SSD6141_PAGE_SIZE	1024
#define SSD6141_MAX_ITEM	10
#define SSD6141_TRIM_SIZE	512

#define SSD6141_ERASE_CHIP	0x01
#define SSD6141_ERASE_PAGE	0x03
#define SSD6141_ERASE_OTP	0x05

struct fwpart{
	unsigned int write_address;
	unsigned int byte_cnt;
	unsigned int erase_cnt;
	unsigned int version;
	unsigned int check_sum;
	unsigned int style_index;
	unsigned int rsvd02;
	unsigned int rsvd03;
	unsigned char *content;
};
struct fwpart g_part[SSD6141_MAX_ITEM];


struct ssl_header_item {
	unsigned int write_address;
	unsigned int byte_cnt;
	unsigned int erase_cnt;
	unsigned int version;
	unsigned int check_sum;
	unsigned int style_index;
	unsigned int rsvd01;
	unsigned int offset;
};

struct ssl_header {
	unsigned int ic_name;
	unsigned int file_check_sum;
	unsigned int fw_version;
	unsigned int usr_version;
	struct ssl_header_item items[SSD6141_MAX_ITEM];
};

struct ssl_header g_ssl_header;
//###############################################################################
#define	SSL_AUTO_UPGRADE //for auto upgrade don't remark

#ifdef SSL_AUTO_UPGRADE
#define FILE_READ_TRY			5
#define ssl_initcode_file1		"/sdcard/ssd614x_fw_update_data.bin"
#define ssl_initcode_file2		"/data/ssd614x_fw_update_data.bin"
#endif
//###############################################################################

extern struct i2c_client *g_misc_client;

int dl_write_reg(int address, unsigned char* buffer, int length)
{
	#define MAX_PACKET_LENGTH 512
	int ret;
	int i;
	int packet_count;
	
	if (length > 0)
	{
		packet_count = (length % MAX_PACKET_LENGTH) ? (length / MAX_PACKET_LENGTH)+1 : (length / MAX_PACKET_LENGTH);
		for (i=0;i<packet_count;i++)
		{
			if ((length-i*MAX_PACKET_LENGTH)>MAX_PACKET_LENGTH)
			{
				ret = ts_write_data(g_misc_client, address, buffer+i*MAX_PACKET_LENGTH, MAX_PACKET_LENGTH);
			}
			else 
			{
				ret = ts_write_data(g_misc_client, address, buffer+i*MAX_PACKET_LENGTH, length-i*MAX_PACKET_LENGTH);
			}
			msleep(20);
		}
	}
	else	
	{
		ret = ts_write_data(g_misc_client, address, NULL, 0);
	}
	return length;
}

int dl_read_reg(int address, unsigned char* buffer, int length)
{
	int ret;
	
	ret = ts_read_data(g_misc_client, address, buffer, length);

	return length;
}

unsigned short dl_update_crc(unsigned short crc, unsigned char buffer)
{
	char i;						// loop counter

	#define CRC_POLY 0x8408		// CRC16-CCITT FCS (X^16+X^12+X^5+1)

	crc = crc ^ (unsigned int)buffer;

	for (i = 0; i < 8; i++)
	{
		if (crc & 0x01)
		{
			crc = crc >> 1;
			crc ^= CRC_POLY;
		}
		else
		{
			crc = crc >> 1;
		}
	}


	return crc;
}

unsigned short dl_update_crc_buf(unsigned char* buffer, int length)
{
	unsigned short crc = 0;
	int i;

	crc = 0;
	for(i = 0; i <length;i++)
	{
		crc = dl_update_crc(crc, buffer[i]);
	}
	return crc;
}

int dl_read_boot_status(void)
{
	unsigned char rd_buf[2];
	int return_val;

	// 0x00FD - MODE_QUERY command to read boot status
	dl_read_reg( 0x00FD, rd_buf, 2);
	return_val = (rd_buf[1]&0xFF)|((rd_buf[0]&0xFF)<<8);

	return return_val;
}

int dl_download_ramloader(unsigned char* buffer, int length)
{
	unsigned char send_data[10];

	unsigned char recv_data[16];
	unsigned int read_value;


//	int sendbyte_cnt = 0;
	int send_address = 0;
	int sendlength = 0;
	int base_address = 0x42000;

	//bootRom mode change 
	dl_write_reg(0x0002, NULL, 0);
	msleep(100);

	//device status check
	// 0x0002 - ADDR command to setup read address to 0x000C0034
	send_data[0] = 0x00;
	send_data[1] = 0x04;
	send_data[2] = 0x00;
	send_data[3] = 0x0c;
	send_data[4] = 0x00;
	send_data[5] = 0x34;

	dl_write_reg(0x0002, send_data, 6);

	// 0x0004 - READ_PKT command to read device status
	dl_read_reg(0x0004, (unsigned char*)&read_value, 4);

	//SSL_DEBUG("RLID : %04X", read_value);

	if(read_value != 0x002)
		return -1;

	//hw id &sw version read
	// 0x0002 - ADDR command to setup read address to 0x000C0038
	send_data[0] = 0x00;
	send_data[1] = 0x04;
	send_data[2] = 0x00;
	send_data[3] = 0x0c;
	send_data[4] = 0x00;
	send_data[5] = 0x38;

	dl_write_reg(0x0002, send_data, 6);

	// 0x0004 - READ_PKT command to read HwID & SwID
	dl_read_reg(0x0004, (unsigned char*)&read_value, 4);

	SSL_DEBUG("RLID : %04X", read_value);
	
	for(sendlength = length;sendlength >= 256; sendlength -= 256)
	{
	// 0x0002 - ADDR command to setup write length & address
		send_data[0] = 0x01;
		send_data[1] = 0x00;

		send_data[2] = (base_address>>24) & 0xFF;
		send_data[3] = (base_address>>16) & 0xFF;
		send_data[4] = (base_address>>8) & 0xFF;
		send_data[5] = base_address& 0xFF;

		dl_write_reg(0x0002, send_data, 6);
	// 0x0003 - PGRM_PKT command to write content
		dl_write_reg(0x0003, buffer + send_address, 256);

		send_address += 256;
		base_address += 256;
		msleep(20);
	}

	if(sendlength > 0)
	{
	// 0x0002 - ADDR command to setup write length & address
		send_data[0] = (sendlength>>8)&0xFF;
		send_data[1] = sendlength&0xFF;

		send_data[2] = (base_address>>24) & 0xFF;
		send_data[3] = (base_address>>16) & 0xFF;
		send_data[4] = (base_address>>8) & 0xFF;
		send_data[5] = base_address& 0xFF;

		dl_write_reg(0x0002, send_data, 6);
	// 0x0003 - PGRM_PKT command to write content
		dl_write_reg(0x0003, buffer + send_address, sendlength);
	}
	msleep(20);

	//Execute the downloaded software
	// 0x0001 - EXECUTE command to start ramloader
	send_data[0] = 0x00;
	send_data[1] = 0x04;
	send_data[2] = 0x20;
	send_data[3] = 0x60;
	dl_write_reg(0x0001, send_data, 4);

	//get verison RL
	// 0x0005 - GET_VERSION command to confirm ramloader is runing properly
	msleep(10);
	dl_read_reg(0x0005, recv_data, 2);

	SSL_DEBUG("dl_download_ramloader ok");

	return 0;
}

int dl_erase_eflash(unsigned char sector, unsigned short page_address, unsigned char reserved, unsigned char mode, unsigned int page_count)
{
	unsigned char send_data[10];
	int i;
//mode
//0x01 : chip erase
//0x02 : sector erase
//0x03 : page erase
//0x04 : address erase
//=================
//ykdefine
//0x05 : OTP area erase
//

	if(mode == 0x01)//chip erase
	{
// 0x0009 - BLOCK_SEL command to switch to USER area for eflash
		send_data[0] = 0x00;
		send_data[1] = 0x00;
		dl_write_reg(0x0009, send_data, 2);
		msleep(20);

// 0x0006 - ERASE command to erase eflash
		send_data[0] = 0;
		send_data[1] = 0;
		send_data[2] = 0;
		send_data[3] = mode;

		dl_write_reg(0x0006, send_data, 4);
		msleep(100);
	}
	else if(mode == 0x03)//user area page erase
	{
// 0x0009 - BLOCK_SEL command to switch to USER area for eflash
		send_data[0] = 0x00;
		send_data[1] = 0x00;
		dl_write_reg(0x0009, send_data, 2);
		msleep(20);

		for(i=0;i<page_count;i++)
		{
// 0x0006 - ERASE command to erase eflash
			send_data[0] = 0;
			send_data[1] = page_address+i;
			send_data[2] = 0;
			send_data[3] = mode;

			dl_write_reg(0x0006, send_data, 4);
			msleep(20);
		}
	}
	else if(mode == 0x05)//otp area page erase
	{
// 0x0009 - BLOCK_SEL command to switch to USER area for eflash
		send_data[0] = 0x00;
		send_data[1] = 0x01;
		dl_write_reg(0x0009, send_data, 2);
		msleep(20);

		for(i=0;i<2;i++)
		{
// 0x0006 - ERASE command to erase eflash
			send_data[0] = 0;
			send_data[1] = i;
			send_data[2] = 0;
			send_data[3] = mode;

			dl_write_reg(0x0006, send_data, 4);
			msleep(50);
		}
	}

	return 0;
}

int dl_write_eflash(unsigned int address, unsigned char* buffer, int length)
{
	unsigned char send_data[10];
	int send_address=0;
	unsigned int crc = 0;
	unsigned char recv_data[10];
	int byte_cnt = 0;
	int i,j;
	unsigned char *fwdata;
	int fwdata_len;
	unsigned short page_address;
	unsigned int page_count;

	page_address = address / SSD6141_PAGE_SIZE;
	page_count = (length % SSD6141_PAGE_SIZE)? (length / SSD6141_PAGE_SIZE +1) : (length / SSD6141_PAGE_SIZE);
	fwdata = (unsigned char *)kmalloc(SSD6141_PAGE_SIZE, GFP_KERNEL);
	//fwdata = (unsigned char *)kzalloc(SSD6141_PAGE_SIZE, GFP_KERNEL);
	if (fwdata == NULL)
	{
		printk("write_eflash buffer kzalloc OK.\n");
		return -1;
	}

	byte_cnt = page_count*SSD6141_PAGE_SIZE;

	dl_erase_eflash(0, page_address, 0, SSD6141_ERASE_PAGE, page_count);

	SSL_DEBUG("");
	SSL_DEBUG("WriteEFlash Address :%06X", address);
	SSL_DEBUG("WriteEFlash Size %d : %d", length, byte_cnt);

	for(j = 0; j<page_count; j++)
	{
		send_address = j + page_address;

		fwdata_len =((length- j*SSD6141_PAGE_SIZE)>SSD6141_PAGE_SIZE)?SSD6141_PAGE_SIZE:length- j*SSD6141_PAGE_SIZE;
	//	memset(fwdata, 0xFF, sizeof(fwdata));
		
		memset(fwdata, 0xFF, SSD6141_PAGE_SIZE);
		
		memcpy(fwdata, buffer+j*SSD6141_PAGE_SIZE, fwdata_len);

		crc = dl_update_crc_buf(fwdata, SSD6141_PAGE_SIZE);
		SSL_DEBUG("PageWrite %04X crc %04x", send_address, crc);

		//size & write addrress
		// 0x0002 - ADDR command to setup the packet size (1K page size)
		send_data[0] = 0x04;
		send_data[1] = 0x00;
		send_data[2] = 0x00;//
		send_data[3] = 0x00;//
		send_data[4] = 0x00;//
		send_data[5] = 0x00;//
		dl_write_reg(0x0002, send_data, 6);

		//write buffer
		// 0x0003 - PRGM_PKT command to send the first 512bytes buffer (STAG limit)
		dl_write_reg(0x0003, fwdata, SSD6141_PAGE_SIZE);
		// 0x0003 - PRGM_PKT command to send the second 512bytes buffer (STAG limit)
		//dl_write_reg(0x0003, &fwdata[j*SSD6141_PAGE_SIZE +SSD6141_PAGE_SIZE/2], SSD6141_PAGE_SIZE/2);

		//write page address & crc
		// 0x0003 - PRGM_PKT command to setup the page address & send out the calculated CRC
		send_data[0] = 0x00;//reserved 
		send_data[1] = (send_address)&0xFF;//page address
		send_data[2] = crc&0xFF;//(send_address>>24) & 0xFF;42
		send_data[3] = (crc>>8)&0xFF;//(send_address>>16) & 0xFF;ac
		dl_write_reg(0x0003, send_data, 4);

		for(i=0;i<10;i++)
		{
			//read crc
			// 0x0008 - CSUM_REQ command to read out the checksum
			dl_read_reg(0x0008, recv_data, 6);
			msleep(1);

			if((send_data[2] == recv_data[3])&&(send_data[3] == recv_data[2]))
			{
				break;
			}
			else
			{
				//read ic status
				// 0x0007 - READ_STATUS command to read the status
				dl_read_reg(0x0007, recv_data, 2);
				msleep(1);
			}
		}
		if(i>= 10)
		{
			SSL_DEBUG("CRC time over!");
			kfree(fwdata);
			return -1;
		}
	}

	kfree(fwdata);

	return 0;
}

int dl_read_eflash(unsigned int address, unsigned char* buffer, int length)
{
	int ret=0;
	
	return ret;
}

int dl_switch_isp_mode(void)
{
	unsigned char send_data[10];
	unsigned short return_data;
	int timeout = 0;

	return_data = dl_read_boot_status();
	if (return_data == 0x01)
	{
		return 0;
	}

// 0x0002 - ISP_BOOT command to switch to ISP (In System Programming) mode
	send_data[0] = 0x00;
	send_data[1] = 0x01;
	dl_write_reg(0x0002, send_data, 2);
	msleep(100);

// 0x00FD - MODE_QUERY command to read boot status
	while(!(return_data == 0x01)&&(timeout++ < 20))
	{
		return_data = dl_read_boot_status();
		msleep(100);
	}

	if(timeout >= 20)
	{
		return -1;
	}

	SSL_DEBUG("switch_isp_mode ok");

	return 0;
}

int dl_download_fwpart_last_page(void)
{
	unsigned char send_data[10];
	unsigned char recv_data[10];

// CRC Starting
// 0x0008 - CSUM_REQ command to request for checksum (FlashCRC Start)
	send_data[0] = 0x00;//Reserved 
	send_data[1] = 0x00;//lastpage address
	send_data[2] = 0x00;//(send_address>>24) & 0xFF;
	send_data[3] = 0x00;//(send_address>>16) & 0xFF;
	send_data[4] = 0x00;//Reserved 
	send_data[5] = 0x01;//lastpage address

	dl_write_reg(0x0008, send_data, 6);

// 0x0008 - CSUM_REQ command to read out the checksum matcing status
	dl_read_reg(0x0008, recv_data, 6);
	if((recv_data[0] != 0 )|| (recv_data[1] != 0))
	{
		SSL_DEBUG("CS Total	=	0x%02X 0x%02X", recv_data[0], recv_data[1]);
		SSL_DEBUG("CS Last	 =	0x%02X 0x%02X", recv_data[2], recv_data[3]);
		SSL_DEBUG("CS Status =	0x%02X 0x%02X", recv_data[4], recv_data[5]);
		return -1;
	}

// ****************************************************************************
// *** This is to inform 6141 that FlashCRC done
// ****************************************************************************

// CRC Done Clear
// 0x0008 - CSUM_REQ command to confirm checksum done (FlashCRC Done)
	send_data[0] = 0x00;//Reserved 
	send_data[1] = 0x00;//lastpage address
	send_data[2] = 0x00;//(send_address>>24) & 0xFF;
	send_data[3] = 0x00;//(send_address>>16) & 0xFF;
	send_data[4] = 0x01;//Reserved 
	send_data[5] = 0x00;//lastpage address
	dl_write_reg(0x0008, send_data, 6);

// Exits bootloading Mode and restart
// 0x0001 - RESET command to restart 6141
	send_data[0] = 0x00;//
	send_data[1] = 0x00;//
	dl_write_reg(0x0001, send_data, 2);

	SSL_DEBUG("Download LastPage");

	return 0;
}

int dl_download_fwpart(void)
{
	int return_data;
	int i;
	unsigned short crc;

	printk("ssd614x : dl_download_fwpart start.\n");

	return_data = dl_read_boot_status();
	SSL_DEBUG("BootStatus : %04X", (unsigned int)return_data);
	if(dl_switch_isp_mode()==-1)
	{
		printk("switch ISPMode Fail!\n");
		return -1;
	}

	return_data = dl_read_boot_status();
	SSL_DEBUG("BootStatus : %04X", (unsigned int)return_data);

	SSL_DEBUG("Download Ramloader");
	if (dl_download_ramloader(g_part[SSD6141_RAM].content, g_part[SSD6141_RAM].byte_cnt))
	{
		printk("Download Ramloader Fail!\n");
		return -1;
	}

	SSL_DEBUG("Download EFlash");
	for(i = 0; i <SSD6141_MAX_ITEM;i++)
	{
		if ((g_part[i].byte_cnt > 0)&& (i!=SSD6141_RAM))
		{
			SSL_DEBUG("");
			SSL_DEBUG("#################################");
			SSL_DEBUG("write_address : %04X", g_part[i].write_address);
			SSL_DEBUG("#################################");
			SSL_DEBUG("     byte_cnt : %04X", g_part[i].byte_cnt);
			SSL_DEBUG("      version : %04X", g_part[i].version);
			SSL_DEBUG("    check_sum : %04X", g_part[i].check_sum);
			SSL_DEBUG("  style_index : %04X", g_part[i].style_index);
			SSL_DEBUG("#################################");
	
			crc=dl_update_crc_buf(g_part[i].content, g_part[i].byte_cnt);
			if (crc!=g_part[i].check_sum)
			{
				printk("Write %06X Crc %04X %04X Error!\n", g_part[i].write_address, crc, g_part[i].check_sum);
				return -1;
			}

			if(dl_write_eflash(g_part[i].write_address, g_part[i].content, g_part[i].byte_cnt))
			{
				printk("Write %06X Fail!\n", g_part[i].write_address);
				return -1;
			}
		}
	}
	dl_download_fwpart_last_page();
	SSL_DEBUG("Download OK");

	printk("ssd614x : dl_download_fwpart ok.\n");
	
	return 0;
}

int dl_load_buf_to_fwpart(char *buffer, int size)
{
	struct ssl_header *ssl_header;	
	int file_check_sum;
	int i;
	unsigned short crc;

	if 	(size< sizeof(struct ssl_header)) {
		printk("upgrade file to small.\n");
		return -1;
	}

	ssl_header = (struct ssl_header *)buffer;

	SSL_DEBUG("");
	SSL_DEBUG("#################################");
	SSL_DEBUG("       ic_name : %04X", ssl_header->ic_name);
	SSL_DEBUG("file_check_sum : %04X", ssl_header->file_check_sum);
	SSL_DEBUG("    fw_version : %04X", ssl_header->fw_version);
	SSL_DEBUG("   usr_version : %04X", ssl_header->usr_version);
	SSL_DEBUG("#################################");
	file_check_sum=ssl_header->file_check_sum;
	ssl_header->file_check_sum = 0;

	crc = dl_update_crc_buf(buffer, size);
	if (crc!=file_check_sum) {
		printk("upgrade file Crc %04X %04X Error!\n", crc, file_check_sum);
		return -1;
	}
	ssl_header->file_check_sum = file_check_sum;

	for(i=0;i<sizeof(g_part)/sizeof(g_part[0]);i++) {
	if (g_part[i].content != NULL) 
	{
		kfree(g_part[i].content);
		g_part[i].content=NULL;
	}
	}

	memcpy(&g_ssl_header, buffer, sizeof(g_ssl_header));

	for(i = 0; i <SSD6141_MAX_ITEM;i++)
	{
		if(g_ssl_header.items[i].byte_cnt > 0)
		{
			g_part[i].write_address = g_ssl_header.items[i].write_address;
			g_part[i].byte_cnt = g_ssl_header.items[i].byte_cnt;
			g_part[i].version = g_ssl_header.items[i].version;
			g_part[i].check_sum = g_ssl_header.items[i].check_sum;
			g_part[i].style_index = g_ssl_header.items[i].style_index;
			SSL_DEBUG("#################################");
			SSL_DEBUG("write_address : %04X", g_part[i].write_address);
			SSL_DEBUG("#################################");
			SSL_DEBUG("     byte_cnt : %04X", g_part[i].byte_cnt);
			SSL_DEBUG("      version : %04X", g_part[i].version);
			SSL_DEBUG("    check_sum : %04X", g_part[i].check_sum);
			SSL_DEBUG("  style_index : %04X", g_part[i].style_index);
			SSL_DEBUG("#################################");

			if (g_part[i].content != NULL) 
			{
				kfree(g_part[i].content);
				g_part[i].content = NULL;
			}
			g_part[i].content = (char *)kzalloc(g_part[i].byte_cnt, GFP_KERNEL);
	
			memcpy(g_part[i].content, &buffer[g_ssl_header.items[i].offset], g_ssl_header.items[i].byte_cnt);
		}
	}
	
	return 0;
}

int dl_load_file_to_fwpart(char *filename)
{
	struct file *fp;
	mm_segment_t fs;
	char *buf = NULL;
	int buf_size=0;
	int ret=-1;


	fs=get_fs();
	set_fs(KERNEL_DS);

	fp = filp_open(filename, O_RDONLY, 0);
	if(!IS_ERR(fp))
	{
		fp->f_op->llseek(fp, 0, SEEK_SET);
		buf_size = fp->f_op->llseek(fp, 0, SEEK_END);
		fp->f_op->llseek(fp, 0, SEEK_SET);

		buf = (char *)kzalloc(buf_size, GFP_KERNEL);
		if (buf == NULL)
		{
			printk("upgrade buffer kzalloc OK.\n");
			filp_close(fp, NULL);
			return -1;
		}

		fp->f_op->read(fp, buf, buf_size, &fp->f_pos );
		ret=dl_load_buf_to_fwpart(buf, buf_size);

		kfree(buf);
	}
	if (buf_size>0)
	{
		SSL_DEBUG("the upgrade filename: %s", filename);
		SSL_DEBUG("the upgrade filesize: %d", buf_size);
	}
	filp_close(fp, NULL);
	set_fs(fs);

	return ret;
}

int dl_exist_file(char* filename)
{
	struct file *fp;
	int ret=-1;
	mm_segment_t fs;

	fs=get_fs();
	set_fs(KERNEL_DS);

	//find first file exist
	fp = filp_open(filename, O_RDONLY, 0);
	if(IS_ERR(fp))
	{
		ret = -1;
	}
	else
	{
		ret = 0;
		filp_close(fp, NULL);
	}
	set_fs(fs);

	return ret;
}

int dl_update_proc(void *dir)
{
	int i;
	char* filename;
	int ret =-1;

	for (i=0; i < FILE_READ_TRY; i++)
	{
		msleep(30000);
		//find first file exist
		filename = ssl_initcode_file1;
		if(dl_exist_file(filename)!=0)
		{
			filename = NULL;
		}

		//find second file exist
		//filename = ssl_initcode_file2;
		//if(dl_exist_file(filename)!=0)
		//{
		//	filename = NULL;
		//}

		if (filename != NULL) break;
	}

	if (filename != NULL)
	{
		SSL_DEBUG("dl_update_proc : %s", filename);
		ret=dl_load_file_to_fwpart(filename);
	}

	return ret;
}

int dl_get_usr_version(void)
{
	return g_ssl_header.usr_version;
}

/************************************************************
* download data
*************************************************************/

int dl_force_upgrade_by_file(char* filename)
{
	int ret=-1;

	printk("ssd614x : dl_force_upgrade_fw %s.\n", filename);

	if(dl_exist_file(filename)==0)
	{
		ret=dl_load_file_to_fwpart(filename);
		if (ret==0)
		{
			ret=dl_download_fwpart();
		}
	}

	return ret;
}

int dl_force_upgrade_by_default_file(void)
{
	int ret=-1;

	ret = dl_force_upgrade_by_file(ssl_initcode_file2);

	return ret;
}

/************************************************************
* download data
*************************************************************/
int dl_load_data_to_fwpart(void)
{
	char *buf = NULL;
	int buf_size=sizeof(data);
	int ret=-1;

	buf = (char *)kzalloc(buf_size, GFP_KERNEL);
	if (buf == NULL)
	{
		printk("upgrade buffer kzalloc OK.\n");
		return ret;
	}

	memcpy(buf ,data ,buf_size);
	ret=dl_load_buf_to_fwpart(buf, buf_size);

	kfree(buf);

	return ret;
}

int dl_force_upgrade_by_data(void)
{
	int ret=-1;

	ret=dl_load_data_to_fwpart();
	if (ret==0)
	{
		ret=dl_download_fwpart();
	}

	return ret;
}

int dl_upgrade_by_data(void)
{
	int ret = -1;

	ret=dl_load_data_to_fwpart();
	if (ret==0)
	{
		if (dl_get_usr_version()>ssl_get_usr_verion())
		{
			ret = dl_download_fwpart();
		}
	}
	return ret;
}

int dl_auto_update_init(void)
{
	struct task_struct *thread = NULL;
	
	memset(g_part, 0x00, sizeof(g_part));
	memset(&g_ssl_header, 0x00, sizeof(g_ssl_header));

	dl_upgrade_by_data();

	thread = kthread_run(dl_update_proc, (void *)NULL, "dl_update");
	if (IS_ERR(thread))
	{
		printk("Failed to create update thread.\n");
		return -1;
	}
	return 0;
}

int dl_auto_update_exit(void)
{
	int i;

	for(i=0;i<sizeof(g_part)/sizeof(g_part[0]);i++)
	{
		if (g_part[i].content != NULL)
		{
			kfree(g_part[i].content);
			g_part[i].content=NULL;
		}
	}
	return 0;
}


