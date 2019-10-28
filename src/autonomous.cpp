#include "main.h"

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
//void autonomous() {}

#define delay(time) pros::delay(time)

// Define the main controller of the robot.
pros::Controller a_master(pros::E_CONTROLLER_MASTER);

// Define the robot's motors with their respective ports.
// Passing true for the second parameter reverses the motor.
pros::Motor a_left_drive (LEFT_FRONT_PORT, false);
pros::Motor a_left_rear_drive (LEFT_REAR_PORT, false);

pros::Motor a_right_drive (RIGHT_FRONT_PORT, true);
pros::Motor a_right_rear_drive (RIGHT_REAR_PORT, true);

pros::Motor a_left_intake (LEFT_INTAKE_PORT, pros::E_MOTOR_GEARSET_36, false);
pros::Motor a_right_intake (RIGHT_INTAKE_PORT, pros::E_MOTOR_GEARSET_36, true);

pros::Motor a_lift (LIFT_PORT, pros::E_MOTOR_GEARSET_36, true);
pros::Motor a_hinge (HINGE_PORT, pros::E_MOTOR_GEARSET_36, true);


/**
 * Assign specified powers for the right and left sides of the drivetrain to the left and right motors, respectively.
 */
void a_move_drive(int right, int left) {
  a_left_drive.move(left);
  a_left_rear_drive.move(left);
  a_right_drive.move(right);
  a_right_rear_drive.move(right);
}

/**
 * Assign a specified power to the in-take motor.
 */
void a_move_intake(int power) {
  a_left_intake.move(power);
  a_right_intake.move(power);
}

/**
 * Assign a specified power to the lift motor.
 */
void a_move_lift(int power) {
  a_lift.move(power);
}

/**
 * Assign a specified power to the claw motor.
 */
void a_move_hinge(int power) {
  a_hinge.move(power);
}

/**
 * Score in the goal zone
 */
void score() {
  // Push the hinge
  a_move_drive(0, 0);
  a_move_intake(0);

  //move hinge 50 for 2400
  a_move_hinge(50);
  delay(2450); //delay(2300); //delay(2100);
  a_move_hinge(30);
  delay(600); ///changed

  a_move_hinge(0);
  a_move_drive(50,50);
  delay(300);
  a_move_drive(0, 0);
}
/// ---------------------------------------------------------------------------

void small(bool isRed) {
  a_move_lift(-20);
  // start in-take
  a_move_intake(127);

  a_move_drive(127,127);
  delay(375);

  a_move_drive(10, 10);
  delay(800);

  a_move_drive(127,127);
  delay(150);

  a_move_drive(10, 10);
  delay(800);

  a_move_drive(127,127);
  delay(150);

  a_move_drive(10, 10); /// a_move_drive(20, 20);
  delay(1100); ///delay(1200);

  a_move_drive(127,127);
  delay(200);

  a_move_drive(10, 10); ///a_move_drive(20, 20);
  delay(1000); ///delay(700);

  a_move_drive(127, 127); ///
  delay(90); ///
  a_move_drive(20, 20); ///
  delay(1000); ///

  // Just hold the blocks
  a_move_intake(30);

  // drive back to the edge
  /* a_move_drive(-127,-127);
  delay(1100);
  */

  /*///
  a_move_drive(-127,-127);
  delay(800);
  a_move_drive(-50,-50);
  delay(1000); //changed since we moved up further from 900
  *////

  /*
  a_move_drive(-127, -127);
  delay(900);
  */

  if (isRed) {
    a_move_drive(-127, -127);
    delay (900);
  }
  else {
    pros::lcd::print(5, "Hello this is blue small");
    a_move_drive(-110, -127);
    delay(700);
  }

  a_move_drive(0, 0); ///
  delay(200); ///

//////////

  if (isRed )
    //Make a right turn
    a_move_drive(-50, 50);
  else
    //Make a left turn
    a_move_drive(50, -50);

  //delay(1250);
  delay(850); ///delay(1000);

  // Drive to the goal zone
  a_move_drive(50, 50); //a_move_drive(35, 35);
  delay(800); //delay(1100); //delay(1000);

  if (isRed )
    //Make a right turn
    a_move_drive(-50, 50); /// a_move_drive (-40, 40);
  else
    //Make a left turn
    a_move_drive(50, -50); /// a_move_drive (40, -40);

  //delay(1250);
  delay(250); ///delay(150);

  score();

  a_move_drive(-70, -70); //a_move_drive(-50,-50);
  delay(700);
  a_move_drive(0, 0);

  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);

}

