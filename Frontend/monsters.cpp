#include "inventory.h"
#include "monsters.h"
#include "frontend.h"
#include "weapons.h"
#include "structures.h"

double bearingOne(float one, float two) {
	double angle = atan(one - two);
	if (angle < 0.0) {
		angle += pi<float>() * 2;
	}
	return degrees(angle);
}

float bearingTwo(vec2 xyOne, vec2 xyTwo) {
	double angle = atan2(xyTwo.x - xyOne.x,
		xyTwo.y - xyOne.y);
	if (angle < 0.0) {
		angle += pi<float>() * 2;
	}
	return (float) degrees(angle);
}

int earthOne, earthTwo;

void monstersBegin() {
	// earth one
	vector<float> earthOneVerts = { 
		-0.191593f, 0.052576f, 1.00506f, 0.0960784f, 0.330392f, 0.213529f, 0.113036f, 0.000117f, 0.813226f, 0.0960784f, 0.330392f, 0.213529f, -0.191593f, 0.000117f, 1.00506f, 0.0960784f, 0.330392f, 0.213529f, 0.113036f, 0.052576f, 0.813226f, 0.112941f, 0.356667f, 0.210392f, 0.14099f, 0.000117f, 0.857616f, 0.112941f, 0.356667f, 0.210392f, 0.113036f, 0.000117f, 0.813226f, 0.112941f, 0.356667f, 0.210392f, 0.14099f, 0.052576f, 0.857616f, 0.0988235f, 0.318235f, 0.184902f, -0.163639f, 0.000117f, 1.04945f, 0.0988235f, 0.318235f, 0.184902f, 0.14099f, 0.000117f, 0.857616f, 0.0988235f, 0.318235f, 0.184902f, -0.163639f, 0.052576f, 1.04945f, 0.111373f, 0.355882f, 0.200588f, -0.191593f, 0.000117f, 1.00506f, 0.111373f, 0.355882f, 0.200588f, -0.163639f, 0.000117f, 1.04945f, 0.111373f, 0.355882f, 0.200588f, 0.14099f, 0.000117f, 0.857616f, 0.0956863f, 0.334706f, 0.191569f, -0.191593f, 0.000117f, 1.00506f, 0.0956863f, 0.334706f, 0.191569f, 0.113036f, 0.000117f, 0.813226f, 0.0956863f, 0.334706f, 0.191569f, 0.113036f, 0.052576f, 0.813226f, 0.123137f, 0.33902f, 0.171569f, -0.163639f, 0.052576f, 1.04945f, 0.123137f, 0.33902f, 0.171569f, 0.14099f, 0.052576f, 0.857616f, 0.123137f, 0.33902f, 0.171569f, -0.191593f, 0.052576f, 1.00506f, 0.0803922f, 0.35f, 0.172353f, 0.113036f, 0.052576f, 0.813226f, 0.0803922f, 0.35f, 0.172353f, 0.113036f, 0.000117f, 0.813226f, 0.0803922f, 0.35f, 0.172353f, 0.113036f, 0.052576f, 0.813226f, 0.129412f, 0.346078f, 0.180196f, 0.14099f, 0.052576f, 0.857616f, 0.129412f, 0.346078f, 0.180196f, 0.14099f, 0.000117f, 0.857616f, 0.129412f, 0.346078f, 0.180196f, 0.14099f, 0.052576f, 0.857616f, 0.094902f, 0.334706f, 0.178627f, -0.163639f, 0.052576f, 1.04945f, 0.094902f, 0.334706f, 0.178627f, -0.163639f, 0.000117f, 1.04945f, 0.094902f, 0.334706f, 0.178627f, -0.163639f, 0.052576f, 1.04945f, 0.0984314f, 0.322941f, 0.202549f, -0.191593f, 0.052576f, 1.00506f, 0.0984314f, 0.322941f, 0.202549f, -0.191593f, 0.000117f, 1.00506f, 0.0984314f, 0.322941f, 0.202549f, 0.14099f, 0.000117f, 0.857616f, 0.124314f, 0.313529f, 0.185294f, -0.163639f, 0.000117f, 1.04945f, 0.124314f, 0.313529f, 0.185294f, -0.191593f, 0.000117f, 1.00506f, 0.124314f, 0.313529f, 0.185294f, 0.113036f, 0.052576f, 0.813226f, 0.122353f, 0.345686f, 0.209216f, -0.191593f, 0.052576f, 1.00506f, 0.122353f, 0.345686f, 0.209216f, -0.163639f, 0.052576f, 1.04945f, 0.122353f, 0.345686f, 0.209216f, 0.11817f, 0.052576f, 0.871333f, 0.085098f, 0.313922f, 0.176667f, -0.200815f, 0.000117f, -0.12818f, 0.085098f, 0.313922f, 0.176667f, 0.11817f, 0.000117f, 0.871333f, 0.085098f, 0.313922f, 0.176667f, -0.200815f, 0.052576f, -0.12818f, 0.125098f, 0.310392f, 0.194314f, -0.150839f, 0.000117f, -0.144129f, 0.125098f, 0.310392f, 0.194314f, -0.200815f, 0.000117f, -0.12818f, 0.125098f, 0.310392f, 0.194314f, -0.150839f, 0.052576f, -0.144129f, 0.111373f, 0.351961f, 0.177843f, 0.168146f, 0.000117f, 0.855384f, 0.111373f, 0.351961f, 0.177843f, -0.150839f, 0.000117f, -0.144129f, 0.111373f, 0.351961f, 0.177843f, 0.168146f, 0.052576f, 0.855384f, 0.116078f, 0.32098f, 0.205686f, 0.11817f, 0.000117f, 0.871333f, 0.116078f, 0.32098f, 0.205686f, 0.168146f, 0.000117f, 0.855384f, 0.116078f, 0.32098f, 0.205686f, -0.150839f, 0.000117f, -0.144129f, 0.103137f, 0.313922f, 0.177843f, 0.11817f, 0.000117f, 0.871333f, 0.103137f, 0.313922f, 0.177843f, -0.200815f, 0.000117f, -0.12818f, 0.103137f, 0.313922f, 0.177843f, -0.200815f, 0.052576f, -0.12818f, 0.113333f, 0.31902f, 0.216275f, 0.168146f, 0.052576f, 0.855384f, 0.113333f, 0.31902f, 0.216275f, -0.150839f, 0.052576f, -0.144129f, 0.113333f, 0.31902f, 0.216275f, 0.11817f, 0.052576f, 0.871333f, 0.118431f, 0.322549f, 0.204118f, -0.200815f, 0.052576f, -0.12818f, 0.118431f, 0.322549f, 0.204118f, -0.200815f, 0.000117f, -0.12818f, 0.118431f, 0.322549f, 0.204118f, -0.200815f, 0.052576f, -0.12818f, 0.114902f, 0.348039f, 0.176275f, -0.150839f, 0.052576f, -0.144129f, 0.114902f, 0.348039f, 0.176275f, -0.150839f, 0.000117f, -0.144129f, 0.114902f, 0.348039f, 0.176275f, -0.150839f, 0.052576f, -0.144129f, 0.128627f, 0.354706f, 0.206078f, 0.168146f, 0.052576f, 0.855384f, 0.128627f, 0.354706f, 0.206078f, 0.168146f, 0.000117f, 0.855384f, 0.128627f, 0.354706f, 0.206078f, 0.168146f, 0.052576f, 0.855384f, 0.105098f, 0.323333f, 0.172745f, 0.11817f, 0.052576f, 0.871333f, 0.105098f, 0.323333f, 0.172745f, 0.11817f, 0.000117f, 0.871333f, 0.105098f, 0.323333f, 0.172745f, -0.150839f, 0.000117f, -0.144129f, 0.0890196f, 0.330392f, 0.204118f, 0.168146f, 0.000117f, 0.855384f, 0.0890196f, 0.330392f, 0.204118f, 0.11817f, 0.000117f, 0.871333f, 0.0890196f, 0.330392f, 0.204118f, -0.200815f, 0.052576f, -0.12818f, 0.0964706f, 0.31549f, 0.191961f, 0.11817f, 0.052576f, 0.871333f, 0.0964706f, 0.31549f, 0.191961f, 0.168146f, 0.052576f, 0.855384f, 0.0964706f, 0.31549f, 0.191961f, 0.0f, -0.025725f, 0.0f, 0.0858824f, 0.35902f, 0.214314f, 0.3618f, -0.004717f, 0.35749f, 0.0858824f, 0.35902f, 0.214314f, -0.138193f, -0.004717f, 0.578435f, 0.0858824f, 0.35902f, 0.214314f, 0.3618f, -0.004717f, 0.35749f, 0.123137f, 0.317843f, 0.214314f, 0.0f, -0.025725f, 0.0f, 0.123137f, 0.317843f, 0.214314f, 0.3618f, -0.004717f, -0.35749f, 0.123137f, 0.317843f, 0.214314f, 0.0f, -0.025725f, 0.0f, 0.0835294f, 0.356667f, 0.197843f, -0.138193f, -0.004717f, 0.578435f, 0.0835294f, 0.356667f, 0.197843f, -0.447212f, -0.004717f, 0.0f, 0.0835294f, 0.356667f, 0.197843f, 0.0f, -0.025725f, 0.0f, 0.0996078f, 0.341765f, 0.175882f, -0.447212f, -0.004717f, 0.0f, 0.0996078f, 0.341765f, 0.175882f, -0.138193f, -0.004717f, -0.578435f, 0.0996078f, 0.341765f, 0.175882f, 0.0f, -0.025725f, 0.0f, 0.114118f, 0.321373f, 0.175098f, -0.138193f, -0.004717f, -0.578435f, 0.114118f, 0.321373f, 0.175098f, 0.3618f, -0.004717f, -0.35749f, 0.114118f, 0.321373f, 0.175098f, 0.3618f, -0.004717f, 0.35749f, 0.0839216f, 0.34451f, 0.177451f, 0.3618f, -0.004717f, -0.35749f, 0.0839216f, 0.34451f, 0.177451f, 0.447212f, 0.310986f, 0.0f, 0.0839216f, 0.34451f, 0.177451f, -0.138193f, -0.004717f, 0.578435f, 0.100784f, 0.331176f, 0.215098f, 0.3618f, -0.004717f, 0.35749f, 0.100784f, 0.331176f, 0.215098f, 0.138193f, 0.310986f, 0.578435f, 0.100784f, 0.331176f, 0.215098f, -0.447212f, -0.004717f, 0.0f, 0.0878431f, 0.339804f, 0.199412f, -0.138193f, -0.004717f, 0.578435f, 0.0878431f, 0.339804f, 0.199412f, -0.3618f, 0.310986f, 0.35749f, 0.0878431f, 0.339804f, 0.199412f, -0.138193f, -0.004717f, -0.578435f, 0.12549f, 0.329608f, 0.197059f, -0.447212f, -0.004717f, 0.0f, 0.12549f, 0.329608f, 0.197059f, -0.3618f, 0.310986f, -0.35749f, 0.12549f, 0.329608f, 0.197059f, 0.3618f, -0.004717f, -0.35749f, 0.0921569f, 0.312745f, 0.17f, -0.138193f, -0.004717f, -0.578435f, 0.0921569f, 0.312745f, 0.17f, 0.138193f, 0.310986f, -0.578435f, 0.0921569f, 0.312745f, 0.17f, 0.3618f, -0.004717f, 0.35749f, 0.106275f, 0.336275f, 0.208431f, 0.447212f, 0.310986f, 0.0f, 0.106275f, 0.336275f, 0.208431f, 0.138193f, 0.310986f, 0.578435f, 0.106275f, 0.336275f, 0.208431f, -0.138193f, -0.004717f, 0.578435f, 0.0894118f, 0.312745f, 0.177843f, 0.138193f, 0.310986f, 0.578435f, 0.0894118f, 0.312745f, 0.177843f, -0.3618f, 0.310986f, 0.35749f, 0.0894118f, 0.312745f, 0.177843f, -0.447212f, -0.004717f, 0.0f, 0.0996078f, 0.323333f, 0.196667f, -0.3618f, 0.310986f, 0.35749f, 0.0996078f, 0.323333f, 0.196667f, -0.3618f, 0.310986f, -0.35749f, 0.0996078f, 0.323333f, 0.196667f, -0.138193f, -0.004717f, -0.578435f, 0.0968627f, 0.31f, 0.178627f, -0.3618f, 0.310986f, -0.35749f, 0.0968627f, 0.31f, 0.178627f, 0.138193f, 0.310986f, -0.578435f, 0.0968627f, 0.31f, 0.178627f, 0.3618f, -0.004717f, -0.35749f, 0.0984314f, 0.354706f, 0.211961f, 0.138193f, 0.310986f, -0.578435f, 0.0984314f, 0.354706f, 0.211961f, 0.447212f, 0.310986f, 0.0f, 0.0984314f, 0.354706f, 0.211961f, 0.138193f, 0.310986f, 0.578435f, 0.11098f, 0.316667f, 0.170784f, 0.447212f, 0.310986f, 0.0f, 0.11098f, 0.316667f, 0.170784f, 0.0f, 0.480642f, 0.0f, 0.11098f, 0.316667f, 0.170784f, -0.3618f, 0.310986f, 0.35749f, 0.123137f, 0.342157f, 0.176667f, 0.138193f, 0.310986f, 0.578435f, 0.123137f, 0.342157f, 0.176667f, 0.0f, 0.480642f, 0.0f, 0.123137f, 0.342157f, 0.176667f, -0.3618f, 0.310986f, -0.35749f, 0.116863f, 0.311176f, 0.170784f, -0.3618f, 0.310986f, 0.35749f, 0.116863f, 0.311176f, 0.170784f, 0.0f, 0.480642f, 0.0f, 0.116863f, 0.311176f, 0.170784f, 0.138193f, 0.310986f, -0.578435f, 0.0929412f, 0.341373f, 0.206863f, -0.3618f, 0.310986f, -0.35749f, 0.0929412f, 0.341373f, 0.206863f, 0.0f, 0.480642f, 0.0f, 0.0929412f, 0.341373f, 0.206863f, 0.447212f, 0.310986f, 0.0f, 0.124706f, 0.327647f, 0.172745f, 0.138193f, 0.310986f, -0.578435f, 0.124706f, 0.327647f, 0.172745f, 0.0f, 0.480642f, 0.0f, 0.124706f, 0.327647f, 0.172745f
	};
	earthOne = createMonsterTemplate(EARTH_ONE, earthOneVerts, 20.0f, 4.0f, 2.0f, EARTH_ONE_DROP_INDEX, EARTH_ONE_DROP_CHANCE, vec3(EARTH_ONE_SCALE)); // mouse thing
	// earth two
	vector<float> earthTwoVerts = { 
		-0.576064f, 0.08198f, 0.079887f, 0.0960784f, 0.330392f, 0.213529f, -0.551547f, 0.050374f, -0.080113f, 0.0960784f, 0.330392f, 0.213529f, -0.551547f, 0.050374f, 0.079887f, 0.0960784f, 0.330392f, 0.213529f, -0.551547f, 0.050374f, -0.080113f, 0.112941f, 0.356667f, 0.210392f, -0.433836f, 0.192304f, -0.080113f, 0.112941f, 0.356667f, 0.210392f, -0.409319f, 0.160697f, -0.080113f, 0.112941f, 0.356667f, 0.210392f, -0.433836f, 0.192304f, -0.080113f, 0.0988235f, 0.318235f, 0.184902f, -0.409319f, 0.160697f, -0.000113f, 0.0988235f, 0.318235f, 0.184902f, -0.409319f, 0.160697f, -0.080113f, 0.0988235f, 0.318235f, 0.184902f, -0.433836f, 0.192304f, -0.000113f, 0.111373f, 0.355882f, 0.200588f, -0.551547f, 0.050374f, 0.079887f, 0.111373f, 0.355882f, 0.200588f, -0.409319f, 0.160697f, -0.000113f, 0.111373f, 0.355882f, 0.200588f, -0.551547f, 0.050374f, -0.080113f, 0.0956863f, 0.334706f, 0.191569f, -0.409319f, 0.160697f, -0.000113f, 0.0956863f, 0.334706f, 0.191569f, -0.551547f, 0.050374f, 0.079887f, 0.0956863f, 0.334706f, 0.191569f, -0.576064f, 0.08198f, -0.080113f, 0.123137f, 0.33902f, 0.171569f, -0.433836f, 0.192304f, -0.000113f, 0.123137f, 0.33902f, 0.171569f, -0.433836f, 0.192304f, -0.080113f, 0.123137f, 0.33902f, 0.171569f, -0.576064f, 0.08198f, 0.079887f, 0.0803922f, 0.35f, 0.172353f, -0.576064f, 0.08198f, -0.080113f, 0.0803922f, 0.35f, 0.172353f, -0.551547f, 0.050374f, -0.080113f, 0.0803922f, 0.35f, 0.172353f, -0.551547f, 0.050374f, -0.080113f, 0.129412f, 0.346078f, 0.180196f, -0.576064f, 0.08198f, -0.080113f, 0.129412f, 0.346078f, 0.180196f, -0.433836f, 0.192304f, -0.080113f, 0.129412f, 0.346078f, 0.180196f, -0.433836f, 0.192304f, -0.080113f, 0.094902f, 0.334706f, 0.178627f, -0.433836f, 0.192304f, -0.000113f, 0.094902f, 0.334706f, 0.178627f, -0.409319f, 0.160697f, -0.000113f, 0.094902f, 0.334706f, 0.178627f, -0.433836f, 0.192304f, -0.000113f, 0.0984314f, 0.322941f, 0.202549f, -0.576064f, 0.08198f, 0.079887f, 0.0984314f, 0.322941f, 0.202549f, -0.551547f, 0.050374f, 0.079887f, 0.0984314f, 0.322941f, 0.202549f, -0.551547f, 0.050374f, -0.080113f, 0.124314f, 0.313529f, 0.185294f, -0.409319f, 0.160697f, -0.080113f, 0.124314f, 0.313529f, 0.185294f, -0.409319f, 0.160697f, -0.000113f, 0.124314f, 0.313529f, 0.185294f, -0.576064f, 0.08198f, -0.080113f, 0.122353f, 0.345686f, 0.209216f, -0.576064f, 0.08198f, 0.079887f, 0.122353f, 0.345686f, 0.209216f, -0.433836f, 0.192304f, -0.000113f, 0.122353f, 0.345686f, 0.209216f, 0.584963f, 0.08198f, -0.08f, 0.085098f, 0.313922f, 0.176667f, 0.560447f, 0.050374f, 0.08f, 0.085098f, 0.313922f, 0.176667f, 0.560447f, 0.050374f, -0.08f, 0.085098f, 0.313922f, 0.176667f, 0.584963f, 0.08198f, 0.08f, 0.125098f, 0.310392f, 0.194314f, 0.418219f, 0.160697f, 0.0f, 0.125098f, 0.310392f, 0.194314f, 0.560447f, 0.050374f, 0.08f, 0.125098f, 0.310392f, 0.194314f, 0.442735f, 0.192304f, 0.0f, 0.111373f, 0.351961f, 0.177843f, 0.418219f, 0.160697f, -0.08f, 0.111373f, 0.351961f, 0.177843f, 0.418219f, 0.160697f, 0.0f, 0.111373f, 0.351961f, 0.177843f, 0.418219f, 0.160697f, -0.08f, 0.116078f, 0.32098f, 0.205686f, 0.584963f, 0.08198f, -0.08f, 0.116078f, 0.32098f, 0.205686f, 0.560447f, 0.050374f, -0.08f, 0.116078f, 0.32098f, 0.205686f, 0.418219f, 0.160697f, 0.0f, 0.103137f, 0.313922f, 0.177843f, 0.560447f, 0.050374f, -0.08f, 0.103137f, 0.313922f, 0.177843f, 0.560447f, 0.050374f, 0.08f, 0.103137f, 0.313922f, 0.177843f, 0.442735f, 0.192304f, 0.0f, 0.113333f, 0.31902f, 0.216275f, 0.584963f, 0.08198f, -0.08f, 0.113333f, 0.31902f, 0.216275f, 0.442735f, 0.192304f, -0.08f, 0.113333f, 0.31902f, 0.216275f, 0.584963f, 0.08198f, -0.08f, 0.118431f, 0.322549f, 0.204118f, 0.584963f, 0.08198f, 0.08f, 0.118431f, 0.322549f, 0.204118f, 0.560447f, 0.050374f, 0.08f, 0.118431f, 0.322549f, 0.204118f, 0.584963f, 0.08198f, 0.08f, 0.114902f, 0.348039f, 0.176275f, 0.442735f, 0.192304f, 0.0f, 0.114902f, 0.348039f, 0.176275f, 0.418219f, 0.160697f, 0.0f, 0.114902f, 0.348039f, 0.176275f, 0.442735f, 0.192304f, 0.0f, 0.128627f, 0.354706f, 0.206078f, 0.442735f, 0.192304f, -0.08f, 0.128627f, 0.354706f, 0.206078f, 0.418219f, 0.160697f, -0.08f, 0.128627f, 0.354706f, 0.206078f, 0.418219f, 0.160697f, -0.08f, 0.105098f, 0.323333f, 0.172745f, 0.442735f, 0.192304f, -0.08f, 0.105098f, 0.323333f, 0.172745f, 0.584963f, 0.08198f, -0.08f, 0.105098f, 0.323333f, 0.172745f, 0.418219f, 0.160697f, 0.0f, 0.0890196f, 0.330392f, 0.204118f, 0.418219f, 0.160697f, -0.08f, 0.0890196f, 0.330392f, 0.204118f, 0.560447f, 0.050374f, -0.08f, 0.0890196f, 0.330392f, 0.204118f, 0.442735f, 0.192304f, 0.0f, 0.0964706f, 0.31549f, 0.191961f, 0.584963f, 0.08198f, 0.08f, 0.0964706f, 0.31549f, 0.191961f, 0.584963f, 0.08198f, -0.08f, 0.0964706f, 0.31549f, 0.191961f, -0.433223f, 0.190302f, 0.0f, 0.0858824f, 0.35902f, 0.214314f, -0.45774f, 0.158696f, -0.08f, 0.0858824f, 0.35902f, 0.214314f, -0.45774f, 0.158696f, 0.0f, 0.0858824f, 0.35902f, 0.214314f, -0.45774f, 0.158696f, -0.08f, 0.123137f, 0.317843f, 0.214314f, -0.148768f, -0.030344f, -0.08f, 0.123137f, 0.317843f, 0.214314f, -0.173284f, -0.061951f, -0.08f, 0.123137f, 0.317843f, 0.214314f, -0.148768f, -0.030344f, -0.08f, 0.0835294f, 0.356667f, 0.197843f, -0.173284f, -0.061951f, 0.08f, 0.0835294f, 0.356667f, 0.197843f, -0.173284f, -0.061951f, -0.08f, 0.0835294f, 0.356667f, 0.197843f, -0.148768f, -0.030344f, 0.08f, 0.0996078f, 0.341765f, 0.175882f, -0.45774f, 0.158696f, 0.0f, 0.0996078f, 0.341765f, 0.175882f, -0.173284f, -0.061951f, 0.08f, 0.0996078f, 0.341765f, 0.175882f, -0.173284f, -0.061951f, -0.08f, 0.114118f, 0.321373f, 0.175098f, -0.45774f, 0.158696f, 0.0f, 0.114118f, 0.321373f, 0.175098f, -0.45774f, 0.158696f, -0.08f, 0.114118f, 0.321373f, 0.175098f, -0.148768f, -0.030344f, -0.08f, 0.0839216f, 0.34451f, 0.177451f, -0.433223f, 0.190302f, 0.0f, 0.0839216f, 0.34451f, 0.177451f, -0.148768f, -0.030344f, 0.08f, 0.0839216f, 0.34451f, 0.177451f, -0.433223f, 0.190302f, 0.0f, 0.100784f, 0.331176f, 0.215098f, -0.433223f, 0.190302f, -0.08f, 0.100784f, 0.331176f, 0.215098f, -0.45774f, 0.158696f, -0.08f, 0.100784f, 0.331176f, 0.215098f, -0.45774f, 0.158696f, -0.08f, 0.0878431f, 0.339804f, 0.199412f, -0.433223f, 0.190302f, -0.08f, 0.0878431f, 0.339804f, 0.199412f, -0.148768f, -0.030344f, -0.08f, 0.0878431f, 0.339804f, 0.199412f, -0.148768f, -0.030344f, -0.08f, 0.12549f, 0.329608f, 0.197059f, -0.148768f, -0.030344f, 0.08f, 0.12549f, 0.329608f, 0.197059f, -0.173284f, -0.061951f, 0.08f, 0.12549f, 0.329608f, 0.197059f, -0.148768f, -0.030344f, 0.08f, 0.0921569f, 0.312745f, 0.17f, -0.433223f, 0.190302f, 0.0f, 0.0921569f, 0.312745f, 0.17f, -0.45774f, 0.158696f, 0.0f, 0.0921569f, 0.312745f, 0.17f, -0.173284f, -0.061951f, -0.08f, 0.106275f, 0.336275f, 0.208431f, -0.173284f, -0.061951f, 0.08f, 0.106275f, 0.336275f, 0.208431f, -0.45774f, 0.158696f, 0.0f, 0.106275f, 0.336275f, 0.208431f, -0.148768f, -0.030344f, -0.08f, 0.0894118f, 0.312745f, 0.177843f, -0.433223f, 0.190302f, -0.08f, 0.0894118f, 0.312745f, 0.177843f, -0.433223f, 0.190302f, 0.0f, 0.0894118f, 0.312745f, 0.177843f, 0.0f, -0.262674f, -0.0f, 0.0996078f, 0.323333f, 0.196667f, 0.190071f, -0.117472f, 0.138093f, 0.0996078f, 0.323333f, 0.196667f, -0.072599f, -0.117472f, 0.223441f, 0.0996078f, 0.323333f, 0.196667f, 0.190071f, -0.117472f, 0.138093f, 0.0968627f, 0.31f, 0.178627f, 0.0f, -0.262674f, -0.0f, 0.0968627f, 0.31f, 0.178627f, 0.190071f, -0.117472f, -0.138093f, 0.0968627f, 0.31f, 0.178627f, 0.0f, -0.262674f, -0.0f, 0.0984314f, 0.354706f, 0.211961f, -0.072599f, -0.117472f, 0.223441f, 0.0984314f, 0.354706f, 0.211961f, -0.234942f, -0.117472f, -0.0f, 0.0984314f, 0.354706f, 0.211961f, 0.0f, -0.262674f, -0.0f, 0.11098f, 0.316667f, 0.170784f, -0.234942f, -0.117472f, -0.0f, 0.11098f, 0.316667f, 0.170784f, -0.072599f, -0.117472f, -0.223441f, 0.11098f, 0.316667f, 0.170784f, 0.0f, -0.262674f, -0.0f, 0.123137f, 0.342157f, 0.176667f, -0.072599f, -0.117472f, -0.223441f, 0.123137f, 0.342157f, 0.176667f, 0.190071f, -0.117472f, -0.138093f, 0.123137f, 0.342157f, 0.176667f, 0.190071f, -0.117472f, 0.138093f, 0.116863f, 0.311176f, 0.170784f, 0.190071f, -0.117472f, -0.138093f, 0.116863f, 0.311176f, 0.170784f, 0.234942f, 0.117472f, 0.0f, 0.116863f, 0.311176f, 0.170784f, -0.072599f, -0.117472f, 0.223441f, 0.0929412f, 0.341373f, 0.206863f, 0.190071f, -0.117472f, 0.138093f, 0.0929412f, 0.341373f, 0.206863f, 0.072599f, 0.117472f, 0.223441f, 0.0929412f, 0.341373f, 0.206863f, -0.234942f, -0.117472f, -0.0f, 0.124706f, 0.327647f, 0.172745f, -0.072599f, -0.117472f, 0.223441f, 0.124706f, 0.327647f, 0.172745f, -0.190071f, 0.117472f, 0.138093f, 0.124706f, 0.327647f, 0.172745f, -0.072599f, -0.117472f, -0.223441f, 0.0996078f, 0.312745f, 0.218235f, -0.234942f, -0.117472f, -0.0f, 0.0996078f, 0.312745f, 0.218235f, -0.190071f, 0.117472f, -0.138093f, 0.0996078f, 0.312745f, 0.218235f, 0.190071f, -0.117472f, -0.138093f, 0.0803922f, 0.356667f, 0.206078f, -0.072599f, -0.117472f, -0.223441f, 0.0803922f, 0.356667f, 0.206078f, 0.072599f, 0.117472f, -0.223441f, 0.0803922f, 0.356667f, 0.206078f, 0.190071f, -0.117472f, 0.138093f, 0.119216f, 0.323333f, 0.205294f, 0.234942f, 0.117472f, 0.0f, 0.119216f, 0.323333f, 0.205294f, 0.072599f, 0.117472f, 0.223441f, 0.119216f, 0.323333f, 0.205294f, -0.072599f, -0.117472f, 0.223441f, 0.117255f, 0.336275f, 0.207647f, 0.072599f, 0.117472f, 0.223441f, 0.117255f, 0.336275f, 0.207647f, -0.190071f, 0.117472f, 0.138093f, 0.117255f, 0.336275f, 0.207647f, -0.234942f, -0.117472f, -0.0f, 0.113725f, 0.317451f, 0.200588f, -0.190071f, 0.117472f, 0.138093f, 0.113725f, 0.317451f, 0.200588f, -0.190071f, 0.117472f, -0.138093f, 0.113725f, 0.317451f, 0.200588f, -0.072599f, -0.117472f, -0.223441f, 0.121569f, 0.345686f, 0.188039f, -0.190071f, 0.117472f, -0.138093f, 0.121569f, 0.345686f, 0.188039f, 0.072599f, 0.117472f, -0.223441f, 0.121569f, 0.345686f, 0.188039f, 0.190071f, -0.117472f, -0.138093f, 0.108235f, 0.331176f, 0.19902f, 0.072599f, 0.117472f, -0.223441f, 0.108235f, 0.331176f, 0.19902f, 0.234942f, 0.117472f, 0.0f, 0.108235f, 0.331176f, 0.19902f, 0.072599f, 0.117472f, 0.223441f, 0.109804f, 0.321765f, 0.203333f, 0.234942f, 0.117472f, 0.0f, 0.109804f, 0.321765f, 0.203333f, 0.0f, 0.262674f, 0.0f, 0.109804f, 0.321765f, 0.203333f, -0.190071f, 0.117472f, 0.138093f, 0.0843137f, 0.34098f, 0.206471f, 0.072599f, 0.117472f, 0.223441f, 0.0843137f, 0.34098f, 0.206471f, 0.0f, 0.262674f, 0.0f, 0.0843137f, 0.34098f, 0.206471f, -0.190071f, 0.117472f, -0.138093f, 0.0815686f, 0.314314f, 0.173529f, -0.190071f, 0.117472f, 0.138093f, 0.0815686f, 0.314314f, 0.173529f, 0.0f, 0.262674f, 0.0f, 0.0815686f, 0.314314f, 0.173529f, 0.072599f, 0.117472f, -0.223441f, 0.10549f, 0.345294f, 0.203333f, -0.190071f, 0.117472f, -0.138093f, 0.10549f, 0.345294f, 0.203333f, 0.0f, 0.262674f, 0.0f, 0.10549f, 0.345294f, 0.203333f, 0.234942f, 0.117472f, 0.0f, 0.0823529f, 0.324118f, 0.210784f, 0.072599f, 0.117472f, -0.223441f, 0.0823529f, 0.324118f, 0.210784f, 0.0f, 0.262674f, 0.0f, 0.0823529f, 0.324118f, 0.210784f, 0.443826f, 0.190302f, -0.08f, 0.0980392f, 0.346471f, 0.171569f, 0.468343f, 0.158696f, 0.0f, 0.0980392f, 0.346471f, 0.171569f, 0.468343f, 0.158696f, -0.08f, 0.0980392f, 0.346471f, 0.171569f, 0.468343f, 0.158696f, 0.0f, 0.12f, 0.350392f, 0.208431f, 0.159371f, -0.030344f, 0.08f, 0.12f, 0.350392f, 0.208431f, 0.183887f, -0.061951f, 0.08f, 0.12f, 0.350392f, 0.208431f, 0.159371f, -0.030344f, 0.08f, 0.0894118f, 0.317059f, 0.186863f, 0.183887f, -0.061951f, -0.08f, 0.0894118f, 0.317059f, 0.186863f, 0.183887f, -0.061951f, 0.08f, 0.0894118f, 0.317059f, 0.186863f, 0.159371f, -0.030344f, -0.08f, 0.113333f, 0.353137f, 0.209216f, 0.468343f, 0.158696f, -0.08f, 0.113333f, 0.353137f, 0.209216f, 0.183887f, -0.061951f, -0.08f, 0.113333f, 0.353137f, 0.209216f, 0.468343f, 0.158696f, 0.0f, 0.0901961f, 0.323725f, 0.190784f, 0.183887f, -0.061951f, -0.08f, 0.0901961f, 0.323725f, 0.190784f, 0.468343f, 0.158696f, -0.08f, 0.0901961f, 0.323725f, 0.190784f, 0.443826f, 0.190302f, 0.0f, 0.116863f, 0.310784f, 0.21902f, 0.159371f, -0.030344f, -0.08f, 0.116863f, 0.310784f, 0.21902f, 0.159371f, -0.030344f, 0.08f, 0.116863f, 0.310784f, 0.21902f, 0.443826f, 0.190302f, -0.08f, 0.0964706f, 0.351176f, 0.206471f, 0.443826f, 0.190302f, 0.0f, 0.0964706f, 0.351176f, 0.206471f, 0.468343f, 0.158696f, 0.0f, 0.0964706f, 0.351176f, 0.206471f, 0.468343f, 0.158696f, 0.0f, 0.113725f, 0.327647f, 0.183333f, 0.443826f, 0.190302f, 0.0f, 0.113725f, 0.327647f, 0.183333f, 0.159371f, -0.030344f, 0.08f, 0.113725f, 0.327647f, 0.183333f, 0.159371f, -0.030344f, 0.08f, 0.0839216f, 0.311569f, 0.180588f, 0.159371f, -0.030344f, -0.08f, 0.0839216f, 0.311569f, 0.180588f, 0.183887f, -0.061951f, -0.08f, 0.0839216f, 0.311569f, 0.180588f, 0.159371f, -0.030344f, -0.08f, 0.0831373f, 0.343726f, 0.175098f, 0.443826f, 0.190302f, -0.08f, 0.0831373f, 0.343726f, 0.175098f, 0.468343f, 0.158696f, -0.08f, 0.0831373f, 0.343726f, 0.175098f, 0.468343f, 0.158696f, 0.0f, 0.111373f, 0.35549f, 0.203333f, 0.183887f, -0.061951f, 0.08f, 0.111373f, 0.35549f, 0.203333f, 0.183887f, -0.061951f, -0.08f, 0.111373f, 0.35549f, 0.203333f, 0.443826f, 0.190302f, 0.0f, 0.128235f, 0.314706f, 0.190392f, 0.443826f, 0.190302f, -0.08f, 0.128235f, 0.314706f, 0.190392f, 0.159371f, -0.030344f, -0.08f, 0.128235f, 0.314706f, 0.190392f 
	};
	earthTwo = createMonsterTemplate(EARTH_TWO, earthTwoVerts, 80.0f, 8.0f, 3.0f, EARTH_TWO_DROP_INDEX, EARTH_TWO_DROP_CHANCE, vec3(EARTH_TWO_SCALE)); // punchy boi
	getMonsters();
}

