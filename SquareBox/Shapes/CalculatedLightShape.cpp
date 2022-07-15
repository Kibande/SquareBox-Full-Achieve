#include "CalculatedLightShape.h"

void SquareBox::Shapes::CalculatedLightShape::draw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::SpriteBatch & spriteBatch_, int opacity_)
{
	if (!clusterObject_.is_hidden) {
		calculateVisiblityPolygon(clusterObject_.position, clusterObject_.radius, clusterObject_.vec_of_edges, SquareBox::RenderEngine::ColorRGBA8(( clusterObject_.texture_info.color.x), ( clusterObject_.texture_info.color.y), ( clusterObject_.texture_info.color.z), ( clusterObject_.texture_info.color.w)));
		//The drawing
		// Draw each triangle in fan
		for (int i = 0; i < vecVisibilityPolygonPoints.size() - 1 && vecVisibilityPolygonPoints.size()>0; i++)
		{
			drawTriangleLight(clusterObject_.position, glm::vec2(std::get<1>(vecVisibilityPolygonPoints[i]), std::get<2>(vecVisibilityPolygonPoints[i])), glm::vec2(std::get<1>(vecVisibilityPolygonPoints[i + 1]), std::get<2>(vecVisibilityPolygonPoints[i + 1])), spriteBatch_, SquareBox::RenderEngine::ColorRGBA8(( clusterObject_.texture_info.color.x), ( clusterObject_.texture_info.color.y), ( clusterObject_.texture_info.color.z), ( clusterObject_.texture_info.color.w)), true, clusterObject_.radius);
		}
		if (vecVisibilityPolygonPoints.size() > 0) {
			//close the triangle
			// Fan will have one open edge, so draw last point of fan to first
			drawTriangleLight(clusterObject_.position, glm::vec2(std::get<1>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1]), std::get<2>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1])), glm::vec2(std::get<1>(vecVisibilityPolygonPoints[0]), std::get<2>(vecVisibilityPolygonPoints[0])), spriteBatch_, SquareBox::RenderEngine::ColorRGBA8(( clusterObject_.texture_info.color.x), ( clusterObject_.texture_info.color.y), ( clusterObject_.texture_info.color.z), ( clusterObject_.texture_info.color.w)), true, clusterObject_.radius);
		}
	}
}

void SquareBox::Shapes::CalculatedLightShape::keepTrackOfPosition(SquareBox::GWOM::ClusterObject & clusterObject_)
{
}

bool SquareBox::Shapes::CalculatedLightShape::containtsPoint(SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & coordinates_)
{
	if (clusterObject_.is_hidden) { return false; }

	return ((glm::length(glm::vec2(clusterObject_.position) - coordinates_) <= clusterObject_.radius*0.3));
}

void SquareBox::Shapes::CalculatedLightShape::debugDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  SquareBox::RenderEngine::ColorRGBA8 borderColor_)
{
	calculateVisiblityPolygon(clusterObject_.position, clusterObject_.radius, clusterObject_.vec_of_edges, SquareBox::RenderEngine::ColorRGBA8(( clusterObject_.texture_info.color.x), ( clusterObject_.texture_info.color.y), ( clusterObject_.texture_info.color.z), ( clusterObject_.texture_info.color.w)));

	//The drawing
	// Draw each triangle in fan
	for (unsigned int i = 0; i < vecVisibilityPolygonPoints.size() - 1 && vecVisibilityPolygonPoints.size()>0; i++)
	{
		debugRender_.drawLine(clusterObject_.position, glm::vec2(std::get<1>(vecVisibilityPolygonPoints[i]), std::get<2>(vecVisibilityPolygonPoints[i])), borderColor_);
		debugRender_.drawLine(glm::vec2(std::get<1>(vecVisibilityPolygonPoints[i]), std::get<2>(vecVisibilityPolygonPoints[i])), glm::vec2(std::get<1>(vecVisibilityPolygonPoints[i + 1]), std::get<2>(vecVisibilityPolygonPoints[i + 1])), borderColor_);
		debugRender_.drawLine(glm::vec2(std::get<1>(vecVisibilityPolygonPoints[i + 1]), std::get<2>(vecVisibilityPolygonPoints[i + 1])), clusterObject_.position, borderColor_);
		debugRender_.drawCircle(clusterObject_.position, borderColor_, clusterObject_.radius);
	}
	if (vecVisibilityPolygonPoints.size() > 0) {
		//close the triangle
		// Fan will have one open edge, so draw last point of fan to first
		debugRender_.drawLine(clusterObject_.position, glm::vec2(std::get<1>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1]), std::get<2>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1])), borderColor_);
		debugRender_.drawLine(glm::vec2(std::get<1>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1]), std::get<2>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1])), glm::vec2(std::get<1>(vecVisibilityPolygonPoints[0]), std::get<2>(vecVisibilityPolygonPoints[0])), borderColor_);
		debugRender_.drawLine(clusterObject_.position, glm::vec2(std::get<1>(vecVisibilityPolygonPoints[0]), std::get<2>(vecVisibilityPolygonPoints[0])), borderColor_);
	}
	//drawing the edges
	for (unsigned int i = 0; i < clusterObject_.vec_of_edges.size(); i++)
	{
		debugRender_.drawLine(clusterObject_.vec_of_edges[i].first, clusterObject_.vec_of_edges[i].second, SquareBox::Color::white);
	}
	//selectable radius
	debugRender_.drawCircle(clusterObject_.position, borderColor_, clusterObject_.radius*0.3);

	//drawing the diameter
	debugRender_.drawCircle(clusterObject_.position, SquareBox::RenderEngine::ColorRGBA8(( clusterObject_.texture_info.color.x), ( clusterObject_.texture_info.color.y), ( clusterObject_.texture_info.color.z), ( clusterObject_.texture_info.color.w)), clusterObject_.radius);
}

