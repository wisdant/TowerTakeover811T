#include "main.h"

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
/*void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::Motor left_mtr(1);
	pros::Motor right_mtr(2);
	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
		int left = master.get_analog(ANALOG_LEFT_Y);
		int right = master.get_analog(ANALOG_RIGHT_Y);

		left_mtr = left;
		right_mtr = right;
		pros::delay(20);
	}
}*/
const double DRIVE_SPEED = 0.75; // Changes the maximum drive speed (range between 0 and 1)
const int ARCADE = true; // Controls whether the drive is tank style (false) or arcade style (true)
const int DRIVE_THRESHOLD  = 12; // Controls the minimum power that can be assigned to drive motors

const int RIGHT_FRONT_PORT = 1;
const int RIGHT_REAR_PORT = 4;
const int RIGHT_ARM_PORT = 5;

const int LIFT_PORT = 2;
const int HINGE_PORT = 3;

const int LEFT_FRONT_PORT = 10;
const int LEFT_REAR_PORT = 9;
const int LEFT_ARM_PORT = 8;

// Define the main controller of the robot.
pros::Controller master(pros::E_CONTROLLER_MASTER);

// Define the robot's motors with their respective ports.
// Passing true for the second parameter reverses the motor.
pros::Motor left_drive (LEFT_FRONT_PORT, false);
pros::Motor left_rear_drive (LEFT_REAR_PORT, false);

pros::Motor right_drive (RIGHT_FRONT_PORT, true);
pros::Motor right_rear_drive (RIGHT_REAR_PORT, true);

pros::Motor left_arm (LEFT_ARM_PORT, false);
pros::Motor right_arm (RIGHT_ARM_PORT, true);

pros::Motor lift (LIFT_PORT, false);
pros::Motor hinge (HINGE_PORT, false);

/**
 * Assign specified powers for the right and left sides of the drivetrain to the left and right motors, respectively.
 */
void move_drive(int right, int left) {
	right_drive.move(right);
	right_rear_drive.move(right);
	left_drive.move(left);
	left_rear_drive.move(left);
}

/**
 * Assign a specified power to the lift motor.
 */
void move_lift(int power) {
	lift.move(power);
}

/**
 * Assign a specified power to the claw motor.
 */
void move_hinge(int power) {
	hinge.move(power);
}

/**
 * Runs the operator control code. This function will be started in its own task
 * whenever driver control mode is entered when connected to a match controller.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, this task will be stopped.
 * Re-enabling the robot will restart the task, not resume it from where it left off.
 */
void opcontrol() {
	/*
	 * Define variables used by different robot functionality
	 */
	// Drive code variables
	int left_y;  // Left joystick Y
	int right_x; // Right joystick X
	int right_y; // Right joystick Y
	int right_power;
	int left_power;

	while (true) {
		/*
		 * Drivetrain code
		 * This code controls how the robot moves around the field
		 */

		// Assign the values from the controller's joysticks to variables
		left_y = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
		right_x = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
		right_y = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);

		// Filter joystick values to prevent the drive from responding to miscalibrated joysticks
		if (std::abs(left_y) < DRIVE_THRESHOLD) {
			left_y = 0;
		}
		if (std::abs(right_y) < DRIVE_THRESHOLD) {
			right_y = 0;
		}
		if(std::abs(right_x) < DRIVE_THRESHOLD) {
			right_x = 0;
		}

		// Choose between arcade and tank drive controls
		// and assign correct powers for the right and left sides of the robot
		if (ARCADE) { // if arcade mode
			right_power = left_y - right_x;
			left_power = left_y + right_x;
		}
		else { // if tank mode
			right_power = right_y;
			left_power = left_y;
		}

		// Limit motor powers so that they do not exceed the maximum motor possible power value
		if (std::abs(right_power) > 127) {
			right_power = std::copysign(127, right_power); // Copies the sign of right_power to 127
		}
		if (std::abs(left_power) > 127) {
			left_power = std::copysign(127, left_power); // Copies the sign of left_power to 127
		}

		// Limit drivetrain speed based on the constant defined above
		right_power = (int) (right_power * DRIVE_SPEED);
		left_power = (int) (left_power * DRIVE_SPEED);

		// Assign power to the motors
		move_drive(right_power, left_power);

		/*
		 * Lift code
		 * This code controls the arm of the robot
		 */

		if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R1) == 1) {	// Check if a button on the controller is pressed
			move_lift(127); // Move the lift up
			while (master.get_digital(pros::E_CONTROLLER_DIGITAL_R1) == 1) { // Wait for the button to be released
				pros::delay(10);
			}
			move_lift(10); // Assign a little power and allow the motor to hold it's position.
		}
		else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R2) == 1) {
			move_lift(-40); // Move the lift down
			while (master.get_digital(pros::E_CONTROLLER_DIGITAL_R2) == 1) {
				pros::delay(10);
			}
			move_lift(10); // Assign a little power and allow the motor to hold it's position.
		}

		/*
		 * Claw code
		 * This code controls the claw on the robot
		 */
		if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L1) == 1) {
			move_hinge(127); // Close the claw
			while (master.get_digital(pros::E_CONTROLLER_DIGITAL_L1) == 1) {
				pros::delay(10);
			}
			move_hinge(25); // Assign a little power and allow the motor to hold it's position.
		}
		else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2) == 1) {
			move_hinge(-127); // Open the claw
			while (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2) == 1) {
				pros::delay(10);
			}
			move_hinge(-10); // Assign a little power and allow the motor to hold it's position.
		}


		// IMPORTANT: DO NOT REMOVE
		// This delay allows all other robot related functionality to be run between iterations of this while loop.
		pros::delay(10);
	}
}
