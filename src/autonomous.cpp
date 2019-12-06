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
#define DEFAULT_TIMEOUT 2000
#define VERBOSE false     //Used when we want to display more debugging message


extern int autonNumber;
extern bool redAlliance;


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

// start_ts: starting timestamp, recording the starting time. It is used for
// debugging purpose so that we know how much time is spent in each step.
int start_ts;

/**
 * Set the encoder on how to measure how much the drive motors have travelled.
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
 * @isAbsoluteMove: if it is an absolute move or relative move
 * @FR_target: the destination position for Front Right
 * @RR_target: the destination position for Rear Right
 * @FL_target: the destination position for Front Left
 * @RL_target: the destination position for Rear Left
 * @velocity: how fast is the move. 0-100, 100 is the highest speed.
 * @timeout: In case it runs into indefinite loop,
 * the function is forced to return when the timeout (in ms) is reached.
 */
void move_by_distance(bool isAbsoluteMove, double FR_target, double RR_target,
  double FL_target, double RL_target,std::int32_t velocity, int timeout){
  // Converting the distance (inches) to ticks
  FR_target *= TICK_PER_INCH;
  RR_target *= TICK_PER_INCH;
  FL_target *= TICK_PER_INCH;
  RL_target *= TICK_PER_INCH;

  if (VERBOSE) {
    printf("Start moving. isAbsolute: %s, Timeout: %d, LF: %f, LR: %f, RF: %f, RR: %f, LF_target: %f, LR_target: %f, RF_target: %f, RR_target: %f  \n",
      isAbsoluteMove ? "Absolute" : "Relative", timeout,
      left_drive.get_position(), left_rear_drive.get_position(),
      right_drive.get_position(), right_rear_drive.get_position(),
      FL_target, RL_target, FR_target, RR_target
    );
  }

  if (isAbsoluteMove) {
    left_drive.move_absolute(FL_target, velocity);
    left_rear_drive.move_absolute(RL_target, velocity);
    right_drive.move_absolute(FR_target, velocity);
    right_rear_drive.move_absolute(RR_target, velocity);
  }
  else {
    left_drive.move_relative(FL_target, velocity);
    left_rear_drive.move_relative(RL_target, velocity);
    right_drive.move_relative(FR_target, velocity);
    right_rear_drive.move_relative(RR_target, velocity);
  }

  int counter = 0;    // Used to decide if timeout has reached.
  while (true) {
    counter++;
    if (VERBOSE) {
      printf("Counter: %d, FR: %f, RR: %f, FL: %f, RL: %f\n", counter,
        right_drive.get_position(), right_rear_drive.get_position(),
        left_drive.get_position(), left_rear_drive.get_position());
    }

    double LF_pos = left_drive.get_position();
    double LR_pos = left_rear_drive.get_position();
    double RF_pos = right_drive.get_position();
    double RR_pos = right_rear_drive.get_position();

    // If the robot is within 95%-105% of the destination, it is considered reached the destination.
    if (!((abs(LF_pos) > abs(FL_target) * 95/100) &&  (abs(LF_pos) < abs(FL_target) * 105/100))
        || !((abs(LR_pos) > abs(RL_target) * 95/100) &&  (abs(LR_pos) < abs(RL_target) * 105/100))
        || !((abs(RF_pos) > abs(FR_target) * 95/100) &&  (abs(RF_pos) < abs(FR_target) * 105/100))
        || !((abs(RR_pos) > abs(RR_target) * 95/100) &&  (abs(RR_pos) < abs(RR_target) * 105/100))
    ) {
      delay(20);
      // Break the indefinite loop if the robot is blocked by something.
      if ((timeout!=-1) && (counter * 20 > timeout)) break;
    }
    else {
      delay(20);  // Give sometime for it to further adjust position after reaching 95%-105% of the destination
      break;
    }
  }
  if (VERBOSE) {
    printf("Finished moving: %d, FR: %f, RR: %f, FL: %f, RL: %f\n", counter,
      right_drive.get_position(), right_rear_drive.get_position(),
      left_drive.get_position(), left_rear_drive.get_position());
  }
}