void monsterMonsterColliders(){
	float distanceFromMonster = MONSTER_TO_MONSTER_COLLIDER_DISTANCE;
	int mCount = allMonsters.size();
	for(int m = 0; m < mCount; m++){
		vec3 positionOne = allMonsters[m].position;
		vec2 positionOneFloor = vec2(positionOne.x, positionOne.z);
		for(int m2 = 0; m2 < mCount; m2++){
			if(m2==m || allMonsters[m].attacking || allMonsters[m2].attacking){
				continue;
			}
			vec3 positionTwo = allMonsters[m2].position;
			vec2 positionTwoFloor = vec2(positionTwo.x, positionTwo.z);
			
			if(positionOne.z >= positionTwo.z - distanceFromMonster && positionOne.z <= positionTwo.z + distanceFromMonster){
				if(positionOne.x >= positionTwo.x && positionOne.x < positionTwo.x + distanceFromMonster){
					allMonsters[m].position.x = positionTwo.x + distanceFromMonster;
				}
				if(positionOne.x < positionTwo.x && positionOne.x > positionTwo.x - distanceFromMonster){
					allMonsters[m].position.x = positionTwo.x - distanceFromMonster;
				}
			}
			if(positionOne.x >= positionTwo.x - distanceFromMonster && positionOne.x <= positionTwo.x + distanceFromMonster){
				if(positionOne.z >= positionTwo.z && positionOne.z < positionTwo.z + distanceFromMonster){
					allMonsters[m].position.z = positionTwo.z + distanceFromMonster;
				}
				if(positionOne.z < positionTwo.z && positionOne.z > positionTwo.z - distanceFromMonster){
					allMonsters[m].position.z = positionTwo.z - distanceFromMonster;
				}
			}
		}
	}
}

