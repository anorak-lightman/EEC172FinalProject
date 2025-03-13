//*****************************************************************************
//
// Application Name     -   ArcadeWithAWS
// Application Overview -   Arcade machine with 4 games that sends your initials and high score to your email after
//                          finishing the game.
// Application Details  -
// docs\examples\CC32xx_SSL_Demo_Application.pdf
// or
// http://processors.wiki.ti.com/index.php/CC32xx_SSL_Demo_Application
//
//*****************************************************************************


//*****************************************************************************
//
//! \addtogroup ssl
//! @{
//
//*****************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Simplelink includes
#include "simplelink.h"

//Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_nvic.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_apps_rcm.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "gpio.h"
#include "systick.h"
#include "spi.h"
#include "utils.h"
#include "uart.h"

//Common interface includes
#include "pin_mux_config.h"
#include "gpio_if.h"
#include "common.h"
#include "uart_if.h"
#include "Adafruit_SSD1351.h"
#include "Adafruit_GFX.h"
#include "oled_test.h"
#include "glcdfont.h"
#include "i2c_if.h"

// Custom includes
#include "utils/network_utils.h"


//NEED TO UPDATE THIS FOR IT TO WORK!
#define DATE                10    /* Current Date */
#define MONTH               3    /* Month 1-12 */
#define YEAR                2025  /* Current year */
#define HOUR                3    /* Time - hours */
#define MINUTE              50    /* Time - minutes */
#define SECOND              15     /* Time - seconds */


#define APPLICATION_NAME      "SSL"
#define APPLICATION_VERSION   "SQ24"
#define SERVER_NAME           "a1j0q92upkr07j-ats.iot.us-east-1.amazonaws.com" // CHANGE ME
#define GOOGLE_DST_PORT       8443

#define GETHEADER "GET /things/dmroth_CC3200_Board/shadow HTTP/1.1\r\n"
#define POSTHEADER "POST /things/dmroth_CC3200_Board/shadow HTTP/1.1\r\n"             // CHANGE ME
#define HOSTHEADER "Host: a1j0q92upkr07j-ats.iot.us-east-1.amazonaws.com\r\n"  // CHANGE ME
#define CHEADER "Connection: Keep-Alive\r\n"
#define CTHEADER "Content-Type: application/json; charset=utf-8\r\n"
#define CLHEADER1 "Content-Length: "
#define CLHEADER2 "\r\n\r\n"


//*****************************************************************************
//                 BUTTON TABLE -- Start
//*****************************************************************************