/**
 * Using the motor's move_relative move for @target inches at @velocity speed.
 * This is used when all the motors are moving towards the same direction.
 * Default timeout 1000 will be used.
 */
void a_move_relative(double target, std::int32_t velocity){
   move_by_distance(false, target, target, target, target, velocity, DEFAULT_TIMEOUT);;
 }

 /**
  * Using the motor's move_relative move for @target inches at @velocity speed.
  * This is used when all the motors are moving towards the same direction.
  * @timeout: the timeout period for the movement.
  */
void a_move_relative(double target, std::int32_t velocity, int timeout){
   move_by_distance(false, target, target, target, target, velocity, timeout);;
 }

 /**
  * Using the motor's move_relative move for @target inches at @velocity speed.
  * This function is used when different motors are moving in different directions.
  * @FR_target: the destination position for Front Right
  * @RR_target: the destination position for Rear Right
  * @FL_target: the destination position for Front Left
  * @RL_target: the destination position for Rear Left
  * @velocity: how fast is the move. 0-100, 100 is the highest speed.
  * Default timeout 1000 will be used.
  */
 void a_move_relative(double FR_target, double RR_target, double FL_target, double RL_target,
       std::int32_t velocity){
   move_by_distance(false, FR_target, RR_target, FL_target, RL_target, velocity, DEFAULT_TIMEOUT);
 }

 /**
  * Using the motor's move_relative move for @target inches at @velocity speed.
  * This function is used when different motors are moving in different directions.
  * @FR_target: the destination position for Front Right
  * @RR_target: the destination position for Rear Right
  * @FL_target: the destination position for Front Left
  * @RL_target: the destination position for Rear Left
  * @velocity: how fast is the move. 0-100, 100 is the highest speed.
  * @timeout: the timeout period for this movement. It should be slightly longer than
  * the normal expected duration of this movement.
  */
void a_move_relative(double FR_target, double RR_target, double FL_target, double RL_target,
      std::int32_t velocity, int timeout){
  move_by_distance(false, FR_target, RR_target, FL_target, RL_target, velocity, timeout);
}

/**
 * Using the motor's move_absolute move for @target inches at @velocity speed.
 * This is used when all the motors are moving towards the same direction.
 * Default timeout 1000 will be used.
 */
void a_move_absolute(double position, std::int32_t velocity){
  move_by_distance(true, position, position, position, position, velocity, DEFAULT_TIMEOUT);
}

/**
 * Using the motor's move_absolute move for @target inches at @velocity speed.
 * This is used when all the motors are moving towards the same direction.
 * @timeout: the timeout period for the movement.
 */
void a_move_absolute(double position, std::int32_t velocity, int timeout){
  move_by_distance(true, position, position, position, position, velocity, timeout);
}

/**
 * Using the motor's move_absolute move for @target inches at @velocity speed.
 * This function is used when different motors are moving in different directions.
 * @FR_target: the destination position for Front Right
 * @RR_target: the destination position for Rear Right
 * @FL_target: the destination position for Front Left
 * @RL_target: the destination position for Rear Left
 * @velocity: how fast is the move. 0-100, 100 is the highest speed.
 * Default timeout 1000 will be used.
 */
void a_move_absolute(double FR_target, double RR_target, double FL_target, double RL_target,
    std::int32_t velocity){
  move_by_distance(true, FR_target, RR_target, FL_target, RL_target, velocity, DEFAULT_TIMEOUT);
}

/**
 * Using the motor's move_relative move for @target inches at @velocity speed.
 * This function is used when different motors are moving in different directions.
 * @FR_target: the destination position for Front Right
 * @RR_target: the destination position for Rear Right
 * @FL_target: the destination position for Front Left
 * @RL_target: the destination position for Rear Left
 * @velocity: how fast is the move. 0-100, 100 is the highest speed.
 * @timeout: the timeout period for this movement. It should be slightly longer than
 * the normal expected duration of this movement.
 */
void a_move_absolute(double FR_target, double RR_target, double FL_target, double RL_target,
    std::int32_t velocity, int timeout){
  move_by_distance(true, FR_target, RR_target, FL_target, RL_target, velocity, timeout);
}

