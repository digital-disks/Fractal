#include "GeometryEngine.hpp"

#include <cmath>
#include <algorithm>

constexpr float PI = 3.14159265358979323846f;

void GeometryEngine::ApproximateCubic(FloatPoint P0, FloatPoint P1, FloatPoint P2, FloatPoint P3, std::vector <TTFPoint>& OutPoints, int Depth) {

	FloatPoint Q1 = {
		(3.0f * (P1.X + P2.X) - (P0.X + P3.X)) / 4.0f,
		(3.0f * (P1.Y + P2.Y) - (P0.Y + P3.Y)) / 4.0f
	};

	float DifferenceX = (P3.X - 3.0f * P2.X + 3.0f * P1.X - P0.X) * (3.0f / 64.0f);
	float DifferenceY = (P3.Y - 3.0f * P2.Y + 3.0f * P1.Y - P0.Y) * (3.0f / 64.0f);

	float Error = std::sqrt(DifferenceX * DifferenceX + DifferenceY * DifferenceY);

	if (Error > 1.0f && Depth < 10) {

		FloatPoint P01 = { (P0.X + P1.X) / 2.0f, (P0.Y + P1.Y) / 2.0f };
		FloatPoint P12 = { (P1.X + P2.X) / 2.0f, (P1.Y + P2.Y) / 2.0f };
		FloatPoint P23 = { (P2.X + P3.X) / 2.0f, (P2.Y + P3.Y) / 2.0f };

		FloatPoint P012 = { (P01.X + P12.X) / 2.0f, (P01.Y + P12.Y) / 2.0f };
		FloatPoint P123 = { (P12.X + P23.X) / 2.0f, (P12.Y + P23.Y) / 2.0f };

		FloatPoint Midpoint = { (P012.X + P123.X) / 2.0f, (P012.Y + P123.Y) / 2.0f };

		ApproximateCubic(P0, P01, P012, Midpoint, OutPoints, Depth + 1);
		ApproximateCubic(Midpoint, P123, P23, P3, OutPoints, Depth + 1);
	}

	else {

		OutPoints.push_back({ static_cast<int>(std::round(Q1.X)), static_cast<int>(std::round(Q1.Y)), false });
		OutPoints.push_back({ static_cast<int>(std::round(P3.X)), static_cast<int>(std::round(P3.Y)), true });
	}
}

