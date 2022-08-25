#include "config.h"
#include "math_utils.h"
#include "application_instance.h"
#include "application.h"
#include "randerer.h"
#include <iostream>

using namespace raster3d;

struct pyramid
{
	mat4  m_model;
	vec3  m_roation{ 0, 0, 0 };
	vec3  m_pos	   { 0, 0, 0 };

	void compute_model()
	{
		m_model  = constants::identity<mat4>();		
		m_model *= glm::translate(m_model, m_pos);
		m_model *= glm::eulerAngleXYZ(radians(m_roation.x), 
									  radians(m_roation.y),
									  radians(m_roation.z));
	}
};

class pyramid_application : public application_instance
{

public:

	pyramid_application(buffers_context& buffers, const ivec2& size, uint frame_rate_cap) 
	: application_instance(buffers, size, frame_rate_cap)
	, m_render(buffers) 
	{}

	void start()
	{
		
		// Model
		points.push_back(vec3(-50 ,0   ,0 ));
		points.push_back(vec3(50  ,0   ,0 ));
		points.push_back(vec3(0   ,50  ,0 ));
		points.push_back(vec3(0   ,0   ,50));

		//tri-line
		line_index.push_back(0); line_index.push_back(1);
		line_index.push_back(1); line_index.push_back(2);
		line_index.push_back(2); line_index.push_back(0);
		//tri		
		tri_index.push_back(2);
		tri_index.push_back(1);
		tri_index.push_back(0);
		//
		tri_index.push_back(0);
		tri_index.push_back(2);
		tri_index.push_back(3);
		
		tri_index.push_back(1);
		tri_index.push_back(2);
		tri_index.push_back(3);
		//
		tri_index.push_back(0);
		tri_index.push_back(1);
		tri_index.push_back(3);

		// Camera
		m_render.view_port({ {0, 0}, {(uint)size().x, (uint)size().y} });
		m_render.projection(glm::perspective(radians(45.0f), float(size().x) / size().y, 1.0f, 500.0f));
		m_view = constants::identity<mat4>();
		m_pyr[0].m_pos = { 0.0f, 0.0f, 200.0f };
		m_pyr[1].m_pos = { 0.0f, 0.0f, 300.0f };
		m_pyr[0].m_roation.y = radians(45.0f);
	}

	void update(float dt)
	{	
		if (GetAsyncKeyState('W') & 0x8000) m_pyr[0].m_pos.y-=1;
		if (GetAsyncKeyState('S') & 0x8000) m_pyr[0].m_pos.y+=1;
		
		if (GetAsyncKeyState('A') & 0x8000) m_pyr[0].m_pos.x+=1;
		if (GetAsyncKeyState('D') & 0x8000) m_pyr[0].m_pos.x-=1;

		if (GetAsyncKeyState('E') & 0x8000) m_pyr[0].m_pos.z+=1;
		if (GetAsyncKeyState('C') & 0x8000) m_pyr[0].m_pos.z-=1;
		
		if (GetAsyncKeyState(VK_PRIOR) & 0x8000) m_pyr[1].m_pos.y-=1;
		if (GetAsyncKeyState(VK_NEXT) & 0x8000)  m_pyr[1].m_pos.y+=1;
		
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)  m_pyr[1].m_pos.x+=1;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) m_pyr[1].m_pos.x-=1;

		if (GetAsyncKeyState(VK_UP) & 0x8000)    m_pyr[1].m_pos.z+=1;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)  m_pyr[1].m_pos.z-=1;
		////////////////////////////////////////////////////

		if (GetAsyncKeyState('T') & 0x8000) m_pyr[0].m_roation.x-=5;
		if (GetAsyncKeyState('B') & 0x8000) m_pyr[0].m_roation.x+=5;
		
		if (GetAsyncKeyState('F') & 0x8000) m_pyr[0].m_roation.y-=5;
		if (GetAsyncKeyState('H') & 0x8000) m_pyr[0].m_roation.y+=5;

		if (GetAsyncKeyState('R') & 0x8000) m_pyr[0].m_roation.z-=5;
		if (GetAsyncKeyState('Y') & 0x8000) m_pyr[0].m_roation.z+=5;
		////////////////////////////////////////////////////
		buffers().clear_depth();
		buffers().clear_color();

		// Draw pyr 0
		m_pyr[0].compute_model();
		m_render.model_view(m_pyr[0].m_model * m_view);
		m_render.draw_triangles_scanline(points, tri_index, color(255, 255, 0, 255));
		m_render.draw_triangles_wire_frame(points, tri_index, color(255, 0, 0, 255));
		// Draw pyr 1
		m_pyr[1].compute_model();
		m_render.model_view(m_pyr[1].m_model * m_view);
		m_render.draw_triangles_scanline(points, tri_index, color(0, 0, 255, 255));
		m_render.draw_triangles_wire_frame(points, tri_index, color(255, 0, 0, 255));

		// Debug
		if (GetAsyncKeyState('Z') & 0x8000)
			buffers().draw_zbuffer();

	}
	void end()
	{

	}

private:

	// Pyramid model
	vertex_buffer points;
	index_buffer  line_index;
	index_buffer  tri_index;
	
	// CPU render
	randerer m_render;

	// Camera
	mat4 m_view;

	// Nodes
	pyramid m_pyr[2];
};


int main(int,const char**)
{
	application win(create_application_builder< pyramid_application >(), { 640, 480 }, 60);
	win.exec();

	return 0;
}
