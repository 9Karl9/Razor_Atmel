/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */

static u8 u8Index=0;
static u8 au8Input[256]=NULL;
static bool bState1=FALSE;
static bool bState2=FALSE;
static u16 u16TimeCounter=0;
static bool bOn1=FALSE;
static bool bOn2=FALSE;
/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/* Initialize all these variables*/
void Initialize(void)
{
	LedOff(RED);
	LedOff(ORANGE);
	LedOff(YELLOW);
	LedOff(GREEN);
	LedOff(CYAN);
	LedOff(BLUE);
	LedOff(PURPLE);
	LedOff(WHITE);
	LedOff(LCD_RED);
	LedOff(LCD_BLUE);
	LedOff(LCD_GREEN);
	u16TimeCounter=0;
	PWMAudioOff(BUZZER1);
	bState1=FALSE;
	bState2=FALSE;
	LCDClearChars(LINE1_START_ADDR,20);
}

/* Set the Buzzer condition*/
void BuzzerRings(void)
{
	if(bOn1==TRUE)
	{
		u16TimeCounter++;
		
		if(u16TimeCounter==1000)
		{
			u16TimeCounter=0;
			bOn2=TRUE;
			PWMAudioOn(BUZZER1);
		}
	
		if(bOn2==TRUE)
		{
			PWMAudioSetFrequency(BUZZER1,200);
	
			if(u16TimeCounter==100)
			{
				PWMAudioOff(BUZZER1);
				bOn2=FALSE;
			}
		
		}
	}
}
/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{
	UserAPP1_state1();
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
	if(DebugScanf(&au8Input[u8Index])>=1) //Judge if the right input
	{
		u8Index++;
		
		if(au8Input[u8Index-1]=='\r') //Have input "Enter" ?
		{
			if((au8Input[u8Index-2]=='1')&&(u8Index==2)) //Input 1?
			{
				bState1=TRUE;
			}
			
			if((au8Input[u8Index-2]=='2')&&(u8Index==2)) //Input 2?
			{
				bState2=TRUE;
			}
			
			u8Index=0;
		}
	}
	
	/*Button1 pressed or 1 input, then go to UserAPP1_state1*/
	if(WasButtonPressed(BUTTON1)||bState1)
	{
		ButtonAcknowledge(BUTTON1);
		DebugLineFeed();
		Initialize();
		UserApp1_StateMachine=UserAPP1_state1;
	}
	
	/*Button2 pressed or 3 input, then go to UserAPP1_state2*/
	if(WasButtonPressed(BUTTON2)||bState2)
	{
		ButtonAcknowledge(BUTTON2);
		DebugLineFeed();
		Initialize();
		UserApp1_StateMachine=UserAPP1_state2;
	}

	BuzzerRings();
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */

static void UserAPP1_state1(void)
{
	/*Display "Entering state 1" on the PC screen*/
	DebugPrintf("Entering state 1");
	DebugLineFeed();
	Initialize();
	
	/*Display " STATE 1" on the LCD*/
	LCDMessage(LINE1_START_ADDR,"STATE 1");
	
	/*Set the Leds*/
	LedOn(WHITE);
	LedOn(PURPLE);
	LedOn(BLUE);
	LedOn(CYAN);
	
	/*Set the LCD Backlight*/
	LedPWM(LCD_RED,LED_PWM_100);
	LedPWM(LCD_BLUE,LED_PWM_100);
	
	/*Set the Buzzer*/
	PWMAudioOff(BUZZER1);
	bOn1=FALSE;
	
	/*Go back to UserApp1SM_Idle*/
	UserApp1_StateMachine=UserApp1SM_Idle;
}

static void UserAPP1_state2(void)
{
	/*Display "Entering state 2" on the PC screen*/
	DebugPrintf("Entering state 2");
	DebugLineFeed();
	Initialize();
	
	/*Display " STATE 1" on the LCD*/
	LCDMessage(LINE1_START_ADDR,"STATE 2");
	
	/*Set the Leds*/
	LedBlink(GREEN,LED_1HZ);
	LedBlink(YELLOW,LED_2HZ);
	LedBlink(ORANGE,LED_4HZ);
	LedBlink(RED,LED_8HZ);
	
	/*Set the LCD Backlight*/
	LedPWM(LCD_RED,LED_PWM_100);
	LedPWM(LCD_GREEN,LED_PWM_20);
	
	/*Set the Buzzer*/
	PWMAudioOn(BUZZER1);
	bOn1=TRUE;
	bOn2=TRUE;
	
	/*Go back to UserApp1SM_Idle*/
	UserApp1_StateMachine=UserApp1SM_Idle;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
