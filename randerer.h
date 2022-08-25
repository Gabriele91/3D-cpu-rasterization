#pragma once
#include "config.h"
#include "color.h"
#include "types.h"
#include "math_utils.h"
#include "buffers_context.h"

namespace raster3d 
{
	using vertex_buffer = std::vector<vec3>;
	using index_buffer = std::vector<uint>;

	struct polygon
	{
		std::array< vec3, 3 > vt
		{
			  vec3{0}
			, vec3{0}
			, vec3{0}
		};
	};

	struct view_port
	{
		ivec2 position;
		ivec2 size;
	};

	class randerer 
	{
		mat4 m_model_view;
		mat4 m_projection;
		mat4 m_model_view_projection;

		view_port        m_view_port;
		ivec2            m_window;
		ivec2            m_window_size;
		buffers_context& m_buffers;

	public:
		
		// Init
		randerer(buffers_context& buffers) : m_buffers(buffers) {}

		// Set view_port
		void view_port(const view_port& vp)
		{
			m_view_port     = vp;
			m_window.x      = int((m_view_port.position.x + 1) * m_view_port.size.x * 0.5f);
			m_window.y      = int((m_view_port.position.y + 1) * m_view_port.size.y * 0.5f);
			m_window_size.x = m_view_port.size.x - m_view_port.position.x;
			m_window_size.y = m_view_port.size.y - m_view_port.position.y;
		}

		void model_view(const mat4& mv)
		{
			m_model_view = mv;
			m_model_view_projection = m_projection * mv;
		}

		void projection(const mat4& p)
		{
			m_projection = p;
			m_model_view_projection = p * m_model_view;
		}

		void draw_triangles_wire_frame(const vertex_buffer& vb,  const index_buffer& ib, const color& color)
		{
			vec4  clip[3]; 
			vec3  div [3];
			ivec2 view[3];
			for (size_t i = 0; i < ib.size(); i += 3)
			{
				for (uchar j = 0; j < 3; ++j)
				{
					// Clip space
					clip[j] = m_model_view_projection * vec4(vb[ib[i + j]], 1.0f);
					// Normalize
					div[j]  = vec3(clip[j]) / clip[j].w;
					// Window space
					view[j].x = int(div[j].x * m_window_size.x + m_window.x);
					view[j].y = int(div[j].y * m_window_size.y + m_window.y);
				}
				// Draw
				draw_line(color, view[0], view[1]);
				draw_line(color, view[1], view[2]);
				draw_line(color, view[2], view[0]);
			}
		}

		void draw_line(const color& color, const ivec2& v1, const ivec2& v2)
		{
	 
			int x0=v1.x, y0=v1.y, 
				x1=v2.x, y1=v2.y;
			int dx = std::abs(x1-x0), sx = x0 < x1 ? 1 : -1;
			int dy = std::abs(y1-y0), sy = y0 < y1 ? 1 : -1;
			int err = (dx>dy ? dx : -dy)/2, e2=0;
 
			while(true)
			{
				m_buffers.pixel(color,x0,y0);
				if (x0==x1 && y0==y1) break;
				e2 = err;
				if (e2 >-dx) { err -= dy; x0 += sx; }
				if (e2 < dy) { err += dx; y0 += sy; }
			}
		}

		void draw_triangles_parallel(const vertex_buffer& vb, const index_buffer& ib, const color& color)
		{
			vec4    clip[3];
			vec3    div[3];
			ivec2   view[3];
			polygon poly;
			for (size_t i = 0; i < ib.size(); i += 3)
			{
				for (uchar j = 0; j < 3; ++j)
				{
					// Clip space
					clip[j] = m_model_view_projection * vec4(vb[ib[i + j]], 1.0f);
					// Normalize
					div[j] = vec3(clip[j]) / clip[j].w;
					// Window space
					view[j].x = int(div[j].x * m_window_size.x + m_window.x);
					view[j].y = int(div[j].y * m_window_size.y + m_window.y);
					// Set polygon vertex
					poly.vt[j] = vec3(view[j], div[j].z);
				}
				// Draw
				draw_polygon_parallel(poly, color);
			}
		}

