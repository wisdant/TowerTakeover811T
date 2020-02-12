#include "main.h"
using namespace std;
using namespace pros;
#define MOTOR_MAX_SPEED 100
#define TICK_PER_INCH 72
#define LOW_MARK (95 / 100)
#define HIGH_MARK (105 / 100)
#define ALLIANCE_TOWER 1
#define MID_TOWER 2
#define TALL_TOWER 3
#define CENTRAL_TOWER 4
#define DEFAULT_TIMEOUT 3000
#define VERBOSE true
extern int autonNumber;
extern bool redAlliance;
extern Controller master;
extern Motor left_drive;
extern Motor left_rear_drive;
extern Motor right_drive;
extern Motor right_rear_drive;
extern Motor left_intake;
extern Motor right_intake;
extern Motor lift;
extern Motor hinge;
int start_ts;
void a_set_drive_encoding(motor_encoder_units_e_t units) {
  left_drive.set_encoder_units(units);
  left_rear_drive.set_encoder_units(units);
  right_drive.set_encoder_units(units);
  right_rear_drive.set_encoder_units(units);
}
void a_tare_position(){
  left_drive.tare_position();
  left_rear_drive.tare_position();
  right_drive.tare_position();
  right_rear_drive.tare_position();
}
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

    if ((abs(LF_pos - FL_target) > abs(FL_target) * 5/100)
        || (abs(LR_pos - RL_target) > abs(RL_target) * 5/100)
        || (abs(RF_pos - FR_target) > abs(FR_target) * 5/100)
        || (abs(RR_pos - RR_target) > abs(RR_target) * 5/100)
    ) {
      delay(20);
      if ((timeout!=-1) && (counter * 20 > timeout)) break;
    }
    else {
      delay(20);
      break;
    }
  }
  if (VERBOSE) {
    printf("Finished moving: %d, FR: %f, RR: %f, FL: %f, RL: %f\n", counter,
      right_drive.get_position(), right_rear_drive.get_position(),
      left_drive.get_position(), left_rear_drive.get_position());
  }
}
void a_move_relative(double target, std::int32_t velocity){
   move_by_distance(false, target, target, target, target, velocity, DEFAULT_TIMEOUT);;
 }
void a_move_relative(double target, std::int32_t velocity, int timeout){
   move_by_distance(false, target, target, target, target, velocity, timeout);;
}
void a_move_relative(double FR_target, double RR_target, double FL_target, double RL_target,
       std::int32_t velocity){
   move_by_distance(false, FR_target, RR_target, FL_target, RL_target, velocity, DEFAULT_TIMEOUT);
}
void a_move_relative(double FR_target, double RR_target, double FL_target, double RL_target,
      std::int32_t velocity, int timeout){
  move_by_distance(false, FR_target, RR_target, FL_target, RL_target, velocity, timeout);
}
void a_move_absolute(double position, std::int32_t velocity){
  move_by_distance(true, position, position, position, position, velocity, DEFAULT_TIMEOUT);
}
void a_move_absolute(double position, std::int32_t velocity, int timeout){
  move_by_distance(true, position, position, position, position, velocity, timeout);
}
void a_move_absolute(double FR_target, double RR_target, double FL_target, double RL_target,
    std::int32_t velocity){
  move_by_distance(true, FR_target, RR_target, FL_target, RL_target, velocity, DEFAULT_TIMEOUT);
}
void a_move_absolute(double FR_target, double RR_target, double FL_target, double RL_target,
    std::int32_t velocity, int timeout){
  move_by_distance(true, FR_target, RR_target, FL_target, RL_target, velocity, timeout);
}
void a_move_drive(int right, int left) {
  left_drive.move(left);
  left_rear_drive.move(left);
  right_drive.move(right);
  right_rear_drive.move(right);
}
void a_move_intake(int power) {
  left_intake.move(power);
  right_intake.move(power);
}
void a_move_lift(int power) {
  lift.move(power);
}
void a_move_hinge(int power) {
  hinge.move(power);
}



void unfold() {
  a_move_intake(-30);
  a_move_hinge(127);
  delay(600);
  a_move_hinge(-127);
  delay(300);
  a_move_hinge(0);
  //delay(400);
}

void stackSixEight() {
  a_move_intake(0);
  a_move_lift(-20);

  a_move_hinge(100);
  a_move_intake(30);
  delay(900); //300

  a_move_hinge(60);
  a_move_intake(0);
  delay(400);

  a_move_hinge(60);
  a_move_intake(-60);
  delay(700);

  a_move_hinge(40);
  delay(1200);

  a_move_intake(0);
  a_move_drive(0, 0);
  a_move_hinge(0);
  a_move_lift(0);
}

void stackThreeFour() {
  a_move_intake(0);
  a_move_lift(-20);

  a_move_hinge(100);
  a_move_intake(20);
  delay(400); //300

  a_move_hinge(60);
  delay(1100);

  a_move_hinge(40);
  a_move_intake(-50);
  delay(1200);

  a_move_intake(0);
  a_move_drive(0, 0);
  a_move_hinge(0);
  a_move_lift(0);
}

void one(bool isRed) {
  double ticks = -12;
  a_move_absolute(ticks, 80, 2000);
  ticks += 24;
  a_move_absolute(ticks, 80, 2000);

  a_move_hinge(127);
  delay(150);
}

