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
    
    //! Returns value used to offset the projections matrices
    float   getProjectionCenterOffset() const { return mProjectionCenterOffset; }
    //! Set the value used to offset the projections matrices
    void    setProjectionCenterOffset( float offset ) { mProjectionCenterOffset = offset; }
	
    //! Returns Left Eye Projection Matrix
	virtual const ci::Matrix44f&	getProjectionMatrixLeft() const;
    //! Returns Left Eye ModelView Matrix
	virtual const ci::Matrix44f&	getModelViewMatrixLeft() const;
    //! Returns Left Eye Inverse-ModelView Matrix
	virtual const ci::Matrix44f&	getInverseModelViewMatrixLeft() const;
    
    //! Returns Right Eye Projection Matrix
	virtual const ci::Matrix44f&	getProjectionMatrixRight() const;
    //! Returns Right Eye ModelView Matrix
	virtual const ci::Matrix44f&	getModelViewMatrixRight() const;
    //! Returns Right Eye Inverse-ModelView Matrix
	virtual const ci::Matrix44f&	getInverseModelViewMatrixRight() const;
    
protected:
    
	virtual void	calcModelView() const;
	virtual void	calcProjection() const;
    
private:
    
    float           mProjectionCenterOffset;
};