#ifndef OV_CORE_INERTIALINITIALIZER_H
#define OV_CORE_INERTIALINITIALIZER_H


#include <ros/ros.h>
#include <Eigen/Eigen>
#include "utils/quat_ops.h"

/**
 * @namespace ov_core
 * @brief Core algorithms for Open VINS
 */
namespace ov_core {



    /**
     * @brief Initializer for visual-inertial system.
     *
     * This class has a series of functions that can be used to initialize your system.
     * Right now we have our implementation that assumes that the imu starts from standing still.
     * In the future we plan to add support for structure-from-motion dynamic initialization.
     */
    class InertialInitializer {

    public:

        /**
         * @brief Struct for a single imu measurement (time, wm, am)
         */
        struct IMUDATA {

            /// Timestamp of the reading
            double timestamp;

            /// Gyroscope reading, angular velocity (rad/s)
            Eigen::Matrix<double, 3, 1> wm;

            /// Accelerometer reading, linear acceleration (m/s^2)
            Eigen::Matrix<double, 3, 1> am;

        };


        /**
         * @brief Default constructor
         * @param gravity Gravity in the global frame of reference
         * @param window_length Amount of time we will initialize over (seconds)
         * @param imu_excite_threshold Variance threshold on our acceleration to be classified as moving
         */
        InertialInitializer(Eigen::Matrix<double,3,1> gravity, double window_length, double imu_excite_threshold) :
                            _gravity(gravity), _window_length(window_length), _imu_excite_threshold(imu_excite_threshold) {}


        /**
         * @brief Stores incoming inertial readings
         *
         * @param timestamp Timestamp of imu reading
         * @param wm Gyro angular velocity reading
         * @param am Accelerometer linear acceleration reading
         */
        void feed_imu(double timestamp, Eigen::Matrix<double,3,1> wm, Eigen::Matrix<double,3,1> am);


        /**
         * @brief Try to initialize the system using just the imu
         *
         * This will check if we have had a large enough jump in our acceleration.
         * If we have then we will use the period of time before this jump to initialize the state.
         * This assumes that our imu is sitting still and is not moving (so this would fail if we are experiencing constant acceleration).
         *
         * @param[out] time0 Timestamp that the returned state is at
         * @param[out] q_GtoI0 Orientation at initialization
         * @param[out] b_w0 Gyro bias at initialization
         * @param[out] v_I0inG Velocity at initialization
         * @param[out] b_a0 Acceleration bias at initialization
         * @param[out] p_I0inG Position at initialization
         * @return True if we have successfully initialized our system
         */
        bool initialize_with_imu(double &time0, Eigen::Matrix<double,4,1> &q_GtoI0, Eigen::Matrix<double,3,1> &b_w0,
                                 Eigen::Matrix<double,3,1> &v_I0inG, Eigen::Matrix<double,3,1> &b_a0, Eigen::Matrix<double,3,1> &p_I0inG);



    protected:


        /// Gravity vector
        Eigen::Matrix<double,3,1> _gravity;

        /// Amount of time we will initialize over (seconds)
        double _window_length;

        /// Variance threshold on our acceleration to be classified as moving
        double _imu_excite_threshold;

        /// Our history of IMU messages (time, angular, linear)
        std::vector<IMUDATA> imu_data;


    };


}

#endif //OV_CORE_INERTIALINITIALIZER_H
