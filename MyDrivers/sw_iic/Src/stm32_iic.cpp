#include "sw_iic/Inc/stm32_iic.h"

GPIO_TypeDef* IIC_SCL_PORT[] = {DCMI_SCL_GPIO_Port, DCMI_SCL_GPIO_Port, DCMI_SCL_GPIO_Port};
GPIO_TypeDef* IIC_SDA_PORT[] = {DCMI_SDA_GPIO_Port, DCMI_SDA_GPIO_Port, DCMI_SDA_GPIO_Port};

const uint16_t  IIC_SCL_PIN[] = {DCMI_SCL_Pin, DCMI_SCL_Pin, DCMI_SCL_Pin};
const uint16_t  IIC_SDA_PIN[] = {DCMI_SDA_Pin, DCMI_SDA_Pin, DCMI_SDA_Pin};

Stm32IIC::Stm32IIC()
{
    SwIicDly = I2C_SPEED_MAPPING_100K;
}

void Stm32IIC::iic_delay(void)
{
    __IO uint32_t u32Loop = SwIicDly;

    while(u32Loop--)
    {
        #ifdef __mips__
        __asm__ __volatile__ ("nop");
        #endif

        #ifdef __AEONR2__
        __asm__ __volatile__ ("l.nop");
        #endif

        #ifdef __arm__
        __asm__ __volatile__ ("mov r0, r0");
        #endif
    }
}

void Stm32IIC::update(IIC_TypeDef iic_)
{
    SwIicDly = I2C_SPEED_MAPPING_100K;
    iic = iic_;
}

void Stm32IIC::sda_out(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.Pin = IIC_SDA_PIN[this->iic];
    GPIO_InitStructer.Mode = GPIO_MODE_OUTPUT_OD;
    HAL_GPIO_Init(IIC_SDA_PORT[this->iic], &GPIO_InitStructer);
}

void  Stm32IIC::sda_in(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.Pin = IIC_SDA_PIN[this->iic];
    GPIO_InitStructer.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(IIC_SDA_PORT[this->iic], &GPIO_InitStructer);

}

int Stm32IIC::sda_read()
{
    return static_cast<int>(HAL_GPIO_ReadPin(IIC_SDA_PORT[this->iic], IIC_SDA_PIN[this->iic]));
}

void  Stm32IIC::Start(void)
{
    sda_out();
    HAL_GPIO_WritePin(IIC_SDA_PORT[this->iic], IIC_SDA_PIN[this->iic], GPIO_PIN_SET);
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_SET);
    iic_delay(IIC_DELAY);
    HAL_GPIO_WritePin(IIC_SDA_PORT[this->iic], IIC_SDA_PIN[this->iic], GPIO_PIN_RESET);
    iic_delay(IIC_DELAY);
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_RESET);
}

void Stm32IIC::Stop(void)
{   
    sda_out();
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_RESET); 			//SCL_L
    HAL_GPIO_WritePin(IIC_SDA_PORT[this->iic], IIC_SDA_PIN[this->iic], GPIO_PIN_RESET);			//SDA_L
    iic_delay(IIC_DELAY);
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_SET); 				//SCL_H
    HAL_GPIO_WritePin(IIC_SDA_PORT[this->iic], IIC_SDA_PIN[this->iic], GPIO_PIN_SET);	 			//SDA_H
    iic_delay(IIC_DELAY);
}

//????????????:		uint8_t Stm32IIC::Read_Ack(void)
// ??? ???:	    ??????ACK
//????????????1?????????????????????
//        0?????????????????????
uint8_t Stm32IIC::Receive_Ack(void)
{
	uint8_t ucErrTime=0;
    sda_in();
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_SET); 				//SCL_H	
    iic_delay(IIC_DELAY);
	while(sda_read())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			Stop();
			return 1;
		}
	}
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_RESET);			//SCL_L
    return 0;
}

//??????ACK??????
void Stm32IIC::Send_Ack(void)
{
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_RESET); 				//SCL_H	
    sda_out();
    HAL_GPIO_WritePin(IIC_SDA_PORT[this->iic], IIC_SDA_PIN[this->iic], GPIO_PIN_RESET);			//SDA_L
	iic_delay(IIC_DELAY);
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_SET); 				//SCL_H	
	iic_delay(IIC_DELAY);
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_RESET); 				//SCL_H	
}

