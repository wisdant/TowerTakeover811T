#include "main.h"
using namespace std;
using namespace pros;

// Define the main controller of the robot.
extern Controller master;

extern Motor left_drive;
extern Motor left_rear_drive;

extern Motor right_drive;
extern Motor right_rear_drive;

extern Motor left_intake;
extern Motor right_intake;

extern Motor lift;
extern Motor hinge;

extern Vision vision_sensor;

/**
 * Assign specified powers for the right and left sides of the drivetrain to the left and right motors, respectively.
 */
void move_drive(int right, int left) {
	left_drive.move(left);
	left_rear_drive.move(left);
	right_drive.move(right);
	right_rear_drive.move(right);
}

/**
 * Assign a specified power to the in-take motor.
 */
void move_intake(int power) {
	left_intake.move(power);
	right_intake.move(power);
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
	int left_x;  // Left joystick X
	int left_y;  // Left joystick Y
	int right_x; // Right joystick X
	int right_y; // Right joystick Y
	int right_power;
	int left_power;

	int button_l1;
	int button_l2;
	int button_r1;
	int button_r2;

	int button_up;
	int button_down;
	int button_left;
	int button_right;

	int button_x;
	int button_b;
	int button_y;
	int button_a;

	// A special variable to remember if the hinge is at the resting position.
	// When we press "A" to lift the arm, if the hinge is at the resting position,
	// we will move the hinge forward a little bit so that it does not block the arm.
	// However, if the hinge is not at the resting position, we do not need to
	// do anything speical.
	// NOTE: we assume whenever we move the hinge back, we always move all the way back.
	// This technically is not 100% accurate, but in reality, it should be the case.
	// Even if it is not accurate, the driver can always manully move the hinge.
	bool hinge_at_resting = true;

	while (true) {
		static int counter = 0;

		lcd::print(1, "12/29 11:46PM counter=%d", counter++);

		/*
		 * Drivetrain code
		 * This code controls how the robot moves around the field
		 */

		// Assign the values from the controller's joysticks to variables
		left_x = master.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		left_y = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
		right_x = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
		right_y = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_Y);

		// Filter joystick values to prevent the drive from responding to miscalibrated joysticks
		// if (std::abs(left_y) < DRIVE_THRESHOLD) {
		// 	left_y = 0;
		// }
		//if (std::abs(right_y) < DRIVE_THRESHOLD) {
		//	right_y = 0;
		//}
		if(std::abs(right_x) < DRIVE_THRESHOLD_RIGHT_CONTROL) {
			right_x = 0;
		}

		// Choose between arcade and tank drive controls
		// and assign correct powers for the right and left sides of the robot


		if (ARCADE) { // if arcade mode
			// TODO(Angela): Add special logic to make robot less sensitive to turn signals
			// right_x represents the strength of turn signals. A simple division of right_x
			// by 2 will make the robot 50% less sensitive to the turn signals.
			// <Angela's code goes here.>
			right_x /= 2;

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
		 * Intake code
		 * This code controls the intake motors
		 */
		if (master.get_digital(E_CONTROLLER_DIGITAL_L1) == 1) {
			lcd::print(3, "L1 pressed");
			move_lift(-20);

			move_intake(127); // Move the intake belt
			//while (master.get_digital(E_CONTROLLER_DIGITAL_L1) == 1) { // Wait for the button to be released
			//	delay(10);
			//}
		}
		if (master.get_digital(E_CONTROLLER_DIGITAL_L2) == 1) {
			lcd::print(3, "L2 pressed");
			move_intake(-127); // Move the intake belt backward, for removing blocks from towers
			while (master.get_digital(E_CONTROLLER_DIGITAL_L2) == 1) {
				delay(10);
			}
			move_intake(0);
		}

		/*
		 * hinge code
		 * This code controls the hinge that is responsible for pushing the tray
		 */
		 if (master.get_digital(E_CONTROLLER_DIGITAL_R1) == 1) {
 			lcd::print(4, "R1 pressed");
			move_intake(0); // Stop intake roller

 			move_hinge(50); // Move the lift forward
 			while (master.get_digital(E_CONTROLLER_DIGITAL_R1) == 1) { // Wait for the button to be released
 				delay(10);
 			}
 			move_hinge(0);
			delay(10);
			hinge_at_resting = false;
 		}

		 if (master.get_digital(E_CONTROLLER_DIGITAL_R2) == 1) {
 			lcd::print(4, "R2 pressed");
 			move_intake(0);		// Stop intake roller
 			move_hinge(-127); // Move the hinge backward
 			while (master.get_digital(E_CONTROLLER_DIGITAL_R2) == 1) {
 				delay(10);
 			}
 			move_hinge(0);
			hinge_at_resting = true;
 		}

		/*
		 * Lift code
		 * This code controls the lift
		 */
		if (master.get_digital(E_CONTROLLER_DIGITAL_A) == 1) {
			///move_hinge(127);
			///delay(200);
			///move_hinge(0);
			/// above valid?
			/// Technically, this is not right. The hinge will move forward for 200ms
			/// when you press "A" once. If you press it multiple times, the hinge will move
			/// multiple times. We add a variable to remember whether the hinge is
			/// at resting position. We apply this logic only if the hinge is not at resting.

			//if (hinge_at_resting) {
				move_hinge(127);
				delay(150);
				move_hinge(0);
				hinge_at_resting = false;
			//}

			move_lift(127); // move up the lift
			while (master.get_digital(E_CONTROLLER_DIGITAL_A) == 1) {
				delay(10);
			}
			move_lift(20); // Assign a little power and allow the motor to hold it's position.
		}

		// For the high tower
		if (master.get_digital(E_CONTROLLER_DIGITAL_UP) == 1) {
			move_hinge(127);
			delay(350);
			move_hinge(0);
			hinge_at_resting = false;

			move_lift(127);
			delay(1400);
			move_lift(20);
		}

		// For the middle and alliance towers
		if (master.get_digital(E_CONTROLLER_DIGITAL_LEFT) == 1) {
			move_hinge(127);
			delay(350);
			move_hinge(0);
			hinge_at_resting = false;

			move_lift(127);
			delay(1100);
			move_lift(20);
		}

		// For descoring towers
		if (master.get_digital(E_CONTROLLER_DIGITAL_RIGHT) == 1) {
			move_hinge(127);
			delay(300);
			move_hinge(0);
			hinge_at_resting = false;

			move_lift(127);
			delay(830);
			move_lift(20);
		}

		if (master.get_digital(E_CONTROLLER_DIGITAL_B) == 1) {
			move_hinge(127);
			delay(400);
			move_hinge(0);
			move_lift(-50); // Move down the lift
			while (master.get_digital(E_CONTROLLER_DIGITAL_B) == 1) {
				delay(10);
			}
			move_hinge(-127);
			delay(250);
			move_lift(0);
			move_hinge(0);
		}

		//A special button Y to do a little bit reverse spin of the intake, basically L2
		if (master.get_digital(E_CONTROLLER_DIGITAL_Y) == 1) {
			int temp_ts = millis();
			move_intake(-40);
			while (master.get_digital(E_CONTROLLER_DIGITAL_Y) == 1) {
				delay(10);
			}
			printf("Pressing Y for = %d millis\n", millis() - temp_ts);
			move_intake(0);
		}

		//A special button X to do a little bit reverse spin of the intake, basically L2
		if (master.get_digital(E_CONTROLLER_DIGITAL_X) == 1) {

			lcd::print(3, "X pressed");
			move_intake(80);
			while (master.get_digital(E_CONTROLLER_DIGITAL_X) == 1) {
				delay(10);
			}
			move_intake(2);
			/*
			move_intake(0);
	    move_lift(-20);

	    move_hinge(127); // Move the lift forward
	    move_intake(20);
	    delay(500);

	    move_intake(-25);

	    move_hinge(100);
	    delay(200);

	    move_hinge(80);
	    delay(200);

	    move_hinge(60);
	    delay(100);

	    move_hinge(50);
	    delay(250);

	    move_hinge(40);
	    delay(900);

	    move_intake(-80);
	    delay(20);

			move_intake(-40);
		  move_drive(-100, -100);
		  move_hinge(-127);
		  delay(300);

			move_intake(0);
	    move_drive(0, 0);
	    move_hinge(0);
	    move_lift(0);
			*/


		}

		//A special button down to do scoring. It first moves at a fast speed. Then function as R1.
		if (master.get_digital(E_CONTROLLER_DIGITAL_DOWN) == 1) {
			int temp_ts = millis();

			printf("DOWN pressed.\n");
			move_intake(0); // Stop intake roller
			move_lift(0);

 			move_hinge(127); // Move the lift forward
			move_intake(40);
			delay(800);	// changed from 500 to 1000 on 12/27, changed to 800
			move_intake(0);
			printf("After 127. time spent = %d\n", millis() - temp_ts);

			move_hinge(80);
		  delay(400);		// changed from 300 to 400 on 12/27

		  move_hinge(60);
		  delay(400);		// changed from 300 to 400 on 12/27

			move_hinge(40);
			move_intake(-20);
			//move_intake(-10);
 			while (master.get_digital(E_CONTROLLER_DIGITAL_DOWN) == 1) { // Wait for the button to be released
 				delay(10);
 			}

			printf("Done scoring. Time spent  = %d\n", millis() - temp_ts);


 			move_hinge(0);
			hinge_at_resting = false;

		}

		//vision_object_s_t rtn = vision_sensor.get_by_size(0);
    // Gets the largest object
		//lcd::print(5, "sig: %s", rtn.signature);
		//lcd::print(5, "sig: %d", vision_sensor.get_object_count());

		// IMPORTANT: DO NOT REMOVE
		// This delay allows all other robot related functionality to be run between iterations of this while loop.
		delay(10);
	}
}