void monstersMainloop() {
	if (!WorldGeneration.startedBegin) {
		return;
	}
	monsterSpawning();
	monsterInteractions();
	monsterMonsterColliders();
	monsterBuildCollisions();
	monsterTerrainCollisions();
	monsterDamage();
	renderMonsters();
}

void monsterBuildCollisions(){
	int mCount = allMonsters.size();
	for(int m = 0; m < mCount; m++){
		int waste = 0; float wasteF = 0.0f; bool wasteB = false; bool wasteB2 = false;
		buildCollisions(allMonsters[m].position, waste, wasteF, wasteB, wasteB2);
	}
}

void monsterTerrainCollisions(){
	int mCount = allMonsters.size();
	for(int m = 0; m < mCount; m++){
		vec3 monsterPos = allMonsters[m].position;
		vec2 monsterFloorPos = vec2(monsterPos.x, monsterPos.z);
		vec3 monsterThree = vec3(monsterFloorPos.x, 0.0f, monsterFloorPos.y);

		int monsterType = allMonsters[m].monsterName;
		float added = 0.0f;
		if(monsterType == EARTH_TWO){
			added = 4.0f;
		}
		vec4 monsterCollide = terrainColliders(monsterPos, added);
		if(allMonsters[m].position.y != monsterCollide.y){
			allMonsters[m].position.y = monsterCollide.y;
		}

		// area limit colliders
		if(allMonsters[m].position.x < 0.0f){
			allMonsters[m].position.x = 0.0f;
		}
		if(allMonsters[m].position.x > currentPlanetScale.x){
			allMonsters[m].position.x = currentPlanetScale.x;
		}
		if(allMonsters[m].position.z > 0.0f){
			allMonsters[m].position.z = 0.0f;
		}
		if(allMonsters[m].position.z < -currentPlanetScale.y){
			allMonsters[m].position.z = -currentPlanetScale.y;
		}
	}
}

