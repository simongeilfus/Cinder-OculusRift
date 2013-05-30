//
//  OculusVR.h
//  OculusSDKTest
//
//  Created by Simon Geilfus on 29/05/13.
//
//

#pragma once


#include "OVR.h"

#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"


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
    inline ci::Area toCinder( const OVR::Util::Render::Viewport& ovrViewport ){
        return ci::Area( ci::Vec2i( ovrViewport.x, ovrViewport.y ), ci::Vec2i( ovrViewport.x + ovrViewport.w, ovrViewport.y + ovrViewport.h ) );
    }
    
    
    // OculusVR Device Class
    typedef std::shared_ptr< class Device > DeviceRef;
    
    class Device
    {
    public:
        // Returns an empty ptr if we can't initialize correctly the HMD device
        static DeviceRef create();
        ~Device();
        
        float       getIPD() const;
        float       getFov();
        float       getEyeToScreenDistance() const;
        float       getProjectionCenterOffset();
        float       getDistortionScale();
        ci::Vec4f   getDistortionParams() const;
        
        ci::Quatf   getOrientation();
        
        // 
        ci::Area        getLeftEyeViewport();
        ci::Matrix44f   getLeftEyeViewAdjust();
        ci::Matrix44f   getLeftEyeProjection();
        ci::Matrix44f   getLeftEyeOrthoProjection();
        
        ci::Area        getRightEyeViewport();
        ci::Matrix44f   getRightEyeViewAdjust();
        ci::Matrix44f   getRightEyeProjection();
        ci::Matrix44f   getRightEyeOrthoProjection();
        
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
    
    
    // Distortion Shader Class
    typedef std::shared_ptr<class DistortionHelper> DistortionHelperRef;
    
    class DistortionHelper
    {
    public:
        static DistortionHelperRef create( bool chromaticAbCorrection = true );
        
        void render( const ci::gl::TextureRef &texture, const ci::Rectf &rect = ci::Rectf( ci::Vec2f(0,0), ci::Vec2f(1280,800) ) );
        void render( const ci::gl::Texture &texture, const ci::Rectf &rect = ci::Rectf( ci::Vec2f(0,0), ci::Vec2f(1280,800) )  );
        
    protected:
        DistortionHelper( bool chromaticAbCorrection = true );
        
        ci::Vec4f           mDistortionParams;
        float               mDistortionScale;
        
        bool                mUseChromaticAbCorrection;
        ci::Vec4f           mChromaticAbCorrection;
        ci::gl::GlslProgRef mShader;
    };
};

