
// fire_system_Cfg.c



#include "Std_Types.h"
#include "FIRING_SYSTEM_Int.h"


u8 normal_mode_on = 0;
u8 heat_mode_on = 0;
u8 fire_mode_on = 0;
u16 fire_system_temp = 0;
u32 fire_system_smoke = 0;
u8 fire_system_mode = Normal_Mode;
u32 fire_system_password = 12345;



/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/


// fire_system_Cfg.h 



#ifndef FIRING_SYSTEM_CFG_H_
#define FIRING_SYSTEM_CFG_H_


#define  YELLOW_LED PINC1
#define  RED_LED PINC0
#define  BUZZER PINC5
#define  PASSWORD_DIGITS_NUMBER  5
#define  PUMP_MOTOR M4


#endif /* FIRING_SYSTEM_CFG_H_ */



/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/


// fire_system_Priv.h 




#ifndef FIRING_SYSTEM_PRIV_H_
#define FIRING_SYSTEM_PRIV_H_

extern u8 normal_mode_on;
extern u8 heat_mode_on ;
extern u8 fire_mode_on ;
extern u16 fire_system_temp ;
extern u32 fire_system_smoke ;
extern u8 fire_system_mode;
extern u32 fire_system_password;

#endif /* FIRING_SYSTEM_PRIV_H_ */    



/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/


// fire_system_Int.h 





#ifndef FIRING_SYSTEM_INT_H_
#define FIRING_SYSTEM_INT_H_

void NORMAL_STATE(void);
void HEAT_STATE(void);
void FIRE_STATE(void);
void FIRING_SYSTEM(void);

typedef enum{
	Normal_Mode,
	Heat_Mode,
	Fire_Mode
	}FirirngSystemMode_t;


#endif /* FIRING_SYSTEM_INT_H_ */




/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/


// fire_system_Prg.c




#define F_CPU 8000000
#include <util/delay.h>
#include "Std_Types.h"
#include "SENSORS_Int.h"
#include "LCD_Int.h"
#include "DIO_Int.h"
#include "MOTOR_Int.h"
#include "KEYPAD_Int.h"
#include "MeanTempFilter_Int.h"
#include "FIRING_SYSTEM_Cfg.h"
#include "FIRING_SYSTEM_Int.h"
#include "FIRING_SYSTEM_Priv.h"


void NORMAL_STATE(void){
	
	
	if (normal_mode_on == 0)  //1st enter in this func after transition
	{
		MEAN_TEMP_FILTER_Init();
		
		normal_mode_on = 1; //these lines mustn't repeat more than once (1st enter func after transition)
	}
	
	
	LCD_SetCursor(1,1);
	LCD_WriteString("NORMAL MODE");
	LCD_WriteString("     ");
	
	LCD_SetCursor(2,1);
	LCD_WriteString("TEMP : ");
	MEAN_TEMP_FILTER_Runnable();
	fire_system_temp = MEAN_TEMP_FILTER_Read();
	LCD_WriteNumber(fire_system_temp/10);
	LCD_WriteChar('.');
	LCD_WriteNumber(fire_system_temp%10);
	LCD_WriteChar('C');
	LCD_WriteString("    ");
	
	
	
}


void HEAT_STATE(void){
		
	if (heat_mode_on == 0)  //1st enter in this func after transition
	{
		DIO_WritePin(YELLOW_LED , HIGH);
		
		MEAN_TEMP_FILTER_Init();
		
		heat_mode_on = 1; //these lines mustn't repeat more than once (1st enter func after transition)
	}
	
	LCD_SetCursor(1,1);
	LCD_WriteString("HEAT MODE");
	LCD_WriteString("       ");
	LCD_SetCursor(2,1);
	LCD_WriteString("T:");
	MEAN_TEMP_FILTER_Runnable();
	fire_system_temp = MEAN_TEMP_FILTER_Read();
	LCD_WriteNumber(fire_system_temp/10);
	LCD_WriteChar('.');
	LCD_WriteNumber(fire_system_temp%10);
	LCD_WriteChar('C');
	LCD_WriteString(" ");
	LCD_SetCursor(2,9);
	/*
	LCD_WriteString("S:");
	fire_system_smoke = SMOKE_PERCENTAGE_READ_MQ2();
	LCD_WriteNumber(fire_system_smoke);
	LCD_WriteChar('%');
	*/

	LCD_WriteString("P:");
	fire_system_smoke = PRESSURE_Read_MPX4115();
	LCD_WriteNumber(fire_system_smoke/1000);
	LCD_WriteChar('.');
	LCD_WriteNumber((fire_system_smoke%1000)/100);
	LCD_WriteString(" ");
	
	

}


