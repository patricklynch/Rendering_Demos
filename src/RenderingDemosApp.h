#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslPRog.h"
#include "cinder/Camera.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"

#include "Node.h"
#include "Demo.h"

#include <vector>

namespace ly {

class RenderingDemosApp : public ci::app::AppBasic {
public:
	void setup();
	void prepareSettings( ci::app::AppBasic::Settings *settings );
	void update();
	void draw();
	
	void mouseDown( ci::app::MouseEvent event ) { ly::Input::get()->mouseDown( event ); }
	void mouseMove( ci::app::MouseEvent event ) { ly::Input::get()->mouseMove( event ); }
	void mouseDrag( ci::app::MouseEvent event ) { ly::Input::get()->mouseDrag( event ); }
	void mouseUp  ( ci::app::MouseEvent event ) { ly::Input::get()->mouseUp( event ); }
	void keyDown  ( ci::app::KeyEvent event )   { ly::Input::get()->keyDown  ( event ); }
	
private:
	Demo* mCurrentDemo;
	void nextDemo();
	
	float mPrevElapsedSeconds;
	int mCurrentDemoType;
};
	
}