/**
 * @file Application.C
 * @author adem alnajjar (adem.alnajjar@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */


//------------------------------- INCLUDE FILES --------------------------------
#include "Application.h"
#include "defines.h"	
#include <string.h>
#include <stdio.h>
#include "Send_Atmega32.h"
//==============================================================================


//----------------------------- EXTERNAL OBJECTES ------------------------------
GPIO_InitTypeDef LEDS = {0};
LCD_16_2 Lcd_config = {0};
St_Key_pad key_pad_config = {0};
St_7_segment seven_config = {0};
St_Uart_API uart1_config = {0};
St_Uart_API uart2_config = {0};
St_SPI_API spi1_config = {0};
St_SPI_API spi2_config = {0};
St_SPI_API spi3_config = {0};
St_I2C_API i2c1_config = {0};
St_I2C_API i2c2_config = {0};
GPIO_InitTypeDef NSS_EEPROM = {0};
GPIO_InitTypeDef NSS_ATmega1 = {0};
GPIO_InitTypeDef NSS_ATmega2 = {0};
St_EEPROM_25xx256_Typedef EEPORM_25xx_config = {0};
extern const uint8_t KeyPad_Keys[KeyPad_Source][KeyPad_Drain];
//==============================================================================


//------------------------------ GLOBAL OBJECTES -------------------------------
#define SPI_Maseter_En              1

//----------------- 7Segment Objects -----------------
uint8_t Max_Grag = 9;

//----------------- LCD Objects ----------------- 
uint8_t _Display_Welcom[] = "    Welcome!  \0";
uint8_t _Display_Put_Card[] = "  Put The Card\0";
uint8_t _Display_There_is_an[] = "  There is an \0";
uint8_t _Display_There_is_no[] = "   There is no  \0";
uint8_t _Display_Empty_Garage[] = "  Empty garage  \0";
uint8_t _Display_Sorry[] = "      Sorry     \0";
uint8_t _Display_Try_again[] = "    Try again   \0";
uint8_t _Display_Opened_Gate[] = " Gate Will Open \0";
uint8_t _Display_Closed_Gate[] = "  Closed Gate   \0";
uint8_t _Display_Correct_ID[] =  "   Correct ID   \0";
uint8_t _Display_Not_Correct_ID[] =  " Not Correct ID \0";
En_LCD_State LCD_state = IDLE;
uint32_t Timeout = 0;


//----------------- UART1 Objects -----------------
uint16_t _Dispay_Exit_admin_mode[] = {'E','x','i','t',' ','a','d','m','i','n',' ','m','o','d','e','\r','\t','\0'};
uint16_t _Display_Welcome_Admin[] = {' ',' ',' ',' ',' ','W','e','l','c','o','m','e',' ','A','d','m','i','n',' ',' ',' ',' ',' ','\r','\t','\0'};
uint16_t _Display_1_Add_ID[] = {'1','-',' ','A','d','d',' ','I','D','\r','\t','\0'};
uint16_t _Display_2_Remove_ID[] = {'2','-',' ','R','e','m','o','v','e',' ','I','D','\r','\t','\0'};
uint16_t _Display_Hash_Exit[] = {'#','-',' ','E','x','i','t',' ','A','d','m','i','n',' ','M','o','d','e','\r','\t','\0'};
uint16_t _Display_Enter_ID[] = {'E','n','t','e','r','i','n','g',' ','I','D',' ',':',' '};
uint16_t _Display_Enter[] = {'\r','\t','\0'};
uint16_t _Display_Worng_Entry[] = {'W','r','o','n','g',' ','e','n','t','r','y',',',' ','t','r','y',' ','a','g','a','i','n','\r','\t','\0'};
uint16_t _Display_ID_Added[] = {'I','D',' ','A','d','d','e','d','\r','\t','\0'};
uint16_t _Dispay_ID_Removed[] = {'I','D',' ','h','a','s',' ','b','e','e','n',' ','r','e','m','o','v','e','d','\r','\t','\0'};
uint16_t _Display_Try_Enter_Again[] = {'Y','o','u',' ','c','a','n',' ','e','n','t','e','r',' ','a','g','a','i','n','\r','\t','\0'};
uint16_t _Display_Enter_Admin_ID[] = {'P','l','e','a','s','e',' ','e','n','t','e','r',' ','A','d','m','i','n',' ','I','D','\r','\t','\0'};
uint16_t _Display_Timeout_Admin[] = {'S','w','i','t','c','h',' ','t','o',' ','w','a','i','t','i','n','g',' ','f','o','r',' ','t','h','e',' ','u','s','e','r',' ','t','o',' ','e','n','t','e','r',' ','t','h','e',' ','I','D','\r','\t','\0'};
uint8_t correct_character[] = "1234567890";