/**
 * Assign specified powers for the right and left sides of the drivetrain respectively.
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
 * Assign a specified power to the hinge.
 */
void a_move_hinge(int power) {
  hinge.move(power);
}

/**
 * Unfold the robot to get it prepared for moving. The current logic is:
 * 1. Move the hinge forward to release the hinge
 * 2. Move the hinge to resting position
 * 3. Lift the arm to allow intakes to be released
 * 4. Move the lift to resting position
 * 5. Drive backward to reposition the robot. This is needed since releasing
 * the intake step can shift the position of the robot.
 */
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

/**
 * Unfold the robot to get it prepared for moving. The current logic is:
 * 1. Lift the arm to allow intakes to be released and push the tray up
 * 2. Move the lift to resting position
 * 3. Drive backward to reposition the robot. This is needed since releasing
 * the intake step can shift the position of the robot.
 */
void unfold_new() {

  //hinge.move(127);
  //delay(100);

  // Position in the intake.
  lift.move(127);
  delay(400); ///delay(600);

  lift.move(-85); //lift.move(-127);
  delay(600); //delay(400);

  lift.move(0);

  hinge.move(-127);
  delay(100);  /// This 600ms can be saved by moving the next statement after lift.move(127);

  a_move_drive(-120, -120); //changed from -80
  delay(150); //delay(350);

  a_move_drive(0, 0);
  delay(100);
}

/**
 * Score in the goal zone. This function uses simple move function by setting
 * power for each motor and control the movement through time delay. The current logic is:
 * TODO: Need to experiment to move roller in outake mode in this process.
 * 1. Stop driving forward and intake
 * 2. Move the hinge forward at a higher speed for sometime
 * 3. Move the hinge at a lower speed for more time
 * 4. Drive forward a little to push the stack about the first cube forward a little.
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

/**
 * Score in the goal zone. This function uses simple move function by setting
 * power for each motor and control the movement through time delay. The current logic is:
 * TODO: Need to experiment to move roller in outake mode in this process.
 * 1. Stop driving forward and intake
 * 2. Move the hinge forward at a higher speed for sometime
 * 3. Move the hinge at a lower speed for more time
 * 4. Drive forward a little to push the stack about the first cube forward a little.
 */
void score_new() {

  a_move_drive(0, 0);
  a_move_intake(0); // Stop intake roller
  a_move_lift(0);

  a_move_intake(-40);
  delay(100);

  a_move_hinge(127); // Move the lift forward
  delay(600);

  a_move_hinge(80); // Move the lift forward
  delay(600);

  a_move_hinge(40);
  delay(1500);

  a_move_hinge(0);
}

void score_new2() {
  a_move_drive(0, 0);
  a_move_intake(0); // Stop intake roller
  a_move_lift(0);

  a_move_intake(-40);
  delay(100);

  a_move_hinge(100); // Move the lift forward
  delay(600);

  a_move_hinge(80); // Move the lift forward
  delay(600);

  a_move_hinge(30);
  delay(1500);

  a_move_hinge(0);
}

void score_eight() {

  a_move_drive(0, 0);
  a_move_intake(0); // Stop intake roller
  a_move_lift(0);

  //a_move_intake(-40);
  //delay(100);

  a_move_hinge(127); // Move the lift forward
  delay(600);

  a_move_hinge(80); // Move the lift forward
  delay(600);

  a_move_hinge(40);
  delay(1500);

  a_move_hinge(0);
}

/**
 * Score in the goal zone. This function uses profile movement by specifying
 * how much we want to move the hinge or the robot.
 * TODO: Remove the hardcoded timeout and use similar mechanism as
 * in a_move_relative(...) or a_move_absolute(...).
 * TODO: Need to experiment to move roller in outake mode in this process.
 * The current logic is:
 * 1. Stop driving forward and intake
 * 2. Move the hinge forward at a higher speed for some distance
 * 3. Move the hinge at a lower speed for more more distance
 * 4. Drive forward a little to push the stack about the first cube forward a little.
 */
