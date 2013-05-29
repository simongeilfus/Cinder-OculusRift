//
//  OculusVR.cpp
//  OculusSDKTest
//
//  Created by Simon Geilfus on 29/05/13.
//
//

#include "OculusVR.h"

#include "cinder/Utilities.h"

namespace ovr {
    
    
    DeviceRef Device::create()
    {
        return DeviceRef( new Device() );
    }
    Device::~Device()
    {
        mIsAutoCalibrating = false;
        mAutoCalibrationThread.join();
        
        mSensorDevice.Clear();
        mHMD.Clear();
        mManager.Clear();
        
        OVR::System::Destroy();
    }
    
    Device::Device( bool autoCalibrate )
    {
        
        // Init OVR
        OVR::System::Init( OVR::Log::ConfigureDefaultLog( OVR::LogMask_All ) );
        
        // Create Manager and Device Handle
        mManager = *OVR::DeviceManager::Create();
        mHMD     = *mManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
        
        // If everything's fine attach the device, setup the stereo and start callibration
        if( mHMD ){
            
            if (mHMD->GetDeviceInfo(&mHMDInfo))
            {
                mStereoConfig.SetHMDInfo( mHMDInfo );
            }
            
            mSensorDevice = *mHMD->GetSensor();
            
            if (mSensorDevice)
                mSensorFusion.AttachToSensor(mSensorDevice);
            
            if( autoCalibrate ){
                mIsAutoCalibrating = true;
                mMagCalibration.BeginAutoCalibration( mSensorFusion );
                mAutoCalibrationThread = std::thread( &Device::updateAutoCalibration, this );
            }
            else mIsAutoCalibrating = false;
        }
    }
    
    
    float Device::getIPD() const
    {
        return mStereoConfig.GetIPD();
    }
    float Device::getProjectionCenterOffset()
    {
        return mStereoConfig.GetProjectionCenterOffset();
    }
    float Device::getDistortionScale()
    {
        return mStereoConfig.GetDistortionScale();
    }
    ci::Vec4f Device::getDistortionParams() const
    {
        return ci::Vec4f( mHMDInfo.DistortionK[0], mHMDInfo.DistortionK[1], mHMDInfo.DistortionK[2], mHMDInfo.DistortionK[3] );
    }
    
    ci::Quatf Device::getOrientation()
    {
        return toCinder( mSensorFusion.GetOrientation() );
    }
    
    void Device::updateAutoCalibration()
    {
        while ( mIsAutoCalibrating )
        {
            mMagCalibration.UpdateAutoCalibration( mSensorFusion );
            if ( mMagCalibration.IsCalibrated() )
            {
                if ( mSensorFusion.IsMagReady() )
                    mSensorFusion.SetYawCorrectionEnabled(true);
                OVR::Vector3f mc = mMagCalibration.GetMagCenter();
                std::cout << "   Magnetometer Calibration Complete" << std::endl << "Center: " << mc.x << " " << mc.y << " " << mc.z << std::endl;
                
                mIsAutoCalibrating = false;
            }
            else if( !mMagCalibration.IsAutoCalibrating() ){
                mIsAutoCalibrating = false;
            }
            
            sleep( 1 );
        }
    }
}