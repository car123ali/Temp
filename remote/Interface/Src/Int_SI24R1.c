#include "Int_SI24R1.h"

uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x0A, 0x22, 0x07, 0x1E, 0x01}; // 定义一个静态发送地址

static uint8_t SPI_RW(uint8_t byte)
{
	uint8_t receive_byte = 0;
	HAL_SPI_TransmitReceive(&hspi1, &byte, &receive_byte, 1, 100);
	return receive_byte;
}

/********************************************************
函数功能：SI24R1引脚初始化
入口参数：无
返回  值：无
*********************************************************/
void Int_SI24R1_Init(SI24R1_MODE mode)
{
	Int_SI24R1_SelfCheck();
	if (mode == RX_MODE)
	{
		SI24R1_RX_Mode();
	}
	else if (mode == TX_MODE)
	{
		SI24R1_TX_Mode();
	}
}

/********************************************************
函数功能：写寄存器的值（单字节）
入口参数：reg:寄存器映射地址（格式：WRITE_REG_CMD｜reg）
					value:寄存器的值
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;

	NSS_LOW;
	status = SPI_RW(reg);
	SPI_RW(value);
	NSS_HIGH;

	return (status);
}

/********************************************************
函数功能：写寄存器的值（多字节）
入口参数：reg:寄存器映射地址（格式：WRITE_REG_CMD｜reg）
					pBuf:写数据首地址
					bytes:写数据字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status, byte_ctr;

	NSS_LOW;
	status = SPI_RW(reg);
	for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
		SPI_RW(*pBuf++);
	NSS_HIGH;

	return (status);
}

/********************************************************
函数功能：读取寄存器的值（单字节）
入口参数：reg:寄存器映射地址（格式：READ_REG_CMD｜reg）
返回  值：寄存器值
*********************************************************/
uint8_t SI24R1_Read_Reg(uint8_t reg)
{
	uint8_t value;

	NSS_LOW;
	SPI_RW(reg);
	value = SPI_RW(0);
	NSS_HIGH;

	return (value);
}

