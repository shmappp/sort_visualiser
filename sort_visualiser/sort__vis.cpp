#include <SDL.h>
#undef main
#include <iostream>
#include <random>
#include <algorithm>

#define MIN_VAL 1
#define MAX_VAL 99
#define COUNT 100
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define DELAY_TIME 5

void draw_state(std::vector<int>& v, SDL_Renderer* renderer, int b, int r) {
	// current idx should be blue, idx being compared to is red
	int idx = 0;
	//int step_size =  
	for (auto i : v) {
		if (idx == b) { SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); }
		else if (idx == r) {SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);}
		else 
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		}
		
		SDL_RenderDrawLine(renderer, idx, MAX_VAL, idx, MAX_VAL - i);
		idx += 1;
	}
}

void draw(std::vector<int>& v, SDL_Renderer* renderer, int b, int r) {
	// clear screen
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	draw_state(v, renderer, b, r);

	// present
	SDL_RenderPresent(renderer);
	SDL_Delay(DELAY_TIME);

}

int main() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> d(MIN_VAL, MAX_VAL);
	std::vector<int> v;
	
	bool running = true;

	// generate vec
	for (int i = 0; i < COUNT; i++) {
		v.push_back(d(gen));
	}
	
	// print vec
	for (auto i : v) {
		std::cout << i << ' ';
	}
	std::cout << std::endl;

	// init sdl renderer and window context
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
	SDL_RenderSetScale(renderer, WINDOW_WIDTH / COUNT, WINDOW_HEIGHT / MAX_VAL); // throw error if we end up doing subpixel division
	//SDL_RenderSetLogicalSize(renderer, COUNT, MAX_VAL);
	

	// insertion sort
	for (int i = 1; i < COUNT; i++) {
		int val = v[i];
		int j = i - 1;
		while (j >= 0 && val < v[j]) {
			v[j + 1] = v[j];
			draw(v, renderer, j, j+1);
			j = j - 1;

			
		}
		v[j + 1] = val;
		draw(v, renderer, MAX_VAL+1, MAX_VAL+1);
	}


	SDL_Event event;
	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
				case SDL_QUIT:
					running = false;
					break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.scancode) 
					{
						case SDL_SCANCODE_ESCAPE:
						running = false;
						break;
					}
					break;
			}
		}
	}
	

	// print vec
	for (auto i : v) {
		std::cout << i << ' ';
	}
	std::cout << std::endl;
	
	//Quit SDL
	SDL_Quit();

	return 0;


}