//?????????ACK??????		    
void Stm32IIC::Send_NAck(void)
{
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_RESET); 				//SCL_H	
	sda_out();
    HAL_GPIO_WritePin(IIC_SDA_PORT[this->iic], IIC_SDA_PIN[this->iic], GPIO_PIN_SET);			//SDA_L
	iic_delay(IIC_DELAY);
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_SET); 				//SCL_H	
	iic_delay(IIC_DELAY);
    HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_RESET); 				//SCL_H	
}					 				     

//????????????:		void Stm32IIC::Write_OneByte(uint8_t Dat)
//????????????:	    ????????????????????????????????????
//??????	Dat     ???????????????
void Stm32IIC::Send_Byte(uint8_t Data)
{
    uint8_t i;  
    sda_out();
    for(i=0;i<8;i++)
    { 
        HAL_GPIO_WritePin(IIC_SDA_PORT[this->iic], IIC_SDA_PIN[this->iic], (GPIO_PinState)((Data&0x80)>>7));	 		//SDA_H
        Data <<= 1;
		iic_delay(IIC_DELAY);   //???TEA5767??????????????????????????????
        HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_SET);			//SCL_L
		iic_delay(IIC_DELAY); 
        HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_RESET);			//SCL_L
		iic_delay(IIC_DELAY);
    }
}

//????????????:		uint8_t Stm32IIC::Read_OneByte(void)
//????????????:	    ????????????????????????????????????
//???1????????????ack=1????????????ACK???ack=0?????????nACK   
uint8_t Stm32IIC::Receive_Byte(uint8_t ack)
{  
    uint8_t data,i;
    sda_in();
    for( i = 0, data = 0; i < 8 ; i++)
    {
        HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_RESET);			//SCL_L
        iic_delay(IIC_DELAY);
        HAL_GPIO_WritePin(IIC_SCL_PORT[this->iic], IIC_SCL_PIN[this->iic], GPIO_PIN_SET); 				//SCL_H
        data <<= 1;
        if(sda_read()) 
            data++;   
		iic_delay(IIC_DELAY); 
    }
    if (!ack)
        Send_NAck();//??????nACK
    else
        Send_Ack(); //??????ACK   
    return data;
}

//????????????:		int Stm32IIC::Write_Addr_Data(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
//????????????:	    ????????????????????????????????????????????????????????????????????????
int Stm32IIC::Write_Addr_Data(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
    int i;
    Write_OneByte(addr << 1 );
    if (!Wait_Ack_Reply()) 
    {
        Stop();
        return 1;
    }
    Write_OneByte(reg);
    Wait_Ack_Reply();
    for (i = 0; i < len; i++) 
    {
        Write_OneByte(data[i]);
        if (!Wait_Ack_Reply()) 
        {
            Stop();
            return 0;
        }
    }
    Stop();
    return 0;

}

//????????????:		int Stm32IIC::Read_Addr_Data(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
//????????????:	    ????????????????????????????????????????????????????????????
int Stm32IIC::Read_Addr_Data(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    Write_OneByte(addr << 1);
    if (!Wait_Ack_Reply()) 
    {
        Stop();
        return 1;
    }
    Write_OneByte(reg);
    Wait_Ack_Reply();
    Start();
    Write_OneByte((addr << 1)+1);
    Wait_Ack_Reply();
    while (len) {
        if (len == 1)
            *buf = Read_Byte_Ack(0);
        else
            *buf = Read_Byte_Ack(1);
        buf++;
        len--;
    }
    Stop();
    return 0;
}

//????????????:		uint8_t Stm32IIC::Read_DeviceAddr_OneByte(uint8_t Addr,uint8_t addr)
//????????????:	    ?????????????????? ???????????????????????????
//??????	Addr    ??????????????????
//		addr	???????????????
//??????   ???????????????
uint8_t Stm32IIC::Read_DeviceAddr_OneByte(uint8_t Addr,uint8_t addr)
{
    uint8_t res=0;
    Start();	
    Write_OneByte(Addr);	   //???????????????
    res++;
    Wait_Ack_Reply();
    Write_OneByte(addr); res++;  //????????????
    Wait_Ack_Reply();	  
    //IIC_Stop();//????????????????????????	
    Start();
    Write_OneByte(Addr+1); res++;          //??????????????????			   
    Wait_Ack_Reply();
    res=Read_Byte_Ack(0);	   
    Stop();//????????????????????????
    return res;
}
