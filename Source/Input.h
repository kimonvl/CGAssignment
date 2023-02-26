#pragma once

class Input {
public:
	static void Init(void* window);
	static bool IsKeyPressed(int key);
	static bool IsMouseButtonPressed(int button);
};
