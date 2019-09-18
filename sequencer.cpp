#include <iostream>
#include <math.h>
#include "parsers.h"
#include "sequencer.h"
#include "synthesizer.h"

using namespace std;


class Properties {
    float note;          // c2=24, c#2=25, c##2=26, c<2 = 23, d3=38 et.c. (sets note) <NOTE>
						 // c.1#     0.1 halftone sharper c
						 // c.1<     0.1 halftone flatter c
                         // ?+~1,2 osc (adds note to osc) ?*~ (muls note with osc) ?~ (set note to osc??)
                         // ?+:    env (adds note to env) ?*: (muls note with env) ?: (set note to env)
						 // ?*2   mul note with 2
						 // ?2   set note to 2
						 // ~ is short for ?+~ (note add osc)
						 // : is short for ?+: (note add env)
						 // ~amp,freq,waveform,pw 

    float duration;      // from key down to key up <DEF><NOTE>
                         // $ sets duration $+ adds duration, $* muls duration
						 // floatorfrac is a short for $floatorfrac
						 // / is a short for $/ e.g. $/3 set duration to 1/3
						 // * is a short for $* e.g. */3 mul duration with 1/3
                         // = . ' `  shorthand mul duration
    float legato;        // %, %+, %* sets, adds or muls legato <DEF><NOTE>
						 // legato muls duration
						 // without adding to group length or when next note comes
						 // %.8 = legato, %.2 = staccato

    float volume;        // & &+ &* sets, adds or muls volume, <DEF><NOTE>
	                     // &~ set volume to osc
	                     // &: set volume to env
	                     // in <DEF> section, can't use : or ~
    float velocity;      // ^ ^+ ^* sets, adds or muls velocity <DEF><NOTE>

    //<DEF>(<DEF>c#3<NOTE|PAUSE>;)
};




Sequencer::Sequencer(Manager<Value, sizeof(Envelope)>& valueManager, SynthesizerIf& syn) : syn_(syn), valueManager_(valueManager)
{
}

void Sequencer::addTrack(Ittr str)
{
	root_ = subParsers_.mk(str, *this, 30, Mul, 0.8, 0.5, 0.25, 0.5, 0.0);
	root_->init(true);
}

void Sequencer::Stack::init(bool canDef)
{
	Ittr tmp = ittr_;
		
	if (canDef)
		parseDef(tmp);
	
	float pauseDuration;
	if (parsePause(tmp, pauseDuration))
		timeToNext_ += pauseDuration;

	float paranTime;
	if(parseParanthesis(tmp, paranTime))
		canDef = true;

	if (*tmp == ',' || *tmp == ')' || *tmp == 0)
		ended_ = true;

	ittr_ = tmp;
}

bool Sequencer::Stack::parseParanthesis(Ittr& ittr, float& paranTime)
{
	Ittr tmp = ittr;
	if (*tmp == '(') {
		//cout << "parseParanthesis : " << ittr.raw() << endl;
		child_ = seq_.subParsers_.mk(*this);
		child_->ittr_ = tmp;
		++(child_->ittr_);
		child_->init(true);
		++tmp;
		unsigned depth = 0;
		while (*tmp != 0) {
			if (*tmp == ',' && depth == 0) {
				Stack* nw = seq_.subParsers_.mk(*this);
				nw->next_ = child_;
				nw->ittr_ = tmp;
				++(nw->ittr_);
				nw->init(true);
				child_ = nw;
			} else if (*tmp == '(') {
				++depth;
			} else if (*tmp == ')') {
				if (depth == 0) {
					++tmp;
					ittr = tmp;
					return true;
				} else
					--depth;
			}
			++tmp;
		}
	}
	return false;
}

void apply(Sequencer::Modifier modifier, float value1, float value2, float& target)
{
	if (modifier == Sequencer::Mul)
		target = value1 * value2;
	else if (modifier == Sequencer::Add)
		target = value1 + value2;
	else
		target = value1;
}

string prnt(Sequencer::Modifier modifier)
{
	if (modifier == Sequencer::Set)
		return "Set";
	else if (modifier == Sequencer::Add)
		return "Add";
	else if (modifier == Sequencer::Mul)
		return "Mul";
	return "Oth";
}

