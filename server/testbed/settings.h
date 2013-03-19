#ifndef SETTINGS_H
#define SETTINGS_H

// Test settings. Some can be controlled in the GUI.
struct Settings
{
	Settings() :
		viewCenter(0.0f, 0.0f),
		hz(60.0f),
		velocityIterations(8),
		positionIterations(3),
		drawShapes(1),
		drawJoints(1),
		drawAABBs(1),
		drawPairs(1),
		drawCOMs(1),
		drawStats(1),
		drawProfile(0),
		enableWarmStarting(1)
		{}

	b2Vec2  viewCenter;
	float32 hz;
	int32   velocityIterations;
	int32   positionIterations;
	int32   drawShapes;
	int32   drawJoints;
	int32   drawAABBs;
	int32   drawPairs;
	int32   drawCOMs;
	int32   drawStats;
	int32   drawProfile;
	int32   enableWarmStarting;
	int32   enableContinuous;
	int32   enableSubStepping;
};

#endif
