#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "I2C_H.h"
#include "SPI_H.h"
#include "UART_H.h"
#include "GPIO_H.h"
#include "TIMER_H.h"
#include "RTC_H.h"
#include "EEPROM_H.h"
#include "RFID_H.h"
#include "OLED_H.h"


#define LATE_TIME   900UL
#define WARNING_10  600UL
#define WARNING_1   60UL


unsigned long RemainingTime = 0;
unsigned long ElapsedTime = 0;

unsigned char ExamStarted = 0;
unsigned char ExamPaused = 0;

unsigned char Warning10Sent = 0;
unsigned char Warning1Sent = 0;

unsigned char UID[4];

char CommandBuffer[60];
unsigned char CommandIndex = 0;

unsigned int EventNumber = 1;


/* ================= EVENT ================= */

void SendEvent(const char *Type, const char *Data)
{
	RTC_Time Time;
	unsigned char Check = 0;
	unsigned char i;

	UART_SendString("$EVT,");

	UART_SendChar(EventNumber / 100 + '0');
	UART_SendChar((EventNumber / 10) % 10 + '0');
	UART_SendChar(EventNumber % 10 + '0');

	UART_SendChar(',');

	if (RTC_GetTime(&Time))
	{
		UART_SendChar(Time.hour / 10 + '0');
		UART_SendChar(Time.hour % 10 + '0');
		UART_SendChar(':');

		UART_SendChar(Time.minute / 10 + '0');
		UART_SendChar(Time.minute % 10 + '0');
		UART_SendChar(':');

		UART_SendChar(Time.second / 10 + '0');
		UART_SendChar(Time.second % 10 + '0');
	}
	else
	{
		UART_SendString("00:00:00");
	}

	UART_SendChar(',');
	UART_SendString(Type);

	UART_SendChar(',');
	UART_SendString(Data);

	for (i = 0; Type[i] != '\0'; i++)
	{
		Check ^= Type[i];
	}

	for (i = 0; Data[i] != '\0'; i++)
	{
		Check ^= Data[i];
	}

	UART_SendChar('*');

	{
		char Hex[] = "0123456789ABCDEF";

		UART_SendChar(Hex[Check >> 4]);
		UART_SendChar(Hex[Check & 0x0F]);
	}

	UART_SendString("\r\n");

	EventNumber++;
}


/* ================= DISPLAY ================= */

void ShowCountdown(void)
{
	unsigned int Minutes;
	unsigned int Seconds;

	Minutes = RemainingTime / 60;
	Seconds = RemainingTime % 60;

	OLED_Clear();

	OLED_SetCursor(45, 3);

	if (Minutes < 10)
	{
		OLED_WriteChar('0');
	}

	OLED_WriteNumber(Minutes);

	OLED_WriteChar(':');

	if (Seconds < 10)
	{
		OLED_WriteChar('0');
	}

	OLED_WriteNumber(Seconds);
}


/* ================= START ================= */

void StartExam(unsigned int Minutes)
{
	if (Minutes == 0)
	{
		UART_SendString("ERR\r\n");
		return;
	}

	RemainingTime = (unsigned long)Minutes * 60;
	ElapsedTime = 0;

	ExamStarted = 1;
	ExamPaused = 0;

	Warning10Sent = 0;
	Warning1Sent = 0;

	Green_Off();
	Yellow_Off();
	Red_Off();

	ShowCountdown();

	UART_SendString("OK\r\n");

	SendEvent("START", "EXAM");
}


/* ================= PAUSE ================= */

void PauseExam(void)
{
	if (ExamStarted == 1 && ExamPaused == 0)
	{
		ExamPaused = 1;

		Green_Off();
		Yellow_On();

		UART_SendString("OK\r\n");

		SendEvent("PAUSE", "EXAM");
	}
	else
	{
		UART_SendString("ERR\r\n");
	}
}


/* ================= RESUME ================= */

