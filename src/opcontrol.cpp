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
	lcd::print(2, "left intake - current draw: %d, power: %d", left_intake.get_current_draw(), left_intake.get_power());
	lcd::print(3, "right intake - current draw: %d, power: %d", right_intake.get_current_draw(), right_intake.get_power());
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
	bool hinge_down = true;

	left_drive.set_encoder_units(E_MOTOR_ENCODER_COUNTS);
	left_rear_drive.set_encoder_units(E_MOTOR_ENCODER_COUNTS);
	right_drive.set_encoder_units(E_MOTOR_ENCODER_COUNTS);
	right_rear_drive.set_encoder_units(E_MOTOR_ENCODER_COUNTS);
	left_drive.tare_position();
  left_rear_drive.tare_position();
  right_drive.tare_position();
  right_rear_drive.tare_position();


	while (true) {
		static int counter = 0;

		lcd::print(1, "1/18/2020 3:43PM, left intake port#=%d, counter=%d", LEFT_INTAKE_PORT, counter++);

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

		if (right_power != 0) {
			lcd::print(4, "LF: %d, LR: %d, RF: %d, RR: %d",
			left_drive.get_position(), left_rear_drive.get_position(),
			right_drive.get_position(), right_rear_drive.get_position());

			printf("LF: %f, LR: %f, RF: %f, RR: %f\n",
			left_drive.get_position(), left_rear_drive.get_position(),
			right_drive.get_position(), right_rear_drive.get_position());
		}
		/*
		 * Intake code
		 * This code controls the intake motors
		 */
		if (master.get_digital(E_CONTROLLER_DIGITAL_L1) == 1) {
			move_lift(-20);

			move_intake(127); // Move the intake belt
			//while (master.get_digital(E_CONTROLLER_DIGITAL_L1) == 1) { // Wait for the button to be released
			//	delay(10);
			//}
		}
		if (master.get_digital(E_CONTROLLER_DIGITAL_A) == 1) {
			lcd::print(3, "L2 pressed");
			move_intake(-127); // Move the intake belt backward, for removing blocks from towers
			while (master.get_digital(E_CONTROLLER_DIGITAL_A) == 1) {
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
			delay(20);
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
		if (master.get_digital(E_CONTROLLER_DIGITAL_L2) == 1) {
			/*
				move_hinge(127);
				delay(150);
				move_hinge(0);
				hinge_at_resting = false;

			move_lift(127);
			while (master.get_digital(E_CONTROLLER_DIGITAL_A) == 1) {
				delay(10);
			}
			move_lift(20);
			*/
			move_hinge(127);
			delay(900);
			move_hinge(-127);
			delay(700);
			move_hinge(0);
		}

		// For the high tower
		if (master.get_digital(E_CONTROLLER_DIGITAL_UP) == 1) {
			if (hinge_down == true) {
				move_hinge(127);
				delay(500); // Richard: changed from 300 to 500
				move_hinge(0);
				hinge_down = false;
			}
			move_lift(127);
			delay(1570);
			move_lift(20);
		}

		// For the middle and alliance towers
		if (master.get_digital(E_CONTROLLER_DIGITAL_LEFT) == 1) {
			if (hinge_down == true) {
				move_hinge(127);
				delay(500);	// Richard: changed from 300 to 500
				move_hinge(0);
				hinge_down = false;
			}
			move_lift(127);
			delay(1000); //1100
			move_lift(20);
		}

		// For descoring towers
		if (master.get_digital(E_CONTROLLER_DIGITAL_RIGHT) == 1) {
			if (hinge_down == true) {
				move_hinge(127);
				delay(500); // Richard: changed from 300 to 500
				move_hinge(0);
				hinge_down = false;
			}
			move_lift(127);
			delay(800);
			move_lift(20);
		}

		if (master.get_digital(E_CONTROLLER_DIGITAL_Y) == 1) {
			//move_hinge(127);
			//delay(200);
			move_hinge(0);
			move_lift(-50); // Move down the lift
			while (master.get_digital(E_CONTROLLER_DIGITAL_Y) == 1) {
				delay(10);
			}
			//move_hinge(-127);
			//delay(250);	// Richard: commented out this line since it is not useful
			move_lift(0);
			move_drive(0, 0);
			//move_hinge(0);
		}

		//A special button Y to do a little bit reverse spin of the intake, basically L2
		if (master.get_digital(E_CONTROLLER_DIGITAL_DOWN) == 1) {
			int temp_ts = millis();
			move_intake(-40);
			move_drive(-40, -40);
			while (master.get_digital(E_CONTROLLER_DIGITAL_DOWN) == 1) {
				delay(10);
			}
			printf("Pressing Y for = %d millis\n", millis() - temp_ts);
			move_intake(0);
		}

		if (master.get_digital(E_CONTROLLER_DIGITAL_X) == 1) {
			//currently, this code is the same as the stackSixEight in auton (2/9)
			move_intake(0);
		  move_lift(-20);

		  move_hinge(100);
		  move_intake(20);
		  delay(900); //300

		  move_hinge(60);
		  move_intake(-20);
		  delay(1100);

		  move_hinge(40);
		  delay(1200);

		  move_intake(0);
		  move_drive(0, 0);
		  move_hinge(0);
		  move_lift(0);
		}

		if (master.get_digital(E_CONTROLLER_DIGITAL_B) == 1) {
			move_intake(0);
		  move_lift(-20);

		  move_hinge(100);
		  move_intake(80);
		  delay(800);

			move_hinge(100);
		  move_intake(70); //60
		  delay(200);

			move_hinge(100);
		  move_intake(0);
		  delay(700);

		  move_hinge(60);
		  move_intake(-20);
		  delay(400);

		  move_hinge(40);
		  delay(1600);

			//move_hinge(20);	//Richard: commented out since these two lines are not necessary
			//delay(1800);

		  move_intake(0);
		  move_drive(0, 0);
		  move_hinge(0);
		  move_lift(0);
			hinge_at_resting = false;
		}

		delay(10);
	}
}