std::vector<FloatPoint> GeometryEngine::ArcToCubics(float StartX, float StartY, float RadiusX, float RadiusY, float Angle, bool LargeArcFlag, bool SweepFlag, float EndX, float EndY) {

	std::vector<FloatPoint> Cubics;

	if (StartX == EndX && StartY == EndY) return Cubics;

	if (RadiusX == 0.0f || RadiusY == 0.0f) {
		Cubics.push_back({ StartX + (EndX - StartX) / 3.0f, StartY + (EndY - StartY) / 3.0f });
		Cubics.push_back({ StartX + 2.0f * (EndX - StartX) / 3.0f, StartY + 2.0f * (EndY - StartY) / 3.0f });
		Cubics.push_back({ EndX, EndY });
		return Cubics;
	}

	RadiusX = std::abs(RadiusX);
	RadiusY = std::abs(RadiusY);
	float RadianAngle = Angle * PI / 180.0f;
	float CosPhi = std::cos(RadianAngle);
	float SinPhi = std::sin(RadianAngle);

	float DeltaX = (StartX - EndX) / 2.0f;
	float DeltaY = (StartY - EndY) / 2.0f;
	float X1Prime = CosPhi * DeltaX + SinPhi * DeltaY;
	float Y1Prime = -SinPhi * DeltaX + CosPhi * DeltaY;

	float Lambda = (X1Prime * X1Prime) / (RadiusX * RadiusX) + (Y1Prime * Y1Prime) / (RadiusY * RadiusY);
	if (Lambda > 1.0f) {
		float SqrtLambda = std::sqrt(Lambda);
		RadiusX *= SqrtLambda;
		RadiusY *= SqrtLambda;
	}

	float RadiusXSquared = RadiusX * RadiusX;
	float RadiusYSquared = RadiusY * RadiusY;
	float X1PrimeSquared = X1Prime * X1Prime;
	float Y1PrimeSquared = Y1Prime * Y1Prime;

	float Radicand = (RadiusXSquared * RadiusYSquared - RadiusXSquared * Y1PrimeSquared - RadiusYSquared * X1PrimeSquared) / (RadiusXSquared * Y1PrimeSquared + RadiusYSquared * X1PrimeSquared);
	Radicand = std::max(Radicand, 0.0f);
	float Coefficient = std::sqrt(Radicand);
	if (LargeArcFlag == SweepFlag) Coefficient = -Coefficient;

	float CxPrime = Coefficient * ((RadiusX * Y1Prime) / RadiusY);
	float CyPrime = Coefficient * (-(RadiusY * X1Prime) / RadiusX);

	float CenterX = CosPhi * CxPrime - SinPhi * CyPrime + (StartX + EndX) / 2.0f;
	float CenterY = SinPhi * CxPrime + CosPhi * CyPrime + (StartY + EndY) / 2.0f;

	auto VectorAngle = [](float UX, float UY, float VX, float VY) {
		float Sign = (UX * VY - UY * VX) < 0.0f ? -1.0f : 1.0f;
		float Dot = UX * VX + UY * VY;
		float Length = std::sqrt(UX * UX + UY * UY) * std::sqrt(VX * VX + VY * VY);
		float Value = std::max(-1.0f, std::min(1.0f, Dot / Length));
		return Sign * std::acos(Value);
		};

	float StartAngle = VectorAngle(1.0f, 0.0f, (X1Prime - CxPrime) / RadiusX, (Y1Prime - CyPrime) / RadiusY);
	float DeltaAngle = VectorAngle((X1Prime - CxPrime) / RadiusX, (Y1Prime - CyPrime) / RadiusY, (-X1Prime - CxPrime) / RadiusX, (-Y1Prime - CyPrime) / RadiusY);

	if (!SweepFlag && DeltaAngle > 0.0f) DeltaAngle -= 2.0f * PI;
	else if (SweepFlag && DeltaAngle < 0.0f) DeltaAngle += 2.0f * PI;

	int NumSegments = static_cast<int>(std::ceil(std::abs(DeltaAngle) / (PI / 2.0f)));
	float SegmentAngle = DeltaAngle / NumSegments;
	float Alpha = std::sin(SegmentAngle) * (std::sqrt(4.0f + 3.0f * std::pow(std::tan(SegmentAngle / 2.0f), 2.0f)) - 1.0f) / 3.0f;

	float CurrentAngle = StartAngle;
	float CurrentX = StartX;
	float CurrentY = StartY;

	for (int i = 0; i < NumSegments; ++i) {
		float NextAngle = CurrentAngle + SegmentAngle;
		float NextX = CenterX + RadiusX * std::cos(NextAngle) * CosPhi - RadiusY * std::sin(NextAngle) * SinPhi;
		float NextY = CenterY + RadiusX * std::cos(NextAngle) * SinPhi + RadiusY * std::sin(NextAngle) * CosPhi;

		if (i == NumSegments - 1) {
			NextX = EndX;
			NextY = EndY;
		}

		float DeltaX1 = -RadiusX * std::sin(CurrentAngle) * CosPhi - RadiusY * std::cos(CurrentAngle) * SinPhi;
		float DeltaY1 = -RadiusX * std::sin(CurrentAngle) * SinPhi + RadiusY * std::cos(CurrentAngle) * CosPhi;
		float DeltaX2 = -RadiusX * std::sin(NextAngle) * CosPhi - RadiusY * std::cos(NextAngle) * SinPhi;
		float DeltaY2 = -RadiusX * std::sin(NextAngle) * SinPhi + RadiusY * std::cos(NextAngle) * CosPhi;

		Cubics.push_back({ CurrentX + Alpha * DeltaX1, CurrentY + Alpha * DeltaY1 });
		Cubics.push_back({ NextX - Alpha * DeltaX2, NextY - Alpha * DeltaY2 });
		Cubics.push_back({ NextX, NextY });

		CurrentX = NextX;
		CurrentY = NextY;
		CurrentAngle = NextAngle;
	}

	return Cubics;
}

