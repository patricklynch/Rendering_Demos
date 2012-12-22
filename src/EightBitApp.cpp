#include "EightBitApp.h"
#include "SsaoDemo.h"
#include "ToonDemo.h"
#include "ScatteringDemo.h"

#include "cinder/ip/Hdr.h"

#include <boost/bind.hpp>

using namespace ly;

using namespace ci;
using namespace ci::app;
using namespace std;

void EightBitApp::prepareSettings(cinder::app::AppBasic::Settings *settings)
{
	settings->setWindowSize( 1024, 768 );
	//settings->setFrameRate( 10.0f );
}

void EightBitApp::setup()
{
	mPrevElapsedSeconds = 0.0f;
	
	ly::Input::get()->addListenerForKey( boost::bind( &EightBitApp::nextDemo, this ), KeyEvent::KEY_SPACE );
	
	mCurrentDemo = NULL;
	mCurrentDemoType = Demo::None;
	nextDemo();
}

void EightBitApp::nextDemo()
{
	if ( mCurrentDemo != NULL ) {
		delete mCurrentDemo;
		mCurrentDemo = NULL;
	}
	
	if ( ++mCurrentDemoType >= Demo::Count )
		mCurrentDemoType = 0;
	
	switch( mCurrentDemoType ) {
		case Demo::Ssao:
			mCurrentDemo = new SsaoDemo();
			console() << "Showing SSAO Demo." << std::endl;
			break;
		case Demo::Toon:
			mCurrentDemo = new ToonDemo();
			console() << "Showing Borderlands Toon Demo." << std::endl;
			break;
		case Demo::Scatter:
			mCurrentDemo = new ScatteringDemo();
			console() << "Showing Sub Surface Scattering Demo." << std::endl;
			break;
		default:break;
	}
}

float angle = 0.0f;

void EightBitApp::update()
{
    float currentTime = app::getElapsedSeconds();
    float deltaTime = currentTime - mPrevElapsedSeconds;
    mPrevElapsedSeconds = currentTime;
	
	Input::get()->update( deltaTime );
	mCurrentDemo->update( deltaTime );
}

void EightBitApp::draw()
{
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	mCurrentDemo->draw();
}

CINDER_APP_BASIC( EightBitApp, RendererGl )