//----------------- SPI Objects -----------------
uint16_t ID_data[4] = {0};
uint16_t Reset_Data[4] = {0xFF, 0xFF, 0xFF, 0xFF};
uint16_t First_ID[] = {'1', '2', '1', '1'};
uint16_t Second_ID[] = {'0', '2', '1', '0'};
uint16_t Third_ID[] = {'2', '2', '1', '0'};
uint16_t Fourth_ID[] = {'1', '1', '1', '0'};

//----------------- Keypad Objects -----------------
uint8_t Key_Buffer[8] = {0};
uint8_t Key_Buffer_counter = 0;
uint8_t Admin_Pass[3] = {'1', '2', '3'};
uint8_t Admin_mode = 0;
uint8_t Admin_state = 0;
//==============================================================================


//------------------------------- LOCAL OBJECTS --------------------------------
void Rx_Uart1(St_Uart_API *UARTx);
void Rx_Uart2(St_Uart_API *UARTx);
//==============================================================================


//--------------------------------- FUNCTIONS ----------------------------------

void config_Drives_and_Perpherals(void)
{
    /* Enable AFIO and GPIOA Clock */
    __APB2ENR_IOPAEN_En(); // Enable GPIOA Clock
    __APB2ENR_IOPBEN_En(); // Enable GPIOB Clock
    __APB2ENR_AFIOEN_En(); // Enable Alterntif Clock
    

    //____________________________ Systick Config ______________________________
    Systick_API.Clock_Source = Processor_Clock_AHB;
    Systick_API.Current_Value = 0;
    Systick_API.Enable_Interrupt = Disable_Systick_Req;
    Systick_API.Reload_Value = Microsecond_Prescale;
    Init_Systick();
    
    //==========================================================================


    //_________________________ Config LCD_Driver ______________________________
    Lcd_config.Data_Port = GPIOA;
	Lcd_config.Enable_Port = GPIOB;
	Lcd_config.RS_Port = GPIOB;
	Lcd_config.R_W_Port = GPIOB;
	Lcd_config.Enable_Pin = GPIO_PIN_0;
	Lcd_config.R_W_Pin = GPIO_PIN_1;
	Lcd_config.RS_Pin = GPIO_PIN_3;
    #ifdef LCD_8_Bit
    Lcd_config.Data_Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    // Lcd_config.Data_Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
    #endif
    #ifdef LCD_4_Bit
    // Lcd_config.Data_Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    Lcd_config.Data_Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    #endif
    LCD_init(&Lcd_config);
    //==========================================================================


    //_________________________ Config KeyPad_Driver ___________________________
    key_pad_config.Soruce.Port = GPIOB;
    key_pad_config.Soruce.Pins = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    key_pad_config.Drain.Port = GPIOA;
    key_pad_config.Drain.Pins = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4;
    Key_pad_init(&key_pad_config);
    //==========================================================================


    //_________________________ Config 7_Segment_Driver ________________________
    seven_config.Port = GPIOB;
    seven_config.Pin_D[0] = GPIO_PIN_11;
    seven_config.Pin_D[1] = GPIO_PIN_12;
    seven_config.Pin_D[2] = GPIO_PIN_13;
    seven_config.Pin_D[3] = GPIO_PIN_14;
    init_seven_segment(&seven_config);
    //==========================================================================


    // _________________________ Config UART1_Driver ___________________________
    uart1_config.baudrate = (uint32_t)115200;
    uart1_config.Hw_Flow_CTRL = CTS_RTS_None;
    uart1_config.Mode = Enable_RX_Tx;
    uart1_config.Parity = Parity_None;
    uart1_config.Stop_bit = Stop_1_bit;
    uart1_config.Word_Len = Payload_8_bit;
    uart1_config.UARTx = UART1;
    uart1_config.interrupt = En_RX_Inter;
    uart1_config.P_Rx_CallBack = Rx_Uart1;
    Init_Uart(&uart1_config);
    //==========================================================================
    
    // _________________________ Config UART2_Driver ___________________________
    uart2_config.baudrate = (uint32_t)115200;
    uart2_config.Hw_Flow_CTRL = CTS_RTS_None;
    uart2_config.Mode = Enable_RX_Tx;
    uart2_config.Parity = Parity_None;
    uart2_config.Stop_bit = Stop_1_bit;
    uart2_config.Word_Len = Payload_8_bit;
    uart2_config.UARTx = UART2;
    uart2_config.interrupt = En_RX_Inter;
    uart2_config.P_Rx_CallBack = Rx_Uart2;
    Init_Uart(&uart2_config);
    //==========================================================================

    //__________________________ Config SPI_Driver _____________________________
    spi1_config.SPI_Inst = SPI1 ;
    spi1_config.SPI_Mode = Full_Duplex;
    spi1_config.BaudRate = FPCLK_div_4;
    #if (SPI_Maseter_En == 1)
        spi1_config.Master_Select = SPI_Master;
    #else
        spi1_config.Master_Select = SPI_Slave;
    #endif
    //spi1_config.Half_duplex_Mode = ;
    spi1_config.NSS_Hardware_Mode = NSS_Hardware_Output_Dis_MultiMaster_En;
    spi1_config.Slave_Select_Software = Software_Mang_Enable;
    spi1_config.Frame_format = MSB_Trans;
    spi1_config.Data_Legnth_format = Data_8_bit;
    spi1_config.CLK_Phase_Polarity = Sample_Ris_Setup_Fall;
    spi1_config.Rx_Tx_interrupt = SPI_interrupt_Dis;
    Init_SPI(&spi1_config);
    //==========================================================================

    // _________________ Config NSS PIN As a Normal Pin ________________________
    NSS_EEPROM.Pin = GPIO_PIN_8;
    #if (SPI_Maseter_En == 1)
        NSS_EEPROM.Speed = GPIO_SPEED_FREQ_2MHZ;
        NSS_EEPROM.Mode = GPIO_MODE_OUTPUT_PP ;
        Init_GPIO(GPIOB,&NSS_EEPROM);
        Set_pin(GPIOB, NSS_EEPROM.Pin);
    #else
        NSS_EEPROM.Speed = GPIO_SPEED_INPUT_Mode;
        NSS_EEPROM.Mode = GPIO_MODE_AF_INPUT;
    #endif
    //==========================================================================
    
    //_____________________________ NSS Atmega32_1 _____________________________
    NSS_ATmega1.Pin = GPIO_PIN_9;
    NSS_ATmega1.Speed = GPIO_SPEED_FREQ_2MHZ;
    NSS_ATmega1.Mode = GPIO_MODE_OUTPUT_PP ;
    Init_GPIO(GPIOB,&NSS_ATmega1);
    Set_pin(GPIOB, NSS_ATmega1.Pin);
    //==========================================================================

    //_____________________________ NSS Atmega32_2 _____________________________
    NSS_ATmega2.Pin = GPIO_PIN_10;
    NSS_ATmega2.Speed = GPIO_SPEED_FREQ_2MHZ;
    NSS_ATmega2.Mode = GPIO_MODE_OUTPUT_PP ;
    Init_GPIO(GPIOB,&NSS_ATmega2);
    Set_pin(GPIOB, NSS_ATmega2.Pin);
    //==========================================================================

    //______________________________ LEDS Configs ______________________________
    LEDS.Pin = GPIO_PIN_8 | GPIO_PIN_11;
    LEDS.Speed = GPIO_SPEED_FREQ_2MHZ;
    LEDS.Mode = GPIO_MODE_OUTPUT_PP ;
    Init_GPIO(GPIOA,&LEDS);
    //==========================================================================

    //__________________________ Config EEPORM 25xx ____________________________
    EEPORM_25xx_config.EEPROM_SPI = &spi1_config;
    EEPORM_25xx_config.GPIOx = GPIOB;
    EEPORM_25xx_config.NSS_Pin = GPIO_PIN_8;
    //==========================================================================

    //___________________________ Config EEPROM ________________________________
    // EEPORM_Init(); 
    //==========================================================================

    // ___________________________ Config Timer ________________________________
    // tim1_config.Instance = TIM1;
    // tim1_config.Prescaler = 0;
    // tim1_config.CounterMode = TIM_COUNTERMODE_UP;
    // tim1_config.Period = 0xFFFF;
    // tim1_config.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    // tim1_config.RepetitionCounter = 0;
    // tim1_config.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    // Init_Timer(&tim1_config);
    //==========================================================================


    //___________________________ Config Pin_Out _______________________________
    // GPIO_InitTypeDef gpio_pin_out = {0};
    // gpio_pin_out.Mode = GPIO_MODE_OUTPUT_PP;
    // gpio_pin_out.Speed = GPIO_SPEED_FREQ_10MHZ;
    // gpio_pin_out.Pin = GPIO_PIN_14;
    // Init_GPIO(GPIOA,&gpio_pin_out);
    //==========================================================================

    //________________________ Config EXTI Pin Driver __________________________
    // St_EXTI_config exti_config1 = {0};
    // exti_config1.EXTI_State = EXTI_Enable;
    // exti_config1.EXTI_Trigger = FALLING_EDGE_Trigger;
    // exti_config1.P_IRQ_CallBack = Exti15_CallBack;
    // exti_config1.EXTI_Pin = EXTI0PA15;
    // Init_EXTI(&exti_config1);
    //==========================================================================

}

