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
    
    //! Converts OVR Matrices to Cinder Matrices
    inline ci::Matrix44f toCinder( const OVR::Matrix4f& ovrMat )
    {
        return ci::Matrix44f(ovrMat.M[0][0], ovrMat.M[0][1], ovrMat.M[0][2], ovrMat.M[0][3],
                         ovrMat.M[1][0], ovrMat.M[1][1], ovrMat.M[1][2], ovrMat.M[1][3],
                         ovrMat.M[2][0], ovrMat.M[2][1], ovrMat.M[2][2], ovrMat.M[2][3],
                         ovrMat.M[3][0], ovrMat.M[3][1], ovrMat.M[3][2], ovrMat.M[3][3] );
    }
    //! Converts OVR Quaternions to Cinder Quaternions
    inline ci::Quatf toCinder( const OVR::Quatf& ovrQuat ){
        return ci::Quatf( ovrQuat.w, ovrQuat.x, ovrQuat.y, ovrQuat.z );
    }
    //! Converts OVR Vector3f to Cinder Vec3f
    inline ci::Vec3f toCinder( const OVR::Vector3f& ovrVec ){
        return ci::Vec3f( ovrVec.x, ovrVec.y, ovrVec.z );
    }
    //! Converts OVR Viewports to Cinder Areas
    inline ci::Area toCinder( const OVR::Util::Render::Viewport& ovrViewport ){
        return ci::Area( ci::Vec2i( ovrViewport.x, ovrViewport.y ), ci::Vec2i( ovrViewport.x + ovrViewport.w, ovrViewport.y + ovrViewport.h ) );
    }
    
    
    // OculusVR Device Class
    typedef std::shared_ptr< class Device > DeviceRef;
    
    class Device
    {
    public:
        // ! Returns an empty ptr if we can't initialize correctly the HMD device
        static DeviceRef create();
        ~Device();
        
        //! Returns the Inter-Pupilary Distance
        float       getIPD() const;
        //! Returns the vertical field of view in degrees
        float       getFov();
        //! Returns the distance to the screen used to compute the near plane.
        float       getEyeToScreenDistance() const;
        //! Returns the value used to offset the Projection Matrices
        float       getProjectionCenterOffset();
        //! Returns the value to fit the distortion to the screen
        float       getDistortionScale();
        //! Returns the 4 values used by the distortion correction
        ci::Vec4f   getDistortionParams() const;
        
        //! Returns Device Orientation
        ci::Quatf   getOrientation();
        
        // not tested...
        ci::Area        getLeftEyeViewport();
        ci::Matrix44f   getLeftEyeViewAdjust();
        ci::Matrix44f   getLeftEyeProjection();
        ci::Matrix44f   getLeftEyeOrthoProjection();
        
        ci::Area        getRightEyeViewport();
        ci::Matrix44f   getRightEyeViewAdjust();
        ci::Matrix44f   getRightEyeProjection();
        ci::Matrix44f   getRightEyeOrthoProjection();
        
    protected:
        Device();
      
        OVR::Ptr<OVR::DeviceManager>    mManager;
        OVR::Ptr<OVR::HMDDevice>        mHMD;
        OVR::HMDInfo                    mHMDInfo;
        std::shared_ptr<OVR::SensorFusion> mSensorFusion;
        OVR::Ptr<OVR::SensorDevice>     mSensorDevice;
        OVR::Util::Render::StereoConfig mStereoConfig;
    };
    
    
    // Distortion Shader Class
    typedef std::shared_ptr<class DistortionHelper> DistortionHelperRef;
    
    class DistortionHelper
    {
    public:
        //! Returns a shared_ptr DistortionHelper
        static DistortionHelperRef create( bool chromaticAbCorrection = true );
        
        //! Returns fullscreen quad with both distorted eyes from a gl::TextureRef
        void render( const ci::gl::TextureRef &texture, const ci::Rectf &rect = ci::Rectf( ci::Vec2f(0,0), ci::Vec2f(1280,800) ) );
        //! Returns fullscreen quad with both distorted eyes from a gl::Texture
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

