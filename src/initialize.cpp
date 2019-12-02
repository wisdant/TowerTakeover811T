#include "main.h"
using namespace pros;

int autonNumber = 0;
bool redAlliance = true;

// Define the main controller of the robot.
Controller master(E_CONTROLLER_MASTER);

// Define the robot's motors with their respective ports.
// Passing true for the second parameter reverses the motor.
Motor left_drive (LEFT_FRONT_PORT, false);
Motor left_rear_drive (LEFT_REAR_PORT, false);

Motor right_drive (RIGHT_FRONT_PORT, true);
Motor right_rear_drive (RIGHT_REAR_PORT, true);

Motor left_intake (LEFT_INTAKE_PORT, E_MOTOR_GEARSET_36, false);
Motor right_intake (RIGHT_INTAKE_PORT, E_MOTOR_GEARSET_36, true);

Motor lift (LIFT_PORT, E_MOTOR_GEARSET_36, true);
Motor hinge (HINGE_PORT, E_MOTOR_GEARSET_36, true);

Vision vision_sensor (VISION_PORT);

static const char *alliance_map[] = {"Red", "Blue", ""};
static const char *btnm_map[] = {"1CUBE", "2CUBES", "SMALLZONE", "SKILLS", ""};
static const char *auton_strings[] = {"1CUBE", "2CUBES", "SMALLZONE", "SKILLS"};

static lv_res_t btnm_action(lv_obj_t *btnm, const char *txt)
{
   for (int i = 0; i < sizeof(auton_strings) / sizeof(auton_strings[0]); i++)
   {
      printf("%s\n", auton_strings[i]);
      printf("%s\n", txt);
      printf("------------\n");
      if (strcmp(auton_strings[i], txt) == 0)
      {
         autonNumber = i + 1;
         break;
      }
      lv_btnm_set_toggle(btnm, true, autonNumber);
   }

   return LV_RES_OK; /*Return OK because the button matrix is not deleted*/
}

static lv_res_t btnm_action_color(lv_obj_t *btnm, const char *txt)
{
   lv_btnm_set_toggle(btnm, true, 1);
   lv_btnm_set_toggle(btnm, true, 2);
   printf("FUNCTION CALLED");
   if (strcmp(txt, "Red") == 0)
   {
      redAlliance = true;
   }
   else if (strcmp(txt, "Blue") == 1)
   {
      redAlliance = false;
   }

   return LV_RES_OK; /*Return OK because the button matrix is not deleted*/
}

void initialize() {
	lcd::print(6, "10/29 21:22 In initialize()");
	printf("In initialize()\n");
	lv_theme_alien_init(40, NULL);

	lv_obj_t *title = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(title, "Auton Selection");
	lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

	lv_obj_t *btnm = lv_btnm_create(lv_scr_act(), NULL);
	lv_btnm_set_map(btnm, btnm_map);
	lv_btnm_set_action(btnm, btnm_action);
	lv_obj_set_size(btnm, LV_HOR_RES - 40, LV_VER_RES / 3);
	lv_obj_align(btnm, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

	lv_obj_t *allianceM = lv_btnm_create(lv_scr_act(), NULL);
	lv_btnm_set_map(allianceM, alliance_map);
	lv_btnm_set_action(allianceM, btnm_action_color);
	lv_obj_set_size(allianceM, LV_HOR_RES - 40, 50);
	lv_obj_align(allianceM, btnm, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

}

void initializeDriveMotors(){
}

void disabled() {}

void competition_initialize() {

}
