//
//  OculusVR.h
//  OculusSDKTest
//
//  Created by Simon Geilfus on 29/05/13.
//
//

#pragma once


#include "OVR.h"


namespace ovr {
    
    // Convertion utilities
    inline ci::Matrix44f toCinder( const OVR::Matrix4f& ovrMat )
    {
        return ci::Matrix44f(ovrMat.M[0][0], ovrMat.M[0][1], ovrMat.M[0][2], ovrMat.M[0][3],
                         ovrMat.M[1][0], ovrMat.M[1][1], ovrMat.M[1][2], ovrMat.M[1][3],
                         ovrMat.M[2][0], ovrMat.M[2][1], ovrMat.M[2][2], ovrMat.M[2][3],
                         ovrMat.M[3][0], ovrMat.M[3][1], ovrMat.M[3][2], ovrMat.M[3][3] );
    }
    inline ci::Quatf toCinder( const OVR::Quatf& ovrQuat ){
        return ci::Quatf( ovrQuat.w, ovrQuat.x, ovrQuat.y, ovrQuat.z );
    }
    inline ci::Vec3f toCinder( const OVR::Vector3f& ovrVec ){
        return ci::Vec3f( ovrVec.x, ovrVec.y, ovrVec.z );
    }
    
    
    // OculusVR Device Class
    typedef std::shared_ptr< class Device > DeviceRef;
    
    class Device
    {
    public:
        static DeviceRef create();
        ~Device();
        
        float       getIPD() const;
        float       getProjectionCenterOffset();
        float       getDistortionScale();
        ci::Vec4f   getDistortionParams() const;
        
        ci::Quatf   getOrientation();
        
    protected:
        Device( bool autoCalibrate = true );
        
        void updateAutoCalibration();
        
        OVR::Ptr<OVR::DeviceManager>    mManager;
        OVR::Ptr<OVR::HMDDevice>        mHMD;
        OVR::HMDInfo                    mHMDInfo;
        OVR::SensorFusion               mSensorFusion;
        OVR::Ptr<OVR::SensorDevice>     mSensorDevice;
        OVR::Util::MagCalibration       mMagCalibration;
        OVR::Util::Render::StereoConfig mStereoConfig;
        
        bool                            mIsAutoCalibrating;
        std::thread                     mAutoCalibrationThread;
    };
};