int button_0[15] = {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0};
int button_1[15] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0};
int button_2[15] = {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0};
int button_3[15] = {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0};
int button_4[15] = {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0};
int button_5[15] = {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0};
int button_6[15] = {1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0};
int button_7[15] = {1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0};
int button_8[15] = {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
int button_9[15] = {1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0};
int enter_button[15] = {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0};
int last_button[15] = {1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0};

int right_button[15] = {1,0,0,0,0,0,0,1,0,1,0,0,0,1,0};
int up_button[15] = {1,0,0,0,0,1,0,0,0,1,0,0,0,1,0};
int left_button[15] = {1,0,0,0,0,1,0,1,0,1,0,0,0,1,0};
int down_button[15] = {1,0,0,0,0,0,1,0,0,1,0,0,0,1,0};
int ok_button[15] = {1,0,0,0,0,1,1,1,1,1,0,1,0,1,0};
int menu_button[15] = {1,0,0,0,0,0,0,0,0,0,1,0,0,1,0};

int next=0;
volatile int waiting=0;
volatile int waiting2=0;

int SelectXPosition=40;
int SelectYPosition=13;
volatile int OnSelectScreen=1;
volatile int SelectGame=0;
volatile int GameOver=1;

int string_index = 0;
char cur_character = '?';
char string_to_print[2];

volatile int game1x=50;
volatile int game1y=50;

char button_2_letters[3] = {'a', 'b', 'c'};
char button_3_letters[3] = {'d', 'e', 'f'};
char button_4_letters[3] = {'g', 'h', 'i'};
char button_5_letters[3] = {'j', 'k', 'l'};
char button_6_letters[3] = {'m', 'n', 'o'};
char button_7_letters[4] = {'p', 'q', 'r', 's'};
char button_8_letters[3] = {'t', 'u', 'v'};
char button_9_letters[4] = {'w', 'x', 'y', 'z'};

//*****************************************************************************
//                 BUTTON TABLE -- End
//*****************************************************************************
#define BLACK           0x0000
#define BLUE            0x001F
#define GREEN           0x07E0
#define CYAN            0x07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define SPI_IF_BIT_RATE  100000
#define UART_BAUD_RATE  115200
#define SYSCLK          80000000
//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);

volatile unsigned int buffer[15];
volatile unsigned int index;
volatile bool button_decoded;


// some helpful macros for systick

// the cc3200's fixed clock frequency of 80 MHz
// note the use of ULL to indicate an unsigned long long constant
#define SYSCLKFREQ 80000000ULL

// macro to convert ticks to microseconds
#define TICKS_TO_US(ticks) \
    ((((ticks) / SYSCLKFREQ) * 1000000ULL) + \
    ((((ticks) % SYSCLKFREQ) * 1000000ULL) / SYSCLKFREQ))\

// macro to convert microseconds to ticks
#define US_TO_TICKS(us) ((SYSCLKFREQ / 1000000ULL) * (us))

// systick reload value set to 20ms period
// (PERIOD_SEC) * (SYSCLKFREQ) = PERIOD_TICKS
#define SYSTICK_RELOAD_VAL 1600000UL

// track systick counter periods elapsed
// if it is not 0, we know the transmission ended
volatile int systick_cnt = 0;

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

typedef struct BlockCoordinates {
    int x;
    int y;
} BlockCoordinates;

// an example of how you can use structs to organize your pin settings for easier maintenance
typedef struct PinSetting {
    unsigned long port;
    unsigned int pin;
} PinSetting;

static const PinSetting remoteIn = {.port = GPIOA0_BASE, .pin = 0x40};

//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//*****************************************************************************
static void BoardInit(void);
static inline void SysTickReset(void);

//*****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS
//*****************************************************************************
static void button_pressed_handler(void) {
    unsigned long ulStatus;
    ulStatus = MAP_GPIOIntStatus(remoteIn.port, true);
    MAP_GPIOIntClear(remoteIn.port, ulStatus);        // clear interrupts on GPIOA1

    uint64_t delta = SYSTICK_RELOAD_VAL - SysTickValueGet();
    uint64_t delta_us = TICKS_TO_US(delta); // time in microseconds
    if (systick_cnt != 0) { // if systick rolls over then restart
        index = 0;
    } else if (delta_us > 1800 && delta_us < 2500) { // if time elapsed is greater than 1800 microseconds but less than 2500 microseconds then the bit is a 1
        buffer[index] = 1;
        index++;
    } else if (delta_us > 500 && delta_us < 1500) { // if time elapsed is greater than 500 microseconds but less than 1500 microseconds then bit is a 0
        buffer[index] = 0;
        index++;
    } else { // if more time has elapsed then a button hasn't been pressed in a while so start over
        index = 0;
    }
    if (index == 14) { // if index == 14 means we have read all 15 bits for one button press so now signal that button is decoded
       button_decoded = true;
       index = 0;
    }
    SysTickReset(); // reset counter for next bit
}

/**
 * Reset SysTick Counter
 */
static inline void SysTickReset(void) {
    // any write to the ST_CURRENT register clears it
    // after clearing it automatically gets reset without
    // triggering exception logic
    // see reference manual section 3.2.1
    HWREG(NVIC_ST_CURRENT) = 1;

    // clear the global count variable
    systick_cnt = 0;
}

/**
 * SysTick Interrupt Handler
 *
 * Keep track of whether the systick counter wrapped
 */
static void SysTickHandler(void) {
    // increment every time the systick handler fires
    systick_cnt++;
}

int compareArray(int a[], int b[]) { // compares two arrays to see if they are equal to each other
    int i;
    for (i = 0; i < 15; i++) {
        if (a[i] != b[i]) {
            return 1;
        }
    }
    return 0;
}

int exists_in_array(char c, char char_set[]) { // checks if a character exists in an array
    int length = sizeof(char_set) / sizeof(char_set[0]);
    int i;
    for (i = 0; i < length; i++) {
        if (char_set[i] == c) {
            return i;
        }
    }
    return -1;
}

char character_from_button(char c, char char_set[], int length) { // converts button press to character from character array associated with that button
    int index = exists_in_array(c, char_set);
    char new_character;
    if (index != -1) { // if the button exists in the array
        if (index < (length - 1)) { // go to next character in array if not at end of array
            index++;
            new_character = char_set[index];
        } else { // go to beginning of array
            index = 0;
            new_character = char_set[index];
        }
    } else { // if character not in the array then set our new character to be first character in array associated with new button
        new_character = char_set[0];
    }
    return new_character;
}

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************

#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

//*****************************************************************************
//                 GLOBAL VARIABLES -- End: df
//*****************************************************************************


//****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//****************************************************************************
static int set_time();
static void BoardInit(void);
static int http_post(int, int, char*, int);
static int send_message(int, char*, int);
static void snake(void);
bool check_collision(int x, int y, int* snake_tail_x, int* snake_tail_y, int tail_length);
static void spawn_apple(int* apple_x, int* apple_y);
static void draw_border(int color);
static void breakout(void);
static void drawPaddle(int x);
static void drawBlock(int x, int y, unsigned int color);
static void drawBall(int x, int y);
static void eraseBlock(int x, int y);
static void erasePaddle(int x);
static void eraseBall(int x, int y);
int checkBlockCollision(int ball_x, int ball_y, int ball_x_velocity, int ball_y_velocity, BlockCoordinates* blocks, int blocks_length);
static void game_over(int game, int score);
static void AccelGame();
static void baskets();
//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void BoardInit(void) {
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

static void SysTickInit(void) {

    // configure the reset value for the systick countdown register
    MAP_SysTickPeriodSet(SYSTICK_RELOAD_VAL);

    // register interrupts on the systick module
    MAP_SysTickIntRegister(SysTickHandler);

    // enable interrupts on systick
    // (trigger SysTickHandler when countdown reaches 0)
    MAP_SysTickIntEnable();

    // enable the systick module itself
    MAP_SysTickEnable();
}


//*****************************************************************************
//
//! This function updates the date and time of CC3200.
//!
//! \param None
//!
//! \return
//!     0 for success, negative otherwise
//!
//*****************************************************************************

static int set_time() {
    long retVal;

    g_time.tm_day = DATE;
    g_time.tm_mon = MONTH;
    g_time.tm_year = YEAR;
    g_time.tm_sec = HOUR;
    g_time.tm_hour = MINUTE;
    g_time.tm_min = SECOND;

    retVal = sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION,
                          SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME,
                          sizeof(SlDateTime),(unsigned char *)(&g_time));

    ASSERT_ON_ERROR(retVal);
    return SUCCESS;
}


