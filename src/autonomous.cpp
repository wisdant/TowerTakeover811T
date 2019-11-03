#include "main.h"
using namespace std;
using namespace pros;

// Information about our motors, wheels, field and our robot:
// 1800 ticks/rev with 36:1 gears, 900 ticks/rev with 18:1 gears, 300 ticks/rev with 6:1 gears
//
// Motors:
//  Driving: Standrad gear ratio: 18:1 (200 RPM), 900 ticks / rev
//  Hinge: 36:1 (100 RPM), 1800 ticks / rev
//
// Wheels: Diameter: 4.000" (101.6mm), Perimeter: 12.56" (320.20mm), Width: 1.125" (28.6mm)
// Each inch is about
// Tile: 2' * 2': 24" (609.6mm)
// Cube: 5.5" * 5.5" (139.7mm)
// Robot: C-channel: 17.5", beginning of intake: 20"

#define MOTOR_MAX_SPEED 100
#define TICK_PER_INCH 72
#define LOW_MARK (95 / 100)
#define HIGH_MARK (105 / 100)
#define DEFAULT_TIMEOUT 1000
#define VERBOSE false


extern int autonNumber;
extern bool redAlliance;


// Define the main controller of the robot.
extern Controller master;

// Define the robot's motors with their respective ports.
// Passing true for the second parameter reverses the motor.
extern Motor left_drive;
extern Motor left_rear_drive;

extern Motor right_drive;
extern Motor right_rear_drive;

extern Motor left_intake;
extern Motor right_intake;

extern Motor lift;
extern Motor hinge;

int start_ts;

/**
 * Set the encoder on how to measure how much the four drive motors have travelled.
 * See more at https://pros.cs.purdue.edu/v5/api/cpp/motors.html#set-encoder-units
 */
void a_set_drive_encoding(motor_encoder_units_e_t units) {
  left_drive.set_encoder_units(units);
  left_rear_drive.set_encoder_units(units);
  right_drive.set_encoder_units(units);
  right_rear_drive.set_encoder_units(units);
}

/**
 * Reset the counting for all four drive motors.
 * See more at https://pros.cs.purdue.edu/v5/api/cpp/motors.html#tare-position
 */
void a_tare_position(){
  left_drive.tare_position();
  left_rear_drive.tare_position();
  right_drive.tare_position();
  right_rear_drive.tare_position();
}

/**
 * Profile move function.
 * See more at https://pros.cs.purdue.edu/v5/api/cpp/motors.html#tare-position
 */
void a_profile_move(bool isAbsoluteMove, double RF_target, double RR_target,
  double LF_target, double LR_target,std::int32_t velocity, int timeout){
  if (VERBOSE) {
    printf("Start moving. isAbsolute: %s, Timeout: %d, LF: %f, LR: %f, RF: %f, RR: %f\n",
      isAbsoluteMove ? "Absolute" : "Relative", timeout,
      left_drive.get_position(), left_rear_drive.get_position(),
      right_drive.get_position(), right_rear_drive.get_position());
  }

  if (isAbsoluteMove) {
    left_drive.move_absolute(LF_target, velocity);
    left_rear_drive.move_absolute(LR_target, velocity);
    right_drive.move_absolute(RF_target, velocity);
    right_rear_drive.move_absolute(RR_target, velocity);
  }
  else {
    left_drive.move_relative(LF_target, velocity);
    left_rear_drive.move_relative(LR_target, velocity);
    right_drive.move_relative(RF_target, velocity);
    right_rear_drive.move_relative(RR_target, velocity);
  }

  int counter = 0;
  while (true) {
    counter++;
    if (VERBOSE) {
      printf("Counter: %d, LF: %f, LR: %f, RF: %f, RR: %f\n", counter,
        left_drive.get_position(), left_rear_drive.get_position(),
        right_drive.get_position(), right_rear_drive.get_position());
    }

    double LF_pos = left_drive.get_position();
    double LR_pos = left_rear_drive.get_position();
    double RF_pos = right_drive.get_position();
    double RR_pos = right_rear_drive.get_position();

    if (!((abs(LF_pos) > abs(LF_target) * 95/100) &&  (abs(LF_pos) < abs(LF_target) * 105/100))
        || !((abs(LR_pos) > abs(LR_target) * 95/100) &&  (abs(LR_pos) < abs(LR_target) * 105/100))
        || !((abs(RF_pos) > abs(RF_target) * 95/100) &&  (abs(RF_pos) < abs(RF_target) * 105/100))
        || !((abs(RR_pos) > abs(RR_target) * 95/100) &&  (abs(RR_pos) < abs(RR_target) * 105/100))
    ) {
      delay(20);
      // Break the indefinite loop if the robot is blocked by something.
      if ((timeout!=-1) && (counter * 20 > timeout)) break;
    }
    else {
      break;
    }
  }
  printf("Finished moving. Counter: %d, LF: %f, LR: %f, RF: %f, RR: %f\n", counter,
    left_drive.get_position(), left_rear_drive.get_position(),
    right_drive.get_position(), right_rear_drive.get_position());
}