vector<monster> allMonsterTemplates, allMonsters;
int createMonsterTemplate(int name, vector<float> vertices,
		float health, float damage, float attackDelayChance, int dropIndex, int dropChance, vec3 scale) {
	monster newMonster(name, vertices);
	newMonster.scale = scale;
	newMonster.health = health;
	newMonster.damage = damage;
	newMonster.itemDropChance = dropChance;
	newMonster.itemIndex = dropIndex;
	int size = allMonsterTemplates.size();
	allMonsterTemplates[newVectorPos(&allMonsterTemplates)] = newMonster;
	return size;
}

monster::monster(int name, vector<float> vertices){
	monsterName = name;
	startIrregularColorBuilding(vertices, VAO,
		VBO, size);
}

void monster::render(){
	glUseProgram(playerShader);

	setMat4(playerShader, "model", modelMatrix(position, rotation, scale));
	setMat4(playerShader, "projection", projectionMatrix());
	setMat4(playerShader, "view", viewMatrix());

	setShaderVecThree(playerShader, "multiplyColour", multiplyColour);
	setShaderVecThree(playerShader, "lightPos", lightPos);
	setShaderFloat(playerShader, "lightIntensity", lightIntensity);
	setShaderFloat(playerShader, "lightRadius", lightRadius);
	setShaderInt(playerShader, "useLight", 1);
	setShaderFloat(playerShader, "lowestLight", lowestLight);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, size);
	glLinkProgram(playerShader);
}

