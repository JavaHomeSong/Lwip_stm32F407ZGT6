#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lwip_comm.h"
#include "LAN8720.h"
//#include "usmart.h"
#include "timer.h"
#include "lcd.h"
#include "sram.h"
#include "rtc.h"
#include "beep.h"
#include "adc.h"
#include "temperature.h"
#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "httpd.h"
#include "ds18b20.h"
#include "dht11.h"
#include "moto.h"
#include "exti.h"
#include "usart3.h"
//#include "timer7.h"
//cjk到此一游！！！！！！！！
//大家好 jjjjjjjjjjjjjjjjjjjj
//ALIENTEK 探索者STM32F407开发板
//这是新的技术
//LWIP LWIP综合实验
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
extern u8 DS18B20_Init(void);
extern void Adc_Temperate_Init(void);	//声明内部温度传感器初始化函数

//u16 USART_RX_STA=0; 
//加载UI
//	没有过不去的事情，只有过不去的心情
//	只要把心情变一变，世界完全不一样了

//mode:
//bit0:0,不加载;1,加载前半部分UI
//bit1:0,不加载;1,加载后半部分UI
void lwip_test_ui(u8 mode)
{
	u8 speed;
	u8 buf[30]; 
	POINT_COLOR=RED;
	if(mode&1<<0)
	{
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//清除显示
		LCD_ShowString(30,30,200,16,16,"Explorer STM32F4");
		LCD_ShowString(30,50,200,16,16,"LWIP Webserver Test");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		LCD_ShowString(30,90,200,16,16,"2014/8/15"); 	
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//清除显示
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印动态IP地址
		else sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印静态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		speed=LAN8720_Get_Speed();//得到网速
		if(speed&1<<1)LCD_ShowString(30,150,200,16,16,"Ethernet Speed:100M");
		else LCD_ShowString(30,150,200,16,16,"Ethernet Speed:10M");
	}
}


int main(void)
{ 
	u32 i;
	delay_init(168);      	                        
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组配置
	                                //延时初始化
	uart_init(9600);   	                           //串口波特率设置
  usart3_init(115200); 
	//usmart_dev.init(84); 	                        //初始化USMART
	LED_Init();  			//LED初始化
	
	KEY_Init();  			//按键初始化
	LCD_Init(); 			//LCD初始化
	BEEP_Init(); 			//蜂鸣器初始化
	RTC_Timer_Init();  		//RTC初始化
	Adc_Init(); 		 	//ADC1_CH5初始化
	DS18B20_Init();
	Adc_Temperate_Init(); 	//内部温度传感器初始化
	FSMC_SRAM_Init();		//初始化外部SRAM  
	DHT11_Init();
	EXTIX_Init(); 
	mymem_init(SRAMIN);		//初始化内部内存池
	mymem_init(SRAMEX);		//初始化外部内存池
	mymem_init(SRAMCCM);	//初始化CCM内存池
  MOTO_GPIO_Config();
	POINT_COLOR = RED; 		//红色字体
	lwip_test_ui(1);			//加载前半部分UI
	TIM3_Int_Init(999,839); 	//100khz的频率,计数1000为10ms
	//TIM7_Int_Init(1000-1,8400-1);
	CAR_STOP();
	
	
	while(lwip_comm_init()) 			//lwip初始化
	{
		LCD_ShowString(30,150,200,20,16,"LWIP Init Falied!");
		delay_ms(1200);
		LCD_Fill(30,110,230,130,WHITE); //清除显示
		LCD_ShowString(30,110,200,16,16,"Retrying...");  
	}
	LCD_ShowString(30,110,200,20,16,"LWIP Init Success!");
 	LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");  //等待DHCP获取 
#if LWIP_DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
	{	
		lwip_periodic_handle();
	}
#endif
	lwip_test_ui(2);		//加载后半部分UI 
	delay_ms(500);			//延时1s
	delay_ms(500);
	httpd_init();  			//Web Server模式
	while(1)
	{ lwip_periodic_handle();   
		i++;
		if(i%5000==0)LED0=!LED0;
		delay_us(100);
		
	}                                        
	 
	
}



