/// ----------------------------------------------------------------------------

void big(bool isRed) {
  // Sample Code
  // Move forward
  // a_move_drive(30,30);

  // Move backward
  // a_move_drive(-30, -30);

  // Left turn
  // a_move_drive(30, 0);

  // right turn
  // a_move_drive(0, 30);
  a_move_intake(127);

  // pick up first block
  a_move_drive(127, 127);
  delay(400);


  if (isRed) {
    // turn left at the location
    a_move_drive(50, -50);
  } else {
    // turn right at the location
    a_move_drive(-50, 50);
  }
  delay(600);

  // drive forward to pick up the second block
  a_move_drive(80, 80);
  delay(800);

  a_move_drive(0,0);
  delay(1000);

  // hold the block
  a_move_intake(20);
  if (isRed) {
    // turn left for 45 degrees
    a_move_drive(50, -50);
  } else {
    // turn right for 45 degrees
    a_move_drive(-50, 50);
  }

  delay(550);

  // drive to the goal zone
  a_move_drive(35, 35);
  delay(1000);

  a_move_intake(0);
  a_move_drive(0, 0);

  a_move_hinge(100);
  delay(600);
  a_move_hinge(40);
  delay(1200);
  a_move_hinge(20);
  delay(800);
  a_move_intake(-20);
  delay(100);

  a_move_hinge(0);
  a_move_intake(0);

  a_move_drive(-50, -50);
  delay(500);
  a_move_drive(0, 0);

  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);


}

void big2(bool isRed) {
  // Sample Code
  // Move forward
  // a_move_drive(30,30);

  // Move backward
  // a_move_drive(-30, -30);

  // Left turn
  // a_move_drive(30, 0);

  // right turn
  // a_move_drive(0, 30);
  a_move_intake(127);

  // pick up first block
  a_move_drive(50, 50);
  delay(1000);


  if (isRed) {
    // turn left at the location
    a_move_drive(50, -50);
  } else {
    // turn right at the location
    a_move_drive(-50, 50);
  }
  delay(650);

  // drive forward to pick up the second block
  a_move_drive(50, 50);
  delay(2000); ///changed from 2000 to 2500

  a_move_drive(0,0);
  delay(1000);

  // hold the block
  a_move_intake(20);
  if (isRed) {
    // turn left for 45 degrees
    a_move_drive(50, -50);
  } else {
    // turn right for 45 degrees
    a_move_drive(-50, 50);
  }

  delay(700);

  // drive to the goal zone
  a_move_drive(35, 35);
  delay(1100);

  a_move_intake(0);
  a_move_drive(0, 0);

  a_move_hinge(100);
  delay(600);
  a_move_hinge(40);
  delay(1200);
  a_move_hinge(20);
  delay(800);
  a_move_intake(-20);
  delay(100);

  a_move_hinge(0);
  a_move_intake(0);

  a_move_drive(-50, -50);
  delay(700);
  a_move_drive(0, 0);

  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);


}

/*
void big2 (bool isRed) {
  /// add: a_move_drive(100, 100)
  /// add: delay(200);
  /// add: a_move_drive(0, 0);
  if (isRed) {
    a_move_drive(-50, 50);
  }
  else {
    a_move_drive(50, -50);
  }
  delay(1500);
  a_move_drive(0, 0);
  a_move_drive(-50, -50);
  delay(2000);
  a_move_drive(0, 0);
  a_move_drive(50, 50);
  delay(2000);
  a_move_drive(0,0);
  delay(1000);
}
*/
/// ----------------------------------------------------------------------------



void unfold() {
  // Move hinge forward to make it loose
  a_hinge.move(127);
  delay(600);

  a_hinge.move(-127);
  delay(600);

  a_hinge.move(0);

  //delay(500);

  // Position in the intake.
  a_lift.move(127);
  delay(700); ///delay(600);

  a_lift.move(-127);
  delay(600);

  a_lift.move(0);

  a_move_drive(-120, -120); //changed from -80
  delay(250); //delay(350);

  a_move_drive(0, 0);
  delay(100);
}

void red_small()
{
  small(true);
}

void red_big() {
  big2(true);
}

void blue_small() {
  small(false);
}

void blue_big() {
  big2(false);
}

