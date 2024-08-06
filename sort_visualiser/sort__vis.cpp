#include <SDL.h>
#undef main
#include <SDL_mixer.h>
#include <iostream>
#include <random>
#include <algorithm>
#include <math.h>

#define MIN_VAL 1
#define MAX_VAL 200
#define COUNT 200
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define DELAY_TIME 10
#define AMP 28000
#define SAMPLE_RATE 44100
#define MAX_FREQ 1500
#define MIN_FREQ 100

float triangle_wave(float t, float frequency) {
	float period = 1.0f / frequency;
	float value = 4.0f * fabs(fmod(t, period) - period / 2) / period - 1.0f;
	return value;
}


void play_sound(SDL_AudioDeviceID audio_dev, SDL_AudioSpec wav_spec, int b, int r, int delay = DELAY_TIME) {
	int duration_ms = delay;
	int sample_count = (wav_spec.freq * duration_ms) / 1000;

	std::vector<int16_t> samples(sample_count);

	for (int i = 0; i < sample_count; ++i) {
		float t = static_cast<float>(i) / wav_spec.freq;
		float sample1 = static_cast<float>(triangle_wave(t, 1500 * (static_cast<double>(b) / MAX_VAL)));
		float sample2 = static_cast<float>(triangle_wave(t, 1500 * (static_cast<double>(r) / MAX_VAL)));
		samples[i] = 5000*(sample1 + sample2)/2;
	}
	SDL_QueueAudio(audio_dev, samples.data(), samples.size() * sizeof(int16_t));
	SDL_PauseAudioDevice(audio_dev, 0);
}

void draw_state(std::vector<int>& v, SDL_Renderer* renderer, int b, int r) {
	// current idx should be blue, idx being compared to is red
	int idx = 0;
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

void draw(std::vector<int>& v, SDL_Renderer* renderer, int b, int r, int delay = DELAY_TIME) {
	// clear screen
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	draw_state(v, renderer, b, r);

	// present
	SDL_RenderPresent(renderer);
	SDL_Delay(delay);

}

void pass_over(std::vector<int>& v, SDL_Renderer* renderer, SDL_AudioDeviceID audio_dev, SDL_AudioSpec wav_spec) {
	for (int i = 1; i < COUNT; i++) {
		draw(v, renderer, i, i + 1, 10);
		play_sound(audio_dev, wav_spec, i, i + 1);
	}
	for (int i = 1; i < COUNT; i++) {
		draw(v, renderer, i, i + 1, 20);
		play_sound(audio_dev, wav_spec, i, i + 1, 20);
	}
	for (int i = 1; i < COUNT; i++) {
		draw(v, renderer, i, i + 1, 30);
		play_sound(audio_dev, wav_spec, i, i + 1, 30);
	}
	draw(v, renderer, MAX_VAL+1, MAX_VAL+1);

}

void merge(std::vector<int>& v, int start, int mid, int end, SDL_AudioDeviceID audio_dev, SDL_AudioSpec wav_spec, SDL_Renderer* renderer) {
	int start2 = mid + 1;

	if (v[mid] <= v[start2]) {
		return;
	}

	while (start <= mid && start2 <= end) {
		if (v[start] <= v[start2]) {
			start++;
		}
		else {
			int value = v[start2];
			int index = start2;

			while (index != start) {
				play_sound(audio_dev, wav_spec, v[index], v[index - 1]);
				draw(v, renderer, index, index - 1);
				v[index] = v[index - 1];
				index--;
			}
			v[start] = value;

			start++;
			mid++;
			start2++;
		}
	}
}

void merge_sort(std::vector<int>& v, int l, int r, SDL_AudioDeviceID audio_dev, SDL_AudioSpec wav_spec, SDL_Renderer* renderer) {
	if (l < r) {
		int m = l + (r - l) / 2;

		merge_sort(v, l, m, audio_dev, wav_spec, renderer);
		merge_sort(v, m + 1, r, audio_dev, wav_spec, renderer);

		merge(v, l, m, r, audio_dev, wav_spec, renderer);
	}
}

void insertion_sort(std::vector<int>& v, SDL_AudioDeviceID audio_dev, SDL_AudioSpec wav_spec, SDL_Renderer* renderer) {
	for (int i = 1; i < COUNT; i++) {
		int val = v[i];
		int j = i - 1;
		while (j >= 0 && val < v[j]) {
			v[j + 1] = v[j];
			play_sound(audio_dev, wav_spec, v[j], v[j+1]);
			draw(v, renderer, j, j+1);
			j = j - 1;
		
		}
		v[j + 1] = val;
		draw(v, renderer, MAX_VAL+1, MAX_VAL+1);
}
}

int partition(std::vector<int>& arr, int low, int high, SDL_AudioDeviceID audio_dev, SDL_AudioSpec wav_spec, SDL_Renderer* renderer) {

	int pivot = arr[high];

	int i = low - 1;

	for (int j = low; j <= high - 1; j++) {
		if (arr[j] < pivot) {
			i++;
			std::swap(arr[i], arr[j]);
			play_sound(audio_dev, wav_spec, arr[i], arr[j]);
			draw(arr, renderer, i, j);
		}
	}

	std::swap(arr[i + 1], arr[high]);
	play_sound(audio_dev, wav_spec, arr[i + 1], arr[high]);
	draw(arr, renderer, i + 1, high);
	return i + 1;
}

// The quick_sort function implementation
void quick_sort(std::vector<int>& arr, int low, int high, SDL_AudioDeviceID audio_dev, SDL_AudioSpec wav_spec, SDL_Renderer* renderer) {

	if (low < high) {
		int pi = partition(arr, low, high, audio_dev, wav_spec, renderer);
		quick_sort(arr, low, pi - 1, audio_dev, wav_spec, renderer);
		quick_sort(arr, pi + 1, high, audio_dev, wav_spec, renderer);
	}
}

int main() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> d(MIN_VAL, MAX_VAL);
	std::vector<int> v;
	
	bool running = true;

	// init audio setup
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		std::cerr << "SDL_Init Error : " << SDL_GetError() << std::endl;
	}

	SDL_AudioSpec wav_spec;
	SDL_zero(wav_spec);
	wav_spec.freq = SAMPLE_RATE;
	wav_spec.format = AUDIO_S16SYS;
	wav_spec.channels = 1;
	wav_spec.samples = 4096;
	wav_spec.callback = NULL; //audio_callback;

	SDL_AudioDeviceID audio_dev = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);

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
	SDL_RenderSetScale(renderer, static_cast<float>(WINDOW_WIDTH) / static_cast<float>(COUNT), static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(MAX_VAL)); // throw error if we end up doing subpixel division
	//SDL_RenderSetLogicalSize(renderer, COUNT, MAX_VAL);
	//play_sound(audio_dev, wav_spec, 90, 90);

	// insertion sort
	//insertion_sort(v, audio_dev, wav_spec, renderer);

	// in-place merge sort
	//merge_sort(v, 0, v.size() - 1, audio_dev, wav_spec, renderer);

	// quick sort
	quick_sort(v, 0, v.size() - 1, audio_dev, wav_spec, renderer);
	

	// do the funny pass over thing
	pass_over(v, renderer, audio_dev, wav_spec);

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
	SDL_CloseAudioDevice(audio_dev);
	//Quit SDL
	SDL_Quit();

	return 0;


}