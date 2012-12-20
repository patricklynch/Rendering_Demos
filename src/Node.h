#pragma once

#include <map>

#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/gl/Texture.h"

namespace ly {

class PostFilter;
class Renderer;
	
typedef enum {
	NodeTypeNull,
	NodeTypeInvisible,
	NodeTypeSprite,
	NodeTypeSphere,
	NodeTypeCube
} NodeType;
	
typedef enum {
	MaterialDiffuse,
	MaterialNormal,
	MaterialSpecular,
} MaterialProperty;

class Node {
public:
	Node( NodeType type = NodeTypeNull );
	virtual ~Node();
	
	void update( const float deltaTime );
	void draw();
	
	ci::Vec3f position;
	ci::Vec3f rotation;
	ci::Vec3f scale;
	
	ci::Vec2f spriteSize;
	float spriteRotation;
	
	std::map<MaterialProperty, ci::gl::Texture*> textures;
	std::map<MaterialProperty, ci::ColorA> colors;
	
	Node* parent;
	
	ci::Matrix44f transform() const { return mLastTransform; }
	virtual ci::Vec3f globalPosition();
	
	// Convenience methods making it easier to set/get the diffuse color/texture
	void setTexture( ci::gl::Texture* texture );
	void setColor( ci::ColorA color );
	ci::gl::Texture* texture();
	ci::ColorA color();
	
	bool isObject() const { return mNodeType == NodeTypeSphere || mNodeType == NodeTypeCube; }
	
	bool isUnlit() const { return mIsUnlit; }
	void setIsUnlit( bool inValue ) { mIsUnlit = inValue; }
	
	bool operator==( Node* node ) const { return mNodeId == node->nodeId(); }
	int nodeId() const { return mNodeId; }
	
	NodeType nodeType() const { return mNodeType; }
	void faceCamera();
	
	bool inverted;
	
protected:
	bool mShouldFaceCamera;
	ly::Renderer* mRenderer;
	NodeType mNodeType;
	bool mIsUnlit;
	int mNodeId;
	ci::Matrix44f mLastTransform;
	ci::Matrix44f mTransform;
	
};

}