

#include "LCD_Driver.h"
#include <stddef.h>


void _delay_ms(uint32_t time);

void _delay_ms(uint32_t time) {
	uint32_t i, j;
	for (i = 0; i < time; i++)
		for (j = 0; j < 255; j++);
}

void Kick_Enable_pin(const LCD_16_2 *lcd_instance);

/**
 * @brief This private function activates and reflect the data sent to the LCD
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 */
void Kick_Enable_pin(const LCD_16_2 *lcd_instance)
{
    Set_pin(lcd_instance->Enable_Port,lcd_instance->Enable_Pin);
    _delay_ms(22);
    Reset_pin(lcd_instance->Enable_Port,lcd_instance->Enable_Pin);
}

/**
 * @brief This function is used to configure the LCD each the ports and pins
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 * @return unsigned char 
 */
unsigned char LCD_init(LCD_16_2 const *lcd_instance)
{
    GPIO_InitTypeDef gpio_confg;
    gpio_confg.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_confg.Pin = (lcd_instance->R_W_Pin) | (lcd_instance->RS_Pin) | (lcd_instance->Enable_Pin);
    gpio_confg.Speed = GPIO_SPEED_FREQ_10MHZ;
    Init_GPIO(GPIOA,&gpio_confg);

    Reset_pin(lcd_instance->RS_Port,lcd_instance->RS_Pin); // instruction register
    Reset_pin(lcd_instance->R_W_Port,lcd_instance->R_W_Pin); // Write data
    Reset_pin(lcd_instance->Enable_Port,lcd_instance->Enable_Pin); // Enable read/write

    gpio_confg.Pin = lcd_instance->Data_Pin;
    Init_GPIO(GPIOB,&gpio_confg);
    _delay_ms(50);

    #ifdef LCD_8_Bit
        Write_Command(lcd_instance,CMD_LCD_Clear);
        Write_Command(lcd_instance,CMD_LCD_Function_Set|CMD_LCD_OP_DL|CMD_LCD_OP_N|CMD_LCD_OP_F);   // Function Set
        Write_Command(lcd_instance,CMD_LCD_Entry_Mode_Set|CMD_LCD_OP_I_D);                          // Entry Mode
        Write_Command(lcd_instance,CMD_LCD_Begin_AT_First_Raw);                                     // Begin At First Raw
        Write_Command(lcd_instance,CMD_LCD_Display_On_Off|CMD_LCD_OP_B|CMD_LCD_OP_C|CMD_LCD_OP_D);  // Display Is Active
    #endif

    #ifdef LCD_4_Bit
        // Write_Port_Register(lcd_instance->Data_Port,(((CMD_LCD_Function_Set | CMD_LCD_OP_N | CMD_LCD_OP_F) >> 4));
        // Write_Port_Register(lcd_instance->Data_Port,((CMD_LCD_Function_Set | CMD_LCD_OP_N | CMD_LCD_OP_F));
        
        // Write_Port_Register(lcd_instance->Data_Port,(((CMD_LCD_Display_On_Off|CMD_LCD_OP_D|CMD_LCD_OP_C|CMD_LCD_OP_B) >> 4));
        // Write_Port_Register(lcd_instance->Data_Port,((CMD_LCD_Display_On_Off|CMD_LCD_OP_D|CMD_LCD_OP_C|CMD_LCD_OP_B)));

        // Write_Port_Register(lcd_instance->Data_Port,((CMD_LCD_Entry_Mode_Set|CMD_LCD_OP_I_D|CMD_LCD_OP_S) >> 4)));
        // Write_Port_Register(lcd_instance->Data_Port,((CMD_LCD_Entry_Mode_Set|CMD_LCD_OP_I_D|CMD_LCD_OP_S)));
        
    #endif
    
    _delay_ms(70);

    /*------------------- Init LCD -------------------*/


}

/**
 * @brief This function is used to check the seventh pin in case it is busy
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 */
void Check_BF(const LCD_16_2 *lcd_instance)
{
    GPIO_InitTypeDef gpio_config ={0};
    gpio_config.Mode = GPIO_MODE_INPUT;
    gpio_config.Pull = GPIO_NOPULL;
    gpio_config.Pin = lcd_instance->Data_Pin;
    Init_GPIO(GPIOB,&gpio_config);

    Set_pin(lcd_instance->R_W_Port,lcd_instance->R_W_Pin);
    Reset_pin(lcd_instance->RS_Port,lcd_instance->RS_Pin);
    Kick_Enable_pin(lcd_instance);

    gpio_config.Pull = GPIO_PULLDOWN;
    gpio_config.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_config.Speed = GPIO_SPEED_FREQ_10MHZ;
    gpio_config.Pin = lcd_instance->Data_Pin;
    Init_GPIO(GPIOB,&gpio_config);

    Reset_pin(lcd_instance->R_W_Port,lcd_instance->R_W_Pin);
}

