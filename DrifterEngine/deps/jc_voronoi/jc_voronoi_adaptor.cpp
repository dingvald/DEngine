#include "pch.h"
#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi_adaptor.h"

namespace
{
	sf::Vector2i jcvPoint2Vector2i(jcv_point point)
	{
		sf::Vector2i result;
		result.x = static_cast<int>(point.x);
		result.y = static_cast<int>(point.y);
		return result;
	}
	jcv_point vector2i2jcvPoint(sf::Vector2i point)
	{
		return jcv_point{ .x = static_cast<float>(point.x), .y = static_cast<float>(point.y) };
	}
}

VoronoiSite::VoronoiSite(jcv_site site)
	: point(jcvPoint2Vector2i(site.p))
	, index(site.index)
	, _graphEdge(site.edges)
{}

void VoronoiSite::forEachHalfEdge(std::function<void(Edge)> func)
{
	const jcv_graphedge* edge = _graphEdge;
	while (edge)
	{
		auto point1 = jcvPoint2Vector2i(edge->pos[0]);
		auto point2 = jcvPoint2Vector2i(edge->pos[1]);
		func(std::make_pair(point1, point2));
	}
}

VoronoiDiagram::VoronoiDiagram(const std::vector<sf::Vector2i> points)
{
	auto jcv_points = _pointCopy(points);
	memset(&_diagram, 0, sizeof(jcv_diagram));
	jcv_diagram_generate(jcv_points.size(), jcv_points.data(), nullptr, nullptr, &_diagram);
}

VoronoiDiagram::~VoronoiDiagram()
{
	jcv_diagram_free(&_diagram);
}

void VoronoiDiagram::forEachSite(std::function<void(VoronoiSite)> func) const
{
	const jcv_site* sites = jcv_diagram_get_sites(&_diagram);
	for (int i = 0; i < _diagram.numsites; ++i)
	{
		VoronoiSite site = { sites[i] };
		func(std::move(site));
	}
}

void VoronoiDiagram::forEachEdge(std::function<void(Edge edge)> func) const
{
	const jcv_edge* edge = jcv_diagram_get_edges(&_diagram);
	while (edge)
	{
		auto point1 = jcvPoint2Vector2i(edge->pos[0]);
		auto point2 = jcvPoint2Vector2i(edge->pos[1]);

		func(std::make_pair(point1, point2));

		edge = jcv_diagram_get_next_edge(edge);
	}
}

void VoronoiDiagram::forEachRelaxedPoint(std::function<void(sf::Vector2i)> func) const
{
	const jcv_site* sites = jcv_diagram_get_sites(&_diagram);
	for (int i = 0; i < _diagram.numsites; ++i)
	{
		const jcv_site* site = &sites[i];
		jcv_point sum = site->p;
		int count = 1;

		const jcv_graphedge* edge = site->edges;

		while (edge)
		{
			sum.x += edge->pos[0].x;
			sum.y += edge->pos[0].y;
			++count;
			edge = edge->next;
		}
		sf::Vector2i point = { static_cast<int>(sum.x) / count, static_cast<int>(sum.y) / count };
		func(std::move(point));
	}
}

std::vector<jcv_point> VoronoiDiagram::_pointCopy(const std::vector<sf::Vector2i>& points) const
{
	std::vector<jcv_point> result;
	result.reserve(points.size());

	for (auto&& point : points)
	{
		result.push_back(jcv_point{ .x = static_cast<float>(point.x), .y = static_cast<float>(point.y) });
	}

	return result;
}