GlyphData GeometryEngine::ConvertToTTF(
	const std::vector<PathCommand>& SVGCommands,
	float CanvasWidth,
	float CanvasHeight,
	int EMSquare
) {

	GlyphData Result;

	if (CanvasWidth <= 0.0f || CanvasHeight <= 0.0f) {
		return Result;
	}

	float Scale = static_cast<float>(EMSquare) / std::max(CanvasWidth, CanvasHeight);

	PenX = 0.0f;
	PenY = 0.0f;

	SubpathStartX = 0.0f;
	SubpathStartY = 0.0f;

	char LastCommand = ' ';
	float LastCubicControlX = 0.0f;
	float LastCubicControlY = 0.0f;

	auto AddTTFPoint = [&](float X, float Y) {
		int TTFCoordX = static_cast<int>(std::round(X * Scale));
		int TTFCoordY = static_cast<int>(std::round((CanvasHeight - Y) * Scale));
		Result.Points.push_back({ TTFCoordX, TTFCoordY, true });
	};

	auto Transform = [&](float X, float Y) -> FloatPoint {
		return { X * Scale, (CanvasHeight - Y) * Scale };
	};

	for (const auto& CMD : SVGCommands) {

		char CommandType = CMD.type;
		const auto& CommandArguments = CMD.args;
		size_t ArgumentCount = CommandArguments.size();

		if (CommandType == 'M' || CommandType == 'm') {
			for (size_t Index = 0; Index + 1 < ArgumentCount; Index += 2) {
				if (CommandType == 'M') {
					PenX = CommandArguments[Index];
					PenY = CommandArguments[Index + 1];
				}
				else {
					PenX += CommandArguments[Index];
					PenY += CommandArguments[Index + 1];
				}

				if (Index == 0) {
					SubpathStartX = PenX;
					SubpathStartY = PenY;
				}

				AddTTFPoint(PenX, PenY);

				if (CommandType == 'M') CommandType = 'L';
				if (CommandType == 'm') CommandType = 'l';
			}
		}
		else if (CommandType == 'L' || CommandType == 'l') {
			for (size_t Index = 0; Index + 1 < ArgumentCount; Index += 2) {
				if (CommandType == 'L') {
					PenX = CommandArguments[Index];
					PenY = CommandArguments[Index + 1];
				}
				else {
					PenX += CommandArguments[Index];
					PenY += CommandArguments[Index + 1];
				}

				AddTTFPoint(PenX, PenY);
			}
		}
		else if (CommandType == 'H' || CommandType == 'h') {
			for (size_t Index = 0; Index < ArgumentCount; Index++) {
				if (CommandType == 'H') PenX = CommandArguments[Index];
				else PenX += CommandArguments[Index];
				AddTTFPoint(PenX, PenY);
			}
		}
		else if (CommandType == 'V' || CommandType == 'v') {
			for (size_t Index = 0; Index < ArgumentCount; Index++) {
				if (CommandType == 'V') PenY = CommandArguments[Index];
				else PenY += CommandArguments[Index];
				AddTTFPoint(PenX, PenY);
			}
		}
		else if (CommandType == 'C' || CommandType == 'c') {
			for (size_t Index = 0; Index + 5 < ArgumentCount; Index += 6) {
				float StartX = PenX;
				float StartY = PenY;

				float P1X = CommandArguments[Index];
				float P1Y = CommandArguments[Index + 1];
				float P2X = CommandArguments[Index + 2];
				float P2Y = CommandArguments[Index + 3];
				float P3X = CommandArguments[Index + 4];
				float P3Y = CommandArguments[Index + 5];

				if (CommandType == 'c') {
					P1X += StartX; P1Y += StartY;
					P2X += StartX; P2Y += StartY;
					P3X += StartX; P3Y += StartY;
				}

				LastCubicControlX = P2X;
				LastCubicControlY = P2Y;
				LastCommand = 'C';

				FloatPoint ScaledP0 = Transform(StartX, StartY);
				FloatPoint ScaledP1 = Transform(P1X, P1Y);
				FloatPoint ScaledP2 = Transform(P2X, P2Y);
				FloatPoint ScaledP3 = Transform(P3X, P3Y);

				ApproximateCubic(ScaledP0, ScaledP1, ScaledP2, ScaledP3, Result.Points);

				PenX = P3X;
				PenY = P3Y;
			}
		}
		else if (CommandType == 'S' || CommandType == 's') {
			for (size_t Index = 0; Index + 3 < ArgumentCount; Index += 4) {
				float StartX = PenX;
				float StartY = PenY;

				float P1X, P1Y;

				if (LastCommand == 'C' || LastCommand == 'S') {
					P1X = 2.0f * StartX - LastCubicControlX;
					P1Y = 2.0f * StartY - LastCubicControlY;
				}
				else {
					P1X = StartX;
					P1Y = StartY;
				}

				float P2X = CommandArguments[Index];
				float P2Y = CommandArguments[Index + 1];
				float P3X = CommandArguments[Index + 2];
				float P3Y = CommandArguments[Index + 3];

				if (CommandType == 's') {
					P2X += StartX; P2Y += StartY;
					P3X += StartX; P3Y += StartY;
				}

				LastCubicControlX = P2X;
				LastCubicControlY = P2Y;
				LastCommand = 'S';

				FloatPoint ScaledP0 = Transform(StartX, StartY);
				FloatPoint ScaledP1 = Transform(P1X, P1Y);
				FloatPoint ScaledP2 = Transform(P2X, P2Y);
				FloatPoint ScaledP3 = Transform(P3X, P3Y);

				ApproximateCubic(ScaledP0, ScaledP1, ScaledP2, ScaledP3, Result.Points);

				PenX = P3X;
				PenY = P3Y;
			}
		}

		else if (CommandType == 'A' || CommandType == 'a') {

			for (size_t Index = 0; Index + 6 < ArgumentCount; Index += 7) {

				float ArcStartX = PenX;
				float ArcStartY = PenY;

				float RadiusX = CommandArguments[Index];
				float RadiusY = CommandArguments[Index + 1];
				float AxisRotation = CommandArguments[Index + 2];
				bool LargeArcFlag = CommandArguments[Index + 3] != 0.0f;
				bool ArcSweepFlag = CommandArguments[Index + 4] != 0.0f;
				float ArcEndX = CommandArguments[Index + 5];
				float ArcEndY = CommandArguments[Index + 6];

				if (CommandType == 'a') {
					ArcEndX += ArcStartX;
					ArcEndY += ArcStartY;
				}

				std::vector<FloatPoint> GeneratedCubics = ArcToCubics(ArcStartX, ArcStartY, RadiusX, RadiusY, AxisRotation, LargeArcFlag, ArcSweepFlag, ArcEndX, ArcEndY);

				FloatPoint CurrentP0 = Transform(ArcStartX, ArcStartY);

				for (size_t i = 0; i < GeneratedCubics.size(); i += 3) {

					FloatPoint ScaledP1 = Transform(GeneratedCubics[i].X, GeneratedCubics[i].Y);
					FloatPoint ScaledP2 = Transform(GeneratedCubics[i + 1].X, GeneratedCubics[i + 1].Y);
					FloatPoint ScaledP3 = Transform(GeneratedCubics[i + 2].X, GeneratedCubics[i + 2].Y);

					ApproximateCubic(CurrentP0, ScaledP1, ScaledP2, ScaledP3, Result.Points);

					CurrentP0 = ScaledP3;
				}

				PenX = ArcEndX;
				PenY = ArcEndY;
			}
		}

		else if (CommandType == 'Z' || CommandType == 'z') {

			PenX = SubpathStartX;
			PenY = SubpathStartY;

			if (!Result.Points.empty()) {
				Result.ContourEndIndices.push_back(static_cast<uint16_t>(Result.Points.size() - 1));
			}
		}

		if (CommandType != 'C' && CommandType != 'c' && CommandType != 'S' && CommandType != 's') {
			LastCommand = CommandType;
		}
	}

	if (!Result.Points.empty()) {

		if (Result.ContourEndIndices.empty() || Result.ContourEndIndices.back() != Result.Points.size() - 1) {
			Result.ContourEndIndices.push_back(static_cast<uint16_t>(Result.Points.size() - 1));
		}

		int XMinimum = 32767, XMaximum = -32768;
		int YMinimum = 32767, YMaximum = -32768;

		for (const auto& Point : Result.Points) {

			if (Point.X < XMinimum) XMinimum = Point.X;
			if (Point.X > XMaximum) XMaximum = Point.X;
			if (Point.Y < YMinimum) YMinimum = Point.Y;
			if (Point.Y > YMaximum) YMaximum = Point.Y;
		}

		int CurrentCenterX = (XMinimum + XMaximum) / 2;
		int CurrentCenterY = (YMinimum + YMaximum) / 2;

		int TargetCenterX = EMSquare / 2;
		int TargetCenterY = EMSquare / 2;

		int ShiftX = TargetCenterX - CurrentCenterX;
		int ShiftY = TargetCenterY - CurrentCenterY;

		for (auto& Point : Result.Points) {

			Point.X += ShiftX;
			Point.Y += ShiftY;
		}

	}

	return Result;
}