/**
 * Reset the counting for all four drive motors.
 * See more at https://pros.cs.purdue.edu/v5/api/cpp/motors.html#tare-position
 */
void a_move_relative(double target, std::int32_t velocity){
   a_profile_move(false, target, target, target, target, velocity, DEFAULT_TIMEOUT);;
 }

void a_move_relative(double target, std::int32_t velocity, int timeout){
   a_profile_move(false, target, target, target, target, velocity, timeout);;
 }

 void a_move_relative(double RF_target, double RR_target, double LF_target, double LR_target,
       std::int32_t velocity){
   a_profile_move(false, RF_target, RR_target, LF_target, LR_target, velocity, DEFAULT_TIMEOUT);
 }

void a_move_relative(double RF_target, double RR_target, double LF_target, double LR_target,
      std::int32_t velocity, int timeout){
  a_profile_move(false, RF_target, RR_target, LF_target, LR_target, velocity, timeout);
}


void a_move_absolute(double position, std::int32_t velocity){
  a_profile_move(true, position, position, position, position, velocity, DEFAULT_TIMEOUT);
}

void a_move_absolute(double position, std::int32_t velocity, int timeout){
  a_profile_move(true, position, position, position, position, velocity, timeout);
}

void a_move_absolute(double RF_target, double RR_target, double LF_target, double LR_target,
    std::int32_t velocity){
  a_profile_move(true, RF_target, RR_target, LF_target, LR_target, velocity, DEFAULT_TIMEOUT);
}

void a_move_absolute(double RF_target, double RR_target, double LF_target, double LR_target,
    std::int32_t velocity, int timeout){
  a_profile_move(true, RF_target, RR_target, LF_target, LR_target, velocity, timeout);
}

/**
 * Assign specified powers for the right and left sides of the drivetrain to the left and right motors, respectively.
 */
void a_move_drive(int right, int left) {
  left_drive.move(left);
  left_rear_drive.move(left);
  right_drive.move(right);
  right_rear_drive.move(right);
}

/**
 * Assign a specified power to the in-take motor.
 */
void a_move_intake(int power) {
  left_intake.move(power);
  right_intake.move(power);
}

/**
 * Assign a specified power to the lift motor.
 */
void a_move_lift(int power) {
  lift.move(power);
}

/**
 * Assign a specified power to the claw motor.
 */
void a_move_hinge(int power) {
  hinge.move(power);
}

/**
 * Score in the goal zone
 */
void score() {
  // Push the hinge
  a_move_drive(0, 0);
  a_move_intake(0);

  //move hinge 50 for 2400
  a_move_hinge(55);
  delay (2200); //delay(2450); //delay(2300); //delay(2100);
  a_move_hinge(30);
  delay(850); ///changed

  a_move_hinge(0);
  a_move_drive(50,50);
  delay(450);
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
    a_move_drive(-110, -127);
    delay(700);
  }

  a_move_drive(0, 0);
  delay(200);

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

void unfold() {
  // Move hinge forward to make it loose
  hinge.move(127);
  delay(600);

  hinge.move(-127);
  delay(500);  /// This 600ms can be saved by moving the next statement after lift.move(127);

  hinge.move(0);

  // Position in the intake.
  lift.move(127);
  delay(750); ///delay(600);

  lift.move(-127);
  delay(300);

  lift.move(0);

  a_move_drive(-120, -120); //changed from -80
  delay(250); //delay(350);

  a_move_drive(0, 0);
  delay(100);
}

