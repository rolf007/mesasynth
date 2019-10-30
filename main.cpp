#include "sequencer.h"
#include "synthesizer.h"
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <iostream>
#include <math.h>

using namespace std;

bool done = false;

#if 0
const char base = "####/8(g2,g1)/=2(c2,c1)g2cecgcgecegCis{c2gcecgcgcecg}@@Dis7{d2acfcadacfca)@@"
#endif

const char* test0 = "c2.e'g.c.g'e.c===";
const char* fantasie = "%*1$/4g2$2c2egcgecegcgeCis{c2g2cecgc2g2cecg}@@Dis{d2a2cfcad2a2cfca}@@";
const char* chord0 = "%*1$2cc#d(g,e)c#(g#,e#)d";
const char* chord = "$.5A{c(c,e,g)g(g,d,f,b'b#')c(c,e,g)g#'`a.33<'`a.33#'`a#'`b.33<'`c.67<'`}(c,e,g,c3)@@c0";
const char* david = "$.7A{(c2,f2,g2)}@@@@B{(c2,e2,g2)}@@@@C{(c2,d2,g2)}@@@@BBBB";
const char* scala = "$2cdefgabc";
class Foo {
public:
	Foo(int sampleRate) : manager(80), bufferPool(10), syn(), seq(syn, 120.0, sampleRate), leftOver(0)
	{
		seq.addTrack(scala);
	}
	Foo(const Foo&) = delete;
	ChainPool<Value>::Scope manager;
	ChainPool<AudioBuffer>::Scope bufferPool;
	Synthesizer syn;
	Sequencer seq;
	unsigned leftOver;
};

void audio_callback(void* user_data, Uint8* raw_buffer, int bytes)
{
    Sint16 *buffer = (Sint16*)raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
	Foo* foo = (Foo*)(user_data);
	int16_t* begin = buffer;
	while(true) {
		int16_t* end = begin;
		if (foo->leftOver)
			end += foo->leftOver;
		else
	    	end += foo->seq.timeToNext();
	    if (end >= buffer+length) {
	        foo->syn.generate(begin, buffer+length);
	        foo->leftOver = end - (buffer+length);
	        return;
	    }
	    foo->syn.generate(begin, end);
	    begin = end;
		if (!done) {
			foo->seq.parse(/*syn*/);
			if (foo->seq.ended()) {
				done = true;
			}
		}
	    foo->leftOver = 0;
	}
}

int main(int argc, char *argv[])
{
	const int SAMPLE_RATE = 44100;
    if(SDL_Init(SDL_INIT_AUDIO) != 0) {printf("Failed to initialize SDL: %s\n", SDL_GetError());}

	Foo foo(SAMPLE_RATE);

    SDL_AudioSpec want;
    want.freq = SAMPLE_RATE; // number of samples per second
    want.format = AUDIO_S16SYS; // sample type (here: signed short i.e. 16 bit)
    want.channels = 1; // only one channel
    want.samples = 2048; // buffer-size
    want.callback = audio_callback; // function SDL calls periodically to refill the buffer
    want.userdata = &foo; // counter, keeping track of current sample number

    SDL_AudioSpec have;
    if(SDL_OpenAudio(&want, &have) != 0) {printf("Failed to open audio: %s\n", SDL_GetError());}
    if(want.format != have.format) {printf("Failed to get the desired AudioSpec\n");}
	cout << "Actual FREQ:" << have.freq << endl;

    SDL_PauseAudio(0); // start playing sound
	while (!done) {
    	SDL_Delay(100); // wait while sound is playing
	}
	cout << "DONE..." << endl;
    SDL_Delay(1000); // wait while sound is playing
    SDL_PauseAudio(1); // stop playing sound

    SDL_CloseAudio();
	cout << "DONE!" << endl;

    return 0;
}
