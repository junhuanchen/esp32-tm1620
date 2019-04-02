
#include <stdint.h>

#define MAX_TUBE_NUM			4			//数码管最大位数
#define MAX_TUBE_BRIGHTNESS		0x02	//固定亮度设置

int TM1620_Init(void);
void TM1620_Print(char* ch);
void TM1620_Display(uint16_t din);

//共阴字形码
static const uint8_t fontCode[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, //0-4
    0x6D, 0x7D, 0x07, 0x7F, 0x6F, //5-9
    0x77, 0x7C, 0x39, 0x5E, 0x79, //A-E
    0x71, 0x73, 0x3E, 0x76, 0x38, //F,P,U,H,L
    0xFF                          //test led
};

// static const uint8_t fontCode[] = {
//     0x3F & 0x80, 0x06 & 0x80, 0x5B & 0x80, 0x4F & 0x80, 0x66 & 0x80, //0-4
//     0x6D & 0x80, 0x7D & 0x80, 0x07 & 0x80, 0x7F & 0x80, 0x6F & 0x80, //5-9
//     0x77 & 0x80, 0x7C & 0x80, 0x39 & 0x80, 0x5E & 0x80, 0x79 & 0x80, //A-E
//     0x71 & 0x80, 0x73 & 0x80, 0x3E & 0x80, 0x76 & 0x80, 0x38 & 0x80, //F,P,U,H,L
//     0xFF                          //test led
// };

//DIN PA3 ,CLK PA4 ,STB/CS PA5
#define DIO_HIGH gpio_set_level(GPIO_NUM_21, 1);
#define DIO_LOW gpio_set_level(GPIO_NUM_21, 0);

#define CLK_HIGH gpio_set_level(GPIO_NUM_22, 1);
#define CLK_LOW gpio_set_level(GPIO_NUM_22, 0);

#define STB_HIGH gpio_set_level(GPIO_NUM_4, 1);
#define STB_LOW gpio_set_level(GPIO_NUM_4, 0);

//TM1620_DATA寄存器
#define AUTO_ADDR_MODE 0x00  //地址自增模式
#define FIXED_ADDR_MODE 0x04 //固定地址模式

typedef enum
{
    TM1620_DISP_MODE = 0x0, //显示模式设置
    TM1620_DATA = 0x1,      //数据命令设置
    TM1620_BRIGHT = 0x2,    //显示控制命令设置
    TM1620_ADDR = 0x3       //地址命令设置
} TM1620_CMD;

int TM1620_LowLevel_Init(void);
int TM1620_Write_8bit(uint8_t data);
void TM1620_Write(TM1620_CMD cmd, uint8_t data);
uint8_t chToFontcode(char ch);
void u16ToDisplayArray(uint16_t uin);
void TM1620_Init_Test(void);