/**
 * @brief Initialize the system
 */
void Init_Component(void)
{
    // Init 7 Segment to Display 9 character 
    Display_seven_segment(&seven_config, Max_Grag);
    
    // Init LCD 
    Write_String_with_coordinator(&Lcd_config, _Display_Welcom,  0 ,First_R);
    delay_us(200);
    Write_String_with_coordinator(&Lcd_config, _Display_Put_Card,  0 ,Seconde_R);
    delay_us(200);
    Write_String_with_coordinator(&Lcd_config, _Display_There_is_an, 0 ,Third_R);
    delay_us(200);
    Write_String_with_coordinator(&Lcd_config, _Display_Empty_Garage,  0 ,Fourth_R);
    
    // Init Leds
    Run_Led(Red);
    
    // Add Some IDs in EEPROM
    Write_Bytes_EEPROM_25xx(&EEPORM_25xx_config, 0x0010, 4, First_ID);
    Write_Bytes_EEPROM_25xx(&EEPORM_25xx_config, 0x0014, 4, Second_ID);
    Write_Bytes_EEPROM_25xx(&EEPORM_25xx_config, 0x0018, 4, Third_ID);
    Write_Bytes_EEPROM_25xx(&EEPORM_25xx_config, 0x001C, 4, Fourth_ID);
    
    
}



