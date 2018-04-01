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

static void LedAllOff(void)
{
	LedOff(RED);
	LedOff(ORANGE);
	LedOff(YELLOW);
	LedOff(GREEN);
	LedOff(CYAN);
	LedOff(BLUE);
	LedOff(PURPLE);
	LedOff(WHITE);
}

static void LedAllOn(void)
{
	LedOn(RED);
	LedOn(ORANGE);
	LedOn(YELLOW);
	LedOn(GREEN);
	LedOn(CYAN);
	LedOn(BLUE);
	LedOn(PURPLE);
	LedOn(WHITE);
}

static void Delay(u32 u32DelayTime)
{
	for(u32 i = u32DelayTime;i>0;i--)
	{
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
 	/* Initialize Leds */
	LedAllOff();
	LedOn(WHITE);
	
	/* Initialize LCD */
	LCDCommand(LCD_CLEAR_CMD);
	LCDMessage(LINE1_START_ADDR,"Channel:MUTE");
	//LCDMessage(LINE2_START_ADDR,"");
	
	/* Enable PIO */
	AT91C_BASE_PIOA->PIO_PER |= 0x00015800;
	AT91C_BASE_PIOA->PIO_OER |= 0x00015800;
	AT91C_BASE_PIOB->PIO_PER |= 0x00000010;
	AT91C_BASE_PIOB->PIO_OER |= 0x00000010;
	
	/* Pins Table */
	/* Controlled Broad | Control Broad | Address 			*/
	/*        AD 				|				AN0			|	0x00000010		*/
	/* 				A					|				AN1			|	0x00000008		*/
	/* 				B					|			RX/UPIMO	|	0x00000800		*/
	/* 				C				|			TX/UPOMI		|	0x00001000		*/
	/* 				INC				|			 MOSI			|	0x00004000	  */
	/* 				UD				|			 MISO			|	0x00002000		*/
	/* 				CS				|       SCK			|	0x00008000		*/
	/* 				RE				|				CS			|	0x00010000		*/
	
	/* Set Pins */
	AT91C_BASE_PIOB->PIO_CODR = PB_03_BLADE_AN0;
	AT91C_BASE_PIOB->PIO_CODR = PB_04_BLADE_AN1;
	AT91C_BASE_PIOA->PIO_SODR = PA_11_BLADE_UPIMO;
	AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI;
	AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;
	AT91C_BASE_PIOA->PIO_CODR = PA_13_BLADE_MISO;
	AT91C_BASE_PIOA->PIO_SODR = PA_15_BLADE_SCK;
	AT91C_BASE_PIOA->PIO_CODR = PA_16_BLADE_CS;
	
	/* Set Pins */
	
	
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
	static bool bPressed = FALSE;

	if( WasButtonPressed(BUTTON0) )
	{
		ButtonAcknowledge(BUTTON0);
		bPressed = TRUE;
		
		UserApp1_StateMachine = UserApp1SM_VolumeUp;
	}
	
	if( WasButtonPressed(BUTTON1) )
	{
		ButtonAcknowledge(BUTTON1);
		bPressed = TRUE;
		
		UserApp1_StateMachine = UserApp1SM_VolumeDown;
	}
	
		if( WasButtonPressed(BUTTON2) )
	{
		ButtonAcknowledge(BUTTON2);
		bPressed = TRUE;
		
		//UserApp1_StateMachine = UserApp1SM_Test;
	}
	
	if( WasButtonPressed(BUTTON3) )
	{
		ButtonAcknowledge(BUTTON3);
		bPressed = TRUE;

		UserApp1_StateMachine = UserApp1SM_ModeChange;
	}
	
	if(bPressed)
	{
		bPressed=FALSE;
		
		LedOn(GREEN);
		Delay(50000);
		LedOff(GREEN);
	}
} /* end UserApp1SM_Idle() */
    
static void UserApp1SM_VolumeUp(void)
{
	/* Set Wiper Up */
	AT91C_BASE_PIOA->PIO_CODR = PA_15_BLADE_SCK;
	AT91C_BASE_PIOA->PIO_SODR = PA_13_BLADE_MISO;
	
	Delay(5);
	
	/* Change INC from High to Low */
	AT91C_BASE_PIOA->PIO_SODR = PA_14_BLADE_MOSI;
	Delay(5);
	AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;
	Delay(5);
	
	/* Store Wiper Position */
	AT91C_BASE_PIOA->PIO_SODR = PA_15_BLADE_SCK;
	Delay(5);
	AT91C_BASE_PIOA->PIO_SODR = PA_14_BLADE_MOSI;
	
	UserApp1_StateMachine = UserApp1SM_Idle;
}

static void UserApp1SM_VolumeDown(void)
{
	/* Set Wiper Down */
	AT91C_BASE_PIOA->PIO_CODR = PA_15_BLADE_SCK;
	AT91C_BASE_PIOA->PIO_CODR = PA_13_BLADE_MISO;
	
	Delay(5);
	
	/* Change INC from High to Low */
	AT91C_BASE_PIOA->PIO_SODR = PA_14_BLADE_MOSI;
	Delay(5);
	AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;
	Delay(5);
	
	/* Store Wiper Position */
	AT91C_BASE_PIOA->PIO_SODR = PA_15_BLADE_SCK;
	Delay(5);
	AT91C_BASE_PIOA->PIO_SODR = PA_14_BLADE_MOSI;
	
	UserApp1_StateMachine = UserApp1SM_Idle;
}

/*static void UserApp1SM_Test(void);
{
	
}
*/
static void UserApp1SM_ModeChange(void)
{
	static u8 u8Mode = 1;
	/* Mode Description 										*/
	/* Mode 0 : Mute 					White Led On	*/
	/* Mode 1 : MIC Signal 		Purple Led On	*/
	/* Mode 2 : Phone Signal  Blue Led On		*/
	
	if(u8Mode==3)
	{
		u8Mode=0;
	}
	
	switch(u8Mode)
	{
		case 0:
		{
			AT91C_BASE_PIOB->PIO_SODR = PA_11_BLADE_UPIMO;
			
			LedOff(BLUE);
			LedOn(WHITE);
			
			LCDCommand(LCD_CLEAR_CMD);
			LCDMessage(LINE1_START_ADDR,"Channel:MUTE");
			
			break;
		}
		case 1:
		{
			AT91C_BASE_PIOB->PIO_SODR = PB_04_BLADE_AN1;
			AT91C_BASE_PIOB->PIO_CODR = PA_11_BLADE_UPIMO;
			AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI;
			
			LedOff(WHITE);
			LedOn(PURPLE);
			
			LCDCommand(LCD_CLEAR_CMD);
			LCDMessage(LINE1_START_ADDR,"Channel:MIC");
			
			break;
		}
		case 2:
		{
			AT91C_BASE_PIOB->PIO_CODR = PB_04_BLADE_AN1;
			AT91C_BASE_PIOB->PIO_CODR = PA_11_BLADE_UPIMO;
			AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI;
			
			LedOff(PURPLE);
			LedOn(BLUE);
			
			LCDCommand(LCD_CLEAR_CMD);
			LCDMessage(LINE1_START_ADDR,"Channel:PHONE");
			
			break;
		}
		
		default:
		{
			LedBlink(RED,LED_4HZ);
			
			UserApp1_StateMachine = UserApp1SM_Error;
			
			break;
		}
	}

	u8Mode++;

	UserApp1_StateMachine = UserApp1SM_Idle;
}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
