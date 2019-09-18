#ifndef _PARSERS_H_
#define _PARSERS_H_

#include "manager.h"
#include "value.h"

class Value;

class ParserEnums {
public:
	enum Modifier {Set, Add, Mul};
	enum Property {Legato, Velocity, Volume, Note};
};

template<typename T>
class Parsers : public ParserEnums {
public:
	static bool parseInt(T& str, int& ret);
	static bool parseFloat(T& str, float& value);
	static bool parseFloatOrFraction(T& str, float& value);
	static bool parseModifier(T& str, Modifier& modifier);
	static bool parseDuration(T& str, Modifier& modifier, float& value);
	static bool parseNote(T& str, int oldNote, float& note);
	static bool parseDefaultProperty(T& str, Property& property, Modifier& modifier, float& value);
	static bool parseValue(Manager<Value, sizeof(Envelope)>& manager, T& str, unsigned& valueId);
	static bool parseNoteProperties(Manager<Value, sizeof(Envelope)>&, T& str, Property& property, Modifier& modifier, unsigned& valueId, float& value);
};

template<typename T>
bool Parsers<T>::parseInt(T& str, int& ret)
{
	T tmp = str;
	int isign = 1;
	int i = 0;
	bool valid = false;
	if (*tmp == 0)
			return false;
	if (*tmp == '-') { isign = -1; ++tmp;}
	while(*tmp >= '0' && *tmp <= '9') {
			valid = true;
			i = i*10 + *tmp - '0';
			++tmp;
	}
	if (valid) {
			str = tmp;
			ret = i*isign;
			return true;
	}
	return false;
}

template<typename T>
bool Parsers<T>::parseFloat(T& str, float& value)
{
	T tmp = str;
	float f0 = 0;
	bool valid = false;
	while (*tmp >= '0' && *tmp <= '9') {
		valid = true;
		f0 = f0*10 + *tmp - '0';
		++tmp;
	}
	if (*tmp == '.') {
		++tmp;
		float frac = 0.1;
		while (*tmp >= '0' && *tmp <= '9') {
			valid = true;
			f0 += frac*(*tmp-'0');
			frac *= 0.1;
			++tmp;
		}
	}
	if (!valid)
		return false;
	str = tmp;
	value = f0;
	return true;
}

template<typename T>
bool Parsers<T>::parseFloatOrFraction(T& str, float& value)
{
	T tmp = str;
	float f0;
	bool valid = false;
	if (!parseFloat(tmp, f0))
		f0 = 1.0;
	else
		valid = true;
	if (*tmp == '/') {
		++tmp;
		float f1;
		if (!parseFloat(tmp, f1))
			return false;
		str = tmp;
		value = f0/f1;
		return true;
	}
	if (!valid)
		return false;
	str = tmp;
	value = f0;
	return true;
}

template<typename T>
bool Parsers<T>::parseModifier(T& str, Modifier& modifier)
{
	T tmp = str;
	if (*tmp == '*') {
		++tmp;
		str = tmp;
		modifier = Mul;
		return true;
	} else if (*tmp == '+') {
		++tmp;
		str = tmp;
		modifier = Add;
		return true;
	}
	return false;
}

template<typename T>
bool Parsers<T>::parseDuration(T& str, Modifier& modifier, float& value)
{
	T tmp = str;
	if (*tmp == '*') {
		++tmp;
		if (!parseFloatOrFraction(tmp, value))
			return false;
		str = tmp;
		modifier = Mul;
		return true;
	} else if (*tmp == '$') {
		++tmp;
		if (!parseModifier(tmp, modifier))
			modifier = Set;
		if (!parseFloatOrFraction(tmp, value))
			return false;
		str = tmp;
		return true;
	} else if (*tmp == '/') {
		if (!parseFloatOrFraction(tmp, value))
			return false;
		str = tmp;
		modifier = Set;
		return true;
	}
	if (parseFloatOrFraction(tmp, value)) {
		str = tmp;
		modifier = Set;
		return true;
	}
	bool valid = false;
	float f = 1.0;
	while (*tmp == '=') { valid = true; ++tmp; f += 1.0; }
	float fr = 0.5;
	while (*tmp == '.') { valid = true; ++tmp; f += fr; fr *= 0.5; }
	fr = 0.5;
	while (*tmp == 0x27) { valid = true; ++tmp; f -= fr; fr *= 0.5; }
	while (*tmp == '`') { valid = true; ++tmp; f *= 2.0/3.0; }
	if (!valid)
		return false;
	modifier = Mul;
	value = f;
	str = tmp;
	return true;
}