/**
 * @brief The function of interrupting the data entered by the RFID1
 * @param UARTx : data structure for UART API
 */
void Rx_Uart1(St_Uart_API *UARTx)
{
    static uint16_t Input_ID1[8] = {0}; // RFID 1 Buffer
    static uint8_t Input_ID_counter1 = 0; // RFID 1 Buffer counter
    uint16_t *Rx_Buff = NULL;
    Receive_Char_Uart(UARTx->UARTx, Rx_Buff,Disable);
    Input_ID1[Input_ID_counter1] = *Rx_Buff;
    Input_ID_counter1++;
    if(Input_ID_counter1 == 8)
    {
        // Print enter after 8 charcter
        Send_String_Uart(UARTx->UARTx, _Display_Enter, Enable);
        // if Garage is not empty
        if(Max_Grag == 0)
        {
            Write_String_with_coordinator(&Lcd_config, _Display_Sorry,  0 ,Seconde_R);
            delay_us(200);
            Write_String_with_coordinator(&Lcd_config, _Display_There_is_no,  0 ,Third_R);
            delay_us(200);
            Write_String_with_coordinator(&Lcd_config, _Display_Empty_Garage,  0 ,Fourth_R);
        }else // if Garage is empty
        {
            // Convert part of the data entered as an address
            uint16_t add = Convert_Buffer16_to_Variable(Input_ID1, 3);
            uint16_t temp_id[4] = {0};
            Read_Byte_EEPROM_25xx(&EEPORM_25xx_config, add, 4, temp_id);

            // Check if the address is already exist
            if(comapre_two_string((uint8_t*)&Input_ID1[4], (uint8_t*)temp_id, 8))
            {
                // Decrease the Garage counter
                Inc_Dec_seven_segment(&seven_config, Dec);
                // Run Green Led
                Run_Led(Green);

                Write_String_with_coordinator(&Lcd_config, _Display_Correct_ID,  0 ,Seconde_R);
                delay_us(200);
                Write_String_with_coordinator(&Lcd_config, _Display_Opened_Gate,  0 ,Third_R);
                Send_to_controller(&spi1_config, Vehicle_entry_controller, Open_entry_gate1);
                
                LCD_state = Correct_ID_Open_Gate_Timeout;
                Timeout = 3000;
                
            }else
            {
                Write_String_with_coordinator(&Lcd_config, _Display_Not_Correct_ID,  0 ,Seconde_R);
                delay_us(200);
                Write_String_with_coordinator(&Lcd_config, _Display_There_is_no,  0 ,Third_R);
                delay_us(200);
                Write_String_with_coordinator(&Lcd_config, _Display_Empty_Garage,  0 ,Fourth_R);
                
                LCD_state = Correct_ID_Open_Gate_Timeout;
                Timeout = 3000;
            }
        }
        // Reset RFID 1 Buffer counter
        Input_ID_counter1 = 0;
    }
}