void ResumeExam(void)
{
	if (ExamStarted == 1 && ExamPaused == 1)
	{
		ExamPaused = 0;

		Yellow_Off();
		Green_On();

		UART_SendString("OK\r\n");

		SendEvent("RESUME", "EXAM");
	}
	else
	{
		UART_SendString("ERR\r\n");
	}
}


/* ================= END ================= */

void CheckExamEnd(void)
{
	if (ExamStarted == 1 && RemainingTime == 0)
	{
		ExamStarted = 0;
		ExamPaused = 0;

		Green_Off();
		Yellow_Off();

		Red_On();

		Buzzer_Long();

		SendEvent("END", "EXAM");
	}
}


/* ================= TIMER ================= */

void UpdateTimer(void)
{
	if (ExamStarted == 0)
	{
		return;
	}

	if (ExamPaused == 1)
	{
		return;
	}

	if (RemainingTime > 0)
	{
		RemainingTime--;
		ElapsedTime++;
	}

	ShowCountdown();


	/* 10 MIN WARNING */

	if (RemainingTime == WARNING_10 &&
	Warning10Sent == 0)
	{
		Warning10Sent = 1;

		Green_Off();
		Yellow_On();

		Buzzer_Beep();

		SendEvent("WARNING", "10");
	}


	/* 1 MIN WARNING */

	if (RemainingTime == WARNING_1 &&
	Warning1Sent == 0)
	{
		Warning1Sent = 1;

		Green_Off();
		Yellow_On();

		Buzzer_Beep();

		SendEvent("WARNING", "1");
	}


	CheckExamEnd();
}


/* ================= RFID ================= */

void CheckCard(void)
{
	unsigned char Candidate;
	RTC_Time Time;

	if (RFID_CheckCard(UID) == 0)
	{
		return;
	}


	/* Candidate must be registered */

	Candidate = EEPROM_FindCandidate(UID);

	if (Candidate == 255)
	{
		Red_On();

		SendEvent("ENTRY", "DENIED");

		Buzzer_Beep();

		_delay_ms(500);

		Red_Off();

		return;
	}


	/* Exam has not started */

	if (ExamStarted == 0)
	{
		Red_On();

		SendEvent("ENTRY", "NOT_STARTED");

		Buzzer_Beep();

		_delay_ms(500);

		Red_Off();

		return;
	}


	/* Duplicate card */

	if (EEPROM_IsDuplicate(UID))
	{
		Yellow_On();

		SendEvent("DUPLICATE", "UID");

		_delay_ms(500);

		Yellow_Off();

		return;
	}


	/* Get entry time */

	if (RTC_GetTime(&Time) == 0)
	{
		Yellow_On();

		SendEvent("ERROR", "RTC");

		return;
	}


	/* Normal entry */

	if (ElapsedTime < LATE_TIME)
	{
		Green_On();

		EEPROM_SaveLog(
		UID,
		Time.hour,
		Time.minute,
		Time.second,
		0
		);

		SendEvent("ENTRY", "OK");

		_delay_ms(500);

		Green_Off();
	}


	/* Late entry */

	else
	{
		Yellow_On();

		EEPROM_SaveLog(
		UID,
		Time.hour,
		Time.minute,
		Time.second,
		1
		);

		SendEvent("ENTRY", "LATE");

		_delay_ms(500);

		Yellow_Off();
	}
}


/* ================= ADD CANDIDATE ================= */

void AddCandidate(void)
{
	unsigned int A;
	unsigned int B;
	unsigned int C;
	unsigned int D;

	unsigned char NewUID[4];

	unsigned char Slot;
	unsigned char Used;

	char Name[16];


	if (sscanf(
	CommandBuffer,
	"ADD %2x%2x%2x%2x %15[^\r\n]",
	&A,
	&B,
	&C,
	&D,
	Name) != 5)
	{
		UART_SendString("ERR\r\n");
		return;
	}


	NewUID[0] = A;
	NewUID[1] = B;
	NewUID[2] = C;
	NewUID[3] = D;


	/* Find empty EEPROM slot */

	for (Slot = 0; Slot < 20; Slot++)
	{
		if (EEPROM_ReadByte(
		Slot * 20 + 4,
		&Used) == 0)
		{
			UART_SendString("ERR\r\n");
			return;
		}

		if (Used != 1)
		{
			break;
		}
	}


	if (Slot >= 20)
	{
		UART_SendString("ERR\r\n");
		return;
	}


	if (EEPROM_SaveCandidate(
	Slot,
	NewUID,
	Name) == 1)
	{
		UART_SendString("OK\r\n");
	}
	else
	{
		UART_SendString("ERR\r\n");
	}
}


