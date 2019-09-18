#ifndef _SEQUENCER_H_
#define _SEQUENCER_H_

#include "parsers.h"
#include "value.h"
#include "manager.h"
#include "chain_pool.h"
#include "macro.h"

class SynthesizerIf;

class Sequencer : public Parsers<Ittr> {
public:
    Sequencer(Manager<Value, sizeof(Envelope)>& manager, SynthesizerIf& syn);
	void addTrack(Ittr str);
    bool parse();
	bool ended() { return root_->ended_; }
	float timeToNext() const { return root_->timeToNext(); }
private:
	struct Stack {
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
		Stack* next_;
		Stack* child_;
	};
	ChainPool<Stack, 40> subParsers_;
    SynthesizerIf& syn_;
	Manager<Value, sizeof(Envelope)>& valueManager_;
	Stack* root_;
};

#endif
