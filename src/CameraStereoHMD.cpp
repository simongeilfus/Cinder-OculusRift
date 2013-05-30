//
//  CameraStereoHMD.cpp
//  OculusSDKTest
//
//  Created by Simon Geilfus on 26/05/13.
//
//

#include "CameraStereoHMD.h"

using namespace ci;


CameraStereoHMD::CameraStereoHMD()
: ci::CameraStereo()
, mProjectionCenterOffset( 0.151976f )
{
    setEyeSeparation( 0.64f );//0.00119808f );
    setConvergence(0);
    setFov( 125.871f );
}

CameraStereoHMD::CameraStereoHMD( int pixelWidth, int pixelHeight, float fov )
: CameraStereo( pixelWidth, pixelHeight, fov )
, mProjectionCenterOffset( 0.151976f )
{
    setEyeSeparation( 0.64f );//0.00119808f );
    setConvergence(0);
}

CameraStereoHMD::CameraStereoHMD( int pixelWidth, int pixelHeight, float fov, float nearPlane, float farPlane )
: CameraStereo( pixelWidth, pixelHeight, fov, nearPlane, farPlane )
, mProjectionCenterOffset( 0.151976f )
{
    setEyeSeparation( 0.64f );//0.00119808f );
    setConvergence(0);
}


const Matrix44f& CameraStereoHMD::getProjectionMatrixLeft() const
{
	if( ! mProjectionCached )
		calcProjection();
    
    return mProjectionMatrixLeft;
}
const Matrix44f& CameraStereoHMD::getModelViewMatrixLeft() const
{
	if( ! mModelViewCached )
		calcModelView();
    
    return mModelViewMatrixLeft;
}
const Matrix44f& CameraStereoHMD::getInverseModelViewMatrixLeft() const
{
	if( ! mInverseModelViewCached )
		calcInverseModelView();
    
    return mInverseModelViewMatrixLeft;
}


const Matrix44f& CameraStereoHMD::getProjectionMatrixRight() const
{
	if( ! mProjectionCached )
		calcProjection();
    
    return mProjectionMatrixRight;
}
const Matrix44f& CameraStereoHMD::getModelViewMatrixRight() const
{
	if( ! mModelViewCached )
		calcModelView();
    
    return mModelViewMatrixRight;
}
const Matrix44f& CameraStereoHMD::getInverseModelViewMatrixRight() const
{
	if( ! mInverseModelViewCached )
		calcInverseModelView();
    
    return mInverseModelViewMatrixRight;
}


void CameraStereoHMD::calcModelView() const
{
	// calculate default matrix first
	CameraPersp::calcModelView();
    
	mModelViewMatrixLeft = Matrix44f::createTranslation( Vec3f( getEyeSeparation(), 0, 0 ) ) * mModelViewMatrix;
	mModelViewMatrixRight = Matrix44f::createTranslation( Vec3f( -getEyeSeparation(), 0, 0 ) ) * mModelViewMatrix;
}
void CameraStereoHMD::calcProjection() const
{
	// calculate default matrices first
	CameraPersp::calcProjection();
	
	mProjectionMatrixLeft = Matrix44f::createTranslation( Vec3f( mProjectionCenterOffset, 0, 0 ) ) * mProjectionMatrix;
	mInverseProjectionMatrixLeft = mProjectionMatrixLeft.affineInverted();
	
	mProjectionMatrixRight = Matrix44f::createTranslation( Vec3f( -mProjectionCenterOffset, 0, 0 ) ) * mProjectionMatrix;
	mInverseProjectionMatrixRight = mProjectionMatrixRight.affineInverted();
}