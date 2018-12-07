#include "ofApp.h"
#include "..//Player.h"

//Build map with array of circles/points/blocks
//Underscore for private vars
//for each loops
//shorten lines

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetVerticalSync(true);
	//ofBackgroundHex(0xfdefc2);
	ofBackground(ofColor(255, 255, 255));
	ofSetLogLevel(OF_LOG_NOTICE);

	//Data loaded from bin/data
	background.load("space_background2.png");
	
	soundPlayer.load("TheAbyss.mp3");
	soundPlayer.play();
	soundPlayer.setMultiPlay(true);

	
	bMouseForce = false;
	followMouse = false;

	box2d.init();
	box2d.setGravity(0, 0);
	//box2d.createGround();
	box2d.setFPS(60.0);
	box2d.registerGrabbing();

	// register the listener to get events
	box2d.enableEvents();

	ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &ofApp::contactEnd);

	createPlayer();
}

void ofApp::createPlayer()
{
	players.push_back(shared_ptr<Player>(new Player(ofGetWindowWidth()/15, ofGetWindowHeight()/2)));
	Player* player = players.back().get();
	player->create(box2d.getWorld());
}

//--------------------------------------------------------------
void ofApp::update()
{
	box2d.update();

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

	if (followMouse && (players[0] != nullptr))
	{
		int velocity = 5;

		ofVec2f currPos = players[0]->getPosition();
		int xPos = currPos.x;
		int yPos = currPos.y;

		float angle = atan2(yPos - mouseY, mouseX - xPos);
		players[0]->setVelocity(0, -playerVelocity * sin(angle));

		//Example moving x velocity
		//players[0]->setVelocity(playerVelocity*cos(angle), -playerVelocity * sin(angle));
		
		scrollVelocity = playerVelocity;
		playerXPos += scrollVelocity * 0.1;
	}

	if (mouseDown && fuel > 0)
	{
		playerVelocity += 0.1;
		fuel--;
	}

	//Planet gravity
	//For planet force, gravity should be equivalent to 1/r^2, but not too fast when approaching zero (check w/ if for r > 0.01)
	for (auto planet : planets)
	{
		float gravity = 0;

		ofVec2f planetPos = planet->getPosition();
		int planetX = planetPos.x;
		int planetY = planetPos.y;

		ofVec2f planetVel = planet->getVelocity();
		planet->setVelocity(-scrollVelocity, planetVel.y);

		for (auto circle : circles)
		{
			gravity = ComputeGravity(circle->getPosition(), planet);
			circle->addAttractionPoint(planetX, planetY, gravity);
		}
		for (auto particle : customParticles)
		{
			gravity = ComputeGravity(particle->getPosition(), planet);
			particle->addAttractionPoint(planetX, planetY, gravity);
		}
		for (auto box : boxes)
		{
			gravity = ComputeGravity(box->getPosition(), planet);
			box->addAttractionPoint(planetX, planetY, gravity);
		}
		for (auto triangle : triangles)
		{			
			gravity = ComputeGravity(triangle->getPosition(), planet);
			triangle->addAttractionPoint(planetX, planetY, gravity);
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

	float gravity = 0;

	//Double check to not divide by 0
	if (distance != 0)
	{
		gravity = 500 * float(planetRad) / pow(distance, 2);
	}
	return gravity;
}

float ofApp::ComputeGravity(ofVec2f currPos, std::shared_ptr<ofxBox2dCircle> planet)
{
	int planetRad = planet->getRadius();

	int xPos = currPos.x;
	int yPos = currPos.y;

	ofVec2f planetPos = planet->getPosition();
	int planetX = planetPos.x;
	int planetY = planetPos.y;

	//Hypot taken from https://en.cppreference.com/w/cpp/numeric/math/hypot
	float distance = std::hypot(xPos - planetX, yPos - planetY);

	float gravity = 0;

	//Double check to not divide by 0
	if (distance != 0)
	{
		gravity = 100 * float(planetRad) / pow(distance, 2);
	}

	return gravity;
	return gravity;
}


//--------------------------------------------------------------
void ofApp::draw()
{
	//Three backgrounds drawn of the same image to simulate an infinite scrolling background
	background.draw((int(-playerXPos) % int(4*ofGetWindowWidth())), 0, 2 * ofGetWindowWidth(), ofGetWindowHeight());
	background.draw(int(-playerXPos) % int(4*ofGetWindowWidth()) + 2 * ofGetWindowWidth()-1, 0, 2 * ofGetWindowWidth(), ofGetWindowWidth());
	background.draw((int(-playerXPos) % int(4 * ofGetWindowWidth())) + 4 * ofGetWindowWidth() - 1, 0, 2 * ofGetWindowWidth(), ofGetWindowHeight());
	
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

	for (auto player : players)
	{
		player->draw();
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
	info += "Press spacebar to begin\n";
	info += "Hold down mouse to boost. Current fuel: " + to_string(fuel) + "\n";
	info += "Press [c] for circles\n";
	info += "Press [b] for blocks\n";
	info += "Press [q] for triangles\n";
	info += "Press [=] to create a planet with gravity\n";
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
		circles.back()->setVelocity(-scrollVelocity, 0);
	}

	if (key == 'b')
	{
		float w = ofRandom(10, 30);
		float h = ofRandom(10, 30);
		boxes.push_back(std::make_shared<ofxBox2dRect>());
		boxes.back()->setPhysics(3.0, 0.53, 0.1);
		boxes.back()->setup(box2d.getWorld(), mouseX, mouseY, w, h);
		boxes.back()->setVelocity(-scrollVelocity, 0);
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
		tri->setPhysics(100.0, 0.7, 1.0);
		tri->create(box2d.getWorld());

		triangles.push_back(tri);
		tri->setVelocity(-scrollVelocity, 0);
	}

	if (key == '-')
	{
		players.push_back(shared_ptr<Player>(new Player(mouseX,mouseY)));
		Player* player = players.back().get();
		player->create(box2d.getWorld());
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

	if (key == ' ') //Follow mouse with constant velocity (currently set at 5)
	{
		followMouse = !followMouse;
	}

	if (key == '=') //Create a planet with gravity and varying size
	{
		float r = ofRandom(40, 70);		// a random radius 4px - 20px
		planets.push_back(std::make_shared<ofxBox2dCircle>());
		planets.back()->setPhysics(1000000000.0, 0, 0.1);
		planets.back()->setup(box2d.getWorld(), mouseX, mouseY, r);

	}

	if (key == 'z')
	{
		shootCircle();
	}

	if (key == 'f') bMouseForce = !bMouseForce;
	if (key == 't') ofToggleFullscreen();
}

//Shoots circle by placing it in front of player with an initial forward velocity
void ofApp::shootCircle()
{
	auto player = players[0];

	ofVec2f playerPos = player->getPosition();
	int playerX = playerPos.x;
	int playerY = playerPos.y;

	float r = 10;		// a random radius 4px - 20px
	circles.push_back(std::make_shared<ofxBox2dCircle>());
	circles.back()->setPhysics(10.0, 0.53, 0.1);
	circles.back()->setup(box2d.getWorld(), playerX + r + 22 , playerY , r);
	circles.back()->setVelocity(50.0, 0.0);
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
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	mouseDown = false;
}

//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e)
{
	std::cout << "Entering contact start";

	if (e.a != NULL && e.b != NULL)
	{
		if (e.a->GetType() == b2Shape::e_polygon && e.b->GetType() == b2Shape::e_circle)
		{
			auto player = players[0];
			std::cout << "Contact made with player and circle\n";
			player->takeDamage(5);
			std::cout << "Player health: " << player->currentHealth() << "\n";
		}
	}
}

//--------------------------------------------------------------
void ofApp::contactEnd(ofxBox2dContactArgs &e)
{
	if (e.a != NULL && e.b != NULL)
	{
		if (e.a->GetType() == b2Shape::e_polygon && e.b->GetType() == b2Shape::e_circle)
		{
			std::cout << "Contact made with player and circle\n";
		}
	}
}
