/*********************************************************************
 * Copyright (c) 2008, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.

  \file    gripper_accelerometer_recorder.h

  \author  Peter Pastor
  \date    Jul 19, 2010

**********************************************************************/

#ifndef ACCELEROMETER_STATES_RECORDER_H_
#define ACCELEROMETER_STATES_RECORDER_H_

// system includes
#include <vector>

// ros includes
#include <pr2_msgs/AccelerometerState.h>

#include <tf/transform_listener.h>

#include <filters/transfer_function.h>

// local includes
#include <task_recorder/task_recorder.h>
#include <task_recorder/accumulator.h>

#include <task_recorder/StartRecording.h>
#include <task_recorder/StopRecordingAccelerometerStates.h>

namespace task_recorder
{

class AccelerometerStatesRecorder : public TaskRecorder<pr2_msgs::AccelerometerState, task_recorder::StopRecordingAccelerometerStates>
{

public:

    /*!
     * @return
     */
    AccelerometerStatesRecorder();
    virtual ~AccelerometerStatesRecorder();

    /*!
     * @param node_handle
     * @param topic_name
     * @return
     */
    bool initialize(ros::NodeHandle& node_handle, const std::string& topic_name);

    /*!
     * @param start_time
     * @param end_time
     * @param num_samples
     * @param accelerometer_states
     * @return
     */
    bool filterAndCrop(const ros::Time& start_time, const ros::Time& end_time, const double movement_duration, int num_samples,
                       std::vector<pr2_msgs::AccelerometerState>& accelerometer_states, std::vector<std::string>& message_names,
                       std::vector<ros::Time>& times, std::vector<double>& data);

    /*!
     * @param accelerometer_state
     * @return
     */
    bool transformMessages(pr2_msgs::AccelerometerState& accelerometer_state);

    /*!
     * @param trial_statistics
     * @return
     */
    bool getAccumulatedTrialStatistics(std::vector<std::vector<task_recorder::AccumulatedTrialStatistics> >& vector_of_accumulated_trial_statistics);

private:

    bool resample(std::vector<pr2_msgs::AccelerometerState>& accelerometer_states, const ros::Time& start_time, const ros::Time& end_time, const int num_samples,
                  std::vector<pr2_msgs::AccelerometerState>& resampled_accelerometer_states);

    bool average(const pr2_msgs::AccelerometerState& accelerometer_state, geometry_msgs::Vector3Stamped& vector_stamped);

    bool findAndSetPeakTimes(const ros::Time& start_time, const ros::Time& end_time, const double movement_duration, ros::Time& first_peak_time, ros::Time& second_peak_time);

    bool highPassFilter(geometry_msgs::Vector3Stamped& vector_stamped);

    bool fillAccelerometerStateMsg(const geometry_msgs::Vector3Stamped& vector_stamped, pr2_msgs::AccelerometerState& accelerometer_state);

    void msgCallback(boost::shared_ptr<geometry_msgs::Vector3Stamped>& vector);

    void setMessageNames(std::vector<task_recorder::AccumulatedTrialStatistics>& trial_statistics);

    void getSignalNames(const int signal_index, std::string& signal_name);

    tf::TransformListener listener_;

    filters::MultiChannelTransferFunctionFilter<double> filter_;
    std::vector<double> unfiltered_data_;
    std::vector<double> filtered_data_;

    Accumulator accumulator_;

};

}

#endif /* ACCELEROMETER_STATES_RECORDER_H_ */
