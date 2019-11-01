#include "main.h"
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
 * Reset the counting for all four drive motors.
 * See more at https://pros.cs.purdue.edu/v5/api/cpp/motors.html#tare-position
 */
void a_move_relative(double RF_target, double RR_target, double LF_target, double LR_target, std::int32_t velocity){
  left_drive.move_relative(LF_target, velocity);
  left_rear_drive.move_relative(LR_target, velocity);
  right_drive.move_relative(RF_target, velocity);
  right_rear_drive.move_relative(RR_target, velocity);

  int counter = 0;
  while (true) {
    //printf("Counter: %d, LF: %f, LR: %f, RF: %f, RR: %f\n", counter++, left_drive.get_position(), left_rear_drive.get_position(),
    //  right_drive.get_position(), right_rear_drive.get_position());

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
    }
    else {
      printf("Finished moving. Counter: %d, LF: %f, LR: %f, RF: %f, RR: %f\n", counter, left_drive.get_position(), left_rear_drive.get_position(),
        right_drive.get_position(), right_rear_drive.get_position());
      break;
    }
  }
  printf("Finished moving.\n");
  //while (!((left_drive.get_position() > LF_pos * LOW_MARK) &&  (left_drive.get_position() < LF_pos * HIGH_MARK))
  //    || !((left_rear_drive.get_position() > LR_pos * LOW_MARK) &&  (left_rear_drive.get_position() < LR_pos * HIGH_MARK))
  //    || !((right_drive.get_position() > RF_pos * LOW_MARK) &&  (right_drive.get_position() < RF_pos * HIGH_MARK))
  //    || !((right_rear_drive.get_position() > RR_pos * LOW_MARK) &&  (right_rear_drive.get_position() < RR_pos * HIGH_MARK))
  //  ) {
  //  delay(2);
  //}
}

void a_move_relative(double position, std::int32_t velocity){
  printf("\nNEW relative!!! position = %f, low = %f, high=%f\n\n", position, position * 95/100, position * 105/100);

  a_move_relative(position, position, position, position, velocity);
}

/**
 * Reset the counting for all four drive motors.
 * See more at https://pros.cs.purdue.edu/v5/api/cpp/motors.html#tare-position
 */
void a_move_absolute(double RF_pos, double RR_pos, double LF_pos, double LR_pos, std::int32_t velocity){
  printf("Entering a_move_absolute(%f, %f, %f, %f)\n", LF_pos, LR_pos, RF_pos, RR_pos);
  left_drive.move_absolute(LF_pos, velocity);
  left_rear_drive.move_absolute(LR_pos, velocity);
  right_drive.move_absolute(RF_pos, velocity);
  right_rear_drive.move_absolute(RR_pos, velocity);

  int counter = 0;



  while (true) {
    //printf("Counter: %d, LF: %f, LR: %f, RF: %f, RR: %f\n", counter++, left_drive.get_position(), left_rear_drive.get_position(),
    //  right_drive.get_position(), right_rear_drive.get_position());
    if (!((left_drive.get_position() > LF_pos * 95/100) &&  (left_drive.get_position() < LF_pos * 105/100))
        || !((left_rear_drive.get_position() > LR_pos * 95/100) &&  (left_rear_drive.get_position() < LR_pos * 105/100))
        || !((right_drive.get_position() > RF_pos * 95/100) &&  (right_drive.get_position() < RF_pos * 105/100))
        || !((right_rear_drive.get_position() > RR_pos * 95/100) &&  (right_rear_drive.get_position() < RR_pos * 105/100))
    ) {
      delay(20);
    }
    else {
      printf("Finished moving. Counter: %d, LF: %f, LR: %f, RF: %f, RR: %f\n", counter, left_drive.get_position(), left_rear_drive.get_position(),
        right_drive.get_position(), right_rear_drive.get_position());
      break;
    }
  }
  //while (!((left_drive.get_position() > LF_pos * LOW_MARK) &&  (left_drive.get_position() < LF_pos * HIGH_MARK))
  //    || !((left_rear_drive.get_position() > LR_pos * LOW_MARK) &&  (left_rear_drive.get_position() < LR_pos * HIGH_MARK))
  //    || !((right_drive.get_position() > RF_pos * LOW_MARK) &&  (right_drive.get_position() < RF_pos * HIGH_MARK))
  //    || !((right_rear_drive.get_position() > RR_pos * LOW_MARK) &&  (right_rear_drive.get_position() < RR_pos * HIGH_MARK))
  //  ) {
  //  delay(2);
  //}
}

void a_move_absolute(double position, std::int32_t velocity){
  printf("\nNEW absolute!!! position = %f, low = %f, high=%f\n\n", position, position * 95/100, position * 105/100);
  a_move_absolute(position, position, position, position, velocity);
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
  delay(700); ///delay(600);

  lift.move(-127);
  delay(300);

  lift.move(0);

  a_move_drive(-120, -120); //changed from -80
  delay(250); //delay(350);

  a_move_drive(0, 0);
  delay(100);
}

/**
 * This takes advantage of V5's profiling movements and PID to control how far
 * the motors travel, instead of relying on time.
 */