void score_move_by_distance() {
  a_move_drive(0, 0);
  a_move_intake(0);

  hinge.set_encoder_units(E_MOTOR_ENCODER_ROTATIONS);
  hinge.tare_position();
  hinge.move_absolute(1 * TICK_PER_INCH, 40); // Drive forward for 1"
  delay(1500);
  hinge.move_absolute(1 * TICK_PER_INCH, 30); // was 45
  delay(1800);
  a_move_hinge(0);

  // Drive forward a little bit
  a_tare_position();
  double ticks = 1.5;
  a_move_absolute(ticks, 25, 100);  // velocity 20%, timeout 100ms
}

/**
 * Autonomous program for small zone. It takes advantage of V5's profiling movements
 * and PID to control how far the motors travel, instead of relying on time.
 */



 //-------------------------------------------------------------------------------
 //-------------------------------------------------------------------------------
 //-------------------------------------------------------------------------------
 //-------------------------------------------------------------------------------



void small_move_by_distance(bool isRed) {
  // Start intake. Apply negative force on the arm to prevent it from undesired lifting.
  a_move_lift(-20);
  a_move_intake(127);

  // For direct drive, we will count ticks.
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  // 1. Drive forward 6", pick up first Cube
  double ticks = 6;
  a_move_absolute(ticks, 50);
  printf("Done with first move. Timelap = %d\n", millis() - start_ts);
  // pick up first cube
  delay(200);

  // 2. Drive forward 5.5", pick up 2nd Cube
  ticks += 6.5;
  a_move_absolute(ticks, 75);
  printf("Done with second move. Timelap = %d\n", millis() - start_ts);
  pros::delay(200);   // was 650

  // 3. Drive forward 5.5", pick up 3rd Cube
  ticks += 5.5;
  a_move_absolute(ticks, 50);
  printf("Done with 3rd move.Timelap = %d\n", millis() - start_ts);
  delay(200); // was 650

  // 4. Drive forward 6", pick up 4th Cube
  ticks += 6;
  a_move_absolute(ticks, 50);
  printf("Done with 4th move.Timelap = %d\n", millis() - start_ts);
  delay(200); //800 works for 4 cubes

  // 5. Drive forward 6", pick up 5th Cube
  ticks += 16;
  a_move_absolute(ticks, 50);
  delay(400); //was 450
  printf("****After 5th cube\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);

  a_move_lift(0);

  // 6. Backup to where the center of robot is at the border of first tile and second tile
  ticks -= 39.5;
  a_move_absolute(ticks, 75, 2000);
  delay(100);
  printf("****After backing\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);


  // New 7
  double temp_ticks = 9.75;
  double RF_ticks = ticks;
  double RR_ticks = ticks;
  double LF_ticks = ticks;
  double LR_ticks = ticks;

  if (isRed) {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  printf("Done with turn. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());


  temp_ticks = 11.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 50, 1500);
  delay(100);

  // 9. Score
  score_new();
  delay(100);
  temp_ticks = 0.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 30);
  delay(100);

  // 10. Drive backward

  a_tare_position();
  ticks = -21;
  a_move_relative(ticks, 100);
  printf("****All complete \tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);

  // 11. Move back the hinge.
  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);
  printf("Done with moving back hinge. Timelap = %d\n", millis() - start_ts);

}

void small_move_by_distance_new_five(bool isRed) {
  // Start intake. Apply negative force on the arm to prevent it from undesired lifting.
  a_move_lift(-20);
  a_move_intake(127);

  // For direct drive, we will count ticks.
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  // 1. Drive forward 6", pick up first Cube
  double ticks = 6;
  a_move_absolute(ticks, 75);
  printf("Done with first move. Timelap = %d\n", millis() - start_ts);
  // pick up first cube
  delay(200);

  // 2. Drive forward 5.5", pick up 2nd Cube
  ticks += 6.5;
  a_move_absolute(ticks, 75);
  printf("Done with second move. Timelap = %d\n", millis() - start_ts);
  pros::delay(200);   // was 650

  // 3. Drive forward 5.5", pick up 3rd Cube
  ticks += 5.5;
  a_move_absolute(ticks, 75);
  printf("Done with 3rd move.Timelap = %d\n", millis() - start_ts);
  delay(200); // was 650

  // 4. Drive forward 6", pick up 4th Cube
  ticks += 6;
  a_move_absolute(ticks, 50);
  printf("Done with 4th move.Timelap = %d\n", millis() - start_ts);
  delay(300); //800 works for 4 cubes

  // 5. Drive forward 6", pick up 5th Cube
  ticks += 6;
  a_move_absolute(ticks, 50);
  delay(300); //was 450
  printf("****After 5th cube\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);

  a_move_lift(0);

  // 6. Backup to where the center of robot is at the border of first tile and second tile
  ticks -= 11.5;
  a_move_absolute(ticks, 75);
  delay(100);
  printf("****After backing\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);

  // New 7
  double temp_ticks = 12;
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
  temp_ticks = 20;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;

  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  delay(100);
  printf("****Before adjust\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);

  // 9. Score
  score_new2();
  printf("****After scoring, Timelap: %d\n", millis() - start_ts);

  // 10. Drive backward

  a_tare_position();
  ticks = -21;
  a_move_relative(ticks, 100);
  printf("****All complete \tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);

  // 11. Move back the hinge.
  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);
  printf("Done with moving back hinge. Timelap = %d\n", millis() - start_ts);

}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------




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
  a_move_lift(-20); ///added 11/29 to prevent bouncing
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

void red_big() {
  big2(true);
}

void blue_big() {
  big2(false);
}



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------



void one_cube(bool isRed) {
  // For direct drive, we will count ticks.
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  // 1. Drive backward to push the cube into goal zone
  double ticks = (-6);
  a_move_absolute(ticks, 100);
  printf("Done with first move. \n");

  ticks = 12;
  a_move_relative(ticks, 75);
  delay(1000);

  // Turn the robot to face the goal zone
  ticks = 2;
  if (isRed) {
    a_move_relative(ticks, ticks, (-1)*ticks, (-1)*ticks, 75);
  }
  else {
    a_move_relative((-1)*ticks, (-1)*ticks, ticks, ticks, 75);
  }
  delay(1000);

  ticks = 5;
  a_move_relative(ticks, 75);
  delay(1000);

  ticks = 14;
  if (isRed) {
    a_move_relative(ticks, ticks, (-1)*ticks, (-1)*ticks, 75);
  }
  else {
    a_move_relative((-1)*ticks, (-1)*ticks, ticks, ticks, 75);
  }
  delay(1000);

}



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------



void skills(bool isRed) {

  // First pass to pick up 5
  // 1. Complete auton
  small_move_by_distance(isRed);

  // 2. Turn to face the pole
  double ticks = 12.5;
  a_move_relative(ticks, ticks, (-1)*ticks, (-1)*ticks, 75);
  delay(1000);

  // 3. Drive to the pole.
  ticks = 22;
  a_move_relative(ticks, 75);
  delay(1000);

  // 4. Intake a cube
  a_move_intake(80);
  delay(1100);
  a_move_intake(2);
  delay(1000);

  //double ticks = (-2);
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
}



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------



void small_eightcube(bool isRed) {
  ///unfolding code


  /// setting up robot and intaking preload
  a_move_lift(-20);
  a_move_intake(127);
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();
  delay(500);

  /// driving forward 15" for the first cube
  double ticks = 25;
  a_move_absolute(ticks, 70, 2000);
  //delay(50);
  printf("Done with the first move. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());


  /// driving forward 5.5" for the second cube
  ticks += 5.5;
  a_move_absolute(ticks, 50);
  delay(50);

  printf("Done with the second move. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());

  /// driving forward 5.5" for the third cube
  ticks += 7;
  a_move_absolute(ticks, 50);
  delay(500);

  printf("Done with the third move. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());

  /// driving backward with four cubes
  ticks -= 37.75;
  a_move_absolute(ticks, 100, 2500);
  //delay(20);

  printf("Done with moving back. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());

  /// adjust robot
  double  temp_ticks = 8.5;
  double RF_ticks;
  double RR_ticks;
  double LF_ticks;
  double LR_ticks;
  if (isRed) {
    RF_ticks = ticks - temp_ticks;
    RR_ticks = ticks - temp_ticks;
    LF_ticks = ticks + temp_ticks;
    LR_ticks = ticks + temp_ticks;

    printf("Before the first turn. RF_target (in inches): %f, RR_target: %f, LF_target: %f, LR_target: %f\n",RF_ticks, RR_ticks, LF_ticks, LR_ticks);

    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  else {
    RF_ticks = ticks + temp_ticks;
    RR_ticks = ticks + temp_ticks;
    LF_ticks = ticks - temp_ticks;
    LR_ticks = ticks - temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  printf("Done with first turn. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());

  //delay(50);
  /*
  printf("Done with turning. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());


  ticks += 20;
  a_move_absolute(ticks, 75);

  printf("Done with moving forward again. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());
  */

  temp_ticks = 24;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  printf("Before driving to the second row. RF_target (in inches): %f, RR_target: %f, LF_target: %f, LR_target: %f\n",RF_ticks, RR_ticks, LF_ticks, LR_ticks);
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2500);

  printf("Done with driving to the second row. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());



  /// new turn - may cause issues since the robot is already turning on its own
  temp_ticks = 8.25;
  if (isRed) {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    printf("Before second turn. RF_target (in inches): %f, RR_target: %f, LF_target: %f, LR_target: %f\n",RF_ticks, RR_ticks, LF_ticks, LR_ticks);
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  else {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  printf("Done with second turn. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());


  /// driving forward 11.5" for the fourth cube
  temp_ticks = 11.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;

  //printf("Before second turn. RF_target (in inches): %f, RR_target: %f, LF_target: %f, LR_target: %f\n",RF_ticks, RR_ticks, LF_ticks, LR_ticks);
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 70);
  delay(200);
  /// driving forward 5.5" for the fifth cube
  temp_ticks = 5.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;

  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 70);
  delay(300);
  /// driving forward 5.5" for the sixth cube

  temp_ticks = 6.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 70);
  delay(500);
  /// driving forward 5.5" for the seventh cube

  temp_ticks = 10;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 70);
  delay(600);

  printf("Done with second row pickup. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());


  a_move_lift(0);

  /// driving backward to score and adjusting robot position
  temp_ticks -= 50;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2500);
  delay(50);
  printf("Done with 2nd backing up. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());


  temp_ticks = 9.75;
  if (isRed) {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  printf("Done with 3rd turn. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());


  temp_ticks = 11;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2500);
  delay(100);

  score_new();
  delay(100);

  temp_ticks = 15;
  RF_ticks -= temp_ticks;
  RR_ticks -= temp_ticks;
  LF_ticks -= temp_ticks;
  LR_ticks -= temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  delay(100);

  a_move_hinge(-127);
  delay(600);

  temp_ticks = 8.5;
  if (isRed) {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  else {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }

  temp_ticks = -1;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  delay(100);

  temp_ticks = 44;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  ///tower starts

  a_move_intake(80);
  delay(1400);
  a_move_intake(2);
  delay(1000);

  temp_ticks = -5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  a_move_hinge(127);
  delay(600); /// This value was increased to decrease chances of entanglement.
  a_move_hinge(0);
  delay(1000);

  a_move_lift(127);
  delay(1000);
  a_move_lift(20);
  delay(1000);

  a_move_intake(-127);
  delay(500);
  a_move_intake(0);
  a_move_lift(-127);
  delay(500);
  a_move_hinge(-127);
  delay(200);
}



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------



void autonomous() {
  redAlliance = true; ///variable
  autonNumber = 5; ///variable
    // 1: 1 Cube; 2: 2 Cubes (Large Zone); 3: 5 Cubes (Small Zone); 4: Skills; 5: 8 Cubes (Small Zone)

  start_ts = millis();
  unfold_new();

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
        small_move_by_distance(true);
        break;
      case 4:
        small_eightcube(true);
        break;
      case 5:
        small_eightcube(true);
        break;
      default:
        small_eightcube(true);
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
        small_move_by_distance_new_five(false);
        break;
      case 4:
        small_eightcube(false);
        break;
      case 5:
        small_eightcube(false);
        break;
      default:
        small_eightcube(false);
        break;
    }
  }
}
