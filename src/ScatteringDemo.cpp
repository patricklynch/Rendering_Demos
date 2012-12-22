#include "ScatteringDemo.h"

#include "cinder/app/App.h"

using namespace ly;

using namespace ci;
using namespace ci::app;

using namespace ly;

ScatteringDemo::ScatteringDemo()
{
	Demo::createSomeObjects();
	
	mDepthMapFront		= mRenderer->createDepthMap( app::getWindowWidth(), app::getWindowHeight() );
	mDepthMapBack		= mRenderer->createDepthMap( app::getWindowWidth(), app::getWindowHeight() );
	
	mScatterShader		= gl::GlslProg( loadResource( "passthru.vert"),		loadResource( "scatter.frag" ) );
	
	mCamera->cinderCamera().setPerspective( 30.0f, app::getWindowAspectRatio(), 200.0f, 1200.0f );
	mCamera->setZoom( 400 );
}

ScatteringDemo::~ScatteringDemo() {}

void ScatteringDemo::draw()
{
	// Set the current camera
	mRenderer->setCamera( &mCamera->cinderCamera() );
	
	// Draw scene to the depth buffer
	mRenderer->drawDepthMap( mDepthMapFront->getSize(), mDepthMapFront, mNodes, GL_BACK );
	mRenderer->drawDepthMap( mDepthMapBack->getSize(), mDepthMapBack, mNodes, GL_FRONT );
	
	gl::GlslProg* currentShader = NULL;
	
	// Select our sub surface scattering shader
	currentShader = &mScatterShader;
	currentShader->bind();
	{
		ci::Matrix44f shadowTransMatrix = mCamera->cinderCamera().getProjectionMatrix();
		shadowTransMatrix *= mCamera->cinderCamera().getModelViewMatrix();
		shadowTransMatrix *= mCamera->cinderCamera().getInverseModelViewMatrix();
		
		mDepthMapBack->bindDepthTexture( 0 );
		currentShader->uniform( "depthBackTexture", 0 );
		mDepthMapFront->bindDepthTexture( 1 );
		currentShader->uniform( "depthFrontTexture", 1 );
		currentShader->uniform( "modelViewMatrix", mCamera->cinderCamera().getModelViewMatrix() );
		currentShader->uniform( "projectionMatrix", mCamera->cinderCamera().getProjectionMatrix() );
		currentShader->uniform( "shadowTransMatrix", shadowTransMatrix );
		currentShader->uniform( "ambientColor", ColorA(1,1,1,1) * 0.3f );
		currentShader->uniform( "eyePos", mCamera->globalPosition() );
		currentShader->uniform( "lightPos", Vec3f(.8,1,.5) * 1000.0f );
		mRenderer->drawNodes( mNodes, currentShader );
		mDepthMapFront->unbindTexture();
		mDepthMapBack->unbindTexture();
	}
	currentShader->unbind();
}

void ScatteringDemo::update( const float deltaTime )
{
	Demo::update( deltaTime );
}