void small_fancy(bool isRed) {
  a_move_lift(-20);
  // start in-take
  a_move_intake(127);

  // For direct drive, we will count ticks.
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  // 1. Drive forward 6", pick up first Cube
  double ticks = 6 * TICK_PER_INCH;
  a_move_absolute(ticks, 75);
  printf("Done with first move. Timelap = %d\n", millis() - start_ts);
  delay(450);

  // 2. Drive forward 5.5" at lower speed, pick up 2nd Cube
  ticks += 6.5 * TICK_PER_INCH;
  printf("ticks = %f Before second move.\n", ticks);
  a_move_absolute(ticks, 50);
  printf("Done with second move. Timelap = %d\n", millis() - start_ts);
  pros::delay(650);
  printf("after delay.\n");

  // 3. Drive forward 5.5", pick up 3rd Cube
  ticks += 5.5 * TICK_PER_INCH;
  printf("ticks = %f Before 3rd move.\n", ticks);
  a_move_absolute(ticks, 50);
  printf("Done with 3rd move.Timelap = %d\n", millis() - start_ts);
  delay(650);

  // 4. Drive forward 5.5", pick up 4th Cube
  ticks += 6 * TICK_PER_INCH;
  a_move_absolute(ticks, 50);
  printf("Done with 4th move.Timelap = %d\n", millis() - start_ts);
  delay(700);

  // 5. Drive forward 5.5", pick up 5th Cube
  ticks += 6 * TICK_PER_INCH;
  a_move_absolute(ticks, 50);
  printf("Done with 5th move.Timelap = %d\n", millis() - start_ts);
  delay(450);
  printf("\n****After 5th****\n");
  printf("LF\tLR\tRF\tRR\n");
  printf("%f\t%f\t%f\t%f", left_drive.get_position(), left_rear_drive.get_position(),
    right_drive.get_position(), right_rear_drive.get_position());
  printf("\n*********************\n");

  a_move_lift(0);

  // 6. Backup 13.5" so that the center of robot is at the border of first tile and second tile
  ticks -= 11.5 * TICK_PER_INCH;
  a_move_absolute(ticks, 75);
  printf("Done with backing.Timelap = %d\n", millis() - start_ts);
  delay(100);
  printf("Done with backing. Timelap = %d\n", millis() - start_ts);
  printf("\n****After backing****\n");
  printf("LF\tLR\tRF\tRR\n");
  printf("%f\t%f\t%f\t%f", left_drive.get_position(), left_rear_drive.get_position(),
    right_drive.get_position(), right_rear_drive.get_position());
  printf("\n*********************\n");

  // 7. Turn 135 degrees, which maps to ~16" travel for each wheel.
  a_tare_position();
  ticks = 12.25 * TICK_PER_INCH;
  if (isRed) {
    // Turn right. The right wheel travels backward. The left wheel travels forward
    printf("Red alliance, turn right.\n");
    a_move_relative(-1*ticks, -1*ticks, ticks, ticks, 75);
  }
  else {
    printf("Blue alliance, turn left.\n");
    a_move_relative(ticks, ticks, -1*ticks, -1*ticks, 75);
  }
  printf("Done with backing. Timelap = %d\n", millis() - start_ts);
  printf("\n****After turn****\n");
  printf("LF\tLR\tRF\tRR\n");
  printf("%f\t%f\t%f\t%f", left_drive.get_position(), left_rear_drive.get_position(),
    right_drive.get_position(), right_rear_drive.get_position());
  printf("\n*********************\n");

  //delay(100);

  // 8. Drive to the goal zone.
  a_tare_position();
  ticks = 20 * TICK_PER_INCH;
  a_move_relative(ticks, 100);
  printf("Done with driving to the goal zone. Timelap = %d\n", millis() - start_ts);
  delay(100);
  printf("\n****Before Score****\n");
  printf("LF\tLR\tRF\tRR\n");
  printf("%f\t%f\t%f\t%f", left_drive.get_position(), left_rear_drive.get_position(),
    right_drive.get_position(), right_rear_drive.get_position());
  printf("\n*********************\n");

  // 9. score
  score();
  printf("Done with scoring. Timelap = %d\n", millis() - start_ts);

  // 10. Drive backward for 5"
  a_tare_position();
  ticks = -10 * TICK_PER_INCH;
  a_move_relative(ticks, 100);
  printf("Done with backing up. Timelap = %d\n", millis() - start_ts);

  // 11. Move back the hinge.
  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);
  printf("Done with moving back hinge. Timelap = %d\n", millis() - start_ts);
  //hinge.move_relative((-1)*degrees_to_turn, 100);
  //a_move_hinge(0);
}

void big_one_cube(bool isRed) {

  a_move_drive(80, 80);
  delay(300);
  a_move_drive(80,80);
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

void one_cube(bool isRed) {
  // For direct drive, we will count ticks.
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  // 1. Drive forward 6", pick up first Cube
  double ticks = (-14) * TICK_PER_INCH;
  a_move_relative(ticks, 100);
  printf("Done with first move. \n");

  ticks = 12 * TICK_PER_INCH;
  a_move_relative(ticks, 75);
  printf("Done with one cube auton. ");

}

void autonomous() {

  printf("Welcome to Auton: 10/30 8:28PM. Force to call Red Small.\n");

  // **** at game time, uncomment the one that we are going to use and ***
  //redAlliance = false;
  redAlliance = true;

  //autonNumber = 1;  // One cube program
  //autonNumber = 2;  // 2 cube program for large goal zone
  autonNumber = 3;  // 5 cube program for small goal zone
  //autonNumber = 4;  // skill test program

  int start_ts = millis();
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
        //red_small();
        small_fancy(true);
        break;
      case 4:
        red_skills();
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
        blue_small();
        break;
      case 4:
        //TODO: Add blue skills
        break;
    }
  }




  // red_small();
  // red_big();
  // blue_small();
  // blue_big();
  //red_skills();

}
