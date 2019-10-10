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

void small(bool isRed) {
  // start in-take
  a_move_intake(127);

  // drive forward to pickup the preload and 4 other blocks straight ahead
  a_move_drive(127,127);
  delay(500);

  a_move_drive(30, 30);
  delay(3000);

  a_move_drive(0, 0);
  delay(200);

  // Just hold the blocks
  a_move_intake(20);

  // drive back to the edge
  a_move_drive(-127,-127);
  delay(1000);

  a_move_drive(0, 0);
  delay(200);

  if (isRed )
    //Make a right turn
    a_move_drive(-50, 50);
  else
    //Make a left turn
    a_move_drive(50, -50);

  delay(1200);

  // Drive to the goal zone
  a_move_drive(50, 50);
  delay(1200);

  // Push the hinge
  a_move_drive(0, 0);
  a_move_intake(0);
  a_move_hinge(127);
  delay(1000);
  a_move_hinge(40);
  delay(600);

  // Drive away
  a_move_drive(-50,-50);
  delay(500);
  a_move_drive(0, 0);

  a_move_hinge(-127);
  delay(400);
}

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

  delay(600);

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

void unfold() {
  // Move hinge forward to make it loose
  a_hinge.move(127);
  delay(500);

  a_hinge.move(-127);
  delay(500);

  a_hinge.move(0);

  //delay(500);

  // Position in the intake.
  a_lift.move(127);
  delay(500);

  a_lift.move(-127);
  delay(500);

  a_lift.move(0);
}

void red_small()
{
  small(true);
}

void red_big() {
  big(true);
}

void green_small() {
  small(false);
}

void green_big() {
  big(false);

}

void autonomous() {
  pros::lcd::print(6, "10/7 10:28PM Welcome to auton program");

  // unfold the robot
  //unfold();

  // **** Add game time, uncomment the one that we are going to use and
  // **** comment out the other three programs.
  // **** Need to make changes before each game.
  // red small
  red_small();

  // *** red big ***
  //// red_big();

  // *** green small ***
  //// green_small();

  // *** green big ***
  //// green_big();
}



/* Angela's Logic
//red small
  //intake
  a_intake.move();
  //drive forward to (4,1) --> 32 inches
  a_drive.move();
  //intake
  a_intake.move();
  //drive forward a little bit --> 5.5 inches
  //intake
  //turn right ? degrees --> about 125 degrees
  //drive forward to goal --> about 21 inches
  //hinge forward
  //small outtake
  //drive backward --> about 10 inches

//red large
  //intake
  a_intake.move();
  //drive forward to (1,1) --> 24 inches
  //intake
  //turn left 90 degrees
  //drive forward to other cube --> 12 inches
  //intake
  //turn left ?2 degrees --> about 30 degrees
  //drive forward to goal
  //hinge forward
  //small outtake
  //drive backward
//blue small
  //intake
  a_intake.move();
  //drive forward to (4,1) --> 32 inches
  //intake
  //drive forward a little bit
  //intake
  //turn left ? degrees --> about 125 degrees
  //drive forward to goal --> about 21 inches
  //hinge forward
  //small outtake
  //drive backward --> about 10 inches
//blue large
  //intake
  a_intake.move();
  //drive forward to (1,1) --> 24 inches
  //intake
  //turn right 90 degrees
  //drive forward to other cube --> 12 inches
  //intake
  //turn right ?2 degrees --> about 30 degrees
  //drive forward to goal --> about 21 inches
  //hinge forward
  //small outtake
  //drive backward --> about 10 inches
  */
