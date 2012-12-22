#include "ToonDemo.h"

#include "cinder/app/App.h"

using namespace ly;

using namespace ci;
using namespace ci::app;

ToonDemo::ToonDemo()
{
	Demo::createSomeObjects();
	
	mDepthMap = mRenderer->createDepthMap( app::getWindowWidth(), app::getWindowHeight() );
	
	mObjectShader		= gl::GlslProg( loadResource( "passthru.vert"),			loadResource( "shader_lit.frag" ) );
	mToonShader			= gl::GlslProg( loadResource( "passthru.vert"),			loadResource( "toon.frag" ) );
	mOutlineShader		= gl::GlslProg( loadResource( "passthru_post.vert"),	loadResource( "outline.frag" ) );
	mDebugDepthSahder	= gl::GlslProg( loadResource( "passthru_post.vert"),	loadResource( "debug_depth.frag" ) );
	mSketchShader		= gl::GlslProg( loadResource( "passthru_post.vert"),	loadResource( "sketch.frag" ) );
	
	gl::Fbo::Format fboFormat;
	fboFormat.enableMipmapping();
	fboFormat.setMagFilter( GL_NEAREST );
	mOutlineFbo = gl::Fbo( app::getWindowWidth(), app::getWindowHeight(), fboFormat );
	
	mCamera->cinderCamera().setPerspective( 30.0f, app::getWindowAspectRatio(), 200.0f, 1200.0f );
	mCamera->setZoom( 400 );
	
	settings.drawDepthTexture = false;
	settings.drawPostFilter = true;
	settings.drawPostFilterOnly = false;
	settings.drawTextures = true;
	settings.drawSketchedTextures = true;
	
	if ( settings.drawTextures ) {
		if ( settings.drawSketchedTextures ) {
			mSketchedWord.first	= loadTexture( "wood.png" );
			mSketchedEarth.first = loadTexture( "earth.png" );
			
			createSketch( mSketchedEarth, getWindowSize() );
			createSketch( mSketchedWord, getWindowSize() );
			
			mLargeSphere->setTexture( &mSketchedEarth.second.getTexture() );
			mBackgroundCube->setTexture( &mSketchedWord.second.getTexture() );
		}
		else {
			mBackgroundCube->setTexture( loadTexture( "wood.png" ) );
			mLargeSphere->setTexture( loadTexture( "earth.png" ) );
		}
	}
}

ToonDemo::~ToonDemo() {}

void ToonDemo::createSketch( std::pair<ci::gl::Texture*, ci::gl::Fbo>& sketchPair, ci::Vec2f size )
{
	sketchPair.second = gl::Fbo( size.x, size.y );
	sketchPair.second.bindFramebuffer();
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	gl::setMatricesWindow( app::getWindowSize(), false );
	mSketchShader.bind();
	sketchPair.first->bind( 0 );
	mSketchShader.uniform( "imageTexture", 0 );
	gl::drawSolidRect( Rectf( 0, 0, size.x, size.y ) );
	sketchPair.first->unbind( 0 );
	mSketchShader.unbind();
	sketchPair.second.unbindFramebuffer();
}

void ToonDemo::draw()
{
	// Set the current camera
	mRenderer->setCamera( &mCamera->cinderCamera() );
	
	// Draw scene to the depth buffer
	mRenderer->drawDepthMap( mDepthMap->getSize(), mDepthMap, mNodes );
	
	gl::GlslProg* currentShader = NULL;
	
	// Draw the scene into the SSAO FBO
	mOutlineFbo.bindFramebuffer();
	{
		// Select our toon shader
		currentShader = &mToonShader;
		if ( !settings.drawPostFilter )
			currentShader = &mObjectShader; // swith to a differnet shader to turn it off
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
	mOutlineFbo.unbindFramebuffer();
	
	// Draw depth map
	if ( settings.drawDepthTexture ) {
		mRenderer->debugDrawDepthMap( mDepthMap );
		return;
	}
	
	// Draw outlines
	if ( settings.drawPostFilter ) {
	}
	
	currentShader = &mOutlineShader;
	{
		currentShader->bind();
		{
			gl::setMatricesWindow( app::getWindowSize(), false );
			mOutlineFbo.bindTexture( 0 );
			currentShader->uniform( "mainTexture", 0 );
			mDepthMap->bindDepthTexture( 1 );
			currentShader->uniform( "mainColor", ColorA( 1, 1, 1, 1 ) );
			currentShader->uniform( "depthTexture", 1 );
			currentShader->uniform( "outlineThickness", 1.2f );
			currentShader->uniform( "outlineColor", ColorA(0, 0, 0, 1 ) );
			currentShader->uniform( "transformMatrix", Matrix44f::identity() );
			gl::drawSolidRect( Rectf( app::getWindowBounds() ) );
			mOutlineFbo.unbindTexture();
		}
		currentShader->unbind();
	}
	mDepthMap->unbindTexture();
}

void ToonDemo::update( const float deltaTime )
{
	Demo::update( deltaTime );
}