/**
 * @brief The function of interrupting the data entered by the RFID2
 * @param UARTx : data structure for UART API
 */
void Rx_Uart2(St_Uart_API *UARTx)
{
    static uint16_t Output_ID1[8] = {0}; // RFID 2 Buffer
    static uint8_t Output_ID_counter1 = 0; // // RFID 2 Buffer counter
    uint16_t *Rx_Buff1 = NULL;
    Receive_Char_Uart(UARTx->UARTx, Rx_Buff1,Disable);
    Output_ID1[Output_ID_counter1] = *Rx_Buff1;
    Output_ID_counter1++;
    if(Output_ID_counter1 == 8)
    {
        // Print enter after 8 charcter
        Send_String_Uart(UARTx->UARTx, _Display_Enter, Enable);
        
        // Convert part of the data entered as an address
        uint16_t add = Convert_Buffer16_to_Variable(Output_ID1, 3);
        uint16_t temp_id[4] = {0};
        Read_Byte_EEPROM_25xx(&EEPORM_25xx_config, add, 4, temp_id);
        // Check if the address is already exist
        if(comapre_two_string((uint8_t*)&Output_ID1[4], (uint8_t*)temp_id, 8))
        {
            // Increment  the Garage counter
            Inc_Dec_seven_segment(&seven_config, Inc);
            Send_to_controller(&spi1_config, Vehicle_exit_controller, Open_entry_gate1);
            if(Max_Grag == 0)
            {
                    Write_String_with_coordinator(&Lcd_config, _Display_Welcom,  0 ,First_R);
                    Write_String_with_coordinator(&Lcd_config, _Display_Put_Card,  0 ,Seconde_R);
                    Write_String_with_coordinator(&Lcd_config, _Display_There_is_an, 0 ,Third_R);
                    Write_String_with_coordinator(&Lcd_config, _Display_Empty_Garage,  0 ,Fourth_R);
            }
        }else
        {

        }
        // Reset RFID 1 Buffer counter
        Output_ID_counter1 = 0;
    }
    
}


/**
 * @brief is used to reaction of set an Soruce pin
 * @param Soruce_pin_index : Seted Soruce pin index
 * @param Drain_pin_index Seted Drain pin index
 */