		void draw_triangles_scanline(const vertex_buffer& vb, const index_buffer& ib, const color& color)
		{
			vec4    clip[3];
			vec3    div[3];
			ivec2   view[3];
			polygon poly;
			for (size_t i = 0; i < ib.size(); i += 3)
			{
				for (uchar j = 0; j < 3; ++j)
				{
					// Clip space
					clip[j] = m_model_view_projection * vec4(vb[ib[i + j]], 1.0f);
					// Normalize
					div[j] = vec3(clip[j]) / clip[j].w;
					// Window space
					view[j].x = int(div[j].x * m_window_size.x + m_window.x);
					view[j].y = int(div[j].y * m_window_size.y + m_window.y);
					// Set polygon vertex
					poly.vt[j] = vec3(view[j], div[j].z);
				}
				// Draw
				draw_polygon_scanline(poly, color);
				std::swap(poly.vt[0], poly.vt[1]);
				draw_polygon_scanline(poly, color);
			}
		}

		static vec3 barycentric(const vec3* const pts, const vec2& P)
		{
			auto u = cross(vec3(pts[2][0] - pts[0][0], 
							    pts[1][0] - pts[0][0], 
							    pts[0][0] - P[0]), 
						   vec3(pts[2][1] - pts[0][1], 
							    pts[1][1] - pts[0][1], 
							    pts[0][1] - P[1]));

			/* `pts` and `P` has integer value as coordinates
			   so `abs(u[2])` < 1 means `u[2]` is 0, that means
			   triangle is degenerate, in this case return something with negative coordinates */
			if (std::abs(u.z) < 1) 
				return vec3(-1, 1, 1);

			// Compute P
			return vec3
			(
				1.f - (u.x + u.y) / u.z, 
				u.y / u.z, 
				u.x / u.z
			);
		}

		void draw_polygon_parallel(const polygon& poly, const color& color)
		{
			vec2 bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
			vec2 bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
			for (uchar i = 0; i < 3; ++i)
			{
				for (uchar j = 0; j < 2; ++j)
				{
					bboxmin[j] = std::max<float>(m_view_port.position[j], std::min(bboxmin[j], poly.vt[i][j]));
					bboxmax[j] = std::min<float>(m_window_size[j],        std::max(bboxmax[j], poly.vt[i][j]));
				}
			}

			#pragma omp parallel for
			for (int x = (int)bboxmin.x; x <= (int)bboxmax.x; ++x)
			{
				for (int y = (int)bboxmin.y; y <= (int)bboxmax.y; ++y)
				{
					vec3 bc_screen = barycentric(poly.vt.data(), vec2{ x,y });

					if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)  continue;

					float depth = 0.f;
					for (uchar i = 0; i < 3; i++)
						depth += poly.vt[i][2] * bc_screen[i];

					if (m_buffers.depth(x, y) < depth)
					{
						m_buffers.depth(depth, x, y);
						m_buffers.pixel(color, x, y);
					}
				}
			}
			#pragma omp barrier  
		}

#if 0
		void compute_interpolants(const vec3& v0, const vec3& v1, const vec3& v2, float& a, float& b, float& c, float& d)
		{
			a = (v1.z - v2.z) * v0.y + (v2.z - v0.z) * v1.y + (v0.z - v1.z) * v2.y;
			b = v0.z * (v1.x - v2.x) + v1.z * (v2.x - v0.x) + v2.z * (v0.x - v1.x);
			c = v0.x * (v1.y - v2.y) + v1.x * (v2.y - v0.y) + v2.x * (v0.y - v1.y);
			d = -((v2.z * v1.y - v1.z * v2.y) * v0.x
				+ (v0.z * v2.y - v2.z * v0.y) * v1.x
				+ (v1.z * v0.y - v0.z * v1.y) * v2.x);
		}