void SquareBox::Shapes::CalculatedLightShape::traceDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  glm::vec2 & mouseinworld, const  SquareBox::RenderEngine::ColorRGBA8 & borderColor_, const  float cameraScale_)
{
	debugRender_.drawEdge(clusterObject_.vertices, borderColor_, clusterObject_.angle);
	for (size_t i = 0; i < clusterObject_.vertices.size(); i++)
	{
		debugRender_.drawCircle(clusterObject_.vertices[i], borderColor_, 1.0f / cameraScale_);
	}

	calculateVisiblityPolygon(mouseinworld, clusterObject_.radius, clusterObject_.vec_of_edges, SquareBox::RenderEngine::ColorRGBA8(( clusterObject_.texture_info.color.x), ( clusterObject_.texture_info.color.y), ( clusterObject_.texture_info.color.z), ( clusterObject_.texture_info.color.w)));
	// Draw each triangle in fan

	for (int i = 0; i < vecVisibilityPolygonPoints.size() - 1 && vecVisibilityPolygonPoints.size()>0; i++)
	{
		debugRender_.drawLine(mouseinworld, glm::vec2(std::get<1>(vecVisibilityPolygonPoints[i]), std::get<2>(vecVisibilityPolygonPoints[i])), SquareBox::Color::yellow);
		debugRender_.drawLine(glm::vec2(std::get<1>(vecVisibilityPolygonPoints[i]), std::get<2>(vecVisibilityPolygonPoints[i])), glm::vec2(std::get<1>(vecVisibilityPolygonPoints[i + 1]), std::get<2>(vecVisibilityPolygonPoints[i + 1])), SquareBox::Color::yellow);
		debugRender_.drawLine(glm::vec2(std::get<1>(vecVisibilityPolygonPoints[i + 1]), std::get<2>(vecVisibilityPolygonPoints[i + 1])), mouseinworld, SquareBox::Color::yellow);

		debugRender_.drawCircle(mouseinworld, SquareBox::Color::yellow, clusterObject_.radius);
	}
	if (vecVisibilityPolygonPoints.size() > 0) {
		//close the triangle
		// Fan will have one open edge, so draw last point of fan to first
		debugRender_.drawLine(mouseinworld, glm::vec2(std::get<1>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1]), std::get<2>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1])), SquareBox::Color::blue);
		debugRender_.drawLine(glm::vec2(std::get<1>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1]), std::get<2>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1])), glm::vec2(std::get<1>(vecVisibilityPolygonPoints[0]), std::get<2>(vecVisibilityPolygonPoints[0])), SquareBox::Color::blue);
		debugRender_.drawLine(mouseinworld, glm::vec2(std::get<1>(vecVisibilityPolygonPoints[0]), std::get<2>(vecVisibilityPolygonPoints[0])), SquareBox::Color::blue);
	}

	//drawing the edges
	for (unsigned int i = 0; i < clusterObject_.vec_of_edges.size(); i++)
	{
		debugRender_.drawLine(clusterObject_.vec_of_edges[i].first, clusterObject_.vec_of_edges[i].second, borderColor_);
	}

	//drawing the diameter
	debugRender_.drawCircle(mouseinworld, SquareBox::RenderEngine::ColorRGBA8(( clusterObject_.texture_info.color.x), ( clusterObject_.texture_info.color.y), ( clusterObject_.texture_info.color.z), ( clusterObject_.texture_info.color.w)), clusterObject_.radius);
}