void FIRE_STATE(void){
	
	u8 received_key;
	u32 user_password=0;
	u8 password_count=0;
	
	if (fire_mode_on == 0)  //1st enter in this func after transition
	{
		DIO_WritePin(RED_LED , HIGH);
		
		DIO_WritePin(BUZZER , HIGH);  // BUZZER ON
		
		MOTOR_Init();
		MOTOR_CW(PUMP_MOTOR);       //pump on
		
		MEAN_TEMP_FILTER_Init();
		
		KEYPAD_Init();
		
		
		
		fire_mode_on = 1; //these lines mustn't repeat more than once (1st enter func after transition)
	}
	
	LCD_SetCursor(1,1);
	LCD_WriteString("(F.M) ");
	LCD_WriteString("PASS:");
	LCD_WriteString("           ");
	
	while(1){
		
	
	
	LCD_SetCursor(2,1);
	LCD_WriteString("T:");
	MEAN_TEMP_FILTER_Runnable();
	fire_system_temp = MEAN_TEMP_FILTER_Read();
	LCD_WriteNumber(fire_system_temp/10);
	LCD_WriteChar('.');
	LCD_WriteNumber(fire_system_temp%10);
	LCD_WriteChar('C');
	LCD_WriteString(" ");
	LCD_SetCursor(2,9);
	/*
	LCD_WriteString("S:");
	fire_system_smoke = SMOKE_PERCENTAGE_READ_MQ2();
	LCD_WriteNumber(fire_system_smoke);
	LCD_WriteChar('%');
*/
	
	LCD_WriteString("P:");
	fire_system_smoke = PRESSURE_Read_MPX4115();
	LCD_WriteNumber(fire_system_smoke/1000);
	LCD_WriteChar('.');
	LCD_WriteNumber((fire_system_smoke%1000)/100);
	LCD_WriteString(" ");
	
	
	LCD_SetCursor(1,1);
	LCD_WriteString("(F.M) ");
	LCD_WriteString("PASS:");
	LCD_SetCursor(1,12+password_count);
		
	
	received_key = KEYPAD_Getkey();  // get numbers for operations

	if (received_key != NO_KEY_PRESSED){  // any button is pressed
		
		if ((password_count<PASSWORD_DIGITS_NUMBER) && (received_key >= '0') && (received_key <= '9'))
		{
			password_count++;
			LCD_WriteChar(received_key);
			user_password = (10*user_password) + (received_key - '0');
			
		}else if (received_key == '=') 
		{
			
			password_count = 0;
			if (user_password == fire_system_password)
			{
				normal_mode_on=0;
				heat_mode_on=0;        // these 3 variables to handle instructions in the 3 modes functions that must be performed only 1 time without repeating
				fire_mode_on=0;
				
				DIO_WritePin(RED_LED , LOW);
				
				DIO_WritePin(BUZZER , LOW);  // BUZZER OFF
				
				MOTOR_Stop(PUMP_MOTOR);       //pump off
				fire_system_mode = Normal_Mode;
			}
			
			break;
			
		}
		
		
	}

	}
}


void FIRING_SYSTEM(void){
	
	
	if (fire_system_mode == Normal_Mode)
	{
		NORMAL_STATE();
		
	}else if (fire_system_mode == Heat_Mode)
	{
		HEAT_STATE();
		
	}else if (fire_system_mode == Fire_Mode)
	{
		FIRE_STATE();
	}
	
	
	
	
	
	if ( (fire_system_mode == Normal_Mode) && (fire_system_temp > 500) )   // transition to heat mode
	{
		fire_system_mode = Heat_Mode;
		
		normal_mode_on=0;
		heat_mode_on=0;        // these 3 variables to handle instructions in the 3 modes functions that must be performed only 1 time without repeating
		fire_mode_on=0; 
		           
	}else if ( (fire_system_mode == Heat_Mode) && (fire_system_temp < 450) )   // transition to normal mode
	{
		fire_system_mode = Normal_Mode;
		
		DIO_WritePin(YELLOW_LED , LOW);
		
		normal_mode_on=0;
		heat_mode_on=0;        // these 3 variables to handle instructions in the 3 modes functions that must be performed only 1 time without repeating
		fire_mode_on=0; 
		           
	}else if ( (fire_system_mode == Heat_Mode) && (fire_system_smoke > 60000) )   // transition to fire mode
	{
		fire_system_mode = Fire_Mode;
		
		DIO_WritePin(YELLOW_LED , LOW);
		
		normal_mode_on=0;
		heat_mode_on=0;        // these 3 variables to handle instructions in the 3 modes functions that must be performed only 1 time without repeating
		fire_mode_on=0; 
		           
	}
	
	
	
	
	
}




/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/


// main.c



#define F_CPU 8000000
#include <util/delay.h>

#include "Std_Types.h"
#include "Memory_Map.h"
#include "Utils.h"
#include "DIO_Int.h"
#include "MOTOR_Int.h"
#include "ECU_Int.h"
#include "MOVE_Int.h"
#include "STEPPER_Int.h"
#include "LCD_Int.h"
#include "KEYPAD_Int.h"
#include "ADC_Int.h"
#include "SENSORS_Int.h"
#include "MeanTempFilter_Int.h"
#include "FIRING_SYSTEM_Int.h"


int main(void)
{

	DIO_Init();
	LCD_Init();
	KEYPAD_Init();
	ADC_Init(VREF_AVCC , ADC_PRESCALER_64);
	MEAN_TEMP_FILTER_Init();
	
	
	
	while (1)
	{
		
	
		FIRING_SYSTEM();
		
	}

	
}