#include "SsaoDemo.h"

#include "cinder/app/App.h"

using namespace ly;

using namespace ci;
using namespace ci::app;

SsaoDemo::SsaoDemo()
{
	Demo::createSomeObjects();
	
	mDepthMap = mRenderer->createDepthMap( app::getWindowWidth(), app::getWindowHeight() );
	
	mObjectShader		= gl::GlslProg( loadResource( "passthru.vert"),			loadResource( "shader_lit.frag" ) );
	mSsaoShader			= gl::GlslProg( loadResource( "passthru_post.vert"),	loadResource( "ssao.frag" ) );
	
	gl::Fbo::Format fboFormat;
	fboFormat.enableMipmapping();
	fboFormat.setMagFilter( GL_NEAREST );
	mSsaoFbo = gl::Fbo( app::getWindowWidth(), app::getWindowHeight(), fboFormat );
	
	mCamera->cinderCamera().setPerspective( 30.0f, app::getWindowAspectRatio(), 200.0f, 1200.0f );
	mCamera->setZoom( 400 );
	
	settings.drawDepthTexture = false;
	settings.drawPostFilter = true;
	settings.drawPostFilterOnly = true;
}

SsaoDemo::~SsaoDemo() {}

void SsaoDemo::draw()
{
	// Set the current camera
	mRenderer->setCamera( &mCamera->cinderCamera() );

	// Draw scene to the depth buffer
	mRenderer->drawDepthMap( mDepthMap->getSize(), mDepthMap, mNodes );
	
	gl::GlslProg* currentShader = NULL;
	
	// Draw the scene into the SSAO FBO
	mSsaoFbo.bindFramebuffer();
	{
		// Select our simple diffuse/specular lighting shader
		currentShader = &mObjectShader;
		currentShader->bind();
		{
			// Clear black if we are drawing the full scene, clear white if we are drawing only SSAO
			gl::clear( settings.drawPostFilterOnly ? Color( 1, 1, 1 ) : Color( 0, 0, 0 ) );
			
			mDepthMap->bindDepthTexture( 1 );
			currentShader->uniform( "depthMap", 1 );
			currentShader->uniform( "modelViewMatrix", mCamera->cinderCamera().getModelViewMatrix() );
			currentShader->uniform( "projectionMatrix", mCamera->cinderCamera().getProjectionMatrix() );
			currentShader->uniform( "ambientColor", ColorA(1,1,1,1) * 0.3f );
			currentShader->uniform( "eyePos", mCamera->globalPosition() );
			currentShader->uniform( "lightPos", Vec3f(.5,1,.5) * 1000.0f );
			mDepthMap->unbindTexture();
			
			// Render objects(nodes) in the scene
			if ( !settings.drawPostFilterOnly )
				mRenderer->drawNodes( mNodes, currentShader );
		}
		currentShader->unbind();

	}
	mSsaoFbo.unbindFramebuffer();
	
	// Draw depth map
	if ( settings.drawDepthTexture ) {
		mRenderer->debugDrawDepthMap( mDepthMap );
		return;
	}
	
	// Draw SSAO
	currentShader = &mSsaoShader;
	{
		currentShader->bind();
		{
			gl::setMatricesWindow( app::getWindowSize(), false );
			mSsaoFbo.bindTexture( 0 );
			currentShader->uniform( "mainTexture", 0 );
			mDepthMap->bindDepthTexture( 1 );
			currentShader->uniform( "mainColor", ColorA( 1, 1, 1, 1 ) );
			currentShader->uniform( "depthTexture", 1 );
			currentShader->uniform( "spread", 0.002f );
			currentShader->uniform( "samples", 7 );
			if ( !settings.drawPostFilter )
				currentShader->uniform( "samples", 0 ); // easy way to turn it off
			currentShader->uniform( "strength", 0.5f );
			currentShader->uniform( "depthRange", .5f );
			gl::drawSolidRect( Rectf( app::getWindowBounds() ) );
		}
		mSsaoFbo.unbindTexture();
	}
	currentShader->unbind();
	mDepthMap->unbindTexture();
}

void SsaoDemo::update( const float deltaTime )
{
	Demo::update( deltaTime );
}