#pragma once
#include <jc_voronoi/jc_voronoi.h>

using Edge = std::pair<sf::Vector2i, sf::Vector2i>;

struct VoronoiSite
{
	VoronoiSite(jcv_site site);

	sf::Vector2i point;
	size_t index;
	void forEachHalfEdge(std::function<void(Edge)> func);

private:
	const jcv_graphedge* _graphEdge;
};

class VoronoiDiagram
{
public:
	VoronoiDiagram(const std::vector<sf::Vector2i> points);
	~VoronoiDiagram();

	void forEachSite(std::function<void(VoronoiSite)> func) const;

	void forEachEdge(std::function<void(Edge)> func) const;
	std::vector<Edge> getEdges() const;
	void forEachRelaxedPoint(std::function<void(sf::Vector2i)> func) const;
	std::vector<sf::Vector2i> getRelaxedPoints() const;

private:
	std::vector<jcv_point> _pointCopy(const std::vector<sf::Vector2i>& points) const;

private:
	jcv_diagram _diagram;
};