void large_four(bool isRed) {
    a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
    a_tare_position();
    unfold();
    a_move_lift(-20);
    a_move_intake(127);
    double ticks = 27; //31
    a_move_absolute(ticks, 110, 2400);
    delay(200);
    ticks += 19;
    a_move_absolute(ticks, 50);
    delay(300);
    //ticks += 4;
    //a_move_absolute(ticks, 50);
    //delay(300);
    ticks -= 45;
    a_move_absolute (ticks, 85, 600);
    ticks += 3.5;
    a_move_absolute (ticks, 75);
    double temp_ticks = 8.75;
    double RF_ticks = ticks;
    double RR_ticks = ticks;
    double LF_ticks = ticks;
    double LR_ticks = ticks;

    if (isRed) {
      RF_ticks += temp_ticks;
      RR_ticks += temp_ticks;
      LF_ticks -= temp_ticks;
      LR_ticks -= temp_ticks;
      a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 85, 1400);
    }
    else {
      RF_ticks -= temp_ticks;
      RR_ticks -= temp_ticks;
      LF_ticks += temp_ticks;
      LR_ticks += temp_ticks;
      a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 85, 1400);
    }
    a_move_intake(40);
    temp_ticks = 24; ////temp_ticks = 18.25;
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 85);

    temp_ticks = 4; //temp_ticks = 7.5;
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 30);
    stackThreeFour();

    temp_ticks = -12;
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 95);

    a_move_hinge(80);
    a_move_intake(-40);
    delay(1700);
    delay(300);

    a_move_relative(0, 60);
    a_move_hinge(0);
    a_move_intake(0);
}

void small_five(bool isRed) {
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  double ticks = 3; ///

  double RF_ticks = ticks; ///
  double RR_ticks = ticks; ///
  double LF_ticks = ticks; ///
  double LR_ticks = ticks; ///

  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75); ///

  double temp_ticks = 3; ///
  RF_ticks -= temp_ticks; ///
  RR_ticks -= temp_ticks; ///
  LF_ticks -= temp_ticks; ///
  LR_ticks -= temp_ticks; ///

  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75); ///

  unfold();

  a_move_lift(-20);
  a_move_intake(127);
  delay(200); //delay(400);

  temp_ticks = 28;

  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  delay(100);

  temp_ticks = 9;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 30); //45
  delay(600); //delay 300

  a_move_lift(0);

  temp_ticks = -16;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2000);
  delay(100);

  temp_ticks = 11;

  if (isRed) {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1700);
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1700);
  }

  temp_ticks = 27; //temp_ticks = 21;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 45, 2000); //1800
  delay(30);

  a_move_intake(0);

  a_tare_position();

  temp_ticks = -1.5; //-1.75
  RF_ticks = temp_ticks;
  RR_ticks = temp_ticks;
  LF_ticks = temp_ticks;
  LR_ticks = temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 70, 600);
  delay(30);

  lcd::print(2, "Before stacking. Timelap=%d\n", millis() - start_ts);

  a_move_intake(0);
  stackSixEight();
  a_move_intake(-45); //
  delay(200); //


  a_tare_position();

  a_move_intake(-40);
  a_move_drive(-100, -100);
  a_move_hinge(-127);
  delay(400);

  a_move_lift(0);
  a_move_intake(0);

  lcd::print(3, "After back off. Timelap=%d\n", millis() - start_ts);

  // Drive to position for manual control
  a_tare_position();

  temp_ticks = 10;

  if (isRed) {
    RF_ticks = temp_ticks;
    RR_ticks = temp_ticks;
    LF_ticks = (-1)*temp_ticks;
    LR_ticks = (-1)*temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  else {
    RF_ticks = (-1)*temp_ticks;
    RR_ticks = (-1)*temp_ticks;
    LF_ticks = temp_ticks;
    LR_ticks = temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }

  temp_ticks = -30;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;

  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2000);

  temp_ticks = 5;
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

  a_move_hinge(0);
  a_move_drive(0, 0);
  a_move_intake(0);
}

void small_six(bool isRed) {
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  double ticks = 3; ///

  double RF_ticks = ticks; ///
  double RR_ticks = ticks; ///
  double LF_ticks = ticks; ///
  double LR_ticks = ticks; ///

  //a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75); ///

  double temp_ticks = 3; ///
  RF_ticks -= temp_ticks; ///
  RR_ticks -= temp_ticks; ///
  LF_ticks -= temp_ticks; ///
  LR_ticks -= temp_ticks; ///

  //a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75); ///

  unfold();

  /*
  a_move_lift(-20);
  a_move_intake(127);
  delay(200); //delay(400);
  */

  a_move_intake(127); //

  temp_ticks = 28;

  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  //delay(100);

  temp_ticks = 9;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 30); //45
  delay(300); //delay 300

  temp_ticks = 2.5;

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

  temp_ticks = 8;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 55, 1200); //speed 75
  delay(600); //700

  temp_ticks = -8;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1200);

  temp_ticks = -4;
  if (isRed) {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 4000);
  }
  else {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 4000);
  }

  a_move_lift(0);

  temp_ticks = -18;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 85, 2000);
  delay(50);

  a_move_intake(80);

  if (isRed) {
    temp_ticks = 9.75; //11.25 //10
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 85, 1900);
  }
  else {
    temp_ticks = 11.25; //11.55 //10
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 85, 1900);
  }

  temp_ticks = 28.5; //temp_ticks = 21;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 65, 2050); //45 speed

  a_move_intake(0);

  a_tare_position();

  temp_ticks = -3; //-1.75
  RF_ticks = temp_ticks;
  RR_ticks = temp_ticks;
  LF_ticks = temp_ticks;
  LR_ticks = temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 127, 600);

  a_move_intake(-127);
  delay(150);
  stackSixEight();

  temp_ticks = -12;
  RF_ticks = temp_ticks;
  RR_ticks = temp_ticks;
  LF_ticks = temp_ticks;
  LR_ticks = temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 120, 600);

  a_tare_position();
  /*
  a_move_intake(-40);
  a_move_drive(-100, -100);
  a_move_hinge(-127);
  delay(400);
  */

  a_move_lift(0);
  a_move_intake(0);
  a_move_hinge(-127);
  delay(300);

  a_tare_position();

  temp_ticks = 10;

  if (isRed) {
    RF_ticks = temp_ticks;
    RR_ticks = temp_ticks;
    LF_ticks = (-1)*temp_ticks;
    LR_ticks = (-1)*temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  else {
    RF_ticks = (-1)*temp_ticks;
    RR_ticks = (-1)*temp_ticks;
    LF_ticks = temp_ticks;
    LR_ticks = temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }

  temp_ticks = -30;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;

  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2000);

  temp_ticks = 5;
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

  a_move_hinge(0);
  a_move_drive(0, 0);
  a_move_intake(0);
}