//*****************************************************************************
//
//! Main 
//!
//! \param  none
//!
//! \return None
//!
//*****************************************************************************
void main() {

    //
    // Initialize board configuration
    //
    unsigned long ulStatus;

    BoardInit();

    PinMuxConfig();

    SysTickInit();

    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

    MAP_GPIOIntRegister(remoteIn.port, button_pressed_handler);

    MAP_GPIOIntTypeSet(remoteIn.port, remoteIn.pin, GPIO_FALLING_EDGE);

    ulStatus = MAP_GPIOIntStatus(remoteIn.port, false);
    MAP_GPIOIntClear(remoteIn.port, ulStatus);          // clear interrupts on GPIOA0

    MAP_PRCMPeripheralReset(PRCM_GSPI);

    MAP_SPIReset(GSPI_BASE);

    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                         SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                         (SPI_SW_CTRL_CS |
                         SPI_4PIN_MODE |
                         SPI_TURBO_OFF |
                         SPI_CS_ACTIVEHIGH |
                         SPI_WL_8));

    MAP_SPIEnable(GSPI_BASE);
    GPIOPinWrite(GPIOA1_BASE, 0x1, 0x1); // CS High
    GPIOPinWrite(GPIOA0_BASE, 0x80, 0x80); //reset high
    I2C_IF_Open(I2C_MASTER_MODE_FST);
    Adafruit_Init();

    InitTerm();
    ClearTerm();
    UART_PRINT("My terminal works!\n\r");

    // initialize global default app configuration
    g_app_config.host = SERVER_NAME;
    g_app_config.port = GOOGLE_DST_PORT;

    index = 0;
    button_decoded = false;

    // Enable interrupts
    MAP_GPIOIntEnable(remoteIn.port, remoteIn.pin);

    char Select[13] = "Select A Game";
    int s1;
    char Game1[9] = "Dodgeball";
    char Game2[5] = "Snake";
    char Game3[8] = "Breakout";
    char Game4[7] = "Baskets";

    while (1) {
        if(OnSelectScreen==1){
            fillScreen(BLUE);
            draw_border(YELLOW);
            for(s1=0;s1<4;s1++){
                drawRect(15,s1*20+20,80,15,RED);
            }
            for(s1=0; s1<13; s1++){
                drawChar(s1*width()/17+20,10,Select[s1],WHITE,BLUE,1.5);
            }
            for(s1=0; s1<9; s1++){
                drawChar(s1*8+20,25,Game1[s1],WHITE,BLUE,1.5);
            }
            for(s1=0; s1<5; s1++){
                drawChar(s1*8+20,45,Game2[s1],WHITE,BLUE,1.5);
            }
            for(s1=0; s1<8; s1++){
                drawChar(s1*8+20,65,Game3[s1],WHITE,BLUE,1.5);
            }
            for(s1=0; s1<7; s1++){
                drawChar(s1*8+20,85,Game4[s1],WHITE,BLUE,1.5);
            }
            OnSelectScreen=2;
        }

        int cursor=1;
        fillCircle(110,cursor*20+7,5,GREEN);
        while(OnSelectScreen==2){
            int saved_buffer[15];
            int i;
            for (i = 0; i < 15; i++) {
                saved_buffer[i] = buffer[i];
            }
            if (!compareArray(saved_buffer, up_button)) {
                fillCircle(110,cursor*20+7,5,BLUE);
                if(cursor == 1){
                    cursor = 4;
                }
                else{
                    cursor--;
                }
                fillCircle(110,cursor*20+7,5,GREEN);
            }
            if (!compareArray(saved_buffer, down_button)) {
                fillCircle(110,cursor*20+7,5,BLUE);
                if(cursor == 4){
                    cursor=1;
                }
                else{
                    cursor++;
                }
                fillCircle(110,cursor*20+7,5,GREEN);
            }
            if (!compareArray(saved_buffer, ok_button)) {
                fillCircle(110,cursor*20+7,5,RED);
                OnSelectScreen=0;
            }
            button_decoded = false;
        }

        if(OnSelectScreen==0){
            OnSelectScreen=3;
            if(cursor==1){
                GameOver=0;
                AccelGame();
                SelectGame=0;
                OnSelectScreen=1;
            }
            if(cursor==2){
                GameOver=0;
                fillScreen(BLACK);
                snake();
                SelectGame=0;
                OnSelectScreen=1;
            }
            if(cursor==3){
                GameOver=0;
                fillScreen(BLACK);
                breakout();
                SelectGame=0;
                OnSelectScreen=1;
            }
            if(cursor==4){
                GameOver=0;
                baskets();
                SelectGame=0;
                OnSelectScreen=1;
            }
        }
    }
}
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

static int send_message(int score, char* initials, int game) {
    long lRetVal = -1;
    //Connect the CC3200 to the local access point
    lRetVal = connectToAccessPoint();
    //Set time so that encryption can be used
    lRetVal = set_time();
    if(lRetVal < 0) {
        UART_PRINT("Unable to set time in the device");
        LOOP_FOREVER();
    }
    //Connect to the website with TLS encryption
    lRetVal = tls_connect();
    if(lRetVal < 0) {
        ERR_PRINT(lRetVal);
    }
    http_post(lRetVal, score, initials, game);
    //http_get(lRetVal);

    sl_Stop(SL_STOP_TIMEOUT);
    return 0;
}

