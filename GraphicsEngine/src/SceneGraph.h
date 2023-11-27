#pragma once
#include <iostream>
#include <vector>
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "ModelSceneInstance.h"
#include "Cube.h"
#include "Quad.h"
#include "Camera.h"
#include "GridInstance.h"
#include "Point.h"

class SceneGraph
{
public:
	SceneGraph();
	std::vector<Camera> m_Cameras;
	std::vector<PointLight> m_PointLights;
	std::vector<DirectionalLight> m_DirectionalLights;
	std::vector<SpotLight> m_SpotLights;
	std::vector<Cube> m_Cubes;
	std::vector<Quad> m_Quads;
	std::vector<ModelSceneInstance> m_Backpacks;
	std::vector<GridInstance> m_Grids;	
	std::vector<Point> m_Points;
	
private:

};

