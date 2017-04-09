#include <DriveControl.h>

DriveControl driver(65, 1);

void setup() {
	driver.addInstruction(21, 21);
}

void loop() {
	driver.run();
}