/********************************************************
函数功能：读取寄存器的值（多字节）
入口参数：reg:寄存器映射地址（READ_REG_CMD｜reg）
					pBuf:接收缓冲区的首地址
					bytes:读取字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status, byte_ctr;

	NSS_LOW;
	status = SPI_RW(reg);
	for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
		pBuf[byte_ctr] = SPI_RW(0); // 读取数据，低字节在前
	NSS_HIGH;

	return (status);
}

/********************************************************
函数功能：SI24R1接收模式初始化
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_RX_Mode(void)
{
	CE_LOW;
	SI24R1_Write_Buf(WRITE_REG_CMD + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 接收设备接收通道0使用和发送设备相同的发送地址
	SI24R1_Write_Reg(WRITE_REG_CMD + EN_AA, 0x01);							// 使能接收通道0自动应答
	SI24R1_Write_Reg(WRITE_REG_CMD + EN_RXADDR, 0x01);						// 使能接收通道0
	SI24R1_Write_Reg(WRITE_REG_CMD + RF_CH, CH);							// 选择射频通道0x40
	SI24R1_Write_Reg(WRITE_REG_CMD + RX_PW_P0, TX_PLOAD_WIDTH);				// 接收通道0选择和发送通道相同有效数据宽度
	SI24R1_Write_Reg(WRITE_REG_CMD + RF_SETUP, 0x0f);						// 数据传输率2Mbps，发射功率7dBm
	SI24R1_Write_Reg(WRITE_REG_CMD + CONFIG, 0x0f);							// CRC使能，16位CRC校验，上电，接收模式
	SI24R1_Write_Reg(WRITE_REG_CMD + STATUS, 0xff);							// 清除所有的中断标志位
	CE_HIGH;																// 拉高CE启动接收设备
}

/********************************************************
函数功能：SI24R1发送模式初始化
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_TX_Mode(void)
{
	CE_LOW;
	SI24R1_Write_Buf(WRITE_REG_CMD + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);	// 写入发送地址
	SI24R1_Write_Buf(WRITE_REG_CMD + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 为了应答接收设备，接收通道0地址和发送地址相同

	SI24R1_Write_Reg(WRITE_REG_CMD + EN_AA, 0x01);		// 使能接收通道0自动应答
	SI24R1_Write_Reg(WRITE_REG_CMD + EN_RXADDR, 0x01);	// 使能接收通道0
	SI24R1_Write_Reg(WRITE_REG_CMD + SETUP_RETR, 0x0a); // 自动重发延时等待250us+86us，自动重发10次
	SI24R1_Write_Reg(WRITE_REG_CMD + RF_CH, CH);		// 选择射频通道40
	SI24R1_Write_Reg(WRITE_REG_CMD + RF_SETUP, 0x0f);	// 数据传输率2Mbps，发射功率7dBm
	SI24R1_Write_Reg(WRITE_REG_CMD + CONFIG, 0x0e);		// CRC使能，16位CRC校验，上电
	CE_HIGH;
}

/********************************************************
函数功能：读取接收数据
入口参数：rxbuf:接收数据存放首地址
返回  值：0:接收到数据
		  1:没有接收到数据
*********************************************************/
uint8_t Int_SI24R1_RxPacket(uint8_t *rxbuf)
{
	uint8_t state;
	state = SI24R1_Read_Reg(STATUS);				 // 读取状态寄存器的值
	SI24R1_Write_Reg(WRITE_REG_CMD + STATUS, state); // 清除RX_DS中断标志

	if (state & RX_DR) // 接收到数据
	{
		SI24R1_Read_Buf(RD_RX_PLOAD, rxbuf, TX_PLOAD_WIDTH); // 读取数据
		SI24R1_Write_Reg(FLUSH_RX, 0xff);					 // 清除RX FIFO寄存器
		return 0;
	}
	return 1; // 没收到任何数据
}

/********************************************************
函数功能：发送一个数据包
入口参数：txbuf:要发送的数据
返回  值：0x10:达到最大重发次数，发送失败
		  0x20:发送成功
		  0xff:发送失败
*********************************************************/
uint8_t Int_SI24R1_TxPacket(uint8_t *txbuf)
{
	uint8_t state;
	CE_LOW;												  // CE拉低，使能SI24R1配置
	SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); // 写数据到TX FIFO,32个字节
	CE_HIGH;											  // CE置高，使能发送

	while (1)
	{
		state = SI24R1_Read_Reg(STATUS);				 // 读取状态寄存器的值
		SI24R1_Write_Reg(WRITE_REG_CMD + STATUS, state); // 清除TX_DS或MAX_RT中断标志
		if (state & (TX_DS | MAX_RT))
		{
			break;
		}
	}

	if (state & MAX_RT) // 达到最大重发次数
	{
		SI24R1_Write_Reg(FLUSH_TX, 0xff); // 清除TX FIFO寄存器
		return MAX_RT;
	}
	if (state & TX_DS) // 发送完成
	{
		return TX_DS;
	}
	return 0XFF; // 发送失败
}

// 自检函数
void Int_SI24R1_SelfCheck(void)
{
	uint8_t buffer[5] = {0};
	while (1)
	{
		SI24R1_Write_Buf(WRITE_REG_CMD + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
		SI24R1_Read_Buf(READ_REG_CMD + RX_ADDR_P0, buffer, TX_ADR_WIDTH);
		// 判断buffer和TX_ADDRESS是否相等
		if (buffer[0] == TX_ADDRESS[0] && buffer[1] == TX_ADDRESS[1] && buffer[2] == TX_ADDRESS[2] && buffer[3] == TX_ADDRESS[3] && buffer[4] == TX_ADDRESS[4])
		{
			printf("SI24R1自检成功\r\n");
			break;
		}
		else
		{
			printf("自检失败\r\n");
		}
	}
}
