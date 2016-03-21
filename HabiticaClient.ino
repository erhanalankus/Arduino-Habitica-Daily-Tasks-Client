#include <WiFi101.h>
#include <LiquidCrystal.h>

char ssid[] = "YOUR_WIFI_NAME";			//ENTER INFO (The SSID of your wi-fi)
char pass[] = "YOUR_WIFI_PASSWORD";	//ENTER INFO (Your wi-fi password)
int status = WL_IDLE_STATUS;
WiFiClient client;
WiFiClient client2;

String habiticaUserId = "YOUR_HABITICA_API_USERID";		//ENTER INFO (User ID)     You can find these at:
String habiticaAPIKey = "YOUR_HABITICA_API_TOKEN";		//ENTER INFO (API Token)   https://habitica.com/#/options/settings/api
char server[] = "habiticaclientmiddleman.azurewebsites.net";

LiquidCrystal lcd(11, 10, 9, 8, 7, 6);

const int greenLED = 0;
const int yellowLED = 1;
const int redLED = 2;

const int buzzer = 21;

const int btnBLU = 14;
const int btnRED = 13;

const int latchPin = 3;
const int dataPin = 4;
const int clockPin = 5;

bool toggleLED = true;
bool printOn = false;
int quoteCounter = 1;
String responseJson = "";
String responseGold = "";

int dailyCount = 0;
String id1 = "";
String name1 = "";
String id2 = "";
String name2 = "";
String id3 = "";
String name3 = "";
String id4 = "";
String name4 = "";
String id5 = "";
String name5 = "";
String id6 = "";
String name6 = "";
String id7 = "";
String name7 = "";
String id8 = "";
String name8 = "";
String id9 = "";
String name9 = "";
String activeId = "";
String activeName = "";

int cycleCounter = 1;

int stateBLU = 0;
int stateRED = 0;
int lastStateBLU = 0;
int lastStateRED = 0;
int readingBLU = 0;
int readingRED = 0;
long lastDebounceTimeBLU = 0;
long lastDebounceTimeRED = 0;
const long debounceDelay = 50;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const unsigned long shortBeepDuration = 100;
const unsigned long longBeepDuration = 800;
const unsigned long celebrateDuration = 1500;
const unsigned long checkServerEveryXMilliseconds = 60000;
unsigned long lastCheckedMillis = 0;

void setup() {
	SetLEDsRedYellowGreen(0, 1, 0);
	lcd.begin(16, 2);
	pinMode(latchPin, 1);
	pinMode(dataPin, 1);
	pinMode(clockPin, 1);

	pinMode(greenLED, 1);
	pinMode(yellowLED, 1);
	pinMode(redLED, 1);

	pinMode(btnBLU, 0);
	pinMode(btnRED, 0);

	pinMode(buzzer, 1);

	while (status != WL_CONNECTED)
	{
		lcd.clear();
		lcd.print("Connecting to");
		lcd.setCursor(0, 1);
		lcd.print("wi-fi");
		status = WiFi.begin(ssid, pass);
		delay(5000);
	}
	lcd.clear();
	lcd.print("Connected to");
	lcd.setCursor(0, 1);
	lcd.print("wi-fi");
	delay(2000);

	CheckServer();
}

void loop() {
	ListenForInput();
	while (client.available())
	{
		char c = client.read();

		if (c == '[')
		{
			printOn = true;
		}
		if (printOn)
		{
			responseJson += c;
			if (c == '"')
			{
				quoteCounter++;
			}
			if (quoteCounter % 4 == 0)
			{
				if (c != '"')
				{
					switch (quoteCounter)
					{
					case 4:
						id1 += c;
						break;
					case 8:
						name1 += c;
						break;
					case 12:
						id2 += c;
						break;
					case 16:
						name2 += c;
						break;
					case 20:
						id3 += c;
						break;
					case 24:
						name3 += c;
						break;
					case 28:
						id4 += c;
						break;
					case 32:
						name4 += c;
						break;
					case 36:
						id5 += c;
						break;
					case 40:
						name5 += c;
						break;
					case 44:
						id6 += c;
						break;
					case 48:
						name6 += c;
						break;
					case 52:
						id7 += c;
						break;
					case 56:
						name7 += c;
						break;
					case 60:
						id8 += c;
						break;
					case 64:
						name8 += c;
						break;
					case 68:
						id9 += c;
						break;
					case 72:
						name9 += c;
						break;
					default:
						break;
					}
				}
			}
		}
		if (c == ']')
		{
			printOn = false;
			dailyCount = GetDailyCount();
			ShowDigit(dailyCount);
			PrepareLCD();
		}
	}

	while (client2.available())
	{
		char c = client2.read();
		if (c == '%')
		{
			printOn = true;
		}
		if (printOn)
		{
			if (c == 'g')
			{				
				ShowEarnedGoldOnLCD();
			}
			else if (c != '%')
			{
				responseGold += c;
			}
		}
	}

	if ((millis() - lastCheckedMillis) == checkServerEveryXMilliseconds)
	{
		ResetValues();
		CheckServer();
	}
}