static int http_post(int iTLSSockID, int score, char* initials, int game){
    char acSendBuff[512];
    char acRecvbuff[1460];
    char cCLLength[200];
    char* pcBufHeaders;
    int lRetVal = 0;

    // copy all of the headers into a character array
    pcBufHeaders = acSendBuff;
    strcpy(pcBufHeaders, POSTHEADER);
    pcBufHeaders += strlen(POSTHEADER);
    strcpy(pcBufHeaders, HOSTHEADER);
    pcBufHeaders += strlen(HOSTHEADER);
    strcpy(pcBufHeaders, CHEADER);
    pcBufHeaders += strlen(CHEADER);
    strcpy(pcBufHeaders, "\r\n\r\n");

    char temp_buffer[170];
    // store the data payload into a buffer and use sprintf to insert the message in the payload
    if (game == 1) {
        sprintf(temp_buffer, "{ \"state\": { \"desired\": { \"default\": \"Congratulations %s on getting %d in Dodgeball\", \"email\": \"Congratulations %s on getting %d in Dodgeball\" } } }", initials, score, initials, score);
    } else if (game == 2) {
        sprintf(temp_buffer, "{ \"state\": { \"desired\": { \"default\": \"Congratulations %s on getting %d in Snake\", \"email\": \"Congratulations %s on getting %d in Snake\" } } }", initials, score, initials, score);
    } else if (game == 3) {
        sprintf(temp_buffer, "{ \"state\": { \"desired\": { \"default\": \"Congratulations %s on getting %d in Breakout\", \"email\": \"Congratulations %s on getting %d in Breakout\" } } }", initials, score, initials, score);
    } else {
        sprintf(temp_buffer, "{ \"state\": { \"desired\": { \"default\": \"Congratulations %s on getting %d in Baskets\", \"email\": \"Congratulations %s on getting %d in Baskets\" } } }", initials, score, initials, score);
    }

    int dataLength = strlen(temp_buffer);
    // copy the rest of the headers into the buffer
    strcpy(pcBufHeaders, CTHEADER);
    pcBufHeaders += strlen(CTHEADER);
    strcpy(pcBufHeaders, CLHEADER1);

    pcBufHeaders += strlen(CLHEADER1);
    sprintf(cCLLength, "%d", dataLength);

    strcpy(pcBufHeaders, cCLLength);
    pcBufHeaders += strlen(cCLLength);
    strcpy(pcBufHeaders, CLHEADER2);
    pcBufHeaders += strlen(CLHEADER2);
    // copy the data payload into the buffer
    strcpy(pcBufHeaders, temp_buffer);
    pcBufHeaders += strlen(temp_buffer);

    int testDataLength = strlen(pcBufHeaders);

    UART_PRINT(acSendBuff);


    //
    // Send the packet to the server */
    //
    lRetVal = sl_Send(iTLSSockID, acSendBuff, strlen(acSendBuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("POST failed. Error Number: %i\n\r",lRetVal);
        sl_Close(iTLSSockID);

        return lRetVal;
    }
    // receive HTTP ack
    lRetVal = sl_Recv(iTLSSockID, &acRecvbuff[0], sizeof(acRecvbuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("Received failed. Error Number: %i\n\r",lRetVal);
        //sl_Close(iSSLSockID);

        return lRetVal;
    }
    else {
        // print out the ack
        acRecvbuff[lRetVal+1] = '\0';
        UART_PRINT(acRecvbuff);
        UART_PRINT("\n\r\n\r");
    }

    return 0;
}
static void snake() {
    int x = 128/2;
    int y = 128/2;
    int old_x = x;
    int old_y = y;
    int width = 5;
    int height = 5;
    int x_speed = 0;
    int y_speed = 0;
    int snake_tail_x[100];
    int snake_tail_y[100];
    int tail_length = 5;
    int apple_x = 0;
    int apple_y = 0;
    int apple_radius = 2.5;
    bool playing = true;
    bool collided = false;
    int delay = 0;
    int score = 0;
    draw_border(GREEN);
    drawChar(100, 5, '0', YELLOW, BLACK, 1);
    drawRect(x, y, width, height, WHITE);
    fillRect(x, y, width, height, WHITE);
    spawn_apple(&apple_x, &apple_y);
    if (apple_x == x && apple_y == y) {
        while (apple_x == x && apple_y == y) {
            spawn_apple(&apple_x, &apple_y);
        }
    }
    drawCircle(apple_x, apple_y, apple_radius, RED);
    fillCircle(apple_x, apple_y, apple_radius, RED);
    while (playing) {
        while (!button_decoded) {
            if (delay % 100000 == 0) {
                if (x + (x_speed * 2) > 5 && x + (x_speed * 2) < 120 && y + (y_speed * 2) > 5 && y + (y_speed * 2) < 120) {
                    x += x_speed * 2;
                    y += y_speed * 2;
                    if (x != old_x || y != old_y) {
                        int old_x_tail_2;
                        int old_y_tail_2;
                        int old_x_tail = snake_tail_x[0];
                        int old_y_tail = snake_tail_y[0];
                        int end_of_tail_x = snake_tail_x[tail_length - 1];
                        int end_of_tail_y = snake_tail_y[tail_length - 1];
                        snake_tail_x[0] = old_x;
                        snake_tail_y[0] = old_y;
                        int i;
                        for (i = 1; i < tail_length; i++) {
                            old_x_tail_2 = snake_tail_x[i];
                            old_y_tail_2 = snake_tail_y[i];
                            snake_tail_x[i] = old_x_tail;
                            snake_tail_y[i] = old_y_tail;
                            old_x_tail = old_x_tail_2;
                            old_y_tail = old_y_tail_2;
                        }
                        old_x = x;
                        old_y = y;
                        drawRect(x, y, width, height, WHITE);
                        fillRect(x, y, width, height, WHITE);
                        drawRect(end_of_tail_x, end_of_tail_y, width, height, BLACK);
                        fillRect(end_of_tail_x, end_of_tail_y, width, height, BLACK);
                        collided = check_collision(x, y, snake_tail_x, snake_tail_y, tail_length);
                        if (x >= apple_x - 5 && x <= apple_x && y <= apple_y && y >= apple_y - 5) {
                            score += 20;
                            drawCircle(apple_x, apple_y, apple_radius, BLACK);
                            fillCircle(apple_x, apple_y, apple_radius, BLACK);
                            spawn_apple(&apple_x, &apple_y);
                            while (apple_x == x && apple_y == y) {
                                spawn_apple(&apple_x, &apple_y);
                            }
                            tail_length += 1;
                            drawCircle(apple_x, apple_y, apple_radius, RED);
                            fillCircle(apple_x, apple_y, apple_radius, RED);
                            char str[10];
                            sprintf(str, "%d", score);
                            int score_x = 100;
                            for (i = 0; i < 4; i++) {
                                drawChar(score_x, 5, str[i], YELLOW, BLACK, 1);
                                score_x += 5;
                            }
                        } else {
                            drawCircle(apple_x, apple_y, apple_radius, RED);
                            fillCircle(apple_x, apple_y, apple_radius, RED);
                        }
                        if (collided == true) {
                            game_over(2, score);
                            playing = false;
                            break;
                        }
                    }
                } else {
                    game_over(2, score);
                    playing = false;
                    break;
                }
            }
            delay++;
            if (delay > 100000) {
                delay = 1;
            }
        }
        int saved_buffer[15];
        int i;
        for (i = 0; i < 15; i++) {
            saved_buffer[i] = buffer[i];
        }
        if (!compareArray(saved_buffer, left_button)) {
            if (x_speed != 1) {
                x_speed = -1;
                y_speed = 0;
            }
        } else if (!compareArray(saved_buffer, right_button)) {
            if (x_speed != -1) {
                x_speed = 1;
                y_speed = 0;
            }
        } else if (!compareArray(saved_buffer, up_button)) {
            if (y_speed != 1) {
                y_speed = -1;
                x_speed = 0;
            }
        } else if (!compareArray(saved_buffer, down_button)) {
            if (y_speed != -1) {
                y_speed = 1;
                x_speed = 0;
            }
        }
        if (!compareArray(saved_buffer, menu_button)) {
            game_over(2, score);
            playing = false;
            button_decoded = false;
            break;
        }
        button_decoded = false;
        delay = 0;
    }
}

bool check_collision(int x, int y, int* snake_tail_x, int* snake_tail_y, int tail_length) {
    int i;
    for (i = 0; i < tail_length; i++) {
        if (x == snake_tail_x[i] && y == snake_tail_y[i]) {
            return true;
        }
    }
    return false;
}

static void spawn_apple(int* apple_x, int* apple_y) {
    *apple_x = 0;
    *apple_y = 0;
    while (*apple_x <= 20 || *apple_x >= 100) {
        *apple_x = rand() % 120;
    }
    while (*apple_y <= 20 || *apple_y >= 100) {
        *apple_y = rand() % 120;
    }
}

static void draw_border(int color) {
    drawRect(0, 0, 128, 5, color);
    drawRect(0, 0, 5, 128, color);
    drawRect(0, 123, 128, 5, color);
    drawRect(123, 0, 5, 128, color);
    fillRect(0, 0, 128, 5, color);
    fillRect(0, 0, 5, 128, color);
    fillRect(0, 123, 128, 5, color);
    fillRect(123, 0, 5, 128, color);
}


static void breakout() {
    int paddle_x = 128/2 - 10;
    int old_paddle_x = paddle_x;
    int block_x = 0;
    int block_y = 10;
    int ball_x = 128/2;
    int ball_y = 118;
    int old_ball_x = ball_x;
    int old_ball_y = ball_y;
    float ball_x_velocity = 2;
    float ball_y_velocity = -2;
    int color_index = 0;
    unsigned int colors[6] = {RED, BLUE, GREEN, CYAN, MAGENTA, YELLOW};
    bool playing = true;
    bool started = false;
    int delay = 0;
    int score = 0;
    BlockCoordinates blocks[48];
    int cur_block = 0;
    int i = 0;
    int j = 0;
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 8; j++) {
            drawBlock(block_x, block_y, colors[color_index]);
            BlockCoordinates new_block = {.x = block_x, .y = block_y};
            blocks[cur_block] = new_block;
            cur_block++;
            block_x += 16;
        }
        block_x = 0;
        block_y += 5;
        if (color_index == 5) {
            color_index = 0;
        } else {
            color_index++;
        }
    }
    drawPaddle(paddle_x);
    drawBall(ball_x, ball_y);
    drawChar(100, 1, '0', YELLOW, BLACK, 1);
    while(playing) {
        while (!button_decoded) {
            if (delay % 100000 == 0 && started == true) {
                if (ball_x + ball_x_velocity <= 0 || ball_x + ball_x_velocity >= 128) {
                    ball_x_velocity *= -1;
                }
                if (ball_y + ball_y_velocity <= 0) {
                    ball_y_velocity *= -1;
                }
                if (ball_y + ball_y_velocity >= 128) {
                    game_over(3, score);
                    playing = false;
                    break;
                }
                if (ball_x + ball_x_velocity >= paddle_x && ball_x + ball_x_velocity <= paddle_x + 20 && ball_y + ball_y_velocity + 2.5 >= 120) {
                    ball_y_velocity *= -1;
                }
                int index_of_collided_block = checkBlockCollision(ball_x, ball_y, ball_x_velocity, ball_y_velocity, blocks, 48);
                if (index_of_collided_block != -1) {
                    int collided_block_x = blocks[index_of_collided_block].x;
                    int collided_block_y = blocks[index_of_collided_block].y;
                    eraseBlock(collided_block_x, collided_block_y);
                    blocks[index_of_collided_block].x = -100;
                    blocks[index_of_collided_block].y = -100;
                    cur_block--;
                    if (cur_block <= 0) {
                        game_over(3, score);
                        playing = false;
                        break;
                    }
                    if ((collided_block_x == ball_x + ball_x_velocity || collided_block_x + 16 == ball_x + ball_x_velocity) && collided_block_y <= ball_y + ball_y_velocity + 2.5 && collided_block_y + 5 >= ball_y + ball_y_velocity) {
                        ball_x_velocity *= -1;
                    } else {
                        ball_y_velocity *= -1;
                    }
                    score += 20;
                    char str[10];
                    sprintf(str, "%d", score);
                    int score_x = 100;
                    for (i = 0; i < 4; i++) {
                        drawChar(score_x, 1, str[i], YELLOW, BLACK, 1);
                        score_x += 5;
                    }
                }
                ball_x = ball_x + ball_x_velocity;
                ball_y = ball_y + ball_y_velocity;
                eraseBall(old_ball_x, old_ball_y);
                drawBall(ball_x, ball_y);
                old_ball_x = ball_x;
                old_ball_y = ball_y;
            }
            delay++;
            if (delay > 100000) {
                delay = 1;
            }
        }
        int saved_buffer[15];
        int i;
        for (i = 0; i < 15; i++) {
            saved_buffer[i] = buffer[i];
        }
        if (!compareArray(saved_buffer, left_button)) {
            if (paddle_x - 10 >= 0) {
                paddle_x -= 10;
            }
            if (!started) {
                ball_x_velocity *= -1;
            }
            started = true;
        } else if (!compareArray(saved_buffer, right_button)) {
            if (paddle_x + 26 <= 128) {
                paddle_x += 10;
            }
            started = true;
        }
        if (!compareArray(saved_buffer, menu_button)) {
            game_over(3, score);
            playing = false;
            button_decoded = false;
            break;
        }
        if (old_paddle_x != paddle_x) {
            erasePaddle(old_paddle_x);
            drawPaddle(paddle_x);
            old_paddle_x = paddle_x;
        }
        button_decoded = false;
        delay = 0;
    }
}

