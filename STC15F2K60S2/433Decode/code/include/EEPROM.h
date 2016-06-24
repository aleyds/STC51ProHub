#ifndef __EEPROM_H__
#define __EEPROM_H__

#define ROM_SECTOR		2   //��������,һ������512�ֽ�
#define ROM_LEN			1024

#define ROM_START_ADDR    0x0000
#define ROM_END_ADDR	  0x03FF

//EEPROM CMD ����ָ��
#define CMD_IDLE		0		//����ģʽ
#define CMD_READ		1		//�ֽڶ�����
#define CMD_WRITE		2		//�ֽ�д������
#define CMD_ERASE		3		//������������

//#deifne ENABLE_IAP		0x80		//SYSCLK < 30MHz
//#deifne ENABLE_IAP		0x81		//SYSCLK < 24MHz
#define ENABLE_IAP		0x82		//SYSCLK < 20MHz
//#deifne ENABLE_IAP		0x83		//SYSCLK < 12MHz
//#deifne ENABLE_IAP		0x84		//SYSCLK < 6MHz
//#deifne ENABLE_IAP		0x85		//SYSCLK < 3MHz
//#deifne ENABLE_IAP		0x86		//SYSCLK < 2MHz
//#deifne ENABLE_IAP		0x87		//SYSCLK < 1MHz
 

#endif