void CheckServer()
{
	SetLEDsRedYellowGreen(0, 1, 0);
	client.stop();
	if (client.connect(server, 80))
	{
		lcd.clear();
		client.println("GET /Hello?habiticaUser=" + habiticaUserId + "&habiticaKey=" + habiticaAPIKey + " HTTP/1.1");
		client.println("Host: habiticaclientmiddleman.azurewebsites.net");
		client.println("Connection: close");
		client.println();
	}
}

void ResetValues()
{
	id1 = "";
	name1 = "";
	id2 = "";
	name2 = "";
	id3 = "";
	name3 = "";
	id4 = "";
	name4 = "";
	id5 = "";
	name5 = "";
	id6 = "";
	name6 = "";
	id7 = "";
	name7 = "";
	id8 = "";
	name8 = "";
	id9 = "";
	name9 = "";
	activeId = "";
	activeName = "";
	cycleCounter = 1;
	dailyCount = 0;
	quoteCounter = 1;
	responseGold = "";
}

void ShowEarnedGoldOnLCD()
{
	lcd.clear();
	lcd.print("You now have:");
	lcd.setCursor(0, 1);
	lcd.print(responseGold + " gold");
	Celebrate();
	ResetValues();
	CheckServer();
}

void Celebrate()
{
	currentMillis = millis();
	while (millis() < (currentMillis + celebrateDuration))
	{
		if (millis() % 2 == 0)
		{
			digitalWrite(buzzer, 1);
		}
		else if (millis() % 2 == 1)
		{
			digitalWrite(buzzer, 0);
		}
		if (millis() % 250 == 0)
		{
			ToggleAllLEDs();
		}
	}
}

void ToggleAllLEDs()
{
	if (toggleLED)
	{
		SetLEDsRedYellowGreen(1, 1, 1);
		toggleLED = false;
	}
	else
	{
		SetLEDsRedYellowGreen(0, 0, 0);
		toggleLED = true;
	}
}

void PrepareLCD()
{
	lastCheckedMillis = millis();
	lcd.clear();
	if (dailyCount == 0)
	{
		lcd.print("You completed");
		lcd.setCursor(0, 1);
		lcd.print("your daily tasks.");
		SetLEDsRedYellowGreen(0, 0, 0);
		return;
	}
	lcd.print("Press 'Cycle'");
	lcd.setCursor(0, 1);
	lcd.print("to see tasks.");
	SetLEDsRedYellowGreen(0, 0, 1);
}

void ShowActiveTaskOnLCD()
{
	lcd.clear();
	lcd.print("Daily Task:");
	lcd.setCursor(0, 1);
	lcd.print(activeName);
}

int GetDailyCount()
{
	if (id1 == "")
	{
		return 0;
	}
	if (id2 == "")
	{
		return 1;
	}
	if (id3 == "")
	{
		return 2;
	}
	if (id4 == "")
	{
		return 3;
	}
	if (id5 == "")
	{
		return 4;
	}
	if (id6 == "")
	{
		return 5;
	}
	if (id7 == "")
	{
		return 6;
	}
	if (id8 == "")
	{
		return 7;
	}
	if (id9 == "")
	{
		return 8;
	}
	return 9;
}

void ListenForInput()    //https://www.arduino.cc/en/Tutorial/Debounce
{
	readingBLU = digitalRead(btnBLU);
	readingRED = digitalRead(btnRED);

	if (readingBLU != lastStateBLU)
	{
		lastDebounceTimeBLU = millis();
	}
	if (readingRED != lastStateRED)
	{
		lastDebounceTimeRED = millis();
	}

	if ((millis() - lastDebounceTimeBLU) > debounceDelay)
	{
		if (readingBLU != stateBLU)
		{
			stateBLU = readingBLU;
			if (stateBLU == LOW)
			{
				ActionBLU();
			}
		}
	}
	if ((millis() - lastDebounceTimeRED) > debounceDelay)
	{
		if (readingRED != stateRED)
		{
			stateRED = readingRED;
			if (stateRED == LOW)
			{
				ActionRED();
			}
		}
	}

	lastStateBLU = readingBLU;
	lastStateRED = readingRED;
}

