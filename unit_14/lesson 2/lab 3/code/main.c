/*
*@author 	Adem Alnajjar
*@data		24.11.2021
*/

//_________________________________ Include Files ______________________________
#include "Application.h"
#include "Stm32f10xx_Common.h"

//==============================================================================

//______________________________ Generic Macro _________________________________


//==============================================================================


//_____________________ Generica Function & Data Structure _____________________


    //______________________ Data Structre of Drives ___________________________

    //==========================================================================

// uint16_t Rx_Buff[14] = {0};

//==============================================================================
int main(void)
{
	
    config_Drives_and_Perpherals(); // config Drives and Peripherals  
    Infinite_loop();

    return 1;
}