/**
 * @brief This function is used to move the cursor towards the right
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 */
void Shift_R_Cursor(const LCD_16_2 *lcd_instance)
{
    Write_Command(lcd_instance,CMD_LCD_Cursor_Display_Shift|CMD_LCD_OP_R_L);
}

/**
 * @brief This function is used to move the cursor towards the left
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 */
void Shift_L_Cursor(const LCD_16_2 *lcd_instance)
{
    Write_Command(lcd_instance,CMD_LCD_Cursor_Display_Shift);
}


unsigned char Read_Cursor_Add(const LCD_16_2 *lcd_instance)
{

}

/**
 * @brief This function is used to print one character on the LCD screen
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 * @param[in] ch Character value to be printed 
 * @return unsigned char 
 */
unsigned char Write_Character(const LCD_16_2 *lcd_instance,unsigned char ch)
{
    Check_BF(lcd_instance);
    Set_pin(lcd_instance->RS_Port,lcd_instance->RS_Pin);
    uint32_t temp = lcd_instance->Data_Port->GPIO_ODR;
    #ifdef LCD_8_Bit
        temp &= 0xFFFFFF00;
        temp |= (uint32_t)ch;
        Set_GPIO_Value(lcd_instance->Data_Port,temp);
    #endif

    #ifdef LCD_4_Bit
        temp &= 0xFFFFFFF0;
        temp |= (uint32_t)ch;
        Set_GPIO_Value(lcd_instance->Data_Port,temp);
        Set_pin(lcd_instance->RS_Port,lcd_instance->RS_Pin);
        Reset_pin(lcd_instance->R_W_Port,lcd_instance->R_W_Pin);
        temp &= 0xFFFFFFF0;
        temp |= ((uint32_t)ch << 4);
        Set_GPIO_Value(lcd_instance->Data_Port,temp);
        Kick_Enable_pin(lcd_instance);
    #endif
    Set_pin(lcd_instance->RS_Port,lcd_instance->RS_Pin);
    Reset_pin(lcd_instance->R_W_Port,lcd_instance->R_W_Pin);
    Kick_Enable_pin(lcd_instance);
}

/**
 * @brief This function is used to print one character on the LCD screen with specifying the column and row
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 * @param[in] ch Character value to be printed
 * @param[in] column It is used to specify the column in which the character will be printed
 * @param[in] Raw: @ref Raw_LCD, It is used to specify the raw in which the character will be printed
 * @return unsigned char 
 */
unsigned char Write_Character_with_coordinator(const LCD_16_2 *lcd_instance,unsigned char ch,unsigned char column,En_Lcd_Raw Raw)
{
    Jump_to_coordinator(lcd_instance,column,Raw);
    Check_BF(lcd_instance);
    Set_pin(lcd_instance->RS_Port,lcd_instance->RS_Pin);
    //Write_Port_Register(lcd_instance->Data_Port,ch);
    Set_pin(lcd_instance->RS_Port,lcd_instance->RS_Pin);
    Reset_pin(lcd_instance->R_W_Port,lcd_instance->R_W_Pin);
    Kick_Enable_pin(lcd_instance);
}


/**
 * @brief To print a many of characters on the LCD screen
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 * @param ch Indicator for the first character to be printed
 * @return unsigned char 
 */
unsigned char Write_String(const LCD_16_2 *lcd_instance,unsigned char *ch)
{
    while((*ch) > 0)
    {
        Write_Character(lcd_instance,*ch);
        ch++;
    }
}


/**
 * @brief This function is used to print characters on the LCD screen with specifying the column and row
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 * @param ch Indicator for the first character to be printed
 * @param[in] column It is used to specify the column in which the characters will be printed
 * @param[in] Raw: @ref Raw_LCD, It is used to specify the raw in which the characters will be printed
 * @return unsigned char 
 */
