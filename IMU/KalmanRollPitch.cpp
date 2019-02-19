#include <iostream>
#include <Eigen/Dense>



using namespace Eigen;

public test(){

	// Initialize matrices and variables
	// i - integer
	// f - float
	// d - double
	MatrixXd C(2,4);
	C << 1, 0, 0, 0,
		 0, 0, 1, 0;
	Matrix4d P = Matrix<double, 4, 4>::Identity();
	Matrix4d Q = Matrix<double, 4, 4>::Identity();
	Matrix2d R = Matrix<double, 2, 2>::Identity();
	Matrix4d  = Matrix<double, 4, 4>::Identity();

	Vector4d state_estimate;
	state_estimate << 0, 0, 0, 0;

	double phi_hat = 0.0;
	double theta_hat = 0.0;

	double phi_dot = 0.0;
	double theta_dot = 0.0;

	double dt = 0.0;

	// Calculate angles offsets from accelerometer
	// Depends on test to inplement

	while(True){
		// Sampling time


		// Get accelerometer angle measurements
		// [ax, ay, az] = self.get_acc()
		// phi_acc  = math.atan2(ay, math.sqrt(ax ** 2.0 + az ** 2.0))
		// theta_acc  = math.atan2(-ax, math.sqrt(ay ** 2.0 + az ** 2.0))

		// Remove offsets ??


		// Get gyroscope angular velocity and calculate Euler angles derivates
		// wx, wy, wz

		phi_dot = wx + sin(phi_hat) * tan(theta_hat) * wy + cos(phi_hat) * tan(theta_hat) * wz;
		theta_dot = cos(phi_hat) * wy - sin(phi_hat) * wz;

		// Kalman filter
		MatrixXd A(4,4);
		A << 1, -dt, 0,   0,
			 0,   1, 0,   0,
			 0,   0, 1, -dt,
			 0,   0, 0,   1;

		MatrixXd B(4,4);
		B << dt,  0,
			  0,  0,
			  0, dt,
			  0,  0;

		Vector2d gyro_input;
		gyro_input << phi_dot, theta_dot;

		state_estimate = A * state_estimate + B * gyro_input;
		P = A * (P * A.transpose()) + Q;

		Vector2d measurement ;
		measurement << phi_acc, theta_acc;

		y_tilde = measurement - C * state_estimate;
		S = R + C * (P * C.transpose());
		K = P * (C.transpose() * S.inverse());
		state_estimate = state_estimate + K * y_tilde;
		P = (I - K * C) * P;

		phi_hat = state_estimate[0];
		theta_hat = state_estimate[2];
	}
}
