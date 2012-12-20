#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslPRog.h"
#include "cinder/Camera.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"

#include "Node.h"

#include <vector>

namespace ly {

class EightBitApp : public ci::app::AppBasic {
public:
	void setup();
	void mouseDown( ci::app::MouseEvent event );
	void prepareSettings( ci::app::AppBasic::Settings *settings );
	void update();
	void draw();
private:
	void createSketch( std::pair<ci::gl::Texture, ci::gl::Fbo>& sketchPair, ci::Vec2f size );
	void drawNodes( std::vector<Node*>& nodes, ci::gl::GlslProg* currentShader );
	ci::gl::Fbo* createDepthMap( int width, int height );
	
	ci::gl::Fbo mFbo;
	ci::gl::Fbo* mDepthMap;
	
	ci::gl::GlslProg mShader;
	ci::gl::GlslProg mBgShader;
	ci::gl::GlslProg mDepthShader;
	ci::gl::GlslProg mOutlineShader;
	ci::gl::GlslProg mSsaoShader;
	ci::gl::GlslProg mToonShader;
	ci::gl::GlslProg mSketchShader;
	
	ci::CameraPersp mCamera;
	
	ci::gl::Texture mBackgroundTexture;
	
	float mPrevElapsedSeconds;
	
	std::vector<Node*> mNodes;
	
	std::pair<ci::gl::Texture, ci::gl::Fbo> mSketchedEarth;
	std::pair<ci::gl::Texture, ci::gl::Fbo> mSketchedWord;
};
	
}