static void drawPaddle(int x) {
    drawRect(x, 120, 20, 5, WHITE);
    fillRect(x, 120, 20, 5, WHITE);
}

static void erasePaddle(int x) {
    drawRect(x, 120, 20, 5, BLACK);
    fillRect(x, 120, 20, 5, BLACK);
}

static void drawBlock(int x, int y, unsigned int color) {
    drawRect(x, y, 16, 5, WHITE);
    fillRect(x, y, 16, 5, color);
}

static void eraseBlock(int x, int y) {
    drawRect(x, y, 16, 5, BLACK);
    fillRect(x, y, 16, 5, BLACK);
}

static void drawBall(int x, int y) {
    drawCircle(x, y, 2.5, WHITE);
    fillCircle(x, y, 2.5, WHITE);
}

static void eraseBall(int x, int y) {
    drawCircle(x, y, 2.5, BLACK);
    fillCircle(x, y, 2.5, BLACK);
}

int checkBlockCollision(int ball_x, int ball_y, int ball_x_velocity, int ball_y_velocity, BlockCoordinates* blocks, int blocks_length) {
    int i = 0;
    int index_of_collided_block = -1;
    for (i = 0; i < blocks_length; i++) {
        if (ball_x + ball_x_velocity >= blocks[i].x && ball_x + ball_x_velocity <= blocks[i].x + 16 && ball_y + ball_y_velocity + 2.5 >= blocks[i].y && ball_y + ball_y_velocity <= blocks[i].y + 5) {
            index_of_collided_block = i;
            break;
        }
    }
    return index_of_collided_block;
}

