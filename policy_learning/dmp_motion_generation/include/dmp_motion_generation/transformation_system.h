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

 \file    dmp_transformation_system.cpp

 \author  Peter Pastor
 \date    May 29, 2009

 **********************************************************************/

#ifndef TRANSFORMATION_SYSTEM_H_
#define TRANSFORMATION_SYSTEM_H_

// system includes
#include <string>
#include <vector>
#include <algorithm>

// ros includes
#include <lwr/lwr.h>
#include <lwr/parameters.h>

#include <Eigen/Eigen>

// local includes
#include <dmp_motion_generation/dynamic_movement_primitive.h>
#include <dmp_motion_generation/TransformationSystem.h>

namespace dmp
{

class TransformationSystem
{

public:

    friend class DynamicMovementPrimitive;

    /*!
     */
    TransformationSystem();

    /*!
     */
    ~TransformationSystem();

    /*!
     *
     * @param index
     * @param directory_name
     * @param lwr_params
     * @return
     */
    bool initialize(std::string library_directory_name, int dmp_id, int trans_id, lwr::Parameters lwr_params);

    /*!
     *
     * @param directory_name
     * @return
     */
    bool writeToDisc(std::string directory_name);

    /*!
     *
     * @param item_name
     * @param item_id
     * @return
     */
    bool readFromDisc(std::string item_name, int item_id);

    /*!
     *
     * @param transformation_system
     * @return
     */
    bool initFromMessage(const dmp_motion_generation::TransformationSystem& transformation_system);

    /*!
     *
     * @param transformation_system
     * @return
     */
    bool writeToMessage(dmp_motion_generation::TransformationSystem& transformation_system);

    /*!
     */
    void print();

    /*!
     * @return
     */
    std::string getInfoString();

private:

    /*! flag that indicates whether the system is initialized
     *
     */
    bool initialized_;

    /*! the id of the transformation system
     *
     */
    int trans_id_;

    /*! determins which DMP version is used. (not used yet)
     *
     */
    int version_id_;

    /*!
     */
    void reset();

    /*!
     */
    void setState(const double y, const double z);
    double getState() const;

    /*!
     */
    void setStart(const double y0);
    void setGoal(const double goal);
    double getGoal() const;

    /*!
     */
    void setInitialStart(const double initial_y0);
    void setInitialGoal(const double initial_goal);

    /*!
     */
    void resetMSE();
    bool getMSE(double& mse);
    bool getNormalizedMSE(double& normalized_mse);
    void computeMSE();

    /*! internal states
     *
     */
    double z_; //vel
    double zd_; //acc

    /*! external states
     *
     *
     */
    double y_; //pos
    double yd_; //vel
    double ydd_; //acc

    /*! the targets used during supervised learning
     *
     */
    double t_;
    double td_;
    double tdd_;

    /*! the start state
     *
     */
    double y0_;
    double initial_y0_;

    /*! the goal state
     *
     */
    double goal_;
    double initial_goal_;

    /*! the current values of the nonlinear function
     *
     */
    double f_;
    double ft_;

//    double scale_;
//    double delta_goal_fit_;
//    double s_;
//    double sd_;

    /*! internal variables that are used to compute the normalized mean squared
     *  error during learning
     */
    double mse_;
    double mean_ft_;
    unsigned int num_mse_data_points_;

    /*! internal variable that is used to store the target function for LWR
     *
     */
    std::vector<double> trajectory_target_;

    /*! lwr model used to approximate the nonlinear function
     *
     */
    lwr::LocallyWeightedRegression lwr_model_;

};

// inline function follow
inline void TransformationSystem::reset()
{
    z_ = 0.0;
    zd_ = 0.0;
    y_ = 0.0;
    yd_ = 0.0;
    ydd_ = 0.0;

    // scale_ = 1.0;
}
inline void TransformationSystem::setState(const double y, const double z)
{
    y_ = y;
    z_ = z;

    // s_ = y_;
    // sd_ = 0.0;
}
inline double TransformationSystem::getState() const
{
    return y_;
}
inline void TransformationSystem::setStart(const double y0)
{
    y0_ = y0;
}
inline void TransformationSystem::setGoal(const double goal)
{
    goal_ = goal;
}
inline double TransformationSystem::getGoal() const
{
    return goal_;
}
inline void TransformationSystem::setInitialStart(const double initial_y0)
{
    initial_y0_ = initial_y0;
}
inline void TransformationSystem::setInitialGoal(const double initial_goal)
{
    initial_goal_ = initial_goal;
}
inline void TransformationSystem::resetMSE()
{
    mse_ = 0.0;
    mean_ft_ = 0.0;
    num_mse_data_points_ = 0;
}
inline bool TransformationSystem::getMSE(double& mse)
{
    if (num_mse_data_points_ == 0)
    {
        ROS_ERROR("No data point seen yet, cannot compute mean squared error.");
        return false;
    }
    mse = mse_ / static_cast<double> (num_mse_data_points_);
    return true;
}
inline bool TransformationSystem::getNormalizedMSE(double& normalized_mse)
{
    if (num_mse_data_points_ == 0)
    {
        ROS_ERROR("No data point seen yet, cannot compute normalized mean squared error.");
        return false;
    }

    normalized_mse = (static_cast<double> (1.0) / std::max(pow(mean_ft_, 2), 1.0)) * (mse_ / static_cast<double> (num_mse_data_points_));
    return true;
}
inline void TransformationSystem::computeMSE()
{
    mse_ += pow(ft_ - f_, 2);
    mean_ft_ = (static_cast<double> (num_mse_data_points_) * mean_ft_ + ft_) / static_cast<double> (num_mse_data_points_ + 1);
    num_mse_data_points_++;
}

}

#endif /* TRANSFORMATION_SYSTEM_H_ */
