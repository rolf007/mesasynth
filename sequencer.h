#ifndef _SEQUENCER_H_
#define _SEQUENCER_H_

#include "refcnt.h"
#include "parsers.h"
#include "value.h"
#include "chain_pool.h"
#include "macro.h"

class SynthesizerIf;

class Sequencer : public Parsers<Ittr> {
public:
    Sequencer(SynthesizerIf& syn, float bpm4, float sampleRate);
	void addTrack(Ittr str);
    bool parse();
	bool ended() { return root_->ended_; }
	unsigned timeToNext() const { return noteLengthToSamples(root_->timeToNext()); }
private:
	unsigned noteLengthToSamples(float noteLength) const {
		return 4*60*sampleRate_*noteLength/bpm4_;
	}
	struct Stack : public refcnt {
		static const unsigned MaxSize;
		Stack(Ittr str, Sequencer&, int oldnote, Modifier legatoModifier, float legato, float volume, float duration, float velocity, float timeToNext);
		Stack(const Stack& s);
		void init(bool canDef);
		bool parse();
		bool parseParanthesis(Ittr&, float& paranTime);
		bool parseDef(Ittr&);
		bool parsePause(Ittr&, float& pauseDuration);
		bool parseSpace(Ittr&);
		void checkChildren();
		float timeToNext() const;
		Ittr ittr_;
		Sequencer& seq_;
    	int oldnote_;
		Modifier legatoModifier_;
    	float legato_;
    	float volume_;
    	float duration_;
		float velocity_;
		float timeToNext_;
		bool ended_;
		ptr<Stack> next_;
		ptr<Stack> child_;
	};
	ChainPool<Stack>::Scope subParsers_;
    SynthesizerIf& syn_;
	float bpm4_;
	float sampleRate_;
	ptr<Stack> root_;
};

#endif
