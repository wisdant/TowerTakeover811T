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

pros::Motor a_left_intake (LEFT_INTAKE_PORT, false);
pros::Motor a_right_intake (RIGHT_INTAKE_PORT, true);

pros::Motor a_lift (LIFT_PORT, true);
pros::Motor a_hinge (HINGE_PORT, true);

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

/// ---------------------------------------------------------------------------

void small(bool isRed) {

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

  a_move_drive(20, 20);
  delay(1200);

  a_move_drive(127,127);
  delay(200);

  a_move_drive(20, 20);
  delay(700);

  // Just hold the blocks
  a_move_intake(30);

  // drive back to the edge
  /* a_move_drive(-127,-127);
  delay(1100);
  */

  a_move_drive(-127,-127);
  delay(800);
  a_move_drive(-50,-50);
  delay(900);


  if (isRed )
    //Make a right turn
    a_move_drive(-50, 50);
  else
    //Make a left turn
    a_move_drive(50, -50);

  //delay(1250);
  delay(1000); ///changed

  // Drive to the goal zone
  a_move_drive(35, 35);
  delay(1000);

  if (isRed )
    //Make a right turn
    a_move_drive(-50, 50); /// a_move_drive (-40, 40);
  else
    //Make a left turn
    a_move_drive(50, -50); /// a_move_drive (40, -40);

  //delay(1250);
  delay(150);

  // Push the hinge
  a_move_drive(0, 0);
  a_move_intake(0);
  a_move_hinge(50); /// a_move_hinge(30);
  delay(2400); /// delay(3000);
  a_move_hinge(0);
  a_move_drive(40,40);
  delay(200);
  a_move_drive(0, 0);
  a_move_intake(-127);
  delay(200);
  a_move_intake(0);
  a_move_drive(-50,-50);
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
  delay(600);

  a_lift.move(-127);
  delay(600);

  a_lift.move(0);

  a_move_drive(-80, -80);
  delay(350);

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
  pros::lcd::print(6, "10/12 12:48 AM Welcome to auton program");

  unfold();

  // **** Add game time, uncomment the one that we are going to use and ***

  // red_small();
  red_big();
  // blue_small();
  // blue_big();
}