void monsterSpawning() {
	if (lowestLight < MONSTER_LIGHT_SPAWN_MAX) {
		if (lightPos.x > 0 && lightPos.x < currentPlanetScale.x) {
			return; // center of light still on surface
		}
		int monsterCount = allMonsters.size();
		// can spawn
		if (monsterCount < MONSTER_MAX_COUNT) {
			int random = randomInt(1, MONSTER_SPAWN_RATE);
			if (random == 1) {
				int type = randomInt(0, 1);
				int newMonster = createMonster(type + WorldGeneration.currentArea);

				vec2 scale = WorldGeneration.currentAreaScale;
				int posX = randomInt(0, (int) scale.x);
				int posZ = -randomInt(0, (int) scale.y);

				allMonsters[newMonster].position = vec3(posX, 3.0f, posZ);
			}
		}
	}
}

float redTime = DAMAGE_SHOW_RED_TIME;
void monsterDamage(){
	int mCount = allMonsters.size();
	for(int m = 0; m < mCount; m++){
		vec3 monsterPos = allMonsters[m].position;
		int type = allMonsters[m].monsterName;
		
		float delay = allMonsters[m].changeBackDelay;
		if(delay < 0 && allMonsters[m].health > 0){
			allMonsters[m].multiplyColour = vec3(1.0f);
		}
		allMonsters[m].changeBackDelay -= deltaTime;

		int bCount = allBullets.size();
		vector<int> bulletCollisionIndexes;
		for(int b = 0; b < bCount; b++){
			vec3 bulletPos = allBullets[b].position;
			if(type == 1){ // MOUSE
				float distance = glm::distance(bulletPos, monsterPos);
				if(distance < EARTH_ONE_BULLET_DISTANCE){
					// collision
					allMonsters[m].health -= allBullets[b].damage;
					allMonsters[m].multiplyColour = vec3(2.5f, 0.2f, 0.2f);
					allMonsters[m].changeBackDelay = redTime;
					bulletCollisionIndexes[newVectorPos(&bulletCollisionIndexes)] = b;
				}
			}
			if(type == 2){ // PUNCHY
			 	 // center part
				float distance = glm::distance(bulletPos, monsterPos - vec3(0.0f, 0.15f, 0.0f));
				if(distance < EARTH_TWO_BULLET_DISTANCE_CENTER){
					// collision
					allMonsters[m].health -= allBullets[b].damage;
					allMonsters[m].multiplyColour = vec3(2.5f, 0.2f, 0.2f);
					allMonsters[m].changeBackDelay = redTime;
					bulletCollisionIndexes[newVectorPos(&bulletCollisionIndexes)] = b;
					continue;
				}
				// arms are at rotation - 90 & rotation + 90
				float rotationOne = degreesClamp(allMonsters[m].rotation.y - 90.0f);
				float rotationTwo = degreesClamp(allMonsters[m].rotation.y + 90.0f);
				float usedBearing = degreesClamp(bearingTwo(vec2(monsterPos.x, monsterPos.z), vec2(bulletPos.x, bulletPos.z)));
				
				float armOneDistance = glm::distance(rotationOne, usedBearing);
				float armTwoDistance = glm::distance(rotationTwo, usedBearing);
				if(armOneDistance < EARTH_TWO_ARM_WIDTH || armTwoDistance < EARTH_TWO_ARM_WIDTH){
					// inside width of arm
					if(distance < EARTH_TWO_BULLET_DISTANCE_CENTER + EARTH_TWO_ARM_LENGTH){
						// collision
						allMonsters[m].health -= allBullets[b].damage;
						allMonsters[m].multiplyColour = vec3(2.5f, 0.2f, 0.2f);
						allMonsters[m].changeBackDelay = redTime;
						bulletCollisionIndexes[newVectorPos(&bulletCollisionIndexes)] = b;
					}
				}
			}
		}

		int oldBulletsCount = bulletCollisionIndexes.size();
		for(int b1 = 0; b1 < oldBulletsCount; b1++){
			removeBullet(bulletCollisionIndexes[b1]);
		}
	}
}