		void draw_polygon_scanline(const polygon& poly, const color& color)
		{
			// Source from : http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html
			// Sorting
			int A = 0, B = 1, C = 2;
			if(poly.vt[B].y > poly.vt[A].y) std::swap(A,B);
			if(poly.vt[C].y > poly.vt[B].y)
			{ 
				std::swap(B,C);
				if(poly.vt[B].y > poly.vt[A].y) std::swap(B,A);
			}
			// Compute delta 
			vec3 start,end;
			float dx1,dx2,dx3;
			float ydiff;
			// Calc diff (and test the edges)
			if (poly.vt[A].y - poly.vt[B].y > 0)
				dx1 = (poly.vt[B].x - poly.vt[A].x) / -(poly.vt[A].y - poly.vt[B].y);
			else
				dx1 = 0;

			if (poly.vt[A].y - poly.vt[C].y > 0)
				dx2 = (poly.vt[C].x - poly.vt[A].x) / -(poly.vt[A].y - poly.vt[C].y);
			else
				dx2 = 0;

			if (poly.vt[B].y - poly.vt[C].y > 0)
				dx3 = (poly.vt[C].x - poly.vt[B].x) / -(poly.vt[B].y - poly.vt[C].y);
			else
				dx3 = 0;
			// Compute zbuffer:
			// ((a * -XPOS + b * -YPOS - d) / c) 
			// #define planeEQ(XPOS,YPOS) ((a * -XPOS + b * -YPOS - d) / c)
			// ((a * -XPOS + b * -YPOS - d) * ic)
			#define plane_exp(XPOS,YPOS) ((a * -XPOS + b * -YPOS - d) * ic)
			float a, b, ic, d, zplane=0.0;
			compute_interpolants (poly.vt[C], poly.vt[B], poly.vt[A], a, b, ic, d);
			if(ic) ic=1.0/ic;
			// Start draw
			start = end = poly.vt[C];
			// Case 1
			if(dx1 > dx2) 
			{
				//first triangle
				ydiff = std::ceilf(poly.vt[B].y - start.y);
				while (ydiff--)
				{
					//span
					for (uint x = (uint)start.x; x < (uint)end.x; ++x)
					{	
						zplane = plane_exp(x,start.y);
						if(zplane < m_buffers.depth(x, (uint)start.y))
						{
							m_buffers.depth(zplane, x, (uint)start.y);
							m_buffers.pixel(color,  x, (uint)start.y);
						}			
					}
					++start.y;
					++end.y;
					start.x += dx2;
					end.x += dx1;
				}
				// Second triangle
				end = poly.vt[B];
				ydiff = std::ceilf(poly.vt[A].y - start.y);
				while(ydiff--)
				{
					//span
					for (uint x = (uint)start.x; x < (uint)end.x; ++x)
					{
						// Compute plane eq for the z value
						zplane = plane_exp(x, start.y);
						// ZBuffer test
						if (zplane < m_buffers.depth(x, (uint)start.y))
						{
							m_buffers.depth(zplane, x, (uint)start.y);
							m_buffers.pixel(color,  x, (uint)start.y);
						}
					}
					//span
					++start.y;
					++end.y;
					start.x += dx2;
					end.x += dx3;
				}
			} 
			else 
			{
				// First triangle
				ydiff = std::ceilf(poly.vt[B].y - start.y);
				while(ydiff--)
				{
					//span
					for (uint x = (uint)start.x; x < (uint)end.x; ++x)
					{
						// Compute plane eq for the z value
						zplane = plane_exp(x, start.y);
						// Z Test
						if(zplane < m_buffers.depth(x, (uint)start.y))
						{
							m_buffers.depth(zplane, x, (uint)start.y);
							m_buffers.pixel(color,  x, (uint)start.y);
						}			
					}
					//span
					++start.y;
					++end.y;
					start.x += dx1;
					end.x += dx2;
				}
				//second triangle
				start = poly.vt[B];
				ydiff = std::ceilf(poly.vt[A].y - start.y);
				while(ydiff--)
				{
					// Span
					for (uint x = (uint)start.x; x < (uint)end.x; ++x)
					{						
						// Compute plane eq for the z value
						zplane = plane_exp(x, start.y);
						// Z test
						if (zplane < m_buffers.depth(x, (uint)start.y))
						{
							m_buffers.depth(zplane, x, (uint)start.y);
							m_buffers.pixel(color,  x, (uint)start.y);
						}
					}
					//span
					++start.y;
					++end.y;
					start.x += dx3;
					end.x += dx2;
				}
			}
			//end draw
		}
#else 
		float computeInterpolants
		(
			float minX, 
			float minY,
			const vec3& v0,
			const vec3& v1,
			const vec3& v2,
			float& dzdx,
			float& dzdy
		) 
		{
			float a = (v1.z - v2.z) * v0.y + (v2.z - v0.z) * v1.y + (v0.z - v1.z) * v2.y;
			float b = v0.z * (v1.x - v2.x) + v1.z * (v2.x - v0.x) + v2.z * (v0.x - v1.x);
			float c = v0.x * (v1.y - v2.y) + v1.x * (v2.y - v0.y) + v2.x * (v0.y - v1.y);
			if (c == 0) return 0;
			//calc diff:
			// - a / c
			dzdx = -a / c;
			// - b / c
			dzdy = -b / c;
			// Calculate initial value, dx and dy	(A * -minX + B * -minY -D) / C		
			float d = -((v2.z * v1.y - v1.z * v2.y) * v0.x
				+ (v0.z * v2.y - v2.z * v0.y) * v1.x
				+ (v1.z * v0.y - v0.z * v1.y) * v2.x);

			return (a * -minX + b * -minY - d) / c;
			//
		}