template<typename T>
bool Parsers<T>::parseNote(T& str, int oldNote, float& note)
{
    T tmp = str;
    if (*tmp == 'c') note = 0.0;
    else if (*tmp == 'd') note = 2.0;
    else if (*tmp == 'e') note = 4.0;
    else if (*tmp == 'f') note = 5.0;
    else if (*tmp == 'g') note = 7.0;
    else if (*tmp == 'a') note = 9.0;
    else if (*tmp == 'b') note = 11.0;
    else return false;
    ++tmp;
	float finetune = 0.0;
    if (*tmp == '.') {
		if (parseFloat(tmp, finetune)) {
    		if (*tmp == '<')
				finetune = -finetune;
			else if (*tmp != '#')
				return false;
			++tmp;
		}
	} 
	while (*tmp == '#') {
		++note;
		++tmp;
	} 
	while (*tmp == '<') {
		--note;
		++tmp;
	}
    int oct = 0;
    if (!parseInt(tmp, oct)) {
        oct = (oldNote-note+5+1200)/12-100;
    }
    note = note + oct*12 + finetune;
    str = tmp;
    return true;
}

template<typename T>
bool Parsers<T>::parseDefaultProperty(T& str, Property& property, Modifier& modifier, float& value)
{
	T tmp = str;
	if (*tmp == '%') {
		++tmp;
		property = Parsers::Legato;
	} else if (*tmp == '^') {
		++tmp;
		property = Parsers::Velocity;
	} else if (*tmp == '&') {
		++tmp;
		property = Parsers::Volume;
	} else
		return false;
	if (!parseModifier(tmp, modifier))
		modifier = Parsers::Set;
	if (!parseFloatOrFraction(tmp, value))
		return false;
	str = tmp;
	return true;
}

template<typename T>
bool Parsers<T>::parseValue(Manager<Value, sizeof(Envelope)>& manager, T& str, unsigned& valueId)
{
	T tmp = str;
	if (*tmp == '~') {
		valueId = manager.mk<Oscillator>();
		++tmp;
		str = tmp;
		return true;
	} else if (*tmp == ':') {
		valueId = manager.mk<Envelope>();
		++tmp;
		str = tmp;
		return true;
	}
	return false;
//	float value;
//	if (!parseFloatOrFraction(tmp, value))
//		return false;
//	valueId = manager.mk<Const>(value);
//	str = tmp;
//	return true;
}

template<typename T>
bool Parsers<T>::parseNoteProperties(Manager<Value, sizeof(Envelope)>& manager, T& str, Property& property, Modifier& modifier, unsigned& valueId, float& value)
{
	T tmp = str;
	if (*tmp == '%') {
		++tmp;
		property = Parsers::Legato;
	} else if (*tmp == '^') {
		++tmp;
		property = Parsers::Velocity;
	} else if (*tmp == '&') {
		++tmp;
		property = Parsers::Volume;
	} else if (*tmp == '?') {
		++tmp;
		property = Parsers::Note;
	} else if (parseValue(manager, tmp, valueId)) {
		property = Parsers::Note;
		modifier = Parsers::Add;
		str = tmp;
		return true;
	} else
		return false;
	if (!parseModifier(tmp, modifier))
		modifier = Parsers::Set;
	if (parseFloatOrFraction(tmp, value)) {
		str = tmp;
		valueId = -1;
		return true;
	}
	if (property == Parsers::Legato || property == Parsers::Velocity)
		return false;
	if (!parseValue(manager, tmp, valueId))
		return false;
	str = tmp;
	return true;
}

#endif