void getMonsters() {
	vector<string> lines = readLines(WorldGeneration.worldLinesPath);
	int count = lines.size();
	for (int l = 0; l < count; l++) {
		string line = lines[l];
		if (contains(line, WorldGeneration.currentAreaPrefix + "Monster")) {
			vec4 monsterData = getVec4File(WorldGeneration.worldLinesPath, 
				WorldGeneration.currentAreaPrefix + "Monster", l);
			int newMonster = createMonster((int) monsterData.w - 1);
			allMonsters[newMonster].position = vec3(monsterData);
		}
	}
}

int createMonster(int monsterType) {
	monster newMonster = allMonsterTemplates[monsterType];
	allMonsters[newVectorPos(&allMonsters)] = newMonster;
	return allMonsters.size() - 1;
}

void renderMonsters() {
	int mCount = allMonsters.size();
	for (int m = 0; m < mCount; m++) {
		allMonsters[m].render();
	}
}

void monsterInteractions() {
	vec2 playerPos = vec2(mainPlayer.position.x, mainPlayer.position.z);
	vector<int> deadMonsters;

	int mCount = allMonsters.size();
	for (int m = 0; m < mCount; m++) {
		vec3 monsterPos = allMonsters[m].position;
		float distance = glm::distance(playerPos, vec2(monsterPos.x, monsterPos.z));
		if (distance < MONSTER_ROTATE_DISTANCE_MAX && distance > MONSTER_MOVE_ROTATE_DISTANCE_MIN) {
			float bearingAngle = bearingTwo(playerPos,
			vec2(monsterPos.x, monsterPos.z));
			allMonsters[m].rotation.y = bearingAngle;
		}
		if(!allMonsters[m].attacking){
			if (distance < MONSTER_MOVE_DISTANCE_MAX && distance > MONSTER_MOVE_ROTATE_DISTANCE_MIN) {
				float xGap = -sin(radians(allMonsters[m].rotation.y));
				float zGap = -cos(radians(allMonsters[m].rotation.y));
				vec3 gap = vec3(xGap, 0.0f, zGap) * vec3((float)deltaTime) * vec3(allMonsters[m].speed);
				allMonsters[m].position += gap;
			}
			// damaging
			if(distance < MONSTER_MOVE_DISTANCE_MAX){
				if(allMonsters[m].attackChanceDelay < 0.0f){
					allMonsters[m].attackChanceDelay = allMonsters[m].defaultAttackDelay;
					int chance = randomInt(1, 2);
					if(chance == 1){
						allMonsters[m].attacking = true;
					}
				}
			}
		}
		if(allMonsters[m].health < 1){
			monsterItemDrops(allMonsters[m]);
			allMonsters[m].multiplyColour = vec3(2.5f, 0.2f, 0.2f);
			allMonsters[m].rotation.x += deltaTime * DYING_ROTATE_SPEED;
			if(allMonsters[m].rotation.x > 90.0f){
				deadMonsters[newVectorPos(&deadMonsters)] = m;
			}
		}
		if(allMonsters[m].attacking && mainPlayer.health > 0 && !mainPlayer.respawning){
			vec2 monsterTwo = vec2(monsterPos.x, monsterPos.z);
			float bearing = bearingTwo(playerPos, monsterTwo);

			distance = glm::distance(playerPos, monsterTwo);
			if(!allMonsters[m].finishedAttackOne){
				distance -= deltaTime * MONSTER_ATTACK_SPEED;
				if(distance < MONSTER_ATTACK_CLOSEST_DISTANCE){
					allMonsters[m].finishedAttackOne = true;
					mainPlayer.health -= allMonsters[m].damage;
					mainPlayer.multiplyColour = vec3(2.5f, 0.2f, 0.2f);
					mainPlayer.redDelay = DAMAGE_SHOW_RED_TIME;
				}
			}
			if(allMonsters[m].finishedAttackOne){
				distance += deltaTime * MONSTER_ATTACK_SPEED;
				if(distance > MONSTER_MOVE_ROTATE_DISTANCE_MIN){
					allMonsters[m].attacking = false;
					allMonsters[m].finishedAttackOne = false;
				}
			}
			float xNew = playerPos.x + sin(radians(bearing)) * distance;
			float zNew = playerPos.y + cos(radians(bearing)) * distance;
			allMonsters[m].position.x = xNew;
			allMonsters[m].position.z = zNew;
		}
		allMonsters[m].attackChanceDelay -= deltaTime;
	}

	int deadMonstersCount = deadMonsters.size();
	for(int dm = 0; dm < deadMonstersCount; dm++){
		allMonsters.erase(allMonsters.begin() + deadMonsters[dm]);

		int deadCount = deadMonsters.size();
		for(int i = 0; i < deadCount; i++){
			deadMonsters[i] -= 1;
		}
	}
}