void Reaction_Of_Prass(uint32_t Soruce_pin_index, uint32_t Drain_pin_index)
{
    // Check if prassed * key 
    if(KeyPad_Keys[Soruce_pin_index][Drain_pin_index] == '*') 
    { 
        // Clear Keypad Buffer
        Clear_Buffer(Key_Buffer, 0, sizeof(Key_Buffer));
        Key_Buffer_counter = 0; // reset Keypad buffer counter
        
        // check if entred to admin mode
        if(Admin_mode != 0)
        {
            Send_String_Uart(uart1_config.UARTx, _Display_Try_Enter_Again, Enable);
        }else
        {
            Send_String_Uart(uart1_config.UARTx, _Display_Enter_Admin_ID, Enable);
            Send_String_Uart(uart1_config.UARTx, _Display_Enter_Admin_ID, Enable);
        }
        LCD_state = Admin_Entry_star;
        Timeout = 1000;
        
    }else if(KeyPad_Keys[Soruce_pin_index][Drain_pin_index] == '#')
    {
        // Clear Keypad Buffer
        Clear_Buffer(Key_Buffer, 0, sizeof(Key_Buffer));
        Key_Buffer_counter = 0; // reset Keypad buffer counter
        // check if entred to admin mode
        if(Admin_mode != 0)
        {
            Send_String_Uart(uart1_config.UARTx, _Dispay_Exit_admin_mode, Enable);
        }
        // Exit from Admin mode
        Admin_mode = 0;

        LCD_state = Admin_Entry_Hash;
        Timeout = 1000;
    }else // Check if prassed number key
    {
        if(Admin_mode == 0)
        {
            Key_Buffer[Key_Buffer_counter] = KeyPad_Keys[Soruce_pin_index][Drain_pin_index];
            Key_Buffer_counter++;
            // After prassed 3 key ID Admin
            if(Key_Buffer_counter == 3)
            {
                // Check if Admin ID correct
                if((Key_Buffer[0] == Admin_Pass[0]) && (Key_Buffer[1] == Admin_Pass[1]) && (Key_Buffer[2] == Admin_Pass[2]))
                {
                    Send_String_Uart(uart1_config.UARTx, _Display_Welcome_Admin, Enable);
                    Send_String_Uart(uart1_config.UARTx, _Display_1_Add_ID, Enable);
                    Send_String_Uart(uart1_config.UARTx, _Display_2_Remove_ID, Enable);
                    Send_String_Uart(uart1_config.UARTx, _Display_Hash_Exit, Enable);
                    
                    // Transition to admin mode
                    Admin_mode = 1;
                    LCD_state = Admin_Correct_ID_Timeout;
                    Timeout = 1000;

                }else // if Admin ID not correct
                {
                    Send_String_Uart(uart1_config.UARTx, _Display_Worng_Entry, Enable);
                    LCD_state = Admin_Not_Correct_ID_Timeout;
                    Timeout = 1000;
                }
                // Reset Keypad Buffer counter
                Key_Buffer_counter = 0; 
            }

        }else
        {      
            // Check the operation
            if(Admin_state == 0)
            {
                // Check if prassed 1-Add ID
                if(KeyPad_Keys[Soruce_pin_index][Drain_pin_index] == '1')
                {
                    Send_String_Uart(uart1_config.UARTx, _Display_Enter_ID, Enable);
                    Admin_state = 1;
                    LCD_state = Admin_Correct_Entry;
                    Timeout = 1000;
                    return ;
                }// Check if prassed 1-Remove ID
                else if(KeyPad_Keys[Soruce_pin_index][Drain_pin_index] == '2')
                {
                    Send_String_Uart(uart1_config.UARTx, _Display_Enter_ID, Enable);
                    Admin_state = 2;
                    LCD_state = Admin_Correct_Entry;
                    Timeout = 1000;
                    return ;
                }else // Check if prassed worng ID
                {
                    Send_String_Uart(uart1_config.UARTx, _Display_Worng_Entry, Enable);
                    LCD_state = Admin_Not_Correct_Entry;
                    Timeout = 1000;
                    return ;
                }
            }
            // Print Entred ID
            Send_Char_Uart(uart1_config.UARTx , (uint16_t*)&KeyPad_Keys[Soruce_pin_index][Drain_pin_index],Enable);
            Key_Buffer[Key_Buffer_counter] = KeyPad_Keys[Soruce_pin_index][Drain_pin_index];
            Key_Buffer_counter++;
            // Check if Entred ID
            if(Key_Buffer_counter == 8)
            {
                // Print enter after 8 charcter
                Send_String_Uart(uart1_config.UARTx, _Display_Enter, Enable);
                // Convert part of the data entered as an address
                uint16_t add = Convert_Buffer8_to_Variable(Key_Buffer, 3);
                // Convert Keypad Buffer to uint16_t an Buffer to send to EEPROM
                Convert_uint8_t_to_uint16_t(ID_data, Key_Buffer, 0, 4, 4);
                // Check if prassed 1 to Add ID
                if(Admin_state == 1)
                {
                    Write_Bytes_EEPROM_25xx(&EEPORM_25xx_config, add, 4,ID_data);
                    Send_String_Uart(uart1_config.UARTx, _Display_ID_Added, Enable);
                } // Check if prassed 2 to Remove ID
                else if(Admin_state == 2)
                {
                    Write_Bytes_EEPROM_25xx(&EEPORM_25xx_config, add, 4,Reset_Data);
                    Send_String_Uart(uart1_config.UARTx, _Dispay_ID_Removed, Enable);
                } // Check if prassed worng key
                else
                {
                    Send_String_Uart(uart1_config.UARTx, _Display_Worng_Entry, Enable);
                    return ;
                }
                
                Admin_state = 0; // Reset Operation state
                Key_Buffer_counter = 0; // Reset Key Buffer counter
            }
            LCD_state = Admin_Entry_Timeout;
            Timeout = 1000;
        }
        
    }
}


