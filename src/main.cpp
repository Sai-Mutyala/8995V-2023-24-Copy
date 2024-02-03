//©2024 Team 8995V

/*
TODO

lock intake on double press
bespoke arcade for judge awards
new puncher code
*/

#include "main.h"
#include "lemlib/api.hpp"

//i hate using undocumented code 😭

//motor setup

//drive motors
pros::Motor rfm(19, pros::E_MOTOR_GEARSET_18, false);
pros::Motor rbm(20, pros::E_MOTOR_GEARSET_18, false);
pros::Motor lfm(1, pros::E_MOTOR_GEARSET_18, true);
pros::Motor lbm(2, pros::E_MOTOR_GEARSET_18, true);
//intake motor
pros::Motor intake(3, pros::E_MOTOR_GEARSET_18, false);
//cata motors here

//pneumatics
pros::ADIDigitalOut pneum(1);//1-8 = "A"-"H"

//controller
pros::Controller master (CONTROLLER_MASTER);

//inertial
pros::Imu imu(11);

//motor group setup
pros::MotorGroup rsm({rfm, rbm});
pros::MotorGroup lsm({lfm, lbm});

//drivetrain setup
//why do you make me use doubles 😢
lemlib::Drivetrain drivetrain {
	&lsm, //left motors
	&rsm, //right motors
	12.0, //track width
	3.25, //wheel diam
	333.0, //rpm (60-36 gear ratio & green motor)
	0.0 //chase power: idk what this does, but it doesn't work wihout it.
};

//odometry sensor setup
lemlib::OdomSensors odomSensors {
	nullptr, //vertical tracking wheel 1
	nullptr, //vertical tracking wheel 2
	nullptr, //horizontal tracking wheel 1
	nullptr, //horizontal tracking wheel 2
	&imu //intertial sensor
};

//linear controller (odom)
lemlib::ControllerSettings linearController {
	8.0, //kP
	1.0, //kI ???
	30.0, //kD
	5.0, //anti-windup (idk what this is or what to make it)
	1.0, //small error
	100.0, //small error timeout
	3.0, //large error
	500.0, //large error timeout
	0.0 //slew rate ???
};

//angular controller (odom)
lemlib::ControllerSettings angularController {
	2.0, //kP
	0.0, //kI ???
	10.0, //kD
	3.0, //anti-windup (idk what this is or what to make it)
	1.0, //small error
	100.0, //small error timeout
	3.0, //large error
	500.0, //large error timeout
	0.0 //slew rate ???
};

//chassis setup (finally)
//bruh it doesnt let me name it "chassis" even though thats what it says in docs
lemlib::Chassis chassi(drivetrain, linearController, angularController, odomSensors);

//brain screen
void screen(){
	while (true){
		//print current position info to brain
		//x and y don't work because inertial only works for turning
		lemlib::Pose pose = chassi.getPose();
		pros::lcd::print(0, "x: %f", pose.x);
		pros::lcd::print(1, "y: %f", pose.y);
		pros::lcd::print(2, "heading: %f", pose.theta);
		pros::lcd::print(3, "you're a bit skibidi");
		pros::delay(10);
	}
}

//initialize
void initialize(){
	pros::lcd::initialize();
	chassi.calibrate();
	chassi.setPose(0,0,0);
	pros::Task screenTask(screen);
}

//auton
void autonomous() {
	pros::lcd::print(4, "peoo");
    chassi.turnTo(30,0,1000);
}

//driver control
bool pneumOut = false;

void opcontrol(){
	while (true){
		//drive (mabe change later)
		chassi.arcade(master.get_analog(ANALOG_LEFT_Y), master.get_analog(ANALOG_LEFT_X), 2.7);
		//intake
		if (master.get_digital(DIGITAL_L1)){
			intake.move_velocity(100);
		} else if (master.get_digital(DIGITAL_L2)) {
			intake.move_velocity(-100);
		} else {
			intake.brake();
		}
		//pneumatics
		if (master.get_digital_new_press(DIGITAL_X)){
			pneumOut = !pneumOut;
			pneum.set_value(pneumOut);
		}
		pros::delay(10);
	}
}