void small_eight(bool isRed) {

}


void skills_towers(bool isRed) {
  small_six(isRed);
  a_move_intake(127);
  double temp_ticks = 6;
  double RF_ticks = 0;
  double LF_ticks = 0;
  double RR_ticks = 0;
  double LR_ticks = 0;

  RF_ticks += temp_ticks;
  LF_ticks -= temp_ticks;
  RR_ticks += temp_ticks;
  LR_ticks -= temp_ticks;

  temp_ticks = 12;
  RF_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LR_ticks += temp_ticks;

  a_move_intake(-60);
  delay(300);

  temp_ticks = -4;
  RF_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LR_ticks += temp_ticks;

  a_move_lift(127);
  delay(1000);
  a_move_intake(-90);

  temp_ticks = -10;
  RF_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 80);
}

void skills_ten(bool isRed) {
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  double ticks = 3; ///

  double RF_ticks = ticks; ///
  double RR_ticks = ticks; ///
  double LF_ticks = ticks; ///
  double LR_ticks = ticks; ///

  double temp_ticks = 3; ///
  RF_ticks -= temp_ticks; ///
  RR_ticks -= temp_ticks; ///
  LF_ticks -= temp_ticks; ///
  LR_ticks -= temp_ticks; ///

  unfold();

  a_move_intake(127); //

  temp_ticks = 28;

  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  //delay(100);

  temp_ticks = 9;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 30); //45
  delay(300); //delay 300

  temp_ticks = 0.5;

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

  temp_ticks = -0.5;

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

  temp_ticks = 38;

  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 65);

}



void autonomous() {
  redAlliance = true;
  autonNumber = 2;
  //1-small_six, 2-large_four, 3-skills
  start_ts = millis();

  switch (autonNumber) {
    case 0:
      unfold();
    case 1:
      small_six(redAlliance);
      break;
    case 2:
      large_four(redAlliance);
      break;
    case 3:
      skills_towers(redAlliance);
      break;
    default:
      unfold();
      break;
  }
}


//driving/strategy: defense, fast stacking, gathering purple
//hardware: secure slider
//software: autonomous, turn speed, buttons





