void score_fancy() {
  a_move_drive(0, 0);
  a_move_intake(0);

  hinge.set_encoder_units(E_MOTOR_ENCODER_ROTATIONS);
  hinge.tare_position();
  hinge.move_absolute(1, 40); // was 45
  delay(1500);  //was 2200
  hinge.move_absolute(1, 30); // was 45
  delay(1800);  //was 2200
  a_move_hinge(0);

  // Drive forward a little bit
  a_tare_position();
  double ticks = 1.5 * TICK_PER_INCH;
  a_move_absolute(ticks, 25, 100);  // velocity 20%, timeout 100ms
}

/**
 * This takes advantage of V5's profiling movements and PID to control how far
 * the motors travel, instead of relying on time.
 */
void small_fancy(bool isRed) {
  // Start intake. Apply negative force on the arm to prevent it from undesired lifting.
  a_move_lift(-20);
  a_move_intake(127);

  // For direct drive, we will count ticks.
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  // 1. Drive forward 6", pick up first Cube
  double ticks = 6 * TICK_PER_INCH;
  a_move_absolute(ticks, 75);
  printf("Done with first move. Timelap = %d\n", millis() - start_ts);
  // pick up first cube
  delay(400);

  // 2. Drive forward 5.5", pick up 2nd Cube
  ticks += 6.5 * TICK_PER_INCH;
  a_move_absolute(ticks, 75);
  printf("Done with second move. Timelap = %d\n", millis() - start_ts);
  pros::delay(800);   // was 650

  // 3. Drive forward 5.5", pick up 3rd Cube
  ticks += 5.5 * TICK_PER_INCH;
  a_move_absolute(ticks, 75);
  printf("Done with 3rd move.Timelap = %d\n", millis() - start_ts);
  delay(800); // was 650

  // 4. Drive forward 6", pick up 4th Cube
  ticks += 6 * TICK_PER_INCH;
  a_move_absolute(ticks, 50);
  printf("Done with 4th move.Timelap = %d\n", millis() - start_ts);
  delay(950); //800 works for 4 cubes

  // 5. Drive forward 6", pick up 5th Cube
  ticks += 6 * TICK_PER_INCH;
  a_move_absolute(ticks, 50);
  delay(600); //was 450
  printf("****After 5th cube\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);

  a_move_lift(0);

  // 6. Backup to where the center of robot is at the border of first tile and second tile
  ticks -= 11.5 * TICK_PER_INCH;
  a_move_absolute(ticks, 75);
  delay(100);
  printf("****After backing\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);

  // New 7
  double temp_ticks = 12 * TICK_PER_INCH;
  double RF_ticks;
  double RR_ticks;
  double LF_ticks;
  double LR_ticks;
  if (isRed) {
    // Turn right. The right wheel travels backward. The left wheel travels forward
    //printf("New!!! Red alliance, turn right.\n");
    RF_ticks = ticks - temp_ticks;
    RR_ticks = ticks - temp_ticks;
    LF_ticks = ticks + temp_ticks;
    LR_ticks = ticks + temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  else {
    //printf("New!!! Blue alliance, turn left.\n");
    RF_ticks = ticks + temp_ticks;
    RR_ticks = ticks + temp_ticks;
    LF_ticks = ticks - temp_ticks;
    LR_ticks = ticks - temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  delay(50);
  printf("****After turning\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);

  // 8. Drive to the goal zone
  temp_ticks = 20 * TICK_PER_INCH;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;

  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  delay(100);
  printf("****Before adjust\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);


  // 9 Fine adjustment to robot position
  /*
  if (isRed) {
    right_drive.move_absolute(1900, 75);
    right_rear_drive.move_absolute(1900, 75);
    left_drive.move_absolute(3650, 75);
    left_rear_drive.move_absolute(3650, 75);
    delay(100);
  }
  else {
    right_drive.move_absolute(3650, 75);
    right_rear_drive.move_absolute(3650, 75);
    left_drive.move_absolute(1900, 75);
    left_rear_drive.move_absolute(1900, 75);
    delay(100);
  }
  printf("****After adjust\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);
  */

  // 10. score
  //score_fancy();
  //printf("****After score\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
  //  right_drive.get_position(), right_rear_drive.get_position(),
  //  left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);
  score();

  // 11. Drive backward

  a_tare_position();
  ticks = -21 * TICK_PER_INCH;
  a_move_relative(ticks, 100);
  printf("****All complete \tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);
  //a_move_drive(0, 0);

/*
  a_move_drive(-70, -70); //a_move_drive(-50,-50);
  delay(600);
  a_move_drive(0, 0);
*/

  // 11. Move back the hinge.
  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);
  printf("Done with moving back hinge. Timelap = %d\n", millis() - start_ts);

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
void skills(bool isRed) {

  // First pass to pick up 5
  // 1. Complete auton
  small_fancy(isRed);

  // 2. Turn to face the pole
  double ticks = 12.5 * TICK_PER_INCH;
  a_move_relative(ticks, ticks, (-1)*ticks, (-1)*ticks, 75);
  delay(1000);

  // 3. Drive to the pole.
  ticks = 22 * TICK_PER_INCH;
  a_move_relative(ticks, 75);
  delay(1000);

  // 4. Intake a cube
  a_move_intake(80);
  delay(1100);
  a_move_intake(2);
  delay(1000);

  //double ticks = (-2) * TICK_PER_INCH;
  //a_move_relative(ticks, 75);
  //delay(3000);

  // 5. Lift the cube
  a_move_hinge(127);
  delay(200);
  a_move_hinge(0);
  delay(1000);

  a_move_lift(127); // move up the lift
  delay(1000);
  a_move_lift(20); // Assign a little power and allow the motor to hold it's position.
  delay(1000);

  // 6. Put the cube to the tower
  a_move_intake(-127); // Move the intake belt backward, for removing blocks from towers
  delay(500);
  a_move_intake(0);
  a_move_lift(-127);
  delay(500);
  a_move_hinge(-127);
  delay(200);


  /*
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

void one_cube(bool isRed) {
  // For direct drive, we will count ticks.
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  // 1. Drive backward to push the cube into goal zone
  double ticks = (-6) * TICK_PER_INCH;
  a_move_absolute(ticks, 100);
  printf("Done with first move. \n");

  ticks = 12 * TICK_PER_INCH;
  a_move_relative(ticks, 75);
  delay(1000);

  // Turn the robot to face the goal zone
  ticks = 2 * TICK_PER_INCH;
  if (isRed) {
    a_move_relative(ticks, ticks, (-1)*ticks, (-1)*ticks, 75);
  }
  else {
    a_move_relative((-1)*ticks, (-1)*ticks, ticks, ticks, 75);
  }
  delay(1000);

  ticks = 5 * TICK_PER_INCH;
  a_move_relative(ticks, 75);
  delay(1000);

  ticks = 14 * TICK_PER_INCH;
  if (isRed) {
    a_move_relative(ticks, ticks, (-1)*ticks, (-1)*ticks, 75);
  }
  else {
    a_move_relative((-1)*ticks, (-1)*ticks, ticks, ticks, 75);
  }
  delay(1000);

}

void autonomous() {

  printf("Welcome to Auton: 11/1 12:00AM. \n");

  // **** For testing only, hardcode test case.
  // **** At game time, we will select from the menu
  redAlliance = true;

  //autonNumber = 4;  // One cube program
  // autonNumber = 2;  // 2 cube program for large goal zone
  autonNumber = 3;  // 5 cube program for small goal zone


  start_ts = millis();
  unfold();
  printf("Done with unfolding. Timelap = %d\n", millis() - start_ts);

  if (redAlliance) {
    lcd::print(5, "Red alliance program %d", autonNumber);
    printf("Red alliance program %d\n", autonNumber);
    switch (autonNumber) {
      case 1:
        one_cube(true);
        break;
      case 2:
        red_big();
        break;
      case 3:
        small_fancy(true);
        break;
      case 4:
        skills(true);
        break;
      default:
        small_fancy(true);
        break;
    }
  }
  else {
    lcd::print(5, "Blue alliance program %d", autonNumber);
    printf("Blue alliance program %d\n", autonNumber);
    switch (autonNumber) {
      case 1:
        one_cube(false);
        break;
      case 2:
        blue_big();
        break;
      case 3:
        small_fancy(false);
        break;
      case 4:
        skills(false);
        break;
      default:
        small_fancy(false);
        break;
    }
  }

// *** DO NOT call these functions anymore ***
// red_small();
// red_big();
// blue_small();
// blue_small();
// blue_big();
//red_skills();
}
