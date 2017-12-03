/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

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
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;        /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static u32 UserApp1_u32DataMsgCount = 0;             /* Counts the number of ANT_DATA packets received */
static u32 UserApp1_u32TickMsgCount = 0;             /* Counts the number of ANT_TICK packets received */

static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */

u8 au8WelcomeMessage[] = "Heart Rate Sensor";
u8 au8Instructions[] = "B0 starts machine";

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/
static void DebugHR( u8 u8HeartRate )
{
		if(u8HeartRate < 50)
	{
		DebugPrintf("****\r\n");
	}
	else if(u8HeartRate < 60)
	{
		DebugPrintf("    -\r\n");
	}
	else if(u8HeartRate < 70)
	{
		DebugPrintf("     -\r\n");
	}
	else if(u8HeartRate < 80)
	{
		DebugPrintf("      -\r\n");
	}
	else if(u8HeartRate < 90)
	{
		DebugPrintf("       -\r\n");
	}
	else if(u8HeartRate < 100)
	{
		DebugPrintf("        -\r\n");
	}
	else if(u8HeartRate < 110)
	{
		DebugPrintf("         -\r\n");
	}
	else if(u8HeartRate < 120)
	{
		DebugPrintf("          -\r\n");
	}
	else if(u8HeartRate < 130)
	{
		DebugPrintf("           -\r\n");
	}
	else if(u8HeartRate < 140)
	{
		DebugPrintf("            -\r\n");
	}
	else if(u8HeartRate < 150)
	{
		DebugPrintf("             -\r\n");
	}
	else if(u8HeartRate < 160)
	{
		DebugPrintf("              -\r\n");
	}
	else if(u8HeartRate < 170)
	{
		DebugPrintf("               -\r\n");
	}
	else if(u8HeartRate < 180)
	{
		DebugPrintf("                -\r\n");
	}
	else if(u8HeartRate < 190)
	{
		DebugPrintf("                 -\r\n");
	}
	else if(u8HeartRate < 200)
	{
		DebugPrintf("                  -\r\n");
	}
	else
	{
		DebugPrintf("                   -\r\n");
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


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
  AntAssignChannelInfoType sAntSetupData;
	u8 au8Ant_Plus_Network_Key[]={0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45};
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 

  /* Start with LED0 in RED state = channel is not configured */
  LedOn(RED);

  
 /* Configure ANT for this application */
  sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_DEFAULT_NETWORK_KEY;
  
	for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupData.AntNetworkKey[i] = au8Ant_Plus_Network_Key[i];
  }
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    /* Channel assignment is queued so start timer */

    UserApp1_u32Timeout = G_u32SystemTime1ms;
    LedOn(RED);
    
    UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */

    LedBlink(RED, LED_4HZ);

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
/* Wait for the ANT channel assignment to finish */
static void UserApp1SM_WaitChannelAssign(void)
{
  /* Check if the channel assignment is complete */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    LedOff(RED);
    LedOn(YELLOW);

    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Monitor for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 5000) )
  {
    DebugPrintf("\n\r***Channel assignment timeout***\n\n\r");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
      
} /* end UserApp1SM_WaitChannelAssign() */


  /*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserApp1SM_Idle(void)
{
  /* Look for BUTTON 0 to open channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue open channel and change LED0 from yellow to blinking green to indicate channel is opening */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);


    LedOff(YELLOW);
    LedBlink(GREEN, LED_2HZ);  
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelOpen;
  }
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to open */
static void UserApp1SM_WaitChannelOpen(void)
{
  /* Monitor the channel status to check if channel is opened */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {

    LedOn(GREEN);  
    LedOn(LCD_RED);
		LedOn(LCD_GREEN);
		LedOn(LCD_BLUE);
		
		LCDClearChars(LINE2_START_ADDR,20);
		LCDMessage(LINE2_START_ADDR,"Receive the message");
		
    UserApp1_StateMachine = UserApp1SM_ChannelOpen;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);

    LedOff(GREEN);
    LedOn(YELLOW);  
    
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
    
} /* end UserApp1SM_WaitChannelOpen() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Channel is open, so monitor data */
static void UserApp1SM_ChannelOpen(void)
{
  static u8 u8LastState = 0xff;
  static u8 au8TickMessage[] = "EVENT x\n\r";  /* "x" at index [6] will be replaced by the current code */
  static u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
  static u8 au8LastAntData[ANT_APPLICATION_MESSAGE_BYTES] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
  bool bGotNewData;
	
	static u8 u8HeartRate=0;
	static u8 u8HeartBeat=0;
	static u8 u8HBCount;
	static u8	u8AverageHR;
	static u16 u16HRSum=0;
	static u8 au8PresentHR[]="PHR:   bpm";
	static u8 au8AverageHR[]="AHR:   bpm";
	
	static bool bBuzzerOn=TRUE;
		
  /* Check for BUTTON0 to close channel */
  if( WasButtonPressed(BUTTON0) )
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue close channel and change LED to blinking green to indicate channel is closing */
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    u8LastState = 0xff;


    LedOff(YELLOW);
    LedOff(BLUE);
    LedBlink(GREEN, LED_2HZ); 
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelClose;
  } /* end if(WasButtonPressed(BUTTON0)) */
  
	/* Switch the Buzzer */
	if( WasButtonPressed(BUTTON1) )
	{
		ButtonAcknowledge(BUTTON1);
		bBuzzerOn=!bBuzzerOn;
	}
	
  /* Always check for ANT messages */
  if( AntReadAppMessageBuffer() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      UserApp1_u32DataMsgCount++;
      
      /* We are synced with a device, so blue is solid */
      LedOff(GREEN);
      LedOn(BLUE);

      /* Check if the new data is the same as the old data and update as we go */
      bGotNewData = FALSE;
      for(u8 i = 0; i < ANT_APPLICATION_MESSAGE_BYTES; i++)
      {
        if(G_au8AntApiCurrentMessageBytes[i] != au8LastAntData[i])
        {
          bGotNewData = TRUE;
        }
      }
      
      if(bGotNewData)
      {
        AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP, au8TestMessage);
				
				/* Check if the HR changes and display present HR & average HR*/
				if(u8HeartRate!=G_au8AntApiCurrentMessageBytes[7])		
				{
					u8HeartRate=G_au8AntApiCurrentMessageBytes[7];
					u16HRSum+=u8HeartRate;
					u8HBCount++;
					u8AverageHR=u16HRSum/u8HBCount;
					
					au8PresentHR[4]= u8HeartRate/100 +48;
					au8PresentHR[5]= (u8HeartRate/10) %10 +48;
					au8PresentHR[6]= u8HeartRate%10 +48;

					au8AverageHR[4]= u8AverageHR/100 +48;
					au8AverageHR[5]= (u8AverageHR/10) %10 +48;
					au8AverageHR[6]= u8AverageHR%10 +48;
					
					
					LCDCommand(LCD_CLEAR_CMD);
					LCDMessage(LINE1_START_ADDR,au8PresentHR);
					LCDMessage(LINE1_START_ADDR+11,au8AverageHR);
					
					if(u8HeartRate<60)
					{
						LedOn(LCD_RED);
						LedOn(LCD_GREEN);
						LedOn(LCD_BLUE);
						LCDClearChars(LINE2_START_ADDR,20);
						LCDMessage(LINE2_START_ADDR,"Low HR");
					}
					
					else if(u8HeartRate<120)
					{
						LedOff(LCD_RED);
						LedOff(LCD_BLUE);
						LedOn(LCD_GREEN);
						LCDClearChars(LINE2_START_ADDR,20);
						LCDMessage(LINE2_START_ADDR,"Healthy HR");
					}
					
					else if(u8HeartRate<180)
					{
						LedOff(LCD_RED);
						LedOff(LCD_GREEN);
						LedOn(LCD_BLUE);
						LCDClearChars(LINE2_START_ADDR,20);
						LCDMessage(LINE2_START_ADDR,"A Little fast HR");
					}
					
					else if(u8HeartRate<200)
					{
						LedOff(LCD_GREEN);
						LedOff(LCD_BLUE);
						LedOn(LCD_RED);
						LCDClearChars(LINE2_START_ADDR,20);
						LCDMessage(LINE2_START_ADDR,"Abnormal HR");
						PWMAudioSetFrequency(BUZZER1,500);
					}
					
					else if(u8HeartRate==200)
					{
						LCDClearChars(LINE2_START_ADDR,20);
						LCDMessage(LINE2_START_ADDR,"HR reaches limit");
						PWMAudioSetFrequency(BUZZER1,1000);
					}
					
					/* Recalculate the average heart, in case sum of the HR overflows */
					if(u8HBCount==200)
					{
						u8HBCount=0;
						u16HRSum=u8AverageHR;
					}
				} /* end if(u8HeartRate!=G_au8AntApiCurrentMessageBytes[7]) */
				
				LedOff(WHITE);
				PWMAudioOff(BUZZER1);
				
				/*Check if the HB changes and buzzer settings*/
				if( u8HeartBeat!=G_au8AntApiCurrentMessageBytes[6] )
				{
					u8HeartBeat=G_au8AntApiCurrentMessageBytes[6];
					LedOn(WHITE);
					
					if(bBuzzerOn)
					{
						if(u8HeartRate<180)
						{
							PWMAudioOff(BUZZER1);
						}
						
						if( (u8HeartRate>180) & (u8HeartRate<200) )
						{
							PWMAudioSetFrequency(BUZZER1,500);
							PWMAudioOn(BUZZER1);
						}
						
						if(u8HeartRate==200)
						{
							PWMAudioSetFrequency(BUZZER1,1000);
							PWMAudioOn(BUZZER1);
						}
					}/* end if(bBuzzerOn) */
				}/* end if(u8HeartBeat!=G_au8AntApiCurrentMessageBytes[6]) */
				
				/* Debug outputs drawings of heart rate*/
				DebugHR(u8HeartRate);
      
			} /* end if(bGotNewData) */
    } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */
    
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      UserApp1_u32TickMsgCount++;

      /* Look at the TICK contents to check the event code and respond only if it's different */
      if(u8LastState != G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX])
      {
        /* The state changed so update u8LastState and queue a debug message */
        u8LastState = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];
        au8TickMessage[6] = HexToASCIICharUpper(u8LastState);
        DebugPrintf(au8TickMessage);

        /* Parse u8LastState to update LED status */
        switch (u8LastState)
        {

          /* If we are paired but missing messages, blue blinks */
          case EVENT_RX_FAIL:
          {
            LedOff(GREEN);
            LedBlink(BLUE, LED_2HZ);
            break;
          }

          /* If we drop to search, LED is green */
          case EVENT_RX_FAIL_GO_TO_SEARCH:
          {
            LedOff(BLUE);
            LedOn(GREEN);
            break;
          }

          /* If the search times out, the channel should automatically close */
          case EVENT_RX_SEARCH_TIMEOUT:
          {
            DebugPrintf("Search timeout event\r\n");
            break;
          }

          case EVENT_CHANNEL_CLOSED:
          {
            DebugPrintf("Channel closed event\r\n");
            break;
          }

            default:
          {
            DebugPrintf("Unexpected Event\r\n");
            break;
          }
        } /* end switch (G_au8AntApiCurrentMessageBytes) */
      } /* end if (u8LastState != G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX]) */
    } /* end else if(G_eAntApiCurrentMessageClass == ANT_TICK) */
    
  } /* end AntReadAppMessageBuffer() */
  
  /* A slave channel can close on its own, so explicitly check channel status */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) != ANT_OPEN)
  {

    LedBlink(GREEN, LED_2HZ);
    LedOff(BLUE);

    u8LastState = 0xff;
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelClose;
  } /* if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) != ANT_OPEN) */
      
} /* end UserApp1SM_ChannelOpen() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to close */
static void UserApp1SM_WaitChannelClose(void)
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {

    LedOff(GREEN);
    LedOn(YELLOW);

		LedOff(WHITE);
		LedOn(LCD_RED);
		LedOn(LCD_GREEN);
		LedOn(LCD_BLUE);
    
		LCDCommand(LCD_CLEAR_CMD);
  	LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  	LCDMessage(LINE2_START_ADDR, au8Instructions); 
		
		UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {

    LedOff(GREEN);
    LedOff(YELLOW);
    LedBlink(RED, LED_4HZ);

    UserApp1_StateMachine = UserApp1SM_Error;
  }
    
} /* end UserApp1SM_WaitChannelClose() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{

} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