/*
LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT
LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT
LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT
LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT
LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT
LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT
LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT LEAFY YARROT
/*
RICHARD'S SKILLS PROGRAMS:

void skills (bool isRed) {
  ///unfolding code


  /// setting up robot and intaking preload
  a_move_lift(-20);
  a_move_intake(127);
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();
  delay(500);

  // 1. driving forward for the first cube
  double ticks = 25;
  a_move_absolute(ticks, 70, 2000);
  delay(50);
  printf("Done with the first move. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());


  // 2. driving forward for the second cube
  ticks += 5.5;
  a_move_absolute(ticks, 50);
  delay(50);
  printf("Done with the second move. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());

  // 3. driving forward for the third cube
  ticks += 7;
  a_move_absolute(ticks, 50);
  delay(500);
  printf("Done with the third move. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());

  // 4. driving backward with four cubes
  ticks -= 37.75;
  a_move_absolute(ticks, 100, 2500);
  printf("Done with moving back. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());

  // 5. Turn the robot towards the goal
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
    printf("Before the first turn. RF_target (in inches): %f, RR_target: %f, LF_target: %f, LR_target: %f\n",
          RF_ticks, RR_ticks, LF_ticks, LR_ticks);
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

  // 6. Drive towards the second row
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

  // 7. Turn to pick up cubes in the second row
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


  // 9. Driving forward for the first cube in the second row
  temp_ticks = 11.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 70);
  delay(200);
  printf("Done w/ Cube#1 in row#2, RF_target (in inches): %f, RR_target: %f, LF_target: %f, LR_target: %f\n",
      RF_ticks, RR_ticks, LF_ticks, LR_ticks);

  // 10. Driving forward for cube #2 in row #2
  temp_ticks = 5.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 70);
  delay(300);
  printf("Done w/ Cube#2 in row#2, RF_target (in inches): %f, RR_target: %f, LF_target: %f, LR_target: %f\n",
      RF_ticks, RR_ticks, LF_ticks, LR_ticks);

  // 11. Driving forward for cube #3 in row #2
  temp_ticks = 6.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 70);
  delay(500);
  printf("Done w/ Cube#3 in row#2, RF_target (in inches): %f, RR_target: %f, LF_target: %f, LR_target: %f\n",
      RF_ticks, RR_ticks, LF_ticks, LR_ticks);

  // 12. Driving forward for cube #4 in row #2
  temp_ticks = 6.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 70);
  delay(600);
  printf("Done with second row pickup. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());

  delay(1600);

  temp_ticks = 6.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 30);
  delay(1000);
  printf("Done with second row pickup. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());

  a_move_lift(0);

  // 13. Driving backward to score and adjusting robot position
  temp_ticks = -38;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2500);
  delay(50);
  printf("Done with 2nd backing up. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());

    // 13a. Driving backward to score and adjusting robot position
    temp_ticks = 1;
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
    delay(50);
    printf("Done with 2nd backing up. FR: %f, RR: %f, FL: %f, RL: %f\n",
      right_drive.get_position(), right_rear_drive.get_position(),
      left_drive.get_position(), left_rear_drive.get_position());


  // 14. Final turn towards the goal
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
  printf("Done with final turn. FR: %f, RR: %f, FL: %f, RL: %f\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position());

  // 15. High speed driving towards the goal zone
  temp_ticks = 9;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2000);
  delay(100);

  // 16. Low speed for the final inches to the goal zone
  temp_ticks = 2;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 40);
  delay(100);
//// This is where Richard's fancy code starts.
  // 16a Final adjustment - minor turn
  temp_ticks = 1;

  if (isRed) {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 300);
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 300);
  }
  printf("****After final adjustment\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);


  // 17. Scoring
  score_eight();
  temp_ticks = 1;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 30);
  delay(500);

  // 18. Back away to go to the first tower
  temp_ticks = 18;
  RF_ticks -= temp_ticks;
  RR_ticks -= temp_ticks;
  LF_ticks -= temp_ticks;
  LR_ticks -= temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  delay(100);

  a_move_hinge(-127);
  delay(600);

  // 19. Turn towards a tower
  temp_ticks = 9.5;
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

  // 20. Bounce against the wall
  temp_ticks = -12;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1000);
  delay(100);

  // 21. Drive to the first tower
  temp_ticks = 45;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  // 22. Intake the cube for tower #1
  a_move_intake(80);
  delay(1000);
  a_move_intake(2);

  // 23. Backup and get ready for tower scoring
  temp_ticks = -5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  // 24. Tower score
  score_tower(MID_TOWER);

  // 25. Drive towards tower #2
  temp_ticks = -24;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  // 26. Towards the high tower
  temp_ticks = 9.75;
  if (isRed) {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
  }
  else {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
  }
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  // 27. Drive to the higher tower
  temp_ticks = 20;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  // 28. Intake and score the higher tower
  a_move_intake(80);
  delay(1000);
  a_move_intake(2);

  // Backup and get ready for tower scoring
  temp_ticks = -5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  score_tower(TALL_TOWER);

  // 29. Turn to intake cube for tower #3 (Pick up the cube by the pole)
  temp_ticks = 4;
  if (isRed) {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
  }
  else {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
  }
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  // 29a Intake
  a_move_intake(80);
  delay(1000);
  a_move_intake(2);

  // 30. Backup to align with the appliance tower
  temp_ticks = -5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  // 31. Turn towards the alliance tower
  temp_ticks = 12;
  if (isRed) {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
  }
  else {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
  }
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  // 32. Drive to alliance tower
  temp_ticks = 30;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2500);

  // 33. score the higher tower

  score_tower(ALLIANCE_TOWER);

  // 34. Backup
  temp_ticks = -100;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 5000);

  /*
  // 35. Turn towards far mid towers
  temp_ticks = 9.75;
  if (isRed) {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
  }
  else {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
  }
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  //36. Drive to the far mid tower
  temp_ticks = 30;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1500);

  // 37. Intake and score the far middle tower
  a_move_intake(80);
  delay(1000);
  a_move_intake(2);

  // Backup and get ready for tower scoring
  temp_ticks = -5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  score_tower(MID_TOWER);
  */