/**
 * @brief This function to turn on one of the Leds
 * @param state The color of the lamp to be turned on
 */
void Run_Led(En_Led_state state)
{
    if(state == Green)
    {
        Reset_pin(GREAN_LED_PORT, GREAN_LED_PIN);
        Set_pin(RED_LED_PORT, RED_LED_PIN);
    }else
    {
        Set_pin(GREAN_LED_PORT, GREAN_LED_PIN);
        Reset_pin(RED_LED_PORT, RED_LED_PIN);
    }
}


/**
 * @brief This function to clear buffer
 * @param buff Start Address of buffer
 * @param st_index Start index of buffer to clear
 * @param end_index End index of buffer to clear
 */
void Clear_Buffer(uint8_t *buff, uint8_t st_index, uint8_t end_index)
{
    for(uint8_t counter = st_index; counter < end_index; counter++)
    {
        *(buff + counter) = '\0';
    }
}


/**
 * @brief This function to convert uint8_t buffer to uint16_t buffer without MSB
 * @param _16_buff start address of uint16_t buffer
 * @param _8_buff start address of uint8_t buffer
 * @param st_16_index Start index of uint16_t buffer to convert
 * @param st_8_index Start index of uint8_t buffer to convert
 * @param len The number of the element to be converted
 */
void Convert_uint8_t_to_uint16_t(uint16_t *_16_buff, uint8_t *_8_buff, uint8_t st_16_index, uint8_t st_8_index, uint8_t len)
{
    uint8_t counter = len;
    while(len)
    {
        *(_16_buff + st_16_index +(counter - len)) = *(_8_buff + st_8_index +(counter - len));
        len--;
    }
}

/**
 * @brief This function is used to convert a character from to a hex number
 * @param Buff Start charcters Address
 * @param len The number of characters to convert to hex
 * @note @param[len] One less number than the number of the character to be converted
 * @example uint8_t example[] = {'1','2','3','4'};
 * Convert_Buffer8_to_Variable(example, 3); // len = 3
 * @example uint8_t example[] = {'1','2','3','4','5','6'};
 * Convert_Buffer8_to_Variable(&example[3], 2); // len = 2
 * @example uint8_t example[] = "123456";
 * Convert_Buffer8_to_Variable(example, sizeof(example) - 2); 
 * @example uint8_t example[] = "123456";
 * Convert_Buffer8_to_Variable(&example[2], 4); 
 * @return uint64_t Hex value
 */
uint64_t Convert_Buffer8_to_Variable(uint8_t *Buff, uint8_t len)
{
    uint64_t temp = 0;
    while(len)
    {
        temp += *(Buff) - 0x30;
        temp = temp << 4;
        Buff++;
        len--;
    }
    temp += *(Buff) - 0x30;

    return temp;
}


/**
 * @brief This function used to convert uint16_t buffer to variable
 * @param Buff Start address of buffer
 * @param len The number of the element to be converted
 * @return uint64_t converted buffer Address to variable 
 */
uint64_t Convert_Buffer16_to_Variable(uint16_t *Buff, uint8_t len)
{
	uint64_t temp = 0;
	while(len)
	{
		temp += (*(Buff) - 0x30);
		temp = temp << 4;
		Buff++;
		len--;
	}
	temp += *(Buff) - 0x30;

	return temp;
}


/**
 * @brief It is used to find the similarity of characters between two buffers
 * @param buff1 First Buffer
 * @param buff2 Second Buffer
 * @param buff1_len The number of the element to be check in First buffer
 * @param buff2_len The number of the element to be check in Second buffer
 * @param type The second buffer type
 * @return uint8_t 0 : There is no similarity
 *                 1 : There is a similarity
 */
