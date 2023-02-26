#include "Renderer.h"
#include "MovementBoundaries.h"
#include "Warrior.h"
#include "Spike.h"
#include "Arrow.h"
#include "Dragon.h"
#include "Totem.h"
#include "Door.h"
#include "Tools.h"
#include "ShaderProgram.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "OBJLoader.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <unordered_map>

static std::unordered_map<std::string, GeometryNode*> s_Models;

// RENDERER
Renderer::Renderer()
{
	this->m_nodes = {};
	this->m_continous_time = 0.0;
}

Renderer::~Renderer()
{
	glDeleteTextures(1, &m_fbo_depth_texture);
	glDeleteTextures(1, &m_fbo_pos_texture);
	glDeleteTextures(1, &m_fbo_normal_texture);
	glDeleteTextures(1, &m_fbo_albedo_texture);
	glDeleteTextures(1, &m_fbo_mask_texture);

	glDeleteFramebuffers(1, &m_fbo);

	glDeleteVertexArrays(1, &m_vao_fbo);
	glDeleteBuffers(1, &m_vbo_fbo_vertices);
}

bool Renderer::Init(int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	this->m_screen_width = SCREEN_WIDTH;
	this->m_screen_height = SCREEN_HEIGHT;

	bool techniques_initialization = InitShaders();

	bool meshes_initialization = InitGeometricMeshes();
	bool light_initialization = InitLights();

	bool common_initialization = InitCommonItems();
	bool inter_buffers_initialization = InitIntermediateBuffers();

	//If there was any errors
	if (Tools::CheckGLError() != GL_NO_ERROR)
	{
		printf("Exiting with error at Renderer::Init\n");
		return false;
	}

	this->BuildWorld();
	this->InitCamera();

	//If everything initialized
	return techniques_initialization && meshes_initialization &&
		common_initialization && inter_buffers_initialization;
}