/*
  // 34. Backup
  temp_ticks = -10;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
}

void merryChristmas (bool isRed) {

  /// setting up robot and intaking preload
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  a_move_lift(-20);
  a_move_intake(127);

  double temp_ticks = 0;
  double RF_ticks = 0;
  double RR_ticks = 0;
  double LF_ticks = 0;
  double LR_ticks = 0;

  // 1. Pick up the first row, away from the goal zone.
  temp_ticks = 37;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(temp_ticks, temp_ticks, temp_ticks, temp_ticks, 75, 2800);
  delay(40);
  printf("Done with the 1st row. Timelap = %d\n", millis() - start_ts);
  lcd::print(3, "Done with the 1st row. Timelap = %d\n", millis() - start_ts);

  // 2. Turn to move to the first row.
  temp_ticks = 3.1;
  if (isRed) {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
  }
  else {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
  }
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 400);
  delay(20);
  printf("Done with first turn. Timelap = %d\n", millis() - start_ts);

  // 3. Back diagonally to the second row, which is close to the goal zone
  temp_ticks = -40.75;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 90, 2300);
  delay(40);
  printf("Done with backing. Timelap = %d\n", millis() - start_ts);

  // 4. Set the robot straight
  temp_ticks = 3.1;
  if (isRed) {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
  }
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 300);
  delay(20);
  printf("Done with second turn. Timelap = %d\n", millis() - start_ts);

  // 5. Bank the wall
  temp_ticks = -5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 80, 400);
  delay(20);
  printf("Done with banking the wall. Timelap = %d\n", millis() - start_ts);
  lcd::print(2, "After 1st wall. Timelap=%d\n", millis() - start_ts);

  // 6. Pick up the second row.
  // TODO: Need to adjust speed to pick up all the cubes.
  // Right now, setting it to 75 since we are short on time.
  temp_ticks = 37;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2750);
  delay(40);
  printf("Done with second row. Timelap = %d\n", millis() - start_ts);
  lcd::print(4, "Done with 2nd row. Timelap=%d\n", millis() - start_ts);

  // 7. Back and bank against the wall
  temp_ticks = 17;
  RF_ticks -= temp_ticks;
  RR_ticks -= temp_ticks;
  LF_ticks -= temp_ticks;
  LR_ticks -= temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 85, 1750);
  delay(20);
  printf("Done with backing. Timelap = %d\n", millis() - start_ts);

  // 9. Final turn
  temp_ticks = 12;
  if (isRed) {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
  }
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1500);
  delay(20);
  printf("Done with turn. FR: %f, RR: %f, FL: %f, RL: %f, Time: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);


  // 10. Drive to the goal zone
  temp_ticks = 15;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 100, 1100);
  delay(20);
  printf("****After driving to zone\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);

  // 11. Final adjustment
  temp_ticks = 1;
  RF_ticks -= temp_ticks;
  RR_ticks -= temp_ticks;
  LF_ticks -= temp_ticks;
  LR_ticks -= temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 150);
  delay(20);
  printf("After final adjustment. Timelap = %d\n", millis() - start_ts);
  lcd::print(4, "After final adjustment. Timelap=%d\n", millis() - start_ts);

/**** Old version - back to the wall.
  // 7. Back and bank against the wall
  temp_ticks = 38;
  RF_ticks -= temp_ticks;
  RR_ticks -= temp_ticks;
  LF_ticks -= temp_ticks;
  LR_ticks -= temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 90, 1750);
  delay(40);
  printf("Done with backing. Timelap = %d\n", millis() - start_ts);

  // 8. Move slightly away from the wall for the turn
  temp_ticks = 1;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 80, 200);
  delay(20);
  printf("Done with bounce back. Timelap = %d\n", millis() - start_ts);
  lcd::print(3, "After 2nd wall. Timelap=%d\n", millis() - start_ts);

  // 9. Final turn
  temp_ticks = 9.75;
  if (isRed) {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
  }
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1000);
  delay(40);
  printf("Done with turn. FR: %f, RR: %f, FL: %f, RL: %f, Time: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);


  // 10. Drive to the goal zone
  temp_ticks = 9.50;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 800);
  delay(20);
  printf("****After driving to zone\tRF: %f\tRR: %f\tLF: %f\tLR: %f Timelap: %d\n",
    right_drive.get_position(), right_rear_drive.get_position(),
    left_drive.get_position(), left_rear_drive.get_position(), millis() - start_ts);

  // 11. Final adjustment
  temp_ticks = 2.5;
  if (isRed) {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 200);
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 200);
  }
  printf("After final adjustment. Timelap = %d\n", millis() - start_ts);
  lcd::print(4, "After final adjustment. Timelap=%d\n", millis() - start_ts);
*/
/*
  // 12. Score
  christmasTree(isRed);
  printf("Done with scoring. Timelap = %d\n", millis() - start_ts);
  lcd::print(7, "After score. Timelap=%d\n", millis() - start_ts);

  // 13. Back away to the original position
  a_move_intake(-40);
  a_move_drive(-100, -100);
  a_move_hinge(-127);
  delay(200);
  a_move_intake(0);
  a_move_drive(0, 0);
  a_move_hinge(0);


  printf("All done. Timelap = %d\n", millis() - start_ts);
  lcd::print(2, "After back away. Timelap=%d\n", millis() - start_ts);

}
/// comment out here












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
#define ALLIANCE_TOWER 1
#define MID_TOWER 2
#define TALL_TOWER 3
#define CENTRAL_TOWER 4


#define DEFAULT_TIMEOUT 3000
#define VERBOSE true     //Used when we want to display more debugging message


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
 /// comment out here
void a_set_drive_encoding(motor_encoder_units_e_t units) {
  left_drive.set_encoder_units(units);
  left_rear_drive.set_encoder_units(units);
  right_drive.set_encoder_units(units);
  right_rear_drive.set_encoder_units(units);
}

/**
 * Reset the counting for all four drive motors.
 * See more at https://pros.cs.purdue.edu/v5/api/cpp/motors.html#tare-position
 /// comment out here
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
 /// comment out here
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
    //if (!((abs(LF_pos) > abs(FL_target) * 95/100) &&  (abs(LF_pos) < abs(FL_target) * 105/100))
      //  || !((abs(LR_pos) > abs(RL_target) * 95/100) &&  (abs(LR_pos) < abs(RL_target) * 105/100))
      //  || !((abs(RF_pos) > abs(FR_target) * 95/100) &&  (abs(RF_pos) < abs(FR_target) * 105/100))
      //  || !((abs(RR_pos) > abs(RR_target) * 95/100) &&  (abs(RR_pos) < abs(RR_target) * 105/100))
      // ){
    if ((abs(LF_pos - FL_target) > abs(FL_target) * 5/100)
        || (abs(LR_pos - RL_target) > abs(RL_target) * 5/100)
        || (abs(RF_pos - FR_target) > abs(FR_target) * 5/100)
        || (abs(RR_pos - RR_target) > abs(RR_target) * 5/100)
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
 /// comment out here
void a_move_relative(double target, std::int32_t velocity){
   move_by_distance(false, target, target, target, target, velocity, DEFAULT_TIMEOUT);;
 }

 /**
  * Using the motor's move_relative move for @target inches at @velocity speed.
  * This is used when all the motors are moving towards the same direction.
  * @timeout: the timeout period for the movement.
  /// comment out here
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
  ///comment out here
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
  /// comment out here
void a_move_relative(double FR_target, double RR_target, double FL_target, double RL_target,
      std::int32_t velocity, int timeout){
  move_by_distance(false, FR_target, RR_target, FL_target, RL_target, velocity, timeout);
}

/**
 * Using the motor's move_absolute move for @target inches at @velocity speed.
 * This is used when all the motors are moving towards the same direction.
 * Default timeout 1000 will be used.
 /// comment out here
void a_move_absolute(double position, std::int32_t velocity){
  move_by_distance(true, position, position, position, position, velocity, DEFAULT_TIMEOUT);
}

/**
 * Using the motor's move_absolute move for @target inches at @velocity speed.
 * This is used when all the motors are moving towards the same direction.
 * @timeout: the timeout period for the movement.
 ///comment out here
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
 /// comment out here
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
 /// comment out here
void a_move_absolute(double FR_target, double RR_target, double FL_target, double RL_target,
    std::int32_t velocity, int timeout){
  move_by_distance(true, FR_target, RR_target, FL_target, RL_target, velocity, timeout);
}

/**
 * Assign specified powers for the right and left sides of the drivetrain respectively.
 /// comment out here
void a_move_drive(int right, int left) {
  left_drive.move(left);
  left_rear_drive.move(left);
  right_drive.move(right);
  right_rear_drive.move(right);
}

/**
 * Assign a specified power to the in-take motor.
 /// comment out here
void a_move_intake(int power) {
  left_intake.move(power);
  right_intake.move(power);
}

/**
 * Assign a specified power to the lift motor.
 /// comment out here
void a_move_lift(int power) {
  lift.move(power);
}

/**
 * Assign a specified power to the hinge.
 /// comment out here
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//UNFOLDING VARIATIONS:
/*
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
/// comment out here
void vertical_unfold() {
  a_move_intake(-127);
  delay(400);
}

void unfold_super_simple() {
  a_move_hinge(127);
  a_move_intake(-127);
  delay(350);
  a_move_hinge(-127);
  delay(100);
  a_move_hinge(0);
  a_move_intake(0);

  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  printf("Done with unfolding. Timelap = %d\n", millis() - start_ts);
  lcd::print(1, "Done with unfolding. Timelap = %d\n", millis() - start_ts);
}

/**
 * Score in the goal zone. This function uses simple move function by setting
 * power for each motor and control the movement through time delay. The current logic is:
 * TODO: Need to experiment to move roller in outake mode in this process.
 * 1. Stop driving forward and intake
 * 2. Move the hinge forward at a higher speed for sometime
 * 3. Move the hinge at a lower speed for more time
 * 4. Drive forward a little to push the stack about the first cube forward a little.
 /// comment out here

 ///////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////

//STACKING VARIATIONS:

 void christmasTree (bool isRed) { //written by Richard; works for 5 to 9 cubes
   a_move_intake(0); // Stop intake roller
   a_move_lift(-20);

   a_move_hinge(127); // Move the lift forward
   a_move_intake(40);
   delay(600);
   printf("Done with 127. Timelap = %d\n", millis() - start_ts);
   lcd::print(5, "After 127. Timelap=%d\n", millis() - start_ts);

   a_move_intake(-20);

   a_move_hinge(100);
   delay(300);

   a_move_hinge(80);
   delay(100);

   a_move_hinge(60);
   delay(100);

   a_move_hinge(50);
   delay(600);
   printf("Done with 50. Timelap = %d\n", millis() - start_ts);
   lcd::print(6, "After 50. Timelap=%d\n", millis() - start_ts);

   a_move_hinge(40);
   delay(600);

   a_move_intake(-80);
   delay(20);

   a_move_intake(0);
   a_move_hinge(0);
   a_move_lift(0);
 }

 void scoreTwoToFive (bool isRed) { //written by Richard; optimized for 2-5
   a_move_intake(0);
   a_move_lift(-20);

   //a_move_intake(-80);
   //delay(100);

   a_move_intake(0);

   a_move_hinge(127);
   a_move_intake(-20);
   delay(500);
   printf("Done with 127. Timelap = %d\n", millis() - start_ts);
   lcd::print(5, "After 127. Timelap=%d\n", millis() - start_ts);

   a_move_intake(-25);

   a_move_hinge(100);
   delay(200);

   a_move_hinge(80);
   delay(200);

   a_move_intake(-30);

   a_move_hinge(60);
   delay(200);

   a_move_hinge(50);
   delay(250);
   printf("Done with 50. Timelap = %d\n", millis() - start_ts);
   lcd::print(6, "After 50. Timelap=%d\n", millis() - start_ts);

   a_move_hinge(40);
   delay(700); //500

   a_move_intake(-80);
   delay(100);

   a_move_intake(-80);
   a_move_drive(-75, -75);
   a_move_hinge(-127);
   delay(300);

   a_move_intake(0);
   a_move_drive(0, 0);
   a_move_hinge(0);
   a_move_lift(0);

   printf("All done. Timelap = %d\n", millis() - start_ts);
   lcd::print(6, "All done. Timelap=%d\n", millis() - start_ts);

 }

 void scoreFourToSix (bool isRed) { //written by Richard; optimized for 4-6
   a_move_intake(0);
   a_move_lift(-20);

   a_move_hinge(127); // Move the lift forward
   a_move_intake(20);
   delay(500);
   printf("Done with 127. Timelap = %d\n", millis() - start_ts);
   lcd::print(5, "After 127. Timelap=%d\n", millis() - start_ts);

   a_move_intake(-25);

   a_move_hinge(100);
   delay(200);

   a_move_hinge(80);
   delay(200);

   a_move_hinge(60);
   delay(100);

   a_move_hinge(50);
   delay(250);
   printf("Done with 50. Timelap = %d\n", millis() - start_ts);
   lcd::print(6, "After 50. Timelap=%d\n", millis() - start_ts);

   a_move_hinge(40);
   delay(900);

   a_move_intake(-80);
   delay(20);

   a_move_intake(0);
   a_move_drive(0, 0);
   a_move_hinge(0);
   a_move_lift(0);

   printf("All done. Timelap = %d\n", millis() - start_ts);
   lcd::print(6, "All done. Timelap=%d\n", millis() - start_ts);

 }


 void score() { //works for 6 to 9 cubes; identical to opcontrol down button
   a_move_intake(0); // Stop intake roller
   a_move_lift(0);

   a_move_hinge(127); // Move the lift forward
   a_move_intake(40);
   delay(1000);	// changed from 500 to 1000 on 12/27
   a_move_intake(0);

   a_move_hinge(80);
   delay(400);		// changed from 300 to 400 on 12/27

   a_move_hinge(60);
   delay(400);		// changed from 300 to 400 on 12/27

   a_move_hinge(40);
   a_move_intake(-20);
   delay(800);

   a_move_hinge(0);

   delay(10);
 }


 ////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////

 //ANY ZONE ONE CUBE:

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

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

//LARGE ZONE AUTONOMOUS(ES):
void large_threepointfive(bool isRed) {

  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  a_move_hinge(127);
  delay(600);
  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);

  a_move_lift(-20); ///added 11/29 to prevent bouncing
  a_move_intake(127);

  double ticks = 30;
  a_move_absolute(ticks, 127, 3000);
  //testing//delay(3000);

  //a_move_intake(0);
  delay(300);
  //a_move_intake(127);

  /*
  ticks += 6;
  a_move_absolute(ticks, 50);
  //testing//delay(3000);

  ticks += 6;
  a_move_absolute(ticks, 50);
  //testing//delay(3000);
  /// comment out here

  ticks += 9;
  a_move_absolute(ticks, 50);
  delay(300);

  ticks += 9;
  a_move_absolute(ticks, 50);
  delay(300);

  ticks -= 48;
  a_move_absolute (ticks, 75, 1800);

  ticks += 2;
  a_move_absolute (ticks, 75);


  double temp_ticks = 9.5;
  double RF_ticks = ticks;
  double RR_ticks = ticks;
  double LF_ticks = ticks;
  double LR_ticks = ticks;

  if (isRed) {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1800);
  }
  else {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1800);
  }

  a_move_intake(40);

  temp_ticks = 21.75; ////temp_ticks = 18.25;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  temp_ticks = 6.65; //temp_ticks = 7.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 40);

  scoreTwoToFive(isRed);
  /*
  a_move_hinge(80);
  a_move_intake(-40);
  delay(1700);
  ///comment out here
  delay(300);

  temp_ticks = -12;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);

  a_move_relative(0, 60);
  a_move_hinge(0);
  a_move_intake(0);
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

//SMALL ZONE AUTONOMOUS(ES):
void small_five(bool isRed) {
  a_move_lift(-20);
  a_move_intake(127);

  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  double ticks = 13.5;
  a_move_absolute(ticks, 75);
  delay(400);

  ticks += 5.5;
  a_move_absolute(ticks, 75);
  delay(600);

  ticks += 5.5;
  a_move_absolute(ticks, 75);
  delay(700);

  ticks += 6.5;
  a_move_absolute(ticks, 75);
  delay(600);

  a_move_lift(0);

  ticks -= 30;
  a_move_absolute(ticks, 75, 2000);
  //delay(100);

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

  temp_ticks = 10.75;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1200);
  delay(100);

  temp_ticks = 2.25;
  int temp_ticks2 = 0;

  if (isRed) {
    RF_ticks -= temp_ticks2;
    RR_ticks -= temp_ticks2;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 300);
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks2;
    LR_ticks -= temp_ticks2;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 300);
  }

  score();
  delay(100);

  a_tare_position();
  ticks = -21;
  a_move_relative(ticks, 100);

  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);

}

void small_six(bool isRed) {

  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  double ticks = 0;

  double RF_ticks = ticks;
  double RR_ticks = ticks;
  double LF_ticks = ticks;
  double LR_ticks = ticks;

  /*
  a_move_intake(-127);
  delay(650); //was 850, shortened due to time constraints
  /// comment out

  a_move_hinge(127);
  delay(600);
  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);

  a_move_intake(0);

  a_move_lift(-20);
  a_move_intake(127);


  double temp_ticks = 30;

  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  delay(100);

  temp_ticks = 7;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 45);
  delay(400);

  temp_ticks = 2.5;

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

  temp_ticks = 8;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 55, 1200); //speed 75
  delay(300);

  temp_ticks = -8;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1200);

  temp_ticks = -4;
  if (isRed) {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 4000);
  }
  else {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 4000);
  }


  a_move_lift(0);

  temp_ticks = -40;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2700);

  temp_ticks = 24; //9
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 300);

  temp_ticks = 12.5; //9.25

  if (isRed) {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1700);
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1700);
  }

  temp_ticks = 6.5;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 60, 1200);
  delay(30);

  a_move_intake(0);
  /*
  temp_ticks = 4.875;
  int temp_ticks2 = 0;

  if (isRed) {
    RF_ticks -= temp_ticks2;
    RR_ticks -= temp_ticks2;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 300);
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks2;
    LR_ticks -= temp_ticks2;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 300);
  }
  ///comment out here
  temp_ticks = -2;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 70, 3000);
  delay(30);

  lcd::print(2, "Before stacking. Timelap=%d\n", millis() - start_ts);

  a_move_intake(0);
  //christmasTree(isRed);
  scoreFourToSix(isRed);
  //delay(100);

  a_tare_position();

  a_move_intake(-40);
  a_move_drive(-100, -100);
  delay(300);

  //ticks = -21;
  //a_move_relative(ticks, 100);

  a_move_hinge(-127);
  delay(400);

  a_move_hinge(0);
  a_move_lift(0);
  a_move_drive(0, 0);
  a_move_intake(0);
}

void small_six_diagonal(bool isRed) {

  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  double ticks = 0;

  double RF_ticks = ticks;
  double RR_ticks = ticks;
  double LF_ticks = ticks;
  double LR_ticks = ticks;

  a_move_intake(-120); //
  a_move_hinge(127);
  delay(700); //600
  a_move_hinge(-127);
  delay(500); //400
  a_move_hinge(0);
  a_move_intake(0); //
  delay(200); //delay(400);

  a_move_lift(-20);
  a_move_intake(127);
  delay(200); //delay(400);

  double temp_ticks = 28;

  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  delay(100);

  temp_ticks = 9;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 30); //45
  delay(600); //delay 300

  temp_ticks = 2.5;

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

  temp_ticks = 8;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 55, 1200); //speed 75
  delay(600); //700

  temp_ticks = -8;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1200);

  temp_ticks = -4;
  if (isRed) {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 4000);
  }
  else {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 4000);
  }

  a_move_lift(0);

  temp_ticks = -16;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2000);
  delay(100);

  temp_ticks = 11;

  if (isRed) {
    RF_ticks -= temp_ticks;
    RR_ticks -= temp_ticks;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1700);
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks;
    LR_ticks -= temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1700);
  }

  temp_ticks = 27; //temp_ticks = 21;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 45, 2000); //1800
  delay(30);

  a_move_intake(0);

  a_tare_position();

  temp_ticks = -1.5; //-1.75
  RF_ticks = temp_ticks;
  RR_ticks = temp_ticks;
  LF_ticks = temp_ticks;
  LR_ticks = temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 70, 600);
  delay(30);

  lcd::print(2, "Before stacking. Timelap=%d\n", millis() - start_ts);

  a_move_intake(0);
  scoreFourToSix(isRed);
  a_move_intake(-45); //
  delay(200); //


  a_tare_position();

  a_move_intake(-40);
  a_move_drive(-100, -100);
  a_move_hinge(-127);
  delay(400);

  a_move_lift(0);
  a_move_intake(0);

  lcd::print(3, "After back off. Timelap=%d\n", millis() - start_ts);

  // Drive to position for manual control
  a_tare_position();

  temp_ticks = 10;

  if (isRed) {
    RF_ticks = temp_ticks;
    RR_ticks = temp_ticks;
    LF_ticks = (-1)*temp_ticks;
    LR_ticks = (-1)*temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }
  else {
    RF_ticks = (-1)*temp_ticks;
    RR_ticks = (-1)*temp_ticks;
    LF_ticks = temp_ticks;
    LR_ticks = temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  }

  temp_ticks = -30;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;

  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2000);

  temp_ticks = 5;
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

  a_move_hinge(0);
  a_move_drive(0, 0);
  a_move_intake(0);

  lcd::print(4, "After positioning. Timelap=%d\n", millis() - start_ts);

}

void small_seven(bool isRed) {
  a_set_drive_encoding(E_MOTOR_ENCODER_COUNTS);
  a_tare_position();

  double ticks = 0;

  double RF_ticks = ticks;
  double RR_ticks = ticks;
  double LF_ticks = ticks;
  double LR_ticks = ticks;


  double temp_ticks = 37;

  a_move_intake(-127);
  delay(850);

  a_move_intake(0);

  //vertical_unfold();

  a_move_lift(-20);
  a_move_intake(127);

  temp_ticks = 37;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75);
  delay(400);

  temp_ticks = 2.5;

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

  temp_ticks = 8;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 55, 1200); //speed 75
  delay(600);

  temp_ticks = -4;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1200);

  temp_ticks = -3.5;
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

  //move forward 6
  delay(600);

  a_move_lift(0);

  temp_ticks = -39;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 2700);

  temp_ticks = 1;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 300);

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

  temp_ticks = 9; //9 was just for Richard's sanity; it should be tested at an actual field with 10.75 //temp_ticks = 10.75;
  RF_ticks += temp_ticks;
  RR_ticks += temp_ticks;
  LF_ticks += temp_ticks;
  LR_ticks += temp_ticks;
  a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 1200);
  delay(100);

  temp_ticks = 2.25;
  int temp_ticks2 = 0;

  if (isRed) {
    RF_ticks -= temp_ticks2;
    RR_ticks -= temp_ticks2;
    LF_ticks += temp_ticks;
    LR_ticks += temp_ticks;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 300);
  }
  else {
    RF_ticks += temp_ticks;
    RR_ticks += temp_ticks;
    LF_ticks -= temp_ticks2;
    LR_ticks -= temp_ticks2;
    a_move_absolute(RF_ticks, RR_ticks, LF_ticks, LR_ticks, 75, 300);
  }

  christmasTree(isRed);
  //delay(100);

  a_tare_position();
  ticks = -21;
  a_move_relative(ticks, 100);

  a_move_hinge(-127);
  delay(400);
  a_move_hinge(0);
  a_move_lift(0);

}

void small_eight(bool isRed) {

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//SKILLS PROGRAM(S):
void skillsprogram() {
  //unfold
  //start intake
  //move forward 24
  //turn left 3
  //go back 28
  //copy and paste from small_six here
  //[after stacking]
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void autonomous() {

  redAlliance = false; ///variable
  autonNumber = 6; ///variable
  // 1: Stacking; 2: 6 Cubes Parallel (S); 3: 6 Cubes Diagonal (S); 4: 7 Cubes (S); 5: 8 Cubes (S); 6: 3.5 Cubes (L); 7: Skills

  start_ts = millis();

  if (redAlliance) lcd::print(1, "Red alliance program %d", autonNumber);
  else lcd::print(1, "Blue alliance program %d", autonNumber);

  switch (autonNumber) {
    case 1:
      scoreFourToSix(redAlliance);
      break;
    case 2:
      small_six(redAlliance);
      break;
    case 3:
      small_six_diagonal(redAlliance);
      break;
    case 4:
      small_seven(redAlliance);
      break;
    case 5:
      small_eight(redAlliance);
      break;
    case 6:
      large_threepointfive(redAlliance);
      break;
    case 7:
      skillsprogram();
      break;
    default:
      small_six_diagonal(redAlliance);
      break;
  }

}
*/