		void draw_polygon_scanline(const polygon& poly, const color& color)
		{
			//" taken "
			//from: https://github.com/Biracun/Acun3D/blob/master/Acun3D/Rasteriser.cpp
			//{ http://www.acunliffe.com/2011/10/3d-software-renderer/ http://luki.webzdarma.cz/eng_05_en.htm }

			const int y1 = (int)(16.0f * poly.vt[0].y + 0.5f);
			const int y2 = (int)(16.0f * poly.vt[1].y + 0.5f);
			const int y3 = (int)(16.0f * poly.vt[2].y + 0.5f);


			const int x1 = (int)(16.0f * poly.vt[0].x + 0.5f);
			const int x2 = (int)(16.0f * poly.vt[1].x + 0.5f);
			const int x3 = (int)(16.0f * poly.vt[2].x + 0.5f);


			// Precalculate half-space function coefficients (so they can be calculated incrementally in the loop)
			const int dy1 = x1 - x2;
			const int dy2 = x2 - x3;
			const int dy3 = x3 - x1;


			const int dx1 = y1 - y2;
			const int dx2 = y2 - y3;
			const int dx3 = y3 - y1;


			const int fdx1 = dx1 << 4;
			const int fdx2 = dx2 << 4;
			const int fdx3 = dx3 << 4;


			const int fdy1 = dy1 << 4;
			const int fdy2 = dy2 << 4;
			const int fdy3 = dy3 << 4;


			// Work out min and max X and Y
			int minX = (raster3d::min<const int>(x1, x2, x3) + 0xF) >> 4;
			int maxX = (raster3d::max<const int>(x1, x2, x3) + 0xF) >> 4;
			int minY = (raster3d::min<const int>(y1, y2, y3) + 0xF) >> 4;
			int maxY = (raster3d::max<const int>(y1, y2, y3) + 0xF) >> 4;


			// Make sure it's not outside of the screen
			if (minX < 0) minX = 0;
			if (minY < 0) minY = 0;

			if (maxX >= m_view_port.size.x) maxX = m_view_port.size.x - 1;
			if (maxY >= m_view_port.size.y) maxY = m_view_port.size.y - 1;


			// Calculate half-space initial values
			int check1 = (int)((dy1 * (minY << 4)) - (dy1 * y1) - (dx1 * (minX << 4)) + (dx1 * x1));
			int check2 = (int)((dy2 * (minY << 4)) - (dy2 * y2) - (dx2 * (minX << 4)) + (dx2 * x2));
			int check3 = (int)((dy3 * (minY << 4)) - (dy3 * y3) - (dx3 * (minX << 4)) + (dx3 * x3));


			// Extend values if required for fill convention purposes
			if (dx1 < 0 || (dx1 == 0 && dy1 > 0))
				check1++;
			if (dx2 < 0 || (dx2 == 0 && dy2 > 0))
				check2++;
			if (dx3 < 0 || (dx3 == 0 && dy3 > 0))
				check3++;


			// The same for z-coordinate for z-buffering
			float dxZ;
			float dyZ;
			float initialZ = computeInterpolants
			(
				(float)minX,
				(float)minY,
				poly.vt[0],
				poly.vt[1],
				poly.vt[2],
				dxZ,
				dyZ
			);



			//loop temp values
			ivec3 check_temp(0);
			float tempZ = 0.0;
			//for y
			for (int y = minY; y < maxY; y++) 
			{
				// Create temporary copies of our incremental values because they'll be needed in the next iteration
				check_temp[0] = check1;
				check_temp[1] = check2;
				check_temp[2] = check3;
				//z value
				tempZ = initialZ;
				//for x
				for (int x = minX; x < maxX; x++) 
				{
					if (check_temp[0] > 0 && check_temp[1] > 0 && check_temp[2] > 0)
					{
						// If on top of screen
						if (m_buffers.depth(x, y) < tempZ) 
						{
							m_buffers.depth(tempZ, x, y);
							m_buffers.pixel(color, x, y);
						}
					}
					// Increment values in x
					check_temp[0] -= fdx1;
					check_temp[1] -= fdx2;
					check_temp[2] -= fdx3;
					//next z
					tempZ += dxZ;
				}
				// Increment values in y
				check1 += fdy1;
				check2 += fdy2;
				check3 += fdy3;
				// Next z
				initialZ += dyZ;
			}


		}
#endif
	};

};