void Renderer::BuildWorld()
{
	constexpr  char DRAGON[] = "Assets/Dungeon/GoldenDragon.obj";
	constexpr  char WARRIOR[] = "Assets/Dungeon/Warrior.obj";
	constexpr  char HALL[] = "Assets/Dungeon/Hall1.obj";
	constexpr  char NARROWCORRIDOR[] = "Assets/Dungeon/Corridor1_Narrow.obj";
	constexpr  char MEDIUMCORRIDOR[] = "Assets/Dungeon/Corridor1_Medium.obj";
	constexpr  char WIDECORRIDOR[] = "Assets/Dungeon/Corridor1_Wide.obj";
	constexpr  char TJUNCTIONCORRIDOR[] = "Assets/Dungeon/Corridor1_Medium_T_Junction.obj";
	constexpr  char SPIKETRAP[] = "Assets/Dungeon/Spiketrap.obj";
	constexpr  char SPIKES[] = "Assets/Dungeon/Spikes.obj";
	constexpr  char MEDIUMTURN[] = "Assets/Dungeon/Corridor1_Medium_Turn.obj";
	constexpr  char WIDETURN[] = "Assets/Dungeon/Corridor1_Wide_Turn.obj";
	constexpr  char MEDIUMROOM[] = "Assets/Dungeon/Room1_Simple_Medium.obj";
	constexpr  char SMALLROOM[] = "Assets/Dungeon/Room1_Simple_Small.obj";
	constexpr  char TOTEM[] = "Assets/Dungeon/Totem.obj";
	constexpr  char ARROW[] = "Assets/Dungeon/Arrow.obj";
	constexpr  char PEDESTAL[] = "Assets/Dungeon/Pedestal.obj";
	constexpr  char DOOR[] = "Assets/Dungeon/Door1.obj";
	constexpr  char WELL[] = "Assets/Dungeon/Well.obj";

	MovementBoundaries* mb = new MovementBoundaries();

	m_nodes.emplace_back(new GameObject(s_Models[HALL]));
	Warrior* war = new Warrior(s_Models[WARRIOR], mb);
	m_nodes.emplace_back(war);

	GameObject* object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, -3.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMTURN]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, -3.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, -5.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, -5.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[SMALLROOM]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, -8.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -6.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[HALL]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, -12.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[WIDECORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, -12.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMROOM]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(6.5f, 0.0f, -12.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	

	object = m_nodes.emplace_back(new GameObject(s_Models[WIDETURN]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f, 0.0f, -3.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[WIDECORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f, 0.0f, -6.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[WIDECORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f, 0.0f, -8.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[NARROWCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -16.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[NARROWCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -18.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMROOM]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -22.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	object = m_nodes.emplace_back(new GameObject(s_Models[NARROWCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(00.0f, 0.0f, -16.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[NARROWCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -18.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMROOM]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -22.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[WIDECORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-7.5f, 0.0f, -12.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[WIDECORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-9.5f, 0.0f, -12.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[TJUNCTIONCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5f, 0.0f, -12.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMROOM]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5f, 0.0f, -18.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[HALL]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5f, 0.0f, -4.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[SMALLROOM]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[DOOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, -2.6f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[SPIKETRAP]));
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(6.5f, 0.0f, -12.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	object->GetTransform() = model;
	object = m_nodes.emplace_back(new Spike(s_Models[SPIKES], model, 90.0f, mb));

	glm::vec3 pos1(-0.5f, 0.0f, -9.75f);
	glm::vec3 offset1(-0.5f, 0.0f, -0.5f);
	Door* door1 = new Door(s_Models[DOOR], war, 0.0f, pos1, offset1, mb);
	m_nodes.emplace_back(door1);
	door1->GetTransform() = glm::translate(glm::mat4(1.0f), pos1)*
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 pos2(3.0f, 0.0f, -12.5f);
	glm::vec3 offset2(0.5f, 0.0f, -0.25f);
	Door* door2 = new Door(s_Models[DOOR], war, 90.0f, pos2, offset2, mb);
	m_nodes.emplace_back(door2);
	door2->GetTransform() = glm::translate(glm::mat4(1.0f), pos2) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 pos3(-0.5f, 0.0f, -15.0f);
	glm::vec3 offset3(-0.25f, 0.0f, 0.5f);
	Door* door3 = new Door(s_Models[DOOR], war, 0.0f, pos3, offset3, mb);
	m_nodes.emplace_back(door3);
	door3->GetTransform() = glm::translate(glm::mat4(1.0f), pos3) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 pos4(-0.5f, 0.0f, -19.0f);
	glm::vec3 offset4(-0.25f, 0.0f, -0.5f);
	Door* door4 = new Door(s_Models[DOOR], war, 0.0f, pos4, offset4, mb);
	m_nodes.emplace_back(door4);
	door4->GetTransform() = glm::translate(glm::mat4(1.0f), pos4) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 pos5(-5.5f, 0.0f, -15.0f);
	glm::vec3 offset5(-0.25f, 0.0f, 0.5f);
	Door* door5 = new Door(s_Models[DOOR], war, 0.0f, pos5, offset5, mb);
	m_nodes.emplace_back(door5);
	door5->GetTransform() = glm::translate(glm::mat4(1.0f), pos5) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 pos6(-5.5f, 0.0f, -19.0f);
	glm::vec3 offset6(-0.25f, 0.0f, -0.5f);
	Door* door6 = new Door(s_Models[DOOR], war, 0.0f, pos6, offset6, mb);
	m_nodes.emplace_back(door6);
	door6->GetTransform() = glm::translate(glm::mat4(1.0f), pos6) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 pos7(-13.0f, 0.0f, -15.0f);
	glm::vec3 offset7(-0.25f, 0.0f, 0.5f);
	Door* door7 = new Door(s_Models[DOOR], war, 0.0f, pos7, offset7, mb);
	m_nodes.emplace_back(door7);
	door7->GetTransform() = glm::translate(glm::mat4(1.0f), pos7) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 pos8(-13.0f, 0.0f, -2.0f);
	glm::vec3 offset8(-0.25f, 0.0f, 0.5f);
	Door* door8 = new Door(s_Models[DOOR], war, 0.0f, pos8, offset8, mb);
	m_nodes.emplace_back(door8);
	door8->GetTransform() = glm::translate(glm::mat4(1.0f), pos8) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 pos9(-3.5f, 0.0f, -3.5f);
	glm::vec3 offset9(-0.25f, 0.0f, 0.5f);
	Door* door9 = new Door(s_Models[DOOR], war, 90.0f, pos9, offset9, mb);
	m_nodes.emplace_back(door9);
	door9->GetTransform() = glm::translate(glm::mat4(1.0f), pos9) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 pos10(-5.5f, 0.0f, -9.5f);
	glm::vec3 offset10(-0.5f, 0.0f, -0.5f);
	Door* door10 = new Door(s_Models[DOOR], war, 0.0f, pos10, offset10, mb);
	m_nodes.emplace_back(door10);
	door10->GetTransform() = glm::translate(glm::mat4(1.0f), pos10) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[PEDESTAL]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(8.5f, 0.0f, -12.5f));

	glm::vec3 dragonPos = { 8.5f, 0.0f, -12.5f };
	Dragon* dragon = new Dragon(s_Models[DRAGON], war, dragonPos, mb);
	m_nodes.emplace_back(dragon);
	dragon->GetTransform() = glm::translate(glm::mat4(1.0f), dragonPos);

	glm::mat4 arrowTr1 = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, -10.0f))* 
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Arrow* arrow1 = new Arrow(s_Models[ARROW], arrowTr1, 0.0f, mb);
	m_nodes.emplace_back(arrow1);

	arrowTr1 = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, -10.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));
	Arrow* arrow2 = new Arrow(s_Models[ARROW], arrowTr1, 180.0f, mb);
	m_nodes.emplace_back(arrow2);

	glm::vec3 totemPos = { -2.5f, 0.0f, -12.5f };
	Totem* totem = new Totem(s_Models[TOTEM], totemPos, mb, arrow1, arrow2);
	totem->GetTransform() = glm::translate(glm::mat4(1.0f), totemPos);
	m_nodes.emplace_back(totem);

	object = m_nodes.emplace_back(new GameObject(s_Models[WELL]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), totemPos);

	arrowTr1 = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, -10.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Arrow* arrow3 = new Arrow(s_Models[ARROW], arrowTr1, 0.0f, mb);
	m_nodes.emplace_back(arrow3);
	arrowTr1 = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, -10.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(glm::pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));
	Arrow* arrow4 = new Arrow(s_Models[ARROW], arrowTr1, 180.0f, mb);
	m_nodes.emplace_back(arrow4);

	totemPos = { -13.0f, 0.0f, -12.5f };
	totem = new Totem(s_Models[TOTEM], totemPos, mb, arrow3, arrow4);
	m_nodes.emplace_back(totem);

	object = m_nodes.emplace_back(new GameObject(s_Models[WELL]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), totemPos);

	object = m_nodes.emplace_back(new GameObject(s_Models[WELL]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), totemPos);

	object = m_nodes.emplace_back(new GameObject(s_Models[PEDESTAL]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5f, 0.0f, -19.5f));

	dragonPos = { -12.5f, 0.0f, -19.5f };
	Dragon* dragon1 = new Dragon(s_Models[DRAGON], war, dragonPos, mb);
	m_nodes.emplace_back(dragon1);
	dragon1->GetTransform() = glm::translate(glm::mat4(1.0f), dragonPos);

	object = m_nodes.emplace_back(new GameObject(s_Models[SPIKETRAP]));
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5f, 0.0f, -17.5f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	object->GetTransform() = model;
	object = m_nodes.emplace_back(new Spike(s_Models[SPIKES], model, 0.0f, mb));

	
	/*
	* 
	* model = glm::translate(glm::mat4(1.0f), glm::vec3(14.0f, 0.0f, -20.0f));
	Arrow* arrow = new Arrow(s_Models[ARROW], glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), 0.0f);
	m_nodes.emplace_back(arrow);
	arrow->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	GameObject* object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -6.0f));
	object = m_nodes.emplace_back(new GameObject(s_Models[TJUNCTIONCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, -10.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	
	

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMTURN]));
	object->GetTransform() = glm::translate(glm::mat4(1.0), glm::vec3(6.0f, 0.0f, -10.0f)) *
		glm::rotate(glm::mat4(1.0f),  glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, -12.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, -14.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, -16.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[TJUNCTIONCORRIDOR]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, -20.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[MEDIUMROOM]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 0.0f, -20.0f)) *
		glm::rotate(glm::mat4(1.0f), -glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[TOTEM]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 0.0f, -20.0f)) *
		glm::rotate(glm::mat4(1.0f), -glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	model = glm::translate(glm::mat4(1.0f), glm::vec3(14.0f, 0.0f, -20.0f));
	object = m_nodes.emplace_back(new Arrow(s_Models[ARROW], glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 0.0f, -20.0f)), -90.0f));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	object = m_nodes.emplace_back(new GameObject(s_Models[PEDESTAL]));
	object->GetTransform() = glm::translate(glm::mat4(1.0f), glm::vec3(14.0f, 0.0f, -20.0f));
	m_nodes.emplace_back(new Dragon(s_Models[DRAGON], glm::translate(glm::mat4(1.0f), glm::vec3(14.0f, 0.0f, -20.0f)), war));*/
	/*
	GeometryNode& dragon = *this->m_nodes[0];
	GeometryNode& pedestral = *this->m_nodes[1];
	GeometryNode& totem = *this->m_nodes[2];
	GeometryNode& warrior = *this->m_nodes[3];

	dragon.model_matrix = glm::translate(glm::mat4(1.f), glm::vec3(-1.5f, 0.f, 0.f));
	dragon.m_aabb.center = glm::vec3(dragon.model_matrix * glm::vec4(dragon.m_aabb.center, 1.f));

	pedestral.model_matrix = glm::translate(glm::mat4(1.f), glm::vec3(1.5f, 0.f, 0.f));
	pedestral.m_aabb.center = glm::vec3(pedestral.model_matrix * glm::vec4(pedestral.m_aabb.center, 1.f));

	warrior.model_matrix = glm::translate(glm::mat4(1.f), glm::vec3(3.f, 0.f, 0.f));
	warrior.m_aabb.center = glm::vec3(warrior.model_matrix * glm::vec4(warrior.m_aabb.center, 1.f));

	totem.model_matrix = glm::mat4(1.0f);

	*/
	this->m_world_matrix = glm::mat4(1.f);
}

void Renderer::InitCamera()
{
	this->m_camera_position = glm::vec3(1, 1.5, 6);
	this->m_camera_target_position = glm::vec3(1, 1.5, 0);
	this->m_camera_up_vector = glm::vec3(0, 1, 0);

	this->m_view_matrix = glm::lookAt(
		this->m_camera_position,
		this->m_camera_target_position,
		m_camera_up_vector);

	this->m_projection_matrix = glm::perspective(
		glm::radians(45.f),
		this->m_screen_width / (float)this->m_screen_height,
		0.1f, 100.f);
}

bool Renderer::InitLights()
{
	this->m_light.SetColor(glm::vec3(100.f));
	this->m_light.SetPosition(glm::vec3(1, 10.5, 6));
	this->m_light.SetTarget(glm::vec3(1, 1.5, 0));
	this->m_light.SetConeSize(400, 500);
	this->m_light.CastShadow(true);
	return true;
}

bool Renderer::InitShaders()
{
	std::string vertex_shader_path = "Assets/Shaders/geometry pass.vert";
	std::string geometry_shader_path = "Assets/Shaders/geometry pass.geom";
	std::string fragment_shader_path = "Assets/Shaders/geometry pass.frag";

	m_geometry_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_geometry_program.LoadGeometryShaderFromFile(geometry_shader_path.c_str());
	m_geometry_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_geometry_program.CreateProgram();

	vertex_shader_path = "Assets/Shaders/deferred pass.vert";
	fragment_shader_path = "Assets/Shaders/deferred pass.frag";

	m_deferred_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_deferred_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_deferred_program.CreateProgram();

	vertex_shader_path = "Assets/Shaders/post_process.vert";
	fragment_shader_path = "Assets/Shaders/post_process.frag";

	m_post_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_post_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_post_program.CreateProgram();

	vertex_shader_path = "Assets/Shaders/shadow_map_rendering.vert";
	fragment_shader_path = "Assets/Shaders/shadow_map_rendering.frag";

	m_spot_light_shadow_map_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_spot_light_shadow_map_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_spot_light_shadow_map_program.CreateProgram();

	return true;
}

bool Renderer::InitIntermediateBuffers()
{
	glGenTextures(1, &m_fbo_depth_texture);
	glGenTextures(1, &m_fbo_pos_texture);
	glGenTextures(1, &m_fbo_normal_texture);
	glGenTextures(1, &m_fbo_albedo_texture);
	glGenTextures(1, &m_fbo_mask_texture);
	glGenTextures(1, &m_fbo_texture);

	glGenFramebuffers(1, &m_fbo);

	return ResizeBuffers(m_screen_width, m_screen_height);
}

bool Renderer::ResizeBuffers(int width, int height)
{
	m_screen_width = width;
	m_screen_height = height;

	// texture
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, m_fbo_pos_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glBindTexture(GL_TEXTURE_2D, m_fbo_normal_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, m_fbo_albedo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, m_fbo_mask_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_screen_width, m_screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// framebuffer to link to everything together
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_pos_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_fbo_normal_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_fbo_albedo_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_fbo_mask_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fbo_depth_texture, 0);

	GLenum status = Tools::CheckFramebufferStatus(m_fbo);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool Renderer::InitCommonItems()
{
	glGenVertexArrays(1, &m_vao_fbo);
	glBindVertexArray(m_vao_fbo);

	GLfloat fbo_vertices[] = {
		-1, -1,
		1, -1,
		-1, 1,
		1, 1, };

	glGenBuffers(1, &m_vbo_fbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_fbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	return true;
}

bool Renderer::InitGeometricMeshes()
{
	std::array<const char*, 20> assets = {
		"Assets/Dungeon/GoldenDragon.obj",
		"Assets/Dungeon/Pedestal.obj",
		"Assets/Dungeon/Totem.obj",
		"Assets/Dungeon/Warrior.obj",
		"Assets/Dungeon/Hall1.obj",
		"Assets/Dungeon/Corridor1_Narrow.obj",
		"Assets/Dungeon/Corridor1_Medium.obj",
		"Assets/Dungeon/Corridor1_Wide.obj",
		"Assets/Dungeon/Corridor1_Medium_T_Junction.obj",
		"Assets/Dungeon/Spiketrap.obj",
		"Assets/Dungeon/Spikes.obj",
		"Assets/Dungeon/Corridor1_Medium_Turn.obj",
		"Assets/Dungeon/Corridor1_Wide_Turn.obj",
		"Assets/Dungeon/Room1_Simple_Medium.obj",
		"Assets/Dungeon/Room1_Simple_Small.obj",
		"Assets/Dungeon/Totem.obj",
		"Assets/Dungeon/Arrow.obj",
		"Assets/Dungeon/Pedestal.obj",
		"Assets/Dungeon/Door1.obj",
		"Assets/Dungeon/Well.obj"
	};

	bool initialized = true;
	OBJLoader loader;

	for (auto& asset : assets)
	{
		GeometricMesh* mesh = loader.load(asset);

		if (mesh != nullptr)
		{
			GeometryNode* node = new GeometryNode();
			node->Init(asset, mesh);
			s_Models.insert({ asset, node });
		}
		else
		{
			initialized = false;
			break;
		}
	}

	return initialized;
}

void Renderer::Update(float dt)
{
	this->UpdateGeometry(dt);
	this->UpdateCamera(dt);
	m_continous_time += dt;
}

void Renderer::UpdateGeometry(float dt)
{
	for (auto* node : this->m_nodes)
		node->Update(dt);

}

void Renderer::UpdateCamera(float dt)
{
	constexpr float minTheta = glm::pi<float>() / 8.0f;
	constexpr float maxTheta = glm::pi<float>() * 3.0f / 8.0f;
	static float f = 0.0f;
	static float theta = minTheta;
	constexpr glm::vec3 s = glm::vec3(1.0, 8.0f, 5.0f);
	constexpr float speed = glm::pi<float>() * 2.2f;
	const glm::vec2 mousedir = m_camera_look_angle_destination;
	m_camera_look_angle_destination = glm::vec2(0.f);

	m_camera_target_position = glm::vec3(m_nodes[1]->GetTransform()[3]) + glm::vec3(0.0f,2.5, 0.0f);
	m_light.SetPosition(glm::vec3(m_nodes[1]->GetTransform()[3]) + glm::vec3(0.0f, 10.0f, 1.0f));
	m_light.SetTarget(glm::vec3(m_nodes[1]->GetTransform()[3]));
	if (mousedir.x > 0.0f)
		f += speed * dt;
	if (mousedir.x < 0.0f)
		f -= speed * dt;
	if (mousedir.y > 0.0f)
		theta += speed * dt;
	if (mousedir.y < 0.0f)
		theta -= speed * dt;
	theta = glm::clamp(theta, minTheta, maxTheta);

	const glm::vec3 sdir = s * glm::vec3(glm::cos( theta) * glm::sin(f), glm::sin(theta), glm::cos(theta) * glm::cos(f));
	m_camera_position = m_camera_target_position + sdir;
	/*
	glm::vec3 direction = glm::normalize(m_camera_target_position - m_camera_position);

	m_camera_position = m_camera_position + (m_camera_movement.x * 5.f * dt) * direction;
	m_camera_target_position = m_camera_target_position + (m_camera_movement.x * 5.f * dt) * direction;

	glm::vec3 right = glm::normalize(glm::cross(direction, m_camera_up_vector));

	m_camera_position = m_camera_position + (m_camera_movement.y * 5.f * dt) * right;
	m_camera_target_position = m_camera_target_position + (m_camera_movement.y * 5.f * dt) * right;

	
	glm::mat4 rotation = glm::rotate(glm::mat4(1.f), m_camera_look_angle_destination.y * speed, right);
	rotation *= glm::rotate(glm::mat4(1.f), m_camera_look_angle_destination.x * speed, m_camera_up_vector);
	m_camera_look_angle_destination = glm::vec2(0.f);

	direction = rotation * glm::vec4(direction, 0.f);
	m_camera_target_position = m_camera_position + direction * glm::distance(m_camera_position, m_camera_target_position);

	*/
	m_view_matrix = glm::lookAt(m_camera_position, m_camera_target_position, m_camera_up_vector);
}

bool Renderer::ReloadShaders()
{
	m_geometry_program.ReloadProgram();
	m_post_program.ReloadProgram();
	m_deferred_program.ReloadProgram();
	m_spot_light_shadow_map_program.ReloadProgram();
	return true;
}

void Renderer::Render()
{
	RenderShadowMaps();
	RenderGeometry();
	RenderDeferredShading();
	RenderPostProcess();

	GLenum error = Tools::CheckGLError();

	if (error != GL_NO_ERROR)
	{
		printf("Reanderer:Draw GL Error\n");
		system("pause");
	}
}

void Renderer::RenderPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.f, 0.8f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	m_post_program.Bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	m_post_program.loadInt("uniform_texture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_light.GetShadowMapDepthTexture());
	m_post_program.loadInt("uniform_shadow_map", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_fbo_pos_texture);
	m_post_program.loadInt("uniform_tex_pos", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_fbo_normal_texture);
	m_post_program.loadInt("uniform_tex_normal", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_fbo_albedo_texture);
	m_post_program.loadInt("uniform_tex_albedo", 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_fbo_mask_texture);
	m_post_program.loadInt("uniform_tex_mask", 5);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	m_post_program.loadInt("uniform_tex_depth", 6);

	glBindVertexArray(m_vao_fbo);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_post_program.Unbind();
}

void Renderer::RenderStaticGeometry()
{
	glm::mat4 proj = m_projection_matrix * m_view_matrix * m_world_matrix;

	for (auto& node : this->m_nodes)
	{
		auto* gnode = node->GetGeometry();
		glBindVertexArray(gnode->m_vao);

		m_geometry_program.loadMat4("uniform_projection_matrix", proj * node->GetTransform());
		m_geometry_program.loadMat4("uniform_normal_matrix", glm::transpose(glm::inverse(m_world_matrix * node->GetTransform())));
		m_geometry_program.loadMat4("uniform_world_matrix", m_world_matrix * node->GetTransform());

		for (int j = 0; j < gnode->parts.size(); ++j)
		{
			m_geometry_program.loadVec3("uniform_diffuse", gnode->parts[j].diffuse);
			m_geometry_program.loadVec3("uniform_ambient", gnode->parts[j].ambient);
			m_geometry_program.loadVec3("uniform_specular", gnode->parts[j].specular);
			m_geometry_program.loadFloat("uniform_shininess", gnode->parts[j].shininess);
			m_geometry_program.loadFloat("uniform_metallic", gnode->parts[j].metallic);
			m_geometry_program.loadInt("uniform_has_tex_diffuse", (gnode->parts[j].diffuse_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_emissive", (gnode->parts[j].emissive_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_mask", (gnode->parts[j].mask_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_normal", (gnode->parts[j].bump_textureID > 0 || gnode->parts[j].normal_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_is_tex_bumb", (gnode->parts[j].bump_textureID > 0) ? 1 : 0);

			glActiveTexture(GL_TEXTURE0);
			m_geometry_program.loadInt("uniform_tex_diffuse", 0);
			glBindTexture(GL_TEXTURE_2D, gnode->parts[j].diffuse_textureID);

			if (gnode->parts[j].mask_textureID > 0)
			{
				glActiveTexture(GL_TEXTURE1);
				m_geometry_program.loadInt("uniform_tex_mask", 1);
				glBindTexture(GL_TEXTURE_2D, gnode->parts[j].mask_textureID);
			}

			if ((gnode->parts[j].bump_textureID > 0 || gnode->parts[j].normal_textureID > 0))
			{
				glActiveTexture(GL_TEXTURE2);
				m_geometry_program.loadInt("uniform_tex_normal", 2);
				glBindTexture(GL_TEXTURE_2D, gnode->parts[j].bump_textureID > 0 ?
					gnode->parts[j].bump_textureID : gnode->parts[j].normal_textureID);
			}

			if (gnode->parts[j].emissive_textureID > 0)
			{
				glActiveTexture(GL_TEXTURE3);
				m_geometry_program.loadInt("uniform_tex_emissive", 3);
				glBindTexture(GL_TEXTURE_2D, gnode->parts[j].emissive_textureID);
			}

			glDrawArrays(GL_TRIANGLES, gnode->parts[j].start_offset, gnode->parts[j].count);
		}

		glBindVertexArray(0);
	}
}

void Renderer::RenderCollidableGeometry()
{
	glm::mat4 proj = m_projection_matrix * m_view_matrix * m_world_matrix;

	glm::vec3 camera_dir = normalize(m_camera_target_position - m_camera_position);

	for (auto& node : this->m_collidables_nodes)
	{
		auto* gnode = node->GetGeometry();
		float_t isectT = 0.f;
		int32_t primID = -1;
		int32_t totalRenderedPrims = 0;

		//if (node->intersectRay(m_camera_position, camera_dir, m_world_matrix, isectT, primID)) continue;
		//node->intersectRay(m_camera_position, camera_dir, m_world_matrix, isectT, primID);

		glBindVertexArray(gnode->m_vao);

		m_geometry_program.loadMat4("uniform_projection_matrix", proj * node->GetTransform());
		m_geometry_program.loadMat4("uniform_normal_matrix", glm::transpose(glm::inverse(m_world_matrix * node->GetTransform())));
		m_geometry_program.loadMat4("uniform_world_matrix", m_world_matrix * node->GetTransform());
		m_geometry_program.loadFloat("uniform_time", m_continous_time);

		for (int j = 0; j < gnode->parts.size(); ++j)
		{
			m_geometry_program.loadVec3("uniform_diffuse", gnode->parts[j].diffuse);
			m_geometry_program.loadVec3("uniform_ambient", gnode->parts[j].ambient);
			m_geometry_program.loadVec3("uniform_specular", gnode->parts[j].specular);
			m_geometry_program.loadFloat("uniform_shininess", gnode->parts[j].shininess);
			m_geometry_program.loadInt("uniform_has_tex_diffuse", (gnode->parts[j].diffuse_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_mask", (gnode->parts[j].mask_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_emissive", (gnode->parts[j].emissive_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_normal", (gnode->parts[j].bump_textureID > 0 || gnode->parts[j].normal_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_is_tex_bumb", (gnode->parts[j].bump_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_prim_id", primID - totalRenderedPrims);

			glActiveTexture(GL_TEXTURE0);
			m_geometry_program.loadInt("uniform_tex_diffuse", 0);
			glBindTexture(GL_TEXTURE_2D, gnode->parts[j].diffuse_textureID);

			if (gnode->parts[j].mask_textureID > 0)
			{
				glActiveTexture(GL_TEXTURE1);
				m_geometry_program.loadInt("uniform_tex_mask", 1);
				glBindTexture(GL_TEXTURE_2D, gnode->parts[j].mask_textureID);
			}

			if ((gnode->parts[j].bump_textureID > 0 || gnode->parts[j].normal_textureID > 0))
			{
				glActiveTexture(GL_TEXTURE2);
				m_geometry_program.loadInt("uniform_tex_normal", 2);
				glBindTexture(GL_TEXTURE_2D, gnode->parts[j].bump_textureID > 0 ?
					gnode->parts[j].bump_textureID : gnode->parts[j].normal_textureID);
			}

			if (gnode->parts[j].emissive_textureID > 0)
			{
				glActiveTexture(GL_TEXTURE3);
				m_geometry_program.loadInt("uniform_tex_emissive", 3);
				glBindTexture(GL_TEXTURE_2D, gnode->parts[j].emissive_textureID);
			}

			glDrawArrays(GL_TRIANGLES, gnode->parts[j].start_offset, gnode->parts[j].count);
			totalRenderedPrims += gnode->parts[j].count;
		}

		glBindVertexArray(0);
	}
}

void Renderer::RenderDeferredShading()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);

	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };

	glDrawBuffers(1, drawbuffers);

	glViewport(0, 0, m_screen_width, m_screen_height);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	glClear(GL_COLOR_BUFFER_BIT);

	m_deferred_program.Bind();

	m_deferred_program.loadVec3("uniform_light_color", m_light.GetColor());
	m_deferred_program.loadVec3("uniform_light_dir", m_light.GetDirection());
	m_deferred_program.loadVec3("uniform_light_pos", m_light.GetPosition());

	m_deferred_program.loadFloat("uniform_light_umbra", m_light.GetUmbra());
	m_deferred_program.loadFloat("uniform_light_penumbra", m_light.GetPenumbra());

	m_deferred_program.loadVec3("uniform_camera_pos", m_camera_position);
	m_deferred_program.loadVec3("uniform_camera_dir", normalize(m_camera_target_position - m_camera_position));

	m_deferred_program.loadMat4("uniform_light_projection_view", m_light.GetProjectionMatrix() * m_light.GetViewMatrix());
	m_deferred_program.loadInt("uniform_cast_shadows", m_light.GetCastShadowsStatus() ? 1 : 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_pos_texture);
	m_deferred_program.loadInt("uniform_tex_pos", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_fbo_normal_texture);
	m_deferred_program.loadInt("uniform_tex_normal", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_fbo_albedo_texture);
	m_deferred_program.loadInt("uniform_tex_albedo", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_fbo_mask_texture);
	m_deferred_program.loadInt("uniform_tex_mask", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	m_deferred_program.loadInt("uniform_tex_depth", 4);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, m_light.GetShadowMapDepthTexture());
	m_deferred_program.loadInt("uniform_shadow_map", 10);

	glBindVertexArray(m_vao_fbo);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	m_deferred_program.Unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthMask(GL_TRUE);
}

void Renderer::RenderGeometry()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_pos_texture, 0);

	GLenum drawbuffers[4] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3 };

	glDrawBuffers(4, drawbuffers);

	glViewport(0, 0, m_screen_width, m_screen_height);
	glClearColor(0.f, 0.8f, 1.f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_geometry_program.Bind();
	RenderStaticGeometry();
	auto e = glGetError();
	RenderCollidableGeometry();

	m_geometry_program.Unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
}

void Renderer::RenderShadowMaps()
{
	if (m_light.GetCastShadowsStatus())
	{
		int m_depth_texture_resolution = m_light.GetShadowMapResolution();

		glBindFramebuffer(GL_FRAMEBUFFER, m_light.GetShadowMapFBO());
		glViewport(0, 0, m_depth_texture_resolution, m_depth_texture_resolution);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Bind the shadow mapping program
		m_spot_light_shadow_map_program.Bind();

		glm::mat4 proj = m_light.GetProjectionMatrix() * m_light.GetViewMatrix() * m_world_matrix;

		for (auto& node : this->m_nodes)
		{
			auto* gnode = node->GetGeometry();
			glBindVertexArray(gnode->m_vao);

			m_spot_light_shadow_map_program.loadMat4("uniform_projection_matrix", proj * node->GetTransform());

			for (int j = 0; j < gnode->parts.size(); ++j)
			{
				glDrawArrays(GL_TRIANGLES, gnode->parts[j].start_offset, gnode->parts[j].count);
			}

			glBindVertexArray(0);
		}

		glm::vec3 camera_dir = normalize(m_camera_target_position - m_camera_position);
		float_t isectT = 0.f;
		int32_t primID;

		for (auto& node : this->m_collidables_nodes)
		{
			auto* gnode = node->GetGeometry();
			//if (node->intersectRay(m_camera_position, camera_dir, m_world_matrix, isectT, primID)) continue;
			node->intersectRay(m_camera_position, camera_dir, m_world_matrix, isectT, primID);

			glBindVertexArray(gnode->m_vao);

			m_spot_light_shadow_map_program.loadMat4("uniform_projection_matrix", proj * node->GetTransform());

			for (int j = 0; j < gnode->parts.size(); ++j)
			{
				glDrawArrays(GL_TRIANGLES, gnode->parts[j].start_offset, gnode->parts[j].count);
			}

			glBindVertexArray(0);
		}

		m_spot_light_shadow_map_program.Unbind();
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Renderer::CameraMoveForward(bool enable)
{
	m_camera_movement.x = (enable) ? 1 : 0;
}
void Renderer::CameraMoveBackWard(bool enable)
{
	m_camera_movement.x = (enable) ? -1 : 0;
}

void Renderer::CameraMoveLeft(bool enable)
{
	m_camera_movement.y = (enable) ? -1 : 0;
}
void Renderer::CameraMoveRight(bool enable)
{
	m_camera_movement.y = (enable) ? 1 : 0;
}

void Renderer::CameraLook(glm::vec2 lookDir)
{
	m_camera_look_angle_destination = lookDir;
}