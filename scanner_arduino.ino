#include <AccelStepper.h>
#include <MultiStepper.h>

int serialBaudrate = 115200;
char stopRead = '/';

int pinTStep = 2; //Установить
int pinTDir = 3; //Установить
int tMaxSpeed = 10; //Установить
int tAcceleration = 10; //Установить
float tReduction = 1.0; //Установить
String tLastCommand = "";

int pinZStep = 4; //Установить
int pinZDir = 5; //Установить
int zMaxSpeed = 10; //Установить
int zAcceleration = 10; //Установить
int zHomingSpeed = 10; //Установить
float zReduction = 1.0; //Установить
String zLastCommand = "";
bool isZHoming = false;
int zEndStop = 12; //Установить

int pinDoorEndstop = 6; //Установить

int pinLaser = 8; //Установить


AccelStepper tStepper(1, pinTStep, pinTDir);
AccelStepper zStepper(1, pinZStep, pinZDir);


void setup() {
	Serial.begin(115200);

	tStepper.setMaxSpeed(tMaxSpeed);
	tStepper.setAcceleration(tAcceleration);

	zStepper.setMaxSpeed(zMaxSpeed);
	zStepper.setAcceleration(zAcceleration);
	pinMode(zEndStop, INPUT);

	pinMode(pinDoorEndstop, INPUT);

	pinMode(pinLaser, OUTPUT);
}

int getStep(float distance) {
	return distance; //НАПИСАТЬ ФОРМУЛЫ
}

void loop() {
	if(digitalRead(pinDoorEndstop) && isZHoming && zStepper.isRunning()) {
		zStepper.setSpeed(0);
		isZHoming = false;
		Serial.println("SZH" + String(stopRead));
	}

	if (Serial.available()) {
    	String data = Serial.readStringUntil(stopRead);

    	if (data == "DI")
    		Serial.println(data + String(digitalRead(pinDoorEndstop)));
	
		if (data == "LN") {
			digitalWrite(pinLaser, HIGH);
			Serial.println(data + String(stopRead));
		}

		if (data == "LF") {
			digitalWrite(pinLaser, LOW);
			Serial.println(data + String(stopRead));
		}

		if (data[0] == 'S') 
		{
			if (data[2] == 'T') {
				float distance = (data.substring(3)).toFloat();
				int steps = getStep(distance);

				if (data[1] == 'T' && !tStepper.isRunning()) {
					tStepper.move(steps);
					tLastCommand = data;
				}

				if (data[1] == 'Z' && !zStepper.isRunning()) {
					zStepper.move(steps);
					zLastCommand = data;
				}
			}

			if (data[2] == 'H' && !zStepper.isRunning()) {
				zStepper.setSpeed(zHomingSpeed);
				isZHoming = true;
			}

		}
	}

	if (!tStepper.isRunning() && tLastCommand != "") {
		Serial.println(tLastCommand + String(stopRead));
		tLastCommand = "";
	}

	if (!zStepper.isRunning() && zLastCommand != "") {
		Serial.println(zLastCommand + String(stopRead));
		zLastCommand = "";
	}

	if (tLastCommand != "")
		tStepper.run();
	if (zLastCommand != "")
		zStepper.run();
	if (isZHoming)
		zStepper.runSpeed();
}
