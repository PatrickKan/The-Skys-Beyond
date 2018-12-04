#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetVerticalSync(true);
	//ofBackgroundHex(0xfdefc2);
	ofBackground(ofColor(255, 255, 255));
	ofSetLogLevel(OF_LOG_NOTICE);

	background.load("C:\\Users\\Patrick Kan\\source\\repos\\finalproject-PatrickKan\\TheSkysBeyond\\TheSkysBeyond\\images\\space_background2.png");

	view.setPosition(ofPoint(mouseX, mouseY, 0));

	bMouseForce = false;
	followMouse = false;

	box2d.init();
	box2d.setGravity(0, 0);
	//box2d.createGround();
	box2d.setFPS(100.0);
	box2d.registerGrabbing();

}

//--------------------------------------------------------------
void ofApp::update()
{
	box2d.update();

	view.move(mouseX, mouseY, 0);

	
	if (bMouseForce)
	{
		float strength = 8.0;
		float damping = 1.0f;
		float minDis = 100;
		for (auto i = 0; i < circles.size(); i++)
		{
			circles[i]->addAttractionPoint(mouseX, mouseY, strength);
			circles[i]->setDamping(damping, damping);
		}
		for (auto i = 0; i < customParticles.size(); i++)
		{
			customParticles[i]->addAttractionPoint(mouseX, mouseY, strength);
			customParticles[i]->setDamping(damping, damping);
		}
		for (auto i = 0; i < boxes.size(); i++)
		{
			boxes[i]->addAttractionPoint(mouseX, mouseY, strength);
			boxes[i]->setDamping(damping, damping);
		}
		for (auto i = 0; i < triangles.size(); i++)
		{
			triangles[i]->addAttractionPoint(mouseX, mouseY, strength);
			triangles[i]->setDamping(damping, damping);
		}
	}

	if (followMouse && triangles[0])
	{
		int velocity = 5;
		ofVec2f currPos = triangles[0]->getPosition();
		int xPos = currPos.x;
		int yPos = currPos.y;

		float angle = atan2(yPos - mouseY, mouseX - xPos);
		triangles[0]->setVelocity(playerVelocity*cos(angle), -playerVelocity * sin(angle));
		
		scrollVelocity = playerVelocity * cos(angle);
		playerXPos += scrollVelocity * 0.1;
	}

	if (mouseDown)
	{
		playerVelocity += 0.05;
	}

	//Planet gravity
	//For planet force, gravity should be equivalent to 1/r^2, but not too fast when approaching zero (check w/ if for r > 0.01)
	for (auto planet : planets)
	{
		float gravity = 0;

		ofVec2f planetPos = planet->getPosition();
		int planetRad = planet->getRadius();
		int planetX = planetPos.x;
		int planetY = planetPos.y;

		ofVec2f planetVel = planet->getVelocity();
		planet->setVelocity(scrollVelocity, planetVel.y);

		for (auto circle : circles)
		{
			gravity = ComputeGravity(circle->getPosition(), planetPos, planetRad);
			circle->addAttractionPoint(planetX, planetY, gravity);
			ofVec2f velocity = circle->getVelocity();
			circle->setVelocity(velocity.x + scrollVelocity, velocity.y);
		}
		for (auto particle : customParticles)
		{
			gravity = ComputeGravity(particle->getPosition(), planetPos, planetRad);
			particle->addAttractionPoint(planetX, planetY, gravity);
			ofVec2f velocity = particle->getVelocity();
			particle->setVelocity(velocity.x + scrollVelocity, velocity.y);
		}
		for (auto box : boxes)
		{
			gravity = ComputeGravity(box->getPosition(), planetPos, planetRad);
			box->addAttractionPoint(planetX, planetY, gravity);
			ofVec2f velocity = box->getVelocity();
			box->setVelocity(velocity.x + scrollVelocity, velocity.y);
		}
		for (auto triangle : triangles)
		{
			gravity = ComputeGravity(triangle->getPosition(), planetPos, planetRad);
			triangle->addAttractionPoint(planetX, planetY, gravity);
			ofVec2f velocity = triangle->getVelocity();
			triangle->setVelocity(velocity.x + scrollVelocity, velocity.y);
		}

		
	}


	// remove shapes offscreen
	ofRemove(boxes, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(circles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(customParticles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(triangles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(planets, ofxBox2dBaseShape::shouldRemoveOffScreen);
}

float ofApp::ComputeGravity(ofVec2f currPos, ofVec2f planetPos, int planetRad)
{
	int xPos = currPos.x;
	int yPos = currPos.y;

	int planetX = planetPos.x;
	int planetY = planetPos.y;

	//Hypot taken from https://en.cppreference.com/w/cpp/numeric/math/hypot
	float distance = std::hypot(xPos - planetX, yPos - planetY);

	float gravity = 100 * float(planetRad) / pow(distance, 2);

	return gravity;
}


//--------------------------------------------------------------
void ofApp::draw()
{

	background.draw(-playerXPos, 0, 2 * ofGetWindowWidth() - playerXPos, ofGetWindowHeight());



	for (auto i = 0; i < circles.size(); i++)
	{
		ofFill();
		ofSetHexColor(0x90d4e3);
		circles[i]->draw();
	}

	for (auto i = 0; i < boxes.size(); i++)
	{
		ofFill();
		ofSetHexColor(0xe63b8b);
		boxes[i]->draw();
	}

	for (auto i = 0; i < customParticles.size(); i++)
	{
		customParticles[i]->draw();
	}

	for (int i = 0; i < triangles.size(); i++)
	{
		ofFill();
		ofSetHexColor(0xEE82EE);
		triangles[i]->draw();
	}

	for (auto planet : planets)
	{
		ofFill();
		ofSetHexColor(0xcc3333);
		planet->draw();
	}


	ofNoFill();
	ofSetHexColor(0xffe6e6);
	if (drawing.size() == 0)
	{
		edgeLine.updateShape();
		edgeLine.draw();
	}
	else drawing.draw();

	string info = "";
	info += "Press [f] to toggle Mouse Force [" + ofToString(bMouseForce) + "]\n";
	info += "Press [c] for circles\n";
	info += "Press [b] for blocks\n";
	info += "Press [q] for triangles\n";
	info += "Press [=] to create a planet with gravity\n";
	info += "Press [0] to have the first triangle follow the mouse\n";
	info += "Use wasd controls to move first triangle\n";
	info += "MouseX: " + std::to_string(mouseX) + "\n";
	info += "MouseY: " + std::to_string(mouseY) + "\n";
	info += "Total Bodies: " + ofToString(box2d.getBodyCount()) + "\n";
	info += "Total Joints: " + ofToString(box2d.getJointCount()) + "\n\n";
	info += "FPS: " + ofToString(ofGetFrameRate()) + "\n";
	ofSetHexColor(0xffffff);
	ofDrawBitmapString(info, 30, 30);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'c')
	{
		float r = ofRandom(4, 20);		// a random radius 4px - 20px
		circles.push_back(std::make_shared<ofxBox2dCircle>());
		circles.back()->setPhysics(1.0, 0.53, 0.1);
		circles.back()->setup(box2d.getWorld(), mouseX, mouseY, r);
	}

	if (key == 'b')
	{
		float w = ofRandom(4, 20);
		float h = ofRandom(4, 20);
		boxes.push_back(std::make_shared<ofxBox2dRect>());
		boxes.back()->setPhysics(3.0, 0.53, 0.1);
		boxes.back()->setup(box2d.getWorld(), mouseX, mouseY, w, h);
	}

	if (key == '1')
	{
		float r = ofRandom(30, 50);		// a random radius 4px - 20px
		circles.push_back(std::make_shared<ofxBox2dCircle>());
		circles.back()->setPhysics(0.0, 0.53, 0.1);
		circles.back()->setup(box2d.getWorld(), mouseX, mouseY, r);
	}

	if (key == 'q')
	{
		auto tri = std::make_shared<ofxBox2dPolygon>();
		tri->addTriangle(ofPoint(mouseX - 10, mouseY), ofPoint(mouseX, mouseY - 10), ofPoint(mouseX + 10, mouseY));
		tri->setPhysics(1.0, 0.7, 1.0);
		tri->create(box2d.getWorld());

		triangles.push_back(tri);
	}

	if (key == 'p')
	{
		auto tri = std::make_shared<ofxBox2dPolygon>();
		tri->addTriangle(ofPoint(mouseX - 10, mouseY), ofPoint(mouseX, mouseY - 10), ofPoint(mouseX + 10, mouseY));
		tri->setPhysics(1000.0, 0.7, 1.0);
		tri->create(box2d.getWorld());
		tri->addForce(ofVec2f(0, 1), 50);
		triangles.push_back(tri);
	}

	if (key == 'm')
	{
		float r = 20;		// a random radius 4px - 20px
		circles.push_back(std::make_shared<ofxBox2dCircle>());
		circles.back()->setPhysics(3.0, 0.53, 0.1);
		circles.back()->setup(box2d.getWorld(), mouseX, mouseY, r);
		circles.back()->setVelocity(5.0, 0.0);
	}

	if (key == 'w')
	{
		triangles[0]->setVelocity(0, -5);
	}

	if (key == 'a')
	{
		triangles[0]->setVelocity(-5, 0);
	}

	if (key == 's')
	{
		triangles[0]->setVelocity(0, 5);
	}

	if (key == 'd')
	{
		triangles[0]->setVelocity(5, 0);
	}

	if (key == '0') //Follow mouse with constant velocity (currently set at 5)
	{
		followMouse = !followMouse;
	}

	if (key == '=') //Create a planet with gravity and varying size
	{
		float r = ofRandom(40, 70);		// a random radius 4px - 20px
		planets.push_back(std::make_shared<ofxBox2dCircle>());
		planets.back()->setPhysics(10000.0, 0, 0.1);
		planets.back()->setup(box2d.getWorld(), mouseX, mouseY, r);

	}

	if (key == 'f') bMouseForce = !bMouseForce;
	if (key == 't') ofToggleFullscreen();
}


//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	//drawing.addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	mouseDown = true;

	std::cout << "Mouse is being pressed\n";

	/*if (edgeLine.isBody())
	{
		drawing.clear();
		edgeLine.destroy();
	}

	drawing.addVertex(x, y);
	*/
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	mouseDown = false;

	/*drawing.setClosed(false);
	drawing.simplify();

	edgeLine.addVertexes(drawing);
	//polyLine.simplifyToMaxVerts(); // this is based on the max box2d verts
	edgeLine.setPhysics(0.0, 0.5, 0.5);
	edgeLine.create(box2d.getWorld());

	drawing.clear(); */
}