glm::vec2 SquareBox::Shapes::CalculatedLightShape::localCoordinatesToWorldCoordinates(const SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & localCoordinates_)
{
	return glm::vec2();
}

void SquareBox::Shapes::CalculatedLightShape::calculateVisiblityPolygon(const glm::vec2& orign, const float radius, const std::vector<std::pair<glm::vec2, glm::vec2>>& vecEdges, const SquareBox::RenderEngine::ColorRGBA8& color)
{
	//Get rid of existing polygon
	vecVisibilityPolygonPoints.clear();

	//For each Edge in polyMap
	for (auto &e1 : vecEdges)
	{
		//Take the start point , then the end points (we could use a pool of
		//no - duplicated points here , it would be more optimal )
		//each of our edges has two points
		for (int i = 0; i < 2; i++)
		{
			//for each edge we need to ast at lest two points to hit the start and end

			//since we need the angle of the array we will start by first calculating the gradient of the array

			float rdx, rdy;
			rdx = (i == 0 ? e1.first.x : e1.second.x) - orign.x;
			rdy = (i == 0 ? e1.first.y : e1.second.y) - orign.y;

			float base_ang = atan2f(rdy, rdx);

			float ang = 0;// the angle we shall shoot the array at

			//for each ray we shall cast three rays with a slight deviation between the two
			//1 directyly at the point and 1 a little bit either side

			for (int j = 0; j < 3; j++)
			{
				if (j == 0) ang = base_ang - 0.0001f;
				if (j == 0) ang = base_ang - 0.0001f;
				if (j == 1) ang = base_ang;
				if (j == 2) ang = base_ang + 0.0001f;

				//Create ray along angle for required distance
				rdx = radius * cosf(ang);
				rdy = radius * sinf(ang);

				float min_t1 = INFINITY;//minium length of aray
				float min_px = 0, min_py = 0, min_ang = 0;
				bool bValid = false;

				// Check for ray intersection with all edges
				for (auto &e2 : vecEdges)
				{
					// Create line segment vector
					float sdx = e2.second.x - e2.first.x;
					float sdy = e2.second.y - e2.first.y;
					//make sure the lines have a possiblity of intersecting , e,g they should not be paralle
					if (fabs(sdx - rdx) > 0.0f && fabs(sdy - rdy) > 0.0f)
					{
						// t2 is normalised distance from line segment start to line segment end of intersect point
						float t2 = (rdx * (e2.first.y - orign.y) + (rdy * (orign.x - e2.first.x))) / (sdx * rdy - sdy * rdx);//distance along the edge
						// t1 is normalised distance from source along ray to ray length of intersect point
						float t1 = (e2.first.x + sdx * t2 - orign.x) / rdx; //distance along the ray

						// If intersect point exists along ray, and along line
						// segment then intersect point is valid
						if (t1 > 0 && t2 >= 0 && t2 <= 1.0f)
						{
							// Check if this intersect point is closest to source. If
							// it is, then store this point and reject others
							if (t1 < min_t1)
							{
								min_t1 = t1;
								min_px = orign.x + rdx * t1;//calculate new intersection point
								min_py = orign.y + rdy * t1;
								min_ang = atan2f(min_py - orign.y, min_px - orign.x);//calculate new angle
								bValid = true;//only add the ray when its actually hitting something
							}
						}
					}
				}

				if (bValid)// Add intersection point to visibility polygon perimeter

					vecVisibilityPolygonPoints.push_back(std::tuple<int, int, int>{min_ang, min_px, min_py });
			}
		}
	}

	// Sort perimeter points by angle from source. This will allow
	// us to draw a triangle fan.
	sort(
		vecVisibilityPolygonPoints.begin(),
		vecVisibilityPolygonPoints.end(),
		[&](const std::tuple<float, float, float> &t1, const std::tuple<float, float, float> &t2)
	{
		return std::get<0>(t1) < std::get<0>(t2);
	});

	// Remove duplicate (or simply similar) points from polygon , since our array is even sorted , this is easy
	auto it = unique(
		vecVisibilityPolygonPoints.begin(),
		vecVisibilityPolygonPoints.end(),
		[&](const std::tuple<float, float, float> &t1, const std::tuple<float, float, float> &t2)
	{
		return fabs(std::get<1>(t1) - std::get<1>(t2)) < 0.1f && fabs(std::get<2>(t1) - std::get<2>(t2)) < 0.1f;
	});
	//actual dup removal
	vecVisibilityPolygonPoints.resize(distance(vecVisibilityPolygonPoints.begin(), it));
}