void CycleTasks()
{
	SetActive(cycleCounter);
	ShowActiveTaskOnLCD();
	cycleCounter++;
	if (cycleCounter % (dailyCount + 1) == 0)
	{
		cycleCounter = 1;
	}
	SetLEDsRedYellowGreen(0, 0, 1);
}

void SetActive(int i)
{
	switch (i)
	{
	case 1:
		activeId = id1;
		activeName = name1;
		break;
	case 2:
		activeId = id2;
		activeName = name2;
		break;
	case 3:
		activeId = id3;
		activeName = name3;
		break;
	case 4:
		activeId = id4;
		activeName = name4;
		break;
	case 5:
		activeId = id5;
		activeName = name5;
		break;
	case 6:
		activeId = id6;
		activeName = name6;
		break;
	case 7:
		activeId = id7;
		activeName = name7;
		break;
	case 8:
		activeId = id8;
		activeName = name8;
		break;
	case 9:
		activeId = id9;
		activeName = name9;
		break;
	default:
		break;
	}
}

void ActionBLU()
{
	SetLEDsRedYellowGreen(0, 1, 0);
	if (dailyCount != 0)
	{
		CycleTasks();		
	}
	else
	{
		ResetValues();
		CheckServer();
	}
	BeepShort();
}

void ActionRED()
{
	SetLEDsRedYellowGreen(0, 1, 0);
	if (dailyCount != 0)
	{
		SendActiveTask();		
	}
	else
	{
		ResetValues();
		CheckServer();
	}
	BeepShort();
}

void SendActiveTask()
{
	client2.stop();
	if (client2.connect(server, 80))
	{
		lcd.clear();
		client2.println("GET /Hello/CheckTask?habiticaUser=" + habiticaUserId + "&habiticaKey=" + habiticaAPIKey + "&taskId=" + activeId + " HTTP/1.1");
		client2.println("Host: habiticaclientmiddleman.azurewebsites.net");
		client2.println("Connection: close");
		client2.println();
	}
}

void Beep()    //https://tkkrlab.nl/wiki/Arduino_KY-006_Small_passive_buzzer_module
{
	currentMillis = millis();
	while (millis() < (currentMillis + longBeepDuration))
	{
		if (millis() % 2 == 0)
		{
			digitalWrite(buzzer, 1);
		}
		else if (millis() % 2 == 1)
		{
			digitalWrite(buzzer, 0);
		}
	}
}

void BeepShort()
{
	currentMillis = millis();
	while (millis() < (currentMillis + shortBeepDuration))
	{
		if (millis() % 3 == 0)
		{
			digitalWrite(buzzer, 0);
		}
		else
		{
			digitalWrite(buzzer, 1);
		}
	}

}

void ShowDigit(int digit)    //https://youtu.be/cHA-fwiA4Qo
{
	digitalWrite(latchPin, LOW);
	switch (digit)
	{
	case 0:
		shiftOut(dataPin, clockPin, MSBFIRST, 63);
		digitalWrite(latchPin, HIGH);
		break;
	case 1:
		shiftOut(dataPin, clockPin, MSBFIRST, 6);
		digitalWrite(latchPin, HIGH);
		break;
	case 2:
		shiftOut(dataPin, clockPin, MSBFIRST, 91);
		digitalWrite(latchPin, HIGH);
		break;
	case 3:
		shiftOut(dataPin, clockPin, MSBFIRST, 79);
		digitalWrite(latchPin, HIGH);
		break;
	case 4:
		shiftOut(dataPin, clockPin, MSBFIRST, 102);
		digitalWrite(latchPin, HIGH);
		break;
	case 5:
		shiftOut(dataPin, clockPin, MSBFIRST, 109);
		digitalWrite(latchPin, HIGH);
		break;
	case 6:
		shiftOut(dataPin, clockPin, MSBFIRST, 125);
		digitalWrite(latchPin, HIGH);
		break;
	case 7:
		shiftOut(dataPin, clockPin, MSBFIRST, 7);
		digitalWrite(latchPin, HIGH);
		break;
	case 8:
		shiftOut(dataPin, clockPin, MSBFIRST, 127);
		digitalWrite(latchPin, HIGH);
		break;
	case 9:
		shiftOut(dataPin, clockPin, MSBFIRST, 103);
		digitalWrite(latchPin, HIGH);
		break;
	default:
		break;
	}
}

void SetLEDsRedYellowGreen(int redState, int yellowState, int greenState)
{
	digitalWrite(redLED, redState);
	digitalWrite(yellowLED, yellowState);
	digitalWrite(greenLED, greenState);
}

void PrintWifiStatus() {
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your WiFi shield's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}
