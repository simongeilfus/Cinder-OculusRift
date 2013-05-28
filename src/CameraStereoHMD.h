//
//  CameraStereoHMD.h
//  OculusSDKTest
//
//  Created by Simon Geilfus on 26/05/13.
//
//

#pragma once

#include "cinder/Camera.h"


class CameraStereoHMD : public ci::CameraStereo {
public:
	CameraStereoHMD();
	CameraStereoHMD( int pixelWidth, int pixelHeight, float fov );
	CameraStereoHMD( int pixelWidth, int pixelHeight, float fov, float nearPlane, float farPlane );
    
    float   getProjectionCenterOffset() const { return mProjectionCenterOffset; }
    void    setProjectionCenterOffset( float offset ) { mProjectionCenterOffset = offset; }
	
	virtual const ci::Matrix44f&	getProjectionMatrixLeft() const;
	virtual const ci::Matrix44f&	getModelViewMatrixLeft() const;
	virtual const ci::Matrix44f&	getInverseModelViewMatrixLeft() const;
    
	virtual const ci::Matrix44f&	getProjectionMatrixRight() const;
	virtual const ci::Matrix44f&	getModelViewMatrixRight() const;
	virtual const ci::Matrix44f&	getInverseModelViewMatrixRight() const;
    
protected:
    
	virtual void	calcModelView() const;
	virtual void	calcProjection() const;
    
private:
    
    float           mProjectionCenterOffset;
};