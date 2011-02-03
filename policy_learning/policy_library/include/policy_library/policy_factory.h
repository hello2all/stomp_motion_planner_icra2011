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

/** \author Mrinal Kalakrishnan */

#ifndef POLICY_FACTORY_H_
#define POLICY_FACTORY_H_

#include <boost/shared_ptr.hpp>

#include <policy_library/policy.h>
#include <dmp_motion_generation/dynamic_movement_primitive.h>

namespace library
{

class PolicyFactory
{
public:
    PolicyFactory(ros::NodeHandle& node_handle);
    virtual ~PolicyFactory();

    /**
     * Instantiates the required Policy sub-class object given the class name
     *
     * @param class_name Name of the Policy sub-class to be instantiated
     * @param policy Pointer to the newly instantiated object
     *
     * @return true on success, false on failure
     */
     bool createPolicyInstanceByName(const std::string& class_name, boost::shared_ptr<LibraryItem>& policy);

     // TODO: UGLY HACK!!!!!! DMPs should eventually not be stored directly in a policy_library
     bool createPolicyInstanceByName(const std::string& class_name, boost::shared_ptr<dmp::DynamicMovementPrimitive>& policy);

private:

     /*!
      */
     ros::NodeHandle node_handle_;

};

}

#endif /* POLICY_FACTORY_H_ */