unsigned char Write_String_with_coordinator(const LCD_16_2 *lcd_instance,unsigned char *ch,unsigned char column,En_Lcd_Raw Raw)
{
    Jump_to_coordinator(lcd_instance,column,Raw);
    while((*ch) > 0)
    {
        Write_Character(lcd_instance,*ch);
        ch++;
    }
}


/**
 * @brief Used to move a cursor for a specific column
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 * @param column :Column number to jump to
 * @param Raw :Row number to jump to
 */
void Jump_to_coordinator(const LCD_16_2 *lcd_instance,unsigned char column,En_Lcd_Raw Raw)
{
    switch (Raw)
    {
    case First_R:
        if(column < 16)
            Write_Command(lcd_instance,CMD_LCD_Begin_AT_First_Raw + column);
        break;
    case Seconde_R:
        if(column < 16)
            Write_Command(lcd_instance,CMD_LCD_Begin_AT_Seconde_Raw + column);
        break;
    default:
        break;
    }
}

/**
 * @brief This function is used to send a command to the LCD
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 * @param command command value
 */
void Write_Command(const LCD_16_2 *lcd_instance,unsigned char command)
{
    uint32_t temp = lcd_instance->Data_Port->GPIO_ODR;
    #ifdef LCD_8_Bit
        Check_BF(lcd_instance);
        
        temp &= 0xFFFFFF00;
        temp |= (uint32_t)command;
        Set_GPIO_Value(lcd_instance->Data_Port,temp);
    
    #endif
    
    #ifdef LCD_4_Bit
        temp &= 0xFFFFFFF0;
        temp |= (uint32_t)command;
        Set_GPIO_Value(lcd_instance->Data_Port,temp);
        Reset_pin(lcd_instance->RS_Port,lcd_instance->RS_Pin);
        Reset_pin(lcd_instance->R_W_Port,lcd_instance->R_W_Pin);  
        Kick_Enable_pin(lcd_instance);
        temp &= 0xFFFFFFF0;
        temp |= ((uint32_t)command << 4);
        Set_GPIO_Value(lcd_instance->Data_Port,temp);

    #endif

    Reset_pin(lcd_instance->RS_Port,lcd_instance->RS_Pin);
    Reset_pin(lcd_instance->R_W_Port,lcd_instance->R_W_Pin);
    Kick_Enable_pin(lcd_instance);
}

/**
 * @brief It is used to print the string on the screen with a left swipe
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 * @param base Indicator for the first character to be printed
 * @param[in] Raw: @ref Raw_LCD, It is used to specify the raw in which the characters will be printed
 * @param buffersize Character count in string
 * @NOTE : Can only be used for one row
 *       : When this function is used, it is forbidden to use other printing functions
 */
void write_with_move(const LCD_16_2 *lcd_instance,char *base,En_Lcd_Raw Raw,unsigned char buffersize)
{
    static char *base_temp = NULL;
    static char *till_temp = NULL;
    static char *current_loc = NULL;
    static unsigned char counter = 0;
    if(base_temp == NULL)
    {
        base_temp = base;
        till_temp = base;
        current_loc = base;
        Write_Command(lcd_instance,CMD_LCD_Entry_Mode_Set);                          // Entry Mode
    }
    Jump_to_coordinator(lcd_instance,15,First_R);
    counter++;
    while(current_loc >= base_temp)
    {
        Write_Character(lcd_instance,*current_loc);
        current_loc--;
    }
    if(*till_temp != '\0')
    {
        till_temp++;
        current_loc = till_temp;
    }else
    {
        for(int i = 0; i < 16; i++)
        {
            Write_Command(lcd_instance,CMD_LCD_Cursor_Display_Shift|CMD_LCD_OP_S_C);
            _delay_ms(70);
        }
        Write_Command(lcd_instance,CMD_LCD_Clear);
        LCD_init(lcd_instance);
        Write_Command(lcd_instance,CMD_LCD_Entry_Mode_Set);                          // Entry Mode
        counter = 0;
        base_temp = base;
        till_temp = base_temp;
        current_loc = base;
    }
    if(counter > 15)
    {
        base_temp++;
    }
}


/**
 * @brief This function is used to clean the LCD
 * @param[in] lcd_instance The lcd_instance data structure containing the port and pins to be configured
 */
void clean_Lcd(LCD_16_2 *lcd_instance)
{
    Write_Command(lcd_instance,CMD_LCD_Clear);
}