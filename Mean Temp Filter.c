
// mean_temp_filter_Cfg.c



#include "Std_Types.h"
#include "MeanTempFilter_Cfg.h"

u16 filtered_temp;
u16 mean_temp_filter[MEAN_TEMP_FILTER_SAMPLES] = {0};
u8 mean_temp_filter_index = 0;



/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/


// mean_temp_filter_Cfg.h 



#ifndef MEANTEMPFILTER_CFG_H_
#define MEANTEMPFILTER_CFG_H_

#define  MEAN_TEMP_FILTER_SAMPLES  10
#define MEAN_TEMP_FILTER_SENSOR_READ  TEMP_Read_LM35   //TEMP_Read_LM35 >> return temp *10



#endif /* MEANTEMPFILTER_CFG_H_ */



/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/


// mean_temp_filter_Priv.h 





#ifndef MEANTEMPFILTER_PRIV_H_
#define MEANTEMPFILTER_PRIV_H_

extern u16 filtered_temp;
extern u16 mean_temp_filter[MEAN_TEMP_FILTER_SAMPLES];
extern u8 mean_temp_filter_index ;


#endif /* MEANTEMPFILTER_PRIV_H_ */ 



/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/


// mean_temp_filter_Int.h 







#ifndef MEANTEMPFILTER_INT_H_
#define MEANTEMPFILTER_INT_H_


void MEAN_TEMP_FILTER_Init(void);

void MEAN_TEMP_FILTER_Runnable(void);

u16 MEAN_TEMP_FILTER_Read(void);

u16 MEAN_TEMP_NO_FILTER_Read(void);



#endif /* MEANTEMPFILTER_INT_H_ */




/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************/


// mean_temp_filter_Prg.c



#include "Std_Types.h"
#include "ADC_Int.h"
#include "SENSORS_Int.h"
#include "MeanTempFilter_Cfg.h"
#include "MeanTempFilter_Priv.h"
#include "MeanTempFilter_Int.h"



void MEAN_TEMP_FILTER_Init(void){

	u8 index;
	
	filtered_temp = MEAN_TEMP_FILTER_SENSOR_READ();
	
	for (index=0 ; index<MEAN_TEMP_FILTER_SAMPLES ; index++)  // fill the array with the first temp as initial condition
	{
		mean_temp_filter[index] = filtered_temp ;
	}
}



void MEAN_TEMP_FILTER_Runnable(void){    // take one new temp and calculate the mean
	
	u8 index;
	u16 temp;
	u32 sum = 0;
	
	temp = MEAN_TEMP_FILTER_SENSOR_READ();
	mean_temp_filter[mean_temp_filter_index] = temp ;   //add new read in the array
	mean_temp_filter_index++;
	
	if (mean_temp_filter_index == MEAN_TEMP_FILTER_SAMPLES)  // if add read in last index , back to first index
	{
		mean_temp_filter_index=0;
	}
	
	for (index=0 ; index<MEAN_TEMP_FILTER_SAMPLES ; index++)  // get mean read temp
	{
		sum = sum + mean_temp_filter[index];
	}
	
	filtered_temp = sum/MEAN_TEMP_FILTER_SAMPLES ;   //mean temp
	
}

u16 MEAN_TEMP_FILTER_Read(void){
	
	return filtered_temp;	
}

u16 MEAN_TEMP_NO_FILTER_Read(void){
	
	if (mean_temp_filter_index == 0)  // it means that array full and index back to first , so read last read
	{
		return mean_temp_filter[MEAN_TEMP_FILTER_SAMPLES - 1];   // last element in array
	}
	else{
		return mean_temp_filter[mean_temp_filter_index - 1]; //-1 because will be increment after new read added , we want here the last read not the index of the next read

	}
	
}