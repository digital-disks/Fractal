#pragma once

#include <vector>

#include "PathCommand.hpp"

struct TTFPoint {
	int X;
	int Y;
	bool OnCurve;
};

struct FloatPoint {
	float X;
	float Y;
};

struct GlyphData {
	std::vector<TTFPoint> Points;
	std::vector<uint16_t> ContourEndIndices;
};

class GeometryEngine
{
public:
	GeometryEngine() = default;
	~GeometryEngine() = default;

	GlyphData ConvertToTTF(
		const std::vector<PathCommand>& SVGCommands,
		float CanvasWidth,
		float CanvasHeight,
		int EMSquare = 2048
	);

private:

	float PenX = 0.0f;
	float PenY = 0.0f;

	float SubpathStartX = 0.0f;
	float SubpathStartY = 0.0f;

	void ApproximateCubic(FloatPoint P0, FloatPoint P1, FloatPoint P2, FloatPoint P3, std::vector <TTFPoint>& OutPoints, int Depth = 0);

	std::vector<FloatPoint> ArcToCubics(float x0, float y0, float rx, float ry, float angle, bool largeArcFlag, bool sweepFlag, float x, float y);
};