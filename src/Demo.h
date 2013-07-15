#pragma once

#include "Node.h"
#include "Camera.h"
#include "Renderer.h"

#include <string>
#include <vector>

#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/Vector.h"

namespace ly {
	
struct DemoSettings {
	bool drawDepthTexture;
	bool drawPostFilter;
	bool drawPostFilterOnly;
	bool drawTextures;
	bool drawSketchedTextures;
};

class Demo {
public:
	Demo();
	~Demo();
	
	enum { Ssao, Scatter, Toon, Detective, Count, None } Type;
	virtual int type() const { return None; }
	
	virtual void update( const float deltaTime );
	virtual void draw();
	
	void addNode( Node* node );
	void removeNode( Node* node );
	void addNodes( std::vector<Node*>& nodes );
	
protected:
	ci::gl::Texture* loadTexture( std::string path );
	void unloadAllTextures();
	void createSomeObjects();
	
	DemoSettings settings;
	
	// A few notable objects that subclasses might want to get ahold of
	Node* mLargeSphere;
	Node* mBackgroundCube;
	
	ly::Renderer* mRenderer;
	ly::Camera* mCamera;
	std::map<std::string, ci::gl::Texture*> mTextures;
	std::vector<Node*> mNodes;
};

}