bool Sequencer::Stack::parseDef(Ittr& ittr)
{
	Ittr tmp = ittr;
	float value;
	Modifier modifier;
	Property property;
	float duration;
	bool valid = false;
	if (parseDuration(tmp, modifier, duration)) {
		cout << "found default duration: " << prnt(modifier) << ", " << duration << endl;
		apply(modifier, duration, duration_, duration_);
		cout << "duration_" << duration_ << endl;
		valid = true;
	}
	while(parseDefaultProperty(tmp, property, modifier, value)) {
		cout << "found default property: " << property << ", " << prnt(modifier) << ", " << value << endl;
		if (property == Volume)
			apply(modifier, value, volume_, volume_);
		else if (property == Velocity)
			apply(modifier, value, velocity_, velocity_);
		else if (property == Legato) {
			legato_ = value;
			legatoModifier_ = modifier;
		}
		else
			cout << "unknown property in default section " << property << endl;
		valid = true;
	}
	if (!valid)
		return false;
	ittr = tmp;
	return true;
}

bool Sequencer::Stack::parsePause(Ittr& ittr, float& pauseDuration)
{
	Ittr tmp = ittr;
	if (*tmp == '_') {
		cout << "found pause: " << endl;
		++tmp;
		Modifier modifier;
		if (parseDuration(tmp, modifier, pauseDuration)) {
			cout << "found pause duration: " << prnt(modifier) << ", " << pauseDuration << endl;
			apply(modifier, duration_, pauseDuration, pauseDuration);
		} else
			pauseDuration = duration_;
		ittr = tmp;
		return true;
	}
	return false;
}

bool Sequencer::parse()
{
	return root_->parse();
}

float Sequencer::Stack::timeToNext() const
{
	float smallest = 10000.0;
	if (child_) {
		for (Stack* s = child_; s; s = s->next_ ) {
			if (!s->ended_ && s->timeToNext() <= smallest) {
				smallest = s->timeToNext();
			}
		}
		return timeToNext_ + smallest;
	}
	return timeToNext_;
}

bool Sequencer::Stack::parseSpace(Ittr& ittr)
{
	Ittr tmp = ittr;
	if (*tmp == ' ') {
		++tmp;
		ittr = tmp;
		return true;
	}
	return false;
}

bool Sequencer::Stack::parse()
{
	timeToNext_ = 0.0;
	if (child_) {
		float ttn = timeToNext();
		bool played = false;
		for (Stack* s = child_; s; s = s->next_ ) {
			if (!played && !s->ended_ && s->timeToNext() <= ttn) {
				played = true;
				//cout << "sub parse!! " << s->ittr_.raw() << endl;
				if (!s->parse())
					return false;
			} else {
				//cout << "time pass " << ttn << ", " << s->ittr_.raw() << endl;
				s->timeToNext_ -= ttn;
			}
		}
		checkChildren();
		return true;
	}

    float note = 0;
	if (!parseNote(ittr_, oldnote_, note))
		return false;
	cout << "found note: " << note << endl;
	oldnote_ = note;
	Modifier modifier;
	float duration;
	if (parseDuration(ittr_, modifier, duration)) {
		cout << "found note duration: " << prnt(modifier) << ", " << duration << endl;
		apply(modifier, duration, duration_, duration);
	} else
		duration = duration_;
	float volume = volume_;
	float velocity = velocity_;
	unsigned valueId;
	float value;
	unsigned volumeValueId = -1;
	unsigned noteValueId = -1;
	Property property;
	while(parseNoteProperties(seq_.valueManager_, ittr_, property, modifier, valueId, value)) {
		if (valueId == -1)
			cout << "found note property value: " << property << ", " << prnt(modifier) << ", " << value << endl;
		else
			cout << "found note property valueId: " << property << ", " << prnt(modifier) << ", " << valueId << endl;
		if (property == Legato) {
			legatoModifier_ = modifier;
			legato_ = value;
		}
		else if (property == Velocity)
			apply(modifier, value, velocity_, velocity);
		else if (property == Volume)
			if (valueId == -1)
				apply(modifier, value, volume_, volume);
			else
				volumeValueId = valueId;
		else if (property == Note)
			if (valueId == -1)
				apply(modifier, value, note, note);
			else
				noteValueId = valueId;
		else
			cout << "unknown property in note section " << property << endl;
	}
	float noteLenght = duration;
	apply(legatoModifier_, legato_, noteLenght, noteLenght);
	seq_.syn_.playNote(noteLenght, velocity, note, volume, noteValueId, volumeValueId);
//---------------
	timeToNext_ = duration;
	init(parseSpace(ittr_));
	return true;
}

