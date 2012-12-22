#pragma once

#include "Node.h"
#include "Input.h"

#include "cinder/Camera.h"

namespace ly {

class Camera : public Node {
public:
	static Camera* get();
	virtual ~Camera();
	virtual void update( const float deltaTime );
	
	ci::CameraPersp& cinderCamera() { return mCinderCamera; }
	virtual ci::Vec3f globalPosition();
	
	void setRange( float near, float far );
	void setZoom( float zoom );
	
private:
	static Camera* sInstance;
	Camera();
	
	ci::Vec3f mRotationStart;
	float mZoomStart;
	
	ly::Input* mInput;
	ly::Node mBody;
	ly::Node mPivot;
	ci::CameraPersp mCinderCamera;
	
};

}