static void AccelGame() {
    int delay=0;
    int xacc = 0;
    int xval= 64;
    int yval = 64;
    int yacc = 0;
    int rad = 10;
    int rectx = 0;
    int recty = 0;
    int rectxprev=0;
    int rectyprev=0;
    int objsize=5;
    int objx[10]={};
    int objy[10]={};
    int objxprev[10]={};
    int objyprev[10]={};
    int objrespawn[10]={};
    int objrespawnlocation[10]={};
    int objxdir[10]={};
    int objydir[10]={};
    int objspeed[10]={6,6,6,6,6,6,6,6,6,6};
    int timepassed=0;
    int amountofobj=1;
    int letsgo=0;

    unsigned char dev = 0x18;
    unsigned char offx = 0x3;
    unsigned char offy = 0x5;
    unsigned char xdata[256];
    unsigned char ydata[256];
    char selectdifficulty[17]="Select Difficulty";
    char easy[7]="1) Easy";
    char normal[9]="2) Normal";
    char hard[7]="3) Hard";
    char instruction[23]= "Tilt to move and dodge!";

    fillScreen(MAGENTA);
    draw_border(YELLOW);
    int s=0;
    for(s=0;s<3;s++){
        drawRect(15,s*20+20,80,15,RED);
    }
    for(s=0; s<17; s++){
        drawChar(s*5+20,10,selectdifficulty[s],WHITE,MAGENTA,1.5);
    }
    for(s=0; s<7; s++){
        drawChar(s*8+20,25,easy[s],WHITE,MAGENTA,1.5);
    }
    for(s=0; s<9; s++){
        drawChar(s*8+20,45,normal[s],WHITE,MAGENTA,1.5);
    }
    for(s=0; s<7; s++){
        drawChar(s*8+20,65,hard[s],WHITE,MAGENTA,1.5);
    }
    for(s=0; s<23; s++){
        drawChar(s*5+7,85,instruction[s],WHITE,MAGENTA,1.5);
    }
    int cursor=1;
    fillCircle(110,cursor*20+7,5,GREEN);
    while(letsgo==0){
        int saved_buffer[15];
        int ii;
        for (ii = 0; ii < 15; ii++) {
            saved_buffer[ii] = buffer[ii];
        }
        if (!compareArray(saved_buffer, up_button)) {
            fillCircle(110,cursor*20+7,5,MAGENTA);
            if(cursor==1){
                cursor=3;
            }
            else{
                cursor--;
            }
            fillCircle(110,cursor*20+7,5,GREEN);
        }
        if (!compareArray(saved_buffer, down_button)) {
            fillCircle(110,cursor*20+7,5,MAGENTA);
            if(cursor==3){
                cursor=1;
            }
            else{
                cursor++;
            }
            fillCircle(110,cursor*20+7,5,GREEN);
        }
        if (!compareArray(saved_buffer, ok_button)) {
            fillCircle(110,cursor*20+7,5,RED);
            letsgo=1;
        }
        if (!compareArray(saved_buffer, menu_button)) {
            OnSelectScreen=1;
            SelectGame=0;
            GameOver=1;
            letsgo=1;
        }
        button_decoded = false;

        if (cursor==1&&letsgo==1) {
            objsize=3;
            for(ii=0;ii<10;ii++){
                objspeed[ii]=3;
            }
        }
        if (cursor==2&&letsgo==1) {
            objsize=4;
            for(ii=0;ii<10;ii++){
                objspeed[ii]=5;
            }
        }
        if (cursor==3&&letsgo==1) {
            objsize=5;
            for(ii=0;ii<10;ii++){
                objspeed[ii]=7;
            }
        }
    }
    char countdown[8]="3 2 1 0 ";
    if(GameOver==0){
        fillScreen(BLUE);
        for(s=0;s<8;s++){
            drawChar(55,55,countdown[s],WHITE,BLUE,1.5);
            MAP_UtilsDelay(10000000);
        }
    }
    while(GameOver==0){
        while (!button_decoded){
            if(delay % 100000 ==0){
                rectxprev=rectx;
                rectyprev=recty;
                I2C_IF_Write(dev,&offx,1,0);
                I2C_IF_Read(dev, &xdata[0], 1);
                I2C_IF_Write(dev,&offy,1,0);
                I2C_IF_Read(dev, &ydata[0], 1);
                int xint = xdata[0];
                int yint = ydata[0];
                if(xint>127){
                    xint-=255;
                }
                if(yint>127){
                    yint-=255;
                }
                xacc=xint/1;
                xval+=xacc;
                yacc=yint/1;
                yval-=yacc;
                if(xval>=126-rad){
                    xval=126-rad;
                }
                if(yval>=126-rad){
                    yval=126-rad;
                }
                if(xval<=2){
                    xval=2;
                }
                if(yval<=2){
                    yval=2;
                }
                rectx=xval;
                recty=yval;

                if(timepassed%50==0 && amountofobj<10 && timepassed>1){
                    amountofobj++;
                }
                int i=0;
                for(i=0;i<amountofobj;i++){
                    if(objx[i]>128 || objx[i]<0-objsize || objy[i]>128 ||objy[i]<0-objsize){
                        objrespawn[i]=0;
                    }
                    if(objrespawn[i]==0){
                        objrespawnlocation[i] = rand()%4+1;
                        objrespawn[i]=1;
                        if(objrespawnlocation[i]==1){
                            objx[i]=0-objsize;
                            objy[i]=rand()%120+1;
                            objxdir[i]=rand()%objspeed[i]+1;
                            objydir[i]=rand()%(objspeed[i]*2+1)-objspeed[i];
                        }
                        if(objrespawnlocation[i]==2){
                            objx[i]=128;
                            objy[i]=rand()%120+1;
                            objxdir[i]=-(rand()%objspeed[i]+1);
                            objydir[i]=rand()%(objspeed[i]*2+1)-objspeed[i];
                        }
                        if(objrespawnlocation[i]==3){
                            objx[i]=rand()%120+1;
                            objy[i]=0-objsize;
                            objxdir[i]=rand()%(objspeed[i]*2+1)-objspeed[i];
                            objydir[i]=rand()%objspeed[i]+1;
                        }
                        if(objrespawnlocation[i]==4){
                            objx[i]=rand()%120+1;
                            objy[i]=128;
                            objxdir[i]=rand()%(objspeed[i]*2+1)-objspeed[i];
                            objydir[i]=-(rand()%objspeed[i]+1);
                        }
                    }

                    objxprev[i]=objx[i];
                    objyprev[i]=objy[i];
                    objx[i]+=objxdir[i];
                    objy[i]+=objydir[i];

                    fillRect(objxprev[i], objyprev[i], objsize, objsize, BLUE);
                    fillRect(objx[i], objy[i], objsize, objsize, WHITE);

                    if(rectx + rad > objx[i] && rectx < objx[i] + objsize){
                        if(recty + rad > objy[i] && recty < objy[i] + objsize){
                            GameOver=1;
                            button_decoded = true;
                        }
                    }
                }

                fillRect(rectxprev, rectyprev, rad, rad, BLUE);
                fillRect(rectx, recty, rad, rad, RED);

                timepassed++;
            }
            delay++;
            if (delay > 100000) {
                delay = 1;
            }
        }
        int saved_buffer[15];
        int i;
        for (i = 0; i < 15; i++) {
            saved_buffer[i] = buffer[i];
        }
        if (!compareArray(saved_buffer, menu_button)) {
            GameOver=1;
        }
        button_decoded = false;
        delay=0;
    }
    game_over(1,timepassed);
}


