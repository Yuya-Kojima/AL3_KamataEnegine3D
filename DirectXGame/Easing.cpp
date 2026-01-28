#include "Easing.h"

float EaseOut(float start, float end, float t) {

	t = std::clamp(t, 0.0f, 1.0f);

	float easeT = 1.0f - powf(1.0f - t, 3.0f);

	return start + (end - start) * easeT;
}

float EaseIn(float start, float end, float t) {
	t = std::clamp(t, 0.0f, 1.0f);

	float easeT = t * t;

	return start + (end - start) * easeT;
}