// Code for autonomous skill contest
void red_skills() {
  // First pass to pick up 5
  // red_small();
  a_move_drive(-70, -70);
  delay(250);
  a_move_drive(0, 0);

  //a_move_hinge(-127);
  //delay(300);
  //a_move_hinge(0);


  // Logic: Back to the wall first to make sure the robot is aligned,
  // then drive forward to pick up three blocks, and continue to
  // drive to the other side of the field to pick up three blocks.
  // Then drive forward and then to the goal zone to park.

  //back to the wall
  a_move_drive(-30, -70);
  delay(2000);
  a_move_drive(-50, -50);
  delay(1000);

  a_move_drive(0, 0);

  // Second pass to pick up 6
  a_move_intake(127);

  // Drive to pick up the first block
  a_move_drive(127,127);
  delay(525);

  a_move_drive(10, 10);
  delay(800);

  // Pick up the second block
  a_move_drive(127,127);
  delay(150);

  a_move_drive(10, 10);
  delay(800);

  // Pick up the third block
  a_move_drive(127,127);
  delay(150);

  a_move_drive(10, 10);
  delay(1100);

  // Drive across the field to reach the second line of blocks
  // Pick up the 4th block
  a_move_drive(127,127);
  delay(525);

  a_move_drive(10, 10);
  delay(800);

  // Pick up the 5th block
  a_move_drive(127,127);
  delay(150);

  a_move_drive(10, 10);
  delay(800);

  // Pick up the 6th block
  a_move_drive(127,127);
  delay(150);

  a_move_drive(10, 10);
  delay(1100);

/*
  // Drive to the wall
  a_move_drive(127,127);
  delay(500);

  a_move_drive(50, 50);
  delay(500);

  // Turn right
  a_move_drive(50, -50);
  delay(1000);

  a_move_drive(50, 50);
  delay(1000);

  // Turn left a little bit for fault-tolerance reason
  a_move_drive(-50, 50);
  delay(100);

  score();

  // Drive back
  a_move_drive(-70, -70);
  delay(700);
  a_move_drive(0, 0);

  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);

  // Turn right
  a_move_drive(50, -50);
  delay(1000);

  // Third pass to pick up 4
  a_move_intake(127);

  a_move_drive(127,127);
  delay(375);

  a_move_drive(10, 10);
  delay(800);

  a_move_drive(127,127);
  delay(150);

  a_move_drive(10, 10);
  delay(800);

  a_move_drive(127,127);
  delay(150);

  a_move_drive(10, 10); /// a_move_drive(20, 20);
  delay(1100); ///delay(1200);

  a_move_drive(127,127);
  delay(200);

  a_move_drive(10, 10); ///a_move_drive(20, 20);
  delay(1000); ///delay(700);

  a_move_drive(127, 127);
  delay(90);
  a_move_drive(20, 20);
  delay(1000);

  //
  a_move_drive(127, 127);
  delay(800);

  // Turn right
  a_move_drive(50, -50);
  delay(1000);

  // Drive towards red big goal
  a_move_drive(127, 127);
  delay(2000);

  score();

  // Drive back
  a_move_drive(-70, -70);
  delay(700);
  a_move_drive(0, 0);

  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);
*/
}


/*
void blue_big() {
  // big(false);
  // Sample Code
  // Move forward
  // a_move_drive(30,30);

  // Move backward
  // a_move_drive(-30, -30);

  // Left turn
  // a_move_drive(30, 0);

  // right turn
  // a_move_drive(0, 30);
  a_move_intake(127);

  // pick up first block
  a_move_drive(127, 127);
  delay(400);
    // turn right at the location
  a_move_drive(-50, 50);
  delay(800);

  // drive forward to pick up the second block
  a_move_drive(80, 80);
  delay(800);

  a_move_drive(0,0);
  delay(1000);

  // hold the block
  a_move_intake(20);
  // turn right for 45 degrees
  a_move_drive(-50, 50);

  delay(650);

  // drive to the goal zone
  a_move_drive(50, 50);
  delay(500);

  a_move_intake(0);
  a_move_drive(0, 0);

  a_move_hinge(60);
  delay(1800);

  a_move_hinge(0);

  a_move_drive(-50, -50);
  delay(500);
  a_move_drive(0, 0);
}
*/

void autonomous() {
  pros::lcd::print(6, "10/26 14:18 Welcome to auton program");

  unfold();

  // **** Add game time, uncomment the one that we are going to use and ***

  // red_small();
  // red_big();
  blue_small();
  // blue_big();
  //red_skills();
}