/* ================= SET TIME ================= */

void SetTimeCommand(void)
{
	unsigned int Year;
	unsigned int Month;
	unsigned int Day;

	unsigned int Hour;
	unsigned int Minute;
	unsigned int Second;


	if (sscanf(
	CommandBuffer,
	"SETTIME %u-%u-%uT%u:%u:%u",
	&Year,
	&Month,
	&Day,
	&Hour,
	&Minute,
	&Second) != 6)
	{
		UART_SendString("ERR\r\n");
		return;
	}


	if (Hour > 23 ||
	Minute > 59 ||
	Second > 59)
	{
		UART_SendString("ERR\r\n");
		return;
	}


	if (RTC_SetTime(
	Hour,
	Minute,
	Second) == 1)
	{
		UART_SendString("OK\r\n");

		SendEvent("SETTIME", "OK");
	}
	else
	{
		UART_SendString("ERR\r\n");

		SendEvent("ERROR", "RTC");
	}
}


/* ================= COMMANDS ================= */

void ProcessCommand(void)
{
	unsigned int Minutes;


	/* START */

	if (strncmp(CommandBuffer, "START ", 6) == 0)
	{
		if (sscanf(
		CommandBuffer,
		"START %u",
		&Minutes) == 1)
		{
			StartExam(Minutes);
		}
		else
		{
			UART_SendString("ERR\r\n");
		}

		return;
	}


	/* ADD */

	if (strncmp(CommandBuffer, "ADD ", 4) == 0)
	{
		AddCandidate();

		return;
	}


	/* LIST */

	if (strcmp(CommandBuffer, "LIST") == 0)
	{
		EEPROM_ListLogs();

		return;
	}


	/* PAUSE */

	if (strcmp(CommandBuffer, "PAUSE") == 0)
	{
		PauseExam();

		return;
	}


	/* RESUME */

	if (strcmp(CommandBuffer, "RESUME") == 0)
	{
		ResumeExam();

		return;
	}


	/* SETTIME */

	if (strncmp(CommandBuffer, "SETTIME ", 8) == 0)
	{
		SetTimeCommand();

		return;
	}


	UART_SendString("ERR\r\n");
}


/* ================= UART ================= */

void ReadUART(void)
{
	char Character;


	if (UART_Available() == 0)
	{
		return;
	}


	Character = UART_ReceiveChar();


	if (Character == '\r' ||
	Character == '\n')
	{
		CommandBuffer[CommandIndex] = '\0';


		if (CommandIndex > 0)
		{
			ProcessCommand();
		}


		CommandIndex = 0;
	}

	else
	{
		if (CommandIndex < 59)
		{
			CommandBuffer[CommandIndex] = Character;

			CommandIndex++;
		}
	}
}


/* ================= MAIN ================= */

int main(void)
{
	/* Initialize Drivers */

	I2C_Init();

	SPI_Init();

	UART_Init();

	GPIO_Init();

	TIMER_Init();

	RTC_Init();

	EEPROM_Init();

	OLED_Init();

	RFID_Init();


	/* Initial State */

	Green_Off();
	Yellow_Off();
	Red_Off();

	OLED_Clear();

	OLED_SetCursor(45, 3);

	OLED_WriteString("00:00");


	/* BOOT Event */

	SendEvent("BOOT", "v1.0");


	/* Main Loop */

	while (1)
	{
		/* PC Commands */

		ReadUART();


		/* RFID */

		CheckCard();


		/* One Second Timer */

		if (TIMER_GetSecond())
		{
			UpdateTimer();
		}
	}


	return 0;
}