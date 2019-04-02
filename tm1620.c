
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tm1620.h"

static char DisplayArray[MAX_TUBE_NUM];

int TM1620_Init(void)
{

    TM1620_LowLevel_Init();

    // TM1620_Init_Test();

    vTaskDelay(500 / portTICK_RATE_MS);

    return 0;
}

#include "driver/gpio.h"
static gpio_config_t init_io(gpio_num_t num)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << num);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    return io_conf;
}

int TM1620_LowLevel_Init(void)
{
    gpio_config_t io_config_4 = init_io(GPIO_NUM_4);
    gpio_config(&io_config_4);
    gpio_config_t io_config_21 = init_io(GPIO_NUM_21);
    gpio_config(&io_config_21);
    gpio_config_t io_config_22 = init_io(GPIO_NUM_22);
    gpio_config(&io_config_22);

    // __HAL_RCC_GPIOA_CLK_ENABLE();
    // GPIO_InitTypeDef GPIO_InitStruct;

    // GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_5;
    // GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    // GPIO_InitStruct.Pull = GPIO_PULLUP;
    // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    // HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    DIO_HIGH;
    STB_HIGH;
    CLK_HIGH;
    vTaskDelay(10 / portTICK_RATE_MS);

    return 0;
}

void TM1620_Write(TM1620_CMD cmd, uint8_t data)
{
    switch (cmd)
    {
    case TM1620_DISP_MODE:
        STB_LOW;
        //显示模式6位8段,0b0000_0010
        TM1620_Write_8bit(data & 0x03);
        STB_HIGH;
        //vTaskDelay(1/ portTICK_RATE_MS);
        break;
    case TM1620_DATA:
        STB_LOW;
        //数据读写模式0b0100_0000
        TM1620_Write_8bit(0x40 | (data & 0x7));
        STB_HIGH;
        //vTaskDelay(1/ portTICK_RATE_MS);
        break;
    case TM1620_BRIGHT:
        STB_LOW;
        //亮度设置0b1000_1xxx
        TM1620_Write_8bit(0x88 | (0x7 & data));
        STB_HIGH;
        //vTaskDelay(1/ portTICK_RATE_MS);
        break;
    case TM1620_ADDR: //显示地址
        STB_LOW;
        TM1620_Write_8bit(0xC0 | (0xF & data));
        //vTaskDelay(1/ portTICK_RATE_MS);
        break;
    }
}

int TM1620_Write_8bit(uint8_t data)
{
    //vTaskDelay(1/ portTICK_RATE_MS);
    for (int i = 0; i < 8; i++)
    {
        CLK_LOW;
        if (data & 0x1)
        {
            DIO_HIGH;
        }
        else
        {
            DIO_LOW;
        }
        data = data >> 1;
        vTaskDelay(1/ portTICK_RATE_MS);
        CLK_HIGH;
        //vTaskDelay(1/ portTICK_RATE_MS);
    }
    return 0;
}

//在数码管上显示字符,该芯片最多6位数码管
void TM1620_Print(char *ch)
{
    uint8_t fcode[MAX_TUBE_NUM];

    for (int i = 0; i < MAX_TUBE_NUM; i++)
    {
        fcode[i] = chToFontcode(ch[i]);
    }

    TM1620_Write(TM1620_DISP_MODE, 0x02);      //显示模式6位8段
    TM1620_Write(TM1620_DATA, AUTO_ADDR_MODE); //地址自增模式

    //偶数为地址从0x00-0x0A分别为数码管的第1-6位
    //奇数地址显示无效
    TM1620_Write(TM1620_ADDR, 0x00); //起始地址0xC0

    for (int i = 0; i < MAX_TUBE_NUM; i++)
    {
        TM1620_Write_8bit(fcode[i]);
        TM1620_Write_8bit(0x00); //因为奇数地址无效，所以补0
    }
    STB_HIGH;
    TM1620_Write(TM1620_BRIGHT, MAX_TUBE_BRIGHTNESS); //亮度设置
}

//显示一个数字，长度最长为MAX_TUBE_NUM
void TM1620_Display(uint16_t din)
{
    u16ToDisplayArray(din);
    TM1620_Print(DisplayArray);
}

void TM1620_Init_Test(void)
{
    for (int i = 0; i < MAX_TUBE_NUM; i++)
    {
        DisplayArray[i] = 'T';
    }
    TM1620_Print(DisplayArray);
    vTaskDelay(1000 / portTICK_RATE_MS);
    u16ToDisplayArray(1);
    TM1620_Print(DisplayArray);
}

uint8_t chToFontcode(char ch)
{
    uint8_t fontcode = 0;

    if (ch >= '0' && ch <= '9')
    {
        fontcode = fontCode[ch - '0'];
    }
    else if (ch >= 'A' && ch <= 'F')
    {
        fontcode = fontCode[ch - 'A' + 10];
    }
    else if (ch == 'P')
    {
        fontcode = fontCode[16];
    }
    else if (ch == 'U')
    {
        fontcode = fontCode[17];
    }
    else if (ch == 'H')
    {
        fontcode = fontCode[18];
    }
    else if (ch == 'L')
    {
        fontcode = fontCode[19];
    }
    else if (ch == 'T')
    {
        fontcode = fontCode[20]; //test led全亮
    }
    else
    {
        fontcode = 0; //off
    }
    return fontcode;
}

void u16ToDisplayArray(uint16_t uin)
{
    uint16_t temp = uin;

    for (int i = (MAX_TUBE_NUM - 1); i >= 0; i--)
    {
        DisplayArray[i] = temp % 10 + '0';
        temp = temp / 10;
    }
}

#ifdef main

int main()
{
    TM1620_Init();
    TM1620_Init_Test(void);
}

#endif