float degreesClamp(float degrees) {
	int returned = (int) degrees;
	returned = (int) returned % 360;
	if (returned < 0) { returned += 360; }
	return (float) returned;
}

float monsterCameraColliders(vec3 cameraPos, vec3 lookAt, float defaultDistance) {
	float returned = defaultDistance;
	float angleAllowanceDefault = MONSTER_CAMERA_ANGLE;
	float monsterAddSize = MONSTER_CAMERA_DISTANCE;

	vec3 playerPos = mainPlayer.position;
	vec2 playerX = vec2(playerPos.x, playerPos.z);

	float changedYaw = degreesClamp(bearingTwo(playerX, vec2(cameraPos.x, cameraPos.z)));
	
	float cameraDistance = glm::distance(playerX, vec2(cameraPos.x, cameraPos.z));
	float yCameraDistance = playerPos.y - cameraPos.y;
	float yCameraAngle = degrees(atan(yCameraDistance / cameraDistance));
	float changedPitch = degreesClamp(yCameraAngle);

	int mCount = allMonsters.size();
	for (int m = 0; m < mCount; m++) {
		vec3 position = allMonsters[m].position;
		vec2 positionX = vec2(position.x, position.z);
		
		if(allMonsters[m].attacking){
			continue;
		}

		float floorLength = glm::distance(playerPos, vec3(position.x, playerPos.y, position.z));
		float yDistance = lookAt.y - position.y;
		float yAngle = degreesClamp(degrees(atan(yDistance / floorLength)));
			
		float floorAngle = degreesClamp(bearingTwo(playerX, positionX));
		float compareYaw = degreesClamp(changedYaw); // x
		float comparePitch = degreesClamp(changedPitch); // y

		float angleAllowance = angleAllowanceDefault / (distanceFromCharacter / 10.0f);
		if(glm::distance(yAngle, comparePitch) < angleAllowance){
			if(glm::distance(floorAngle, compareYaw) < angleAllowance){
				float cameraDistance = glm::distance(cameraPos, lookAt);
				float monsterDistance = glm::distance(position, lookAt);

				if (monsterDistance < cameraDistance + monsterAddSize){
					float newDistance = monsterDistance - monsterAddSize;
					if(newDistance < returned){
						returned = newDistance;
					}
				}
			}
		}
	}
	return returned;
}
	