uint8_t search_char(uint8_t *buff1, uint8_t *buff2, uint8_t buff1_len, uint8_t buff2_len, data_type type)
{
	uint8_t retval = 0;
	uint8_t const_buff2_len = buff2_len;
	uint8_t *const_buff2 = buff2;
	while(buff1_len)
	{

		while(buff2_len)
		{

			if(*buff1 == *buff2)
			{
				retval++;
				break;
			}
            buff2 = buff2 + 1 + type;
			buff2_len--;
		}
		buff2 = const_buff2;
		buff1 = buff1 + 1;
		buff2_len = const_buff2_len;
		buff1_len--;
	}
	return retval;
}


/**
 * @brief This function is used to Increment and Decrement 7segment display
 * @param seven_segment 
 * @param Index Inc : To increment 7segment display
 *              Dec : To Decrement 7segment display
 * @return uint8_t 1
 */
uint8_t Inc_Dec_seven_segment(St_7_segment *seven_segment,Inc_Dec_seg Index)
{
    uint8_t retval = 1;
    switch (Index)
    {
        case Inc:
            if(Max_Grag > 8){break;}
            Display_seven_segment(&seven_config, ++Max_Grag);
            break;
        case Dec:
            if(Max_Grag < 1){break;}
            Display_seven_segment(&seven_config, --Max_Grag);
            break;
        default:
            retval = 0;
            break;
    }
}


/**
 * @brief This function is used to check the parity of two buffers
 * @param buff1 First buffer
 * @param buff2 Second buffer
 * @param len The number of the element to be checked
 * @return uint8_t 1 : two buffers are the same
 *                 0 : two buffers are not the same
 */
uint8_t comapre_two_string(uint8_t *buff1, uint8_t *buff2, uint8_t len)
{
    uint8_t retval = 1;
    while(len)
    {
        if(*buff1 != *buff2)
        {
            retval = 0;
            break;
        }
        buff1 += 1;
        buff2 += 1;
        len--;
    }
    return retval;
}


/**
 * @brief infinite loop
 */
void Infinite_loop()
{
    // Check Keypad if prassed any key
    Check_Prass_Button(&key_pad_config);

    // LCD state with Timeout
    switch (LCD_state)
        {
        case IDLE:
            break;
        // Admin status
        case Admin_Correct_ID_Timeout:
        case Admin_Not_Correct_ID_Timeout:
        case Admin_Entry_star:
        case Admin_Entry_Hash:
        case Admin_Not_Correct_Entry:
        case Admin_Correct_Entry:
        case Admin_Entry_Timeout:
            if(Timeout == 0)
            {
                Send_String_Uart(uart1_config.UARTx, _Display_Timeout_Admin, Enable);
                Key_Buffer_counter = 0;
                Clear_Buffer(Key_Buffer, 0, sizeof(Key_Buffer));
                Admin_mode = 0;
                LCD_state = IDLE;
            }
            delay_us(1);
            Timeout--;
            break;
        // Customer status
        case Correct_ID_Open_Gate_Timeout:
            if(Timeout == 0)
            {
                Run_Led(Red);
                if(Max_Grag > 0)
                {
                    Write_String_with_coordinator(&Lcd_config, _Display_Welcom,  0 ,First_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_Put_Card,  0 ,Seconde_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_There_is_an, 0 ,Third_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_Empty_Garage,  0 ,Fourth_R);
                }else
                {
                    Write_String_with_coordinator(&Lcd_config, _Display_Welcom,  0 ,First_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_Sorry,  0 ,Seconde_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_There_is_no, 0 ,Third_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_Empty_Garage,  0 ,Fourth_R);
                }
                LCD_state = IDLE;
            }
            delay_us(1);
            Timeout--;
            break;
        case Not_Correct_ID:
            if(Timeout == 0)
            {
                if(Max_Grag > 0)
                {
                    Write_String_with_coordinator(&Lcd_config, _Display_Welcom,  0 ,First_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_Put_Card,  0 ,Seconde_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_There_is_an, 0 ,Third_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_Empty_Garage,  0 ,Fourth_R);
                }else
                {
                    Write_String_with_coordinator(&Lcd_config, _Display_Welcom,  0 ,First_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_Sorry,  0 ,Seconde_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_There_is_no, 0 ,Third_R);
                    delay_us(200);
                    Write_String_with_coordinator(&Lcd_config, _Display_Empty_Garage,  0 ,Fourth_R);
                }
                LCD_state = IDLE;
            }
            delay_us(1);
            Timeout--;
            break;
        default:
            break;
        }

}
//==============================================================================


//------------------------------- DATA STRUCTURE -------------------------------

//==============================================================================