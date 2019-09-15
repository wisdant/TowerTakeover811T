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

void autonomous() {
/*  // Create a chassis to use for autonomous (this has built in autonomous control)
  // The last two numbers are used to tune turn and move amounts
  auto chassis = ChassisControllerFactory::create(
    -RIGHT_DRIVE_PORT, LEFT_DRIVE_PORT,
    AbstractMotor::gearset::green,
    {4_in, 12.6_in});

    //chassis.setMaxVelocity(70);

    // Pick up and drop Block 0
    move_claw(127);
    delay(150);
    move_claw(25);

    move_lift(127);
    delay(200);
    move_lift(10);

    chassis.moveDistance(20_in);

    move_claw(-127);
    delay(150);

    chassis.moveDistance(-30_in);
 */
}