static void game_over(int game, int score) {
    fillScreen(BLACK);
    int scorex = 0;
    char score2[5] = "Score";
    char scorestring1[1];
    char scorestring2[2];
    char scorestring3[3];
    char scorestring4[4];
    char scorestring5[5];
    scorex=40;

    int i=0;
    for(i=0;i<5;i++){
        drawChar(scorex,40,score2[i],RED,BLACK,1);
        scorex+=5;
    }
    scorex=40;

    if(score<10){
        sprintf(scorestring1,"%d",score);
        for(i=0;i<1;i++){
            drawChar(scorex,50,scorestring1[i],RED,BLACK,1);
            scorex+=5;
        }
    }
    if(score>=10 && score<100){
        sprintf(scorestring2,"%d",score);
        for(i=0;i<2;i++){
            drawChar(scorex,50,scorestring2[i],RED,BLACK,1);
            scorex+=5;
        }
    }
    if(score>=100 && score<1000){
        sprintf(scorestring3,"%d",score);
        for(i=0;i<3;i++){
            drawChar(scorex,50,scorestring3[i],RED,BLACK,1);
            scorex+=5;
        }
    }
    if(score>=1000 && score<10000){
        sprintf(scorestring4,"%d",score);
        for(i=0;i<4;i++){
            drawChar(scorex,50,scorestring4[i],RED,BLACK,1);
            scorex+=5;
        }
    }
    if(score>=10000 && score<100000){
        sprintf(scorestring5,"%d",score);
        for(i=0;i<5;i++){
            drawChar(scorex,50,scorestring5[i],RED,BLACK,1);
            scorex+=5;
        }
    }

    char prompt[19] = "Enter your initials";
    int promptx = 20;
    int prompty = 60;


    for (i = 0; i < 19; i++) {
        drawChar(promptx, prompty, prompt[i], YELLOW, BLACK, 1);
        promptx += 5;
    }

    int inix=50;
    int iniy=70;
    int inic=1;
    while(inic!=3){
        int saved_buffer[15];
        for (i = 0; i < 15; i++) {
            saved_buffer[i] = buffer[i];
        }
        if (!compareArray(saved_buffer, button_0)) {
            cur_character = ' ';
            drawChar(inix, iniy, cur_character, RED, BLACK, 1);
            next=1;
        } else if (!compareArray(saved_buffer, button_1)&&next==1) {
            string_to_print[string_index] = cur_character;
            string_index++;
            inix+=5;
            next=0;
            inic++;
        } else if (!compareArray(saved_buffer, button_2)) {
            cur_character = character_from_button(cur_character, button_2_letters, 3);
            drawChar(inix, iniy, cur_character, RED, BLACK, 1);
            next=1;
        } else if (!compareArray(saved_buffer, button_3)) {
            cur_character = character_from_button(cur_character, button_3_letters, 3);
            drawChar(inix, iniy, cur_character, RED, BLACK, 1);
            next=1;
        } else if (!compareArray(saved_buffer, button_4)) {
            cur_character = character_from_button(cur_character, button_4_letters, 3);
            drawChar(inix, iniy, cur_character, RED, BLACK, 1);
            next=1;
        } else if (!compareArray(saved_buffer, button_5)) {
            cur_character = character_from_button(cur_character, button_5_letters, 3);
            drawChar(inix, iniy, cur_character, RED, BLACK, 1);
            next=1;
        } else if (!compareArray(saved_buffer, button_6)) {
            cur_character = character_from_button(cur_character, button_6_letters, 3);
            drawChar(inix, iniy, cur_character, RED, BLACK, 1);
            next=1;
        } else if (!compareArray(saved_buffer, button_7)) {
            cur_character = character_from_button(cur_character, button_7_letters, 4);
            drawChar(inix, iniy, cur_character, RED, BLACK, 1);
            next=1;
        } else if (!compareArray(saved_buffer, button_8)) {
            cur_character = character_from_button(cur_character, button_8_letters, 3);
            drawChar(inix, iniy, cur_character, RED, BLACK, 1);
            next=1;
        } else if (!compareArray(saved_buffer, button_9)) {
            cur_character = character_from_button(cur_character, button_9_letters, 4);
            drawChar(inix, iniy, cur_character, RED, BLACK, 1);
            next=1;
        } else if (!compareArray(saved_buffer, last_button)&&inic>1) {
            drawChar(inix, iniy, ' ', RED, BLACK, 1);
            string_index--;
            inix-=5;
            inic--;
        }
        button_decoded = false;
    }
    string_index = 0;
    send_message(score, string_to_print, game);

}

static void baskets() {
    int delay=0;
    int xacc = 0;
    int xval= 64;
    int yval = 100;
    int rad = 10;
    int rectx = 0;
    int recty = 0;
    int rectxprev=0;
    int rectyprev=0;
    int objsize=5;
    int objx[10]={};
    int objy[10]={};
    int objxprev[10]={};
    int objyprev[10]={};
    int objrespawn[10]={};
    int objxdir[10]={};
    int objydir[10]={};
    int objspeed[10]={12,12,12,12,12,12,12,12,12,12};

    int coinsize=5;
    int coinx[10]={};
    int coiny[10]={};
    int coinxprev[10]={};
    int coinyprev[10]={};
    int coinrespawn[10]={};
    int coinxdir[10]={};
    int coinydir[10]={};
    int coinspeed[10]={12,12,12,12,12,12,12,12,12,12};
    int coinscore=0;

    int timepassed=0;
    int amountofobj=1;
    int amountofcoin=1;

    unsigned char dev = 0x18;
    unsigned char offx = 0x3;
    unsigned char offy = 0x5;
    unsigned char xdata[256];
    unsigned char ydata[256];

    fillScreen(BLUE);

    int infox=20;
    int s=0;
    char infoscreen1[17]="Tilt to get coins";
    char infoscreen2[15]="and dodge fire!";
    if(GameOver==0){
        for(s=0;s<17;s++){
            drawChar(infox,55,infoscreen1[s],WHITE,BLUE,1.5);
            infox+=5;
        }
        infox=20;
        for(s=0;s<15;s++){
            drawChar(infox,65,infoscreen2[s],WHITE,BLUE,1.5);
            infox+=5;
        }
        MAP_UtilsDelay(10000000);
    }
    fillScreen(BLUE);

    while(GameOver==0){
        while (!button_decoded){
            if(delay % 100000 ==0){
                rectxprev=rectx;
                rectyprev=recty;
                I2C_IF_Write(dev,&offx,1,0);
                I2C_IF_Read(dev, &xdata[0], 1);
                I2C_IF_Write(dev,&offy,1,0);
                I2C_IF_Read(dev, &ydata[0], 1);
                int xint = xdata[0];
                int yint = ydata[0];
                if(xint>127){
                    xint-=255;
                }
                if(yint>127){
                    yint-=255;
                }
                xacc=xint/1;
                xval+=xacc;
                if(xval>=126-rad){
                    xval=126-rad;
                }
                if(yval>=126-rad){
                    yval=126-rad;
                }
                if(xval<=2){
                    xval=2;
                }
                if(yval<=2){
                    yval=2;
                }
                rectx=xval;
                recty=yval;

                if(timepassed%100==0 && amountofobj<5 && timepassed>1){
                    amountofobj++;
                }
                if(timepassed%100==0 && amountofcoin<5 && timepassed>1){
                    amountofcoin++;
                }
                int i=0;
                for(i=0;i<amountofobj;i++){
                    if(objx[i]>128 || objx[i]<0-objsize || objy[i]>128 ||objy[i]<0-objsize){
                        objrespawn[i]=0;
                    }
                    if(objrespawn[i]==0){
                        objrespawn[i]=1;
                        objx[i]=rand()%120+1;
                        objy[i]=0-objsize;
                        objydir[i]=rand()%objspeed[i]+1;
                    }

                    objxprev[i]=objx[i];
                    objyprev[i]=objy[i];
                    objx[i]+=objxdir[i];
                    objy[i]+=objydir[i];

                    fillRect(objxprev[i], objyprev[i], objsize, objsize, BLUE);
                    fillRect(objx[i], objy[i], objsize, objsize, RED);

                    if(rectx + rad > objx[i] && rectx < objx[i] + objsize){
                        if(recty + rad > objy[i] && recty < objy[i] + objsize){
                            GameOver=1;
                            button_decoded = true;
                        }
                    }
                }

                for(i=0;i<amountofcoin;i++){
                    if(coinx[i]>128 || coinx[i]<0-coinsize || coiny[i]>128 ||coiny[i]<0-coinsize){
                        coinrespawn[i]=0;
                    }
                    if(coinrespawn[i]==0){
                        coinrespawn[i]=1;
                        coinx[i]=rand()%120+1;
                        coiny[i]=0-objsize;
                        coinydir[i]=rand()%coinspeed[i]+1;
                    }

                    coinxprev[i]=coinx[i];
                    coinyprev[i]=coiny[i];
                    coinx[i]+=coinxdir[i];
                    coiny[i]+=coinydir[i];

                    fillRect(coinxprev[i], coinyprev[i], coinsize, coinsize, BLUE);
                    fillRect(coinx[i], coiny[i], coinsize, coinsize, YELLOW);

                    if(rectx + rad > coinx[i] && rectx < coinx[i] + coinsize){
                        if(recty + rad > coiny[i] && recty < coiny[i] + coinsize){
                            fillRect(coinx[i], coiny[i], coinsize, coinsize, BLUE);
                            coinscore+=5;
                            coinrespawn[i]=0;
                            button_decoded = true;
                        }
                    }
                }

                fillRect(rectxprev, rectyprev, rad, rad, BLUE);
                fillRect(rectx, recty, rad, rad, WHITE);

                timepassed++;
            }
            delay++;
            if (delay > 100000) {
                delay = 1;
            }
        }
        int saved_buffer[15];
        int i;
        for (i = 0; i < 15; i++) {
            saved_buffer[i] = buffer[i];
        }
        if (!compareArray(saved_buffer, menu_button)) {
            GameOver=1;
        }
        button_decoded = false;
        delay=0;
    }
    game_over(4,coinscore);
}


//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
