/**
 * @file Time0.h
 * @author adem alnajjar (adem.marangoz95@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef TIME0_H_
#define TIME0_H_

//_______________________________ INCLUDES _____________________________________
#include "Common.h"

//==============================================================================


//_________________________ Explanation of Time Modes __________________________
/**
 * 1- Normal :
 *          // In this mode the counting direction is always up and no counter 
            clear is performed
            // The TOV0(Ovrflow flag) will be set as the TCNT0 becomes zero
            // The counter simply overrun when counter register equal TOP(0xFF)
             and restarts from the bottom (0x00)
            // The output compare unit can be used to generate interrupt at some
             given time
            // NOTE : using the output comapare to generate waveforms in Normal 
            mode is not recommended, since this will occupy too much of the CPU
            time 


 * 2 - PWM Phase :
            // The counter counts repeatedly form BOTTOM(0x00) to MAX(0xFF) and 
            then form MAX(0xFF) to BOTTOM(0x00)  
            // in noninverting comapre output mode, the Output Compare(OC0) is 
            cleared on the compare match between TCNT0 and OCR0 while 
            upcounting, and set on the copmare match while downcounting
            // The Timer/Counter Overflow Flag (TOV0) is set each time the 
            counter reaches BOTTOM. The Interrupt Flag can be used to generate 
            an interrupt each time the counter reaches the BOTTOM value.
            // In phase correct PWM mode, the compare unit allows generation of
            PWM waveforms on the OC0 pin. 
            // Focnpcpwm = (Fclk_i/o)/(N.510)
            // The extreme values for the OCR0 Register represent special cases
            when generating a PWM waveform output in the phase correct PWM mode.
            If the OCR0 is set equal to BOTTOM, the output will be continuously 
            low and if set equal to MAX the output will be continuously high for
            noninverted PWM mode. For inverted PWM the output will have the
            opposite logic values.

 * 3 - CTC :
            // also called Clear Timer on Compare Match(CTC) Mode
            // Is used to manipulate the counter resolution
            // In CTC mode the ocunter is cleared to zero when the counter value 
            (TCNT0) matches the OCR0
            // The OCR0 defines th TOP(0xFF) value for the counter
            //This mode allows greater control of the compare match output 
            frequency.
            // An interrupt can be generated each time the counter value reaches
            the TOP(0xFF or OCR0) value by using the OCF0 flag if interrupt enable
            // Changgin TOP when counter counting if the new value written to 
            OCR0 lower then the current value of TCNT0, hte counter will miss 
            the comapre match. The ocunter will then have to count to tis 
            maximum value(0xFF) and warp around starting at 0x00 before teh 
            compare match can occur.
            // For generating waveform output in CTC mode, the OC0 output can be
            set to toggle its logical level on each compare match by setting 
            Compare Output mode bits to toggle mode (COM1:0 = 1) 
            // Focn = (Fclk_i/o)/(2.N.OCRn) base on the equation The waveform
            generated will have maximum frequency of Focn = (Fclk/2) if OCRn = 0
            and N = 1.
            // The N variable represnets the prescale factor (1, 8, 64, 256, 1024)

 * 4 - PWM Fast : 
            // The counter counts form BOTTOM(0x00) to MAX(0xFF) then restart 
            from BOTTOM
            // in non-inverting compare output mode, the output copmare (OC0) is
            cleared on the compare match between TCNT0 and OCR0 and Set At BOTTOM
            // The timer/counter overflow flag(TOV0) is set each time the counter
            reaches MAX(0xFF)
            // If the interrupt is enabled, the the interrupt handler routine 
            can be used for updating the compare value.
            // Setting the COM01:0 bits to 2 will produce a non-inverted PWM and
            an inverted PWM output can be generated by setting the COM01:0 to 3
            (See Table 40 on page 81).
            // The PWM waveform is generated by setting (or clearing) the OC0
            Register at the compare match between OCR0 and TCNT0, and clearing
            (or setting) the OC0 Register at the timer clock cycle the counter
            is cleared (changes from MAX to BOTTOM).
            // Focnpwm = (Fclk_i/o)/(N.256)


 */


//==============================================================================

// @ref Timr0_Modes_define
typedef enum
{
    Normal_Timer_Mode = 0,             
    PWM_Phase = 8,          
    CTC = 64,                
    PWM_Fast = 72            
}En_Time0_Mode;

typedef enum
{
    Normal_Com_Mode = 0,
    Toggle_OC0 = 16,
    Clear_OC0,
    Set_OC0
}En_Compare_Match_Mode;

typedef enum
{
    Disable_clk = 0,
    CLK_Div_1,
    CLK_Div_8,
    CLK_Div_64,
    CLK_Div_256,
    CLK_Div_1024,
    External_CLK_T0_Falling_Edge,
    External_CLK_T1_Rising_Edge
}En_CLK_Select;

typedef enum
{
    Timer_Disable_Int = 0,
    Enable_OUT_Compare_Match_INT,
    Enable_Over_INT,
    Enable_Com_Over,
}En_Intrrupt_Mode;

//_________________________________ TIMER0_API__________________________________
typedef struct 
{
    En_Time0_Mode Mode;
    En_CLK_Select CLK_Mode;
    En_Intrrupt_Mode Interrupt;
    En_Compare_Match_Mode Com_Mode;
    uint8_t Compare_Time; 
}St_Timer0_API;

//==============================================================================


//______________________________ GLOBAL FUNCTIONS ______________________________

uint8_t Timer0_Init(St_Timer0_API *Timerx);
uint8_t Change_Timer_Mode(St_Timer0_API *Timerx);
uint8_t Timer0_Stop(St_Timer0_API *Timerx);
uint8_t Set_Compare_Value(St_Timer0_API *Timerx, uint8_t *Com_val);
uint8_t Timer0_Get_Compare_Val(St_Timer0_API *Timerx, uint8_t *Com_val);
uint8_t Timer0_Set_Counter_Val(St_Timer0_API *Timerx, uint8_t *Count_val);
uint8_t Timer0_Get_Counter_Val(St_Timer0_API *Timerx, uint8_t *Count_val);
uint8_t En_Dis_Interrupt(St_Timer0_API *Timerx);

//==============================================================================



#endif