void Sequencer::Stack::checkChildren()
{
	bool allEnded = true;
	float largestTtn = 0.0;
	for (Stack* s = child_; s; s = s->next_ ) {
		allEnded &= s->ended_;
		if (s->timeToNext_ > largestTtn)
			largestTtn = s->timeToNext_;
	}

	if (allEnded) {
		timeToNext_ += largestTtn;
		child_ = nullptr; // <- leak!!
		init(true);
	}
}

#if 0
general program flow:
bool init(const char*& str, float& next)
	const char* tmp;
	bool keepGoing = true;
	next = 0;
	while (keepGoing) {
		keepGoing = false;
		if(parseParanthesis(tmp, ptime&)
			return next+ptime;
			
		if (parseDef(tmp))
			keepGoing = true;
		
		if (parsePause(tmp, pause)) {
			keepGoing = true;
			next += pause;
		}

		if (parseSpace())
			keepGoing = true;

		if (*tmp == ',' || *tmp == ')' || *tmp == 0)
			return false;
	}
	return true;

parseOne()
	if (!parseNote(tmp))
		return false;
	parseDuration
	parseNoteProperties
	playNote
	init(next&)
	return duration + next

parseParanthesis(float& next)
	if (*tmp == '(')) {
		find ','s and ')', and generate substacks
		init substacks
		find "next" substack
		parseOne on that substack, which will play a note
		if find "next" substack
			next = how soon that is
			return true;
		else
			close all substacks
			return false;
	}
	return false;
#endif

Sequencer::Stack::Stack(Ittr str, Sequencer& seq, int oldnote, Modifier legatoModifier, float legato, float volume, float duration, float velocity, float timeToNext) :
	ittr_(str), seq_(seq), oldnote_(oldnote), legatoModifier_(legatoModifier), legato_(legato), volume_(volume), duration_(duration), velocity_(velocity), timeToNext_(0.0), next_(nullptr), child_(nullptr)
{
	ended_ = false;
}

Sequencer::Stack::Stack(const Stack& s) :
	ittr_(s.ittr_), seq_(s.seq_), oldnote_(s.oldnote_), legatoModifier_(s.legatoModifier_), legato_(s.legato_), volume_(s.volume_), duration_(s.duration_), velocity_(s.velocity_), timeToNext_(0.0), next_(nullptr), child_(nullptr)
{
	ended_ = false;
}
#if 0
$1a(b,c)(e,f)
-
===init
$1a_(b,c)(e,f)
  -
===parse
pl(a),init(b...),init(c...)
ttn=a+_+min(ttnb,ttnc) = a+_
$1a_(bd,c)(e,f)
     =  =-
===parse
play(minofchildren = c) => false
ttn=minofchildren = b
$1a_(bd,c)(e,f)
     =   =
===parse
play(minofchildren = b)
ttn=minofchildren = b
$1a_(bd,c)(e,f)
      =  =
===parse
play(d),init(e...),init(f...)
ttn=d
$1a_(bd,c)(e,f)
           = =-
===parse
play(f)
ttn=minofchildren = f
$1a_(bd,c)(e,f)
           =  =

ctor()
    ended = false;
    ttn = 0.0;
    init(true);

init(bool canDef)
    if (canDef)
        parseDefaultProperties
    parsePause and add to ttn
    if (parseParan())
        create children
        place tmp just after ')'
        checkChildren();
    if *tmp == ',', ')' or 0
        ended = true;

parse()
    if children:
        find child that is not ended with min ttn
        parse that child
        pass time for all other children
        checkChildren();
    else
        parseNote and play and set ttn
        init(parseSpace());

checkChildren()
    if all children are ended
        add largest child ttn to ttn
        free all children
        init(true);
				cout << "(sub)track ended" << endl;
				if (child_ == s)
					child_ = s->next_;
				else
					for (Stack* s2 = child_; s2; s2 = s2->next_ ) {
						if (s2->next_ == s) {
							s2->next_ = s->next_;
							break;
						}
				}
				if (child_ == nullptr)
					timeToNext_ = s->timeToNext_;
				seq_.subParsers_.release(s);
#endif
