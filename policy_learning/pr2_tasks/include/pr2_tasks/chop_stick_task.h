/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2010, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/** \author Peter Pastor */

#ifndef CHOP_STICK_TASK_H_
#define CHOP_STICK_TASK_H_

// system includes
#include <iostream>
#include <fstream>

// ros includes
#include <sensor_msgs/JointState.h>
#include <pr2_msgs/AccelerometerState.h>
#include <slipgrip_controller/PR2GripperSensorData.h>
#include <sensor_msgs/Imu.h>

#include <task_recorder/task_recorder_client.h>

// #include <task_recorder/StopRecordingTactileStates.h>
#include <task_recorder/StopRecordingJointStates.h>
#include <task_recorder/StopRecordingAccelerometerStates.h>
#include <task_recorder/StopRecordingImuStates.h>

#include <task_manager/task.h>
#include <task_manager/controller_switcher.h>

#include <pr2_tasks_transforms/task_transforms.h>

#include <policy_library/dmp_policy.h>
#include <policy_library/single_parameter_policy.h>
#include <policy_library/mixed_policy.h>

#include <dmp_motion_controller/DMPExecutionStatistics.h>
#include <kdl/chainfksolvervel_recursive.hpp>
#include <policy_improvement_utilities/kdl_chain_wrapper.h>

#include <kdl/frames.hpp>

// local includes
#include <pr2_tasks/dmp_executor.h>

namespace pr2_tasks
{

class ChopStickTask : public task_manager_interface::Task
{
public:
    ChopStickTask();
    virtual ~ChopStickTask();

    /**
     * Initializes the task for a given number of time steps
     * @param num_time_steps
     * @return
     */
    bool initialize(ros::NodeHandle& node_handle, int num_time_steps);

    /**
     * Executes the task for the given policy parameters, and returns the costs per timestep
     * @param parameters [num_dimensions] num_parameters - policy parameters to execute
     * @param costs Vector of num_time_steps, state space cost per timestep (do not include control costs)
     * @return
     */
    bool execute(std::vector<Eigen::VectorXd>& parameters, Eigen::VectorXd& costs, const int iteration_number = 0);

    /**
     * Get the Policy object of this Task
     * @param policy
     * @return
     */
    bool getPolicy(boost::shared_ptr<library::Policy>& policy);

    /**
     * Sets the Policy object of this Task
     * @param policy
     * @return
     */
    bool setPolicy(const boost::shared_ptr<library::Policy> policy);

    /**
     * Gets the weight of the control cost
     * @param control_cost_weight
     * @return
     */
    bool getControlCostWeight(double& control_cost_weight);

private:

    bool preExecute();
    bool postExecute();

    bool execute();
    bool taskInitialize();

    bool computeCosts(Eigen::VectorXd& costs);

    std::string demonstration_bag_file_;

    Eigen::VectorXd dual_arm_dmp_start_;
    Eigen::VectorXd dual_arm_dmp_goal_;

    Eigen::VectorXd dual_arm_joint_reset_dmp_start_;
    Eigen::VectorXd dual_arm_joint_reset_dmp_goal_;

    int num_dmp_dimensions_;

    std::string root_frame_;
    std::string left_arm_tip_frame_;
    std::string right_arm_tip_frame_;

    policy_improvement_utilities::KDLChainWrapper right_arm_kdl_chain_wrapper_;
    policy_improvement_utilities::KDLChainWrapper left_arm_kdl_chain_wrapper_;

    task_recorder::TaskRecorderClient<sensor_msgs::JointState, task_recorder::StopRecordingJointStates> joint_states_;
    task_recorder::TaskRecorderClient<pr2_msgs::AccelerometerState, task_recorder::StopRecordingAccelerometerStates> r_gripper_accelerometer_states_;
    task_recorder::TaskRecorderClient<pr2_msgs::AccelerometerState, task_recorder::StopRecordingAccelerometerStates> l_gripper_accelerometer_states_;
    task_recorder::TaskRecorderClient<sensor_msgs::Imu, task_recorder::StopRecordingImuStates> imu_states_;
    // task_recorder::TaskRecorderClient<slipgrip_controller::PR2GripperSensorData, task_recorder::StopRecordingTactileStates> r_gripper_tactile_states_;
    // task_recorder::TaskRecorderClient<slipgrip_controller::PR2GripperSensorData, task_recorder::StopRecordingTactileStates> l_gripper_tactile_states_;

    bool initialized_;
    int iteration_number_;

    ros::NodeHandle node_handle_;

    int num_time_steps_;            /**< Number of time steps for RL / movement duration only */
    int num_time_steps_total_;      /**< Number of time steps for the entire movement (execution duration) */

    double task_movement_duration_;
    double task_execution_duration_;
    double reset_movement_duration_;
    double reset_execution_duration_;
    double dmp_sampling_frequency_;
    double dmp_dt_;

    int task_dmp_id_;
    int reset_dmp_id_;

    DMPExecutor dmp_executor_;
    dmp_motion_controller::DMPExecutionStatistics execution_statistics_;

    ros::Duration task_execution_duration_ros_;
    ros::Duration task_execution_timeout_duration_ros_;
    ros::Duration reset_execution_duration_ros_;
    ros::Duration reset_execution_timeout_duration_ros_;

    boost::shared_ptr<library::DMPPolicy> dmp_policy_;
    boost::shared_ptr<dmp::DynamicMovementPrimitive> dmp_;
    boost::shared_ptr<dmp::DynamicMovementPrimitive> reset_dmp_;

    bool is_ready_;

    bool reset();
    bool readParams();

    bool initializeDMPFromFile();

    bool learnTaskDMPFromBagFile();
    ros::ServiceClient learn_dmp_service_client_;
    bool learnResetDMP();

    // costs
    Eigen::VectorXd l_gripper_acceleration_costs_;
    Eigen::VectorXd r_gripper_acceleration_costs_;
    Eigen::VectorXd l_arm_effort_costs_;
    Eigen::VectorXd r_arm_effort_costs_;
    Eigen::VectorXd l_gripper_pressure_costs_;
    Eigen::VectorXd r_gripper_pressure_costs_;
    Eigen::VectorXd imu_costs_;

    // weights
    double control_cost_weight_;
    double l_gripper_acceleration_weight_;
    double r_gripper_acceleration_weight_;
    double l_arm_effort_weight_;
    double r_arm_effort_weight_;
    double l_gripper_pressure_weight_;
    double r_gripper_pressure_weight_;
    double imu_weight_;

    // std::vector<std::string> joint_names_;

    std::vector<std::string> l_arm_effort_cost_joint_names_;
    std::vector<std::string> r_arm_effort_cost_joint_names_;

    bool initialize_dmp_from_file_;
    std::string initial_dmp_file_name_;

    bool computeGripperAccelerationCost();
    bool computeEffortCost();
    bool computePressureCost();
    bool computeImuCost();

    void publishCosts(const Eigen::VectorXd& costs);
    void publishText(const std::string& text, const Eigen::Vector3d& color, const int row_index);

    ros::Publisher marker_publisher_;

    std::ofstream cost_log_file_;
    int local_iteration_number_;
};

}

#endif /* CHOP_STICK_TASK_H_ */
