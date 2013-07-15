#pragma once

#include "Demo.h"
#include "Node.h"

#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"

namespace ly {

class DetectiveDemo : public Demo {
public:
	DetectiveDemo();
	virtual ~DetectiveDemo();
	
	virtual void draw();
	virtual void update( const float deltaTime );
	
private:
	ci::gl::Fbo* mDepthMap;
	ci::gl::Fbo mOutlineFbo;
	
	void createSketch( std::pair<ci::gl::Texture*, ci::gl::Fbo>& sketchPair, ci::Vec2f size );
	
	std::pair<ci::gl::Texture*, ci::gl::Fbo> mSketchedEarth;
	std::pair<ci::gl::Texture*, ci::gl::Fbo> mSketchedWord;
	
	ci::gl::GlslProg mObjectShader;
	ci::gl::GlslProg mToonShader;
	ci::gl::GlslProg mDebugDepthSahder;
	ci::gl::GlslProg mOutlineShader;
	ci::gl::GlslProg mSketchShader;
	
};

}