void SquareBox::Shapes::CalculatedLightShape::drawTriangleLight(const glm::vec2 & origin_point, const glm::vec2 & pointA, const glm::vec2 & pointB, SquareBox::RenderEngine::SpriteBatch & SpriteBatch_, const SquareBox::RenderEngine::ColorRGBA8 & color, bool maxLengthSet, float maxLength)
{
	//this function is a scam , its failing to give us the triangles specs expressed as lines
	//i will have to redo it

	/*
	Date 12th April 2021
	Author Kibande Steven
	today in a lecture my mind drifted off and i think i thought abour the solution to the problems we were
	facing before with this

	As We call this function we provide it with triangle points 1, 2 and 3
	and we expect it to run an algorithm that will recreate that triangle inform of many lines combined
	together to form a solid triangle object, and we are taking this lines as rays of light

	 now the issue we were getting came in while drawing the lines, because we were this time drawing the lines
	 using the sprite batch and not the debugRenderer

	 when drawing lines using the debugRenderer you just have to simply provide the lines start and end points
	 and thats it but when drawing using the spirte Batch you have to provide the angle of rotation
	 but also when doing this u have to remember that you want to mamntain the starting point from where it is and not change it
	 so there is a ka math to think about !!!!!!!!!!!!!

	*/

	//int currentX;
	//int currentY;
	//int point_A_x = pointA.x;
	//int point_A_y = pointA.y;

	//int point_B_x = pointB.x;
	//int point_B_y = pointB.y;

	//if (point_A_x > point_B_x) std::swap(point_A_x, point_B_x);
	//if (point_A_y > point_B_y) std::swap(point_A_y, point_B_y);

	//for (currentX = point_A_x; currentX <= point_B_x; currentX++)
	//{
	//	for (currentY = point_A_y; currentY <= point_B_y; currentY++)
	//	{
	//		if (isInsideTriangle(origin_point, pointA, pointB, glm::vec2(currentX, currentY))) {
	//			drawRay(origin_point, glm::vec2(currentX, currentY), SpriteBatch_, color);
	//		}
	//	}
	//}
}

void SquareBox::Shapes::CalculatedLightShape::drawRay(const glm::vec2 & a, const glm::vec2 & b, SquareBox::RenderEngine::SpriteBatch & SpriteBatch_, const SquareBox::RenderEngine::ColorRGBA8 & color)
{
	const float magnifier = 1;
	const float & length = fabs(glm::length(a - b));
	const float & angleInRadians = std::atan2(b.y - a.y, b.x - a.x);
	const float & height = 0.1f;
	const glm::vec4 & destRect = glm::vec4(a.x, a.y, length, height);
	//this will give us a box grid with the source of our light
	// as the center and the brightest also,
	//the further away from the center the less bright a light is
	const glm::vec4 & uvRect = glm::vec4(0.0f, -0.1f, 1.0f, 0.2f);

	SpriteBatch_.draw(destRect, uvRect, 0, 0.0f, color, angleInRadians);
}

bool SquareBox::Shapes::CalculatedLightShape::isInsideTriangle(const glm::vec2 & origin_, const glm::vec2 & pointA_, const glm::vec2 & pointB_, const glm::vec2 & point_)
{
	float x1, y1, x2, y2, x3, y3, x, y;
	x1 = origin_.x;
	y1 = origin_.y;

	x2 = pointA_.x;
	y2 = pointA_.y;

	x3 = pointB_.x;
	y3 = pointB_.y;

	x = point_.x;
	y = point_.y;

	/* Calculate area of triangle ABC */
	const float A = area(x1, y1, x2, y2, x3, y3);

	/* Calculate area of triangle PBC */
	const float A1 = area(x, y, x2, y2, x3, y3);

	/* Calculate area of triangle PAC */
	const float A2 = area(x1, y1, x, y, x3, y3);

	/* Calculate area of triangle PAB */
	const float A3 = area(x1, y1, x2, y2, x, y);

	/* Check if sum of A1, A2 and A3 is same as A */
	//if we are using floats here , the possibility of these being true is a tenth of infinity
	return (A == A1 + A2 + A3);
}

float SquareBox::Shapes::CalculatedLightShape::area(float x1, float y1, float x2, float y2, float x3, float y3)
{
	return abs((x1*(y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}