#include "Camera.h"

#include "cinder/app/App.h"

using namespace ci;
using namespace ci::app;

using namespace ly;

ly::Camera* ly::Camera::sInstance = NULL;

ly::Camera* ly::Camera::get()
{
	if ( !sInstance ) sInstance = new ly::Camera();
	return sInstance;
}

void ly::Camera::setZoom( float zoom )
{
	mBody.position.z = zoom;
}

ly::Camera::Camera()
{
	mCinderCamera.setPerspective( 30, app::getWindowAspectRatio(), 1.0f, 4000.0f );
	
	mPivot.rotation.x	= -20.0f;
	mBody.position.z	= 500.0f;
	
	mBody.parent = &mPivot;
	mPivot.parent = this;
	
	mInput = Input::get();
}

ly::Camera::~Camera() {}

ci::Vec3f ly::Camera::globalPosition()
{
	return mBody.globalPosition();
}

void ly::Camera::update( const float deltaTime )
{
	MouseDrag* drag;
	
	drag = mInput->mouseDrag( Input::MOUSE_LEFT );
	if (drag != NULL) {
		rotation.y = mRotationStart.y - drag->difference().x * 0.57f;
		rotation.x = mRotationStart.x - drag->difference().y * 0.57f;
	}
	else {
		mRotationStart.x = rotation.x;
		mRotationStart.y = rotation.y;
	}
	
	drag = mInput->mouseDrag( Input::MOUSE_RIGHT );
	if (drag != NULL) mBody.position.z = mZoomStart - drag->difference().x * 0.57f;
	else mZoomStart = mBody.position.z;
	
	Node::update( deltaTime );
	mPivot.update( deltaTime );
	mBody.update( deltaTime );
	
	mCinderCamera.setWorldUp( Vec3f::yAxis() );
	mCinderCamera.setEyePoint( mBody.globalPosition() );
	mCinderCamera.setOrientation( Quatf( mBody.transform() ) );
	
}