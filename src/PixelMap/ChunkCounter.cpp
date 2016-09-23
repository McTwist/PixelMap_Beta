

#include "Chunk.hpp"

#include <string.h>

Chunk::Counter::Counter()
{
	memset(this, 0, sizeof(Chunk::Counter));
}
Chunk::Counter & Chunk::Counter::operator=(const Chunk::Counter &c)
{
	if (this == &c)
		return *this;
	memcpy(this, &c, sizeof(Chunk::Counter));
	return *this;
}
Chunk::Counter & Chunk::Counter::operator+=(const Chunk::Counter &c)
{
	for (int i = 0; i < DATAVALUES_AMOUNT; ++i)
		block[i] += c.block[i];
	chicken += c.chicken;
	sheep += c.sheep;
	zombie += c.zombie;
	pig += c.pig;
	skeleton += c.skeleton;
	creeper += c.creeper;
	cow += c.cow;
	slime += c.slime;
	pigzombie += c.pigzombie;
	ghast += c.ghast;

	filesize += c.filesize;
	return *this;
}
const Chunk::Counter & Chunk::Counter::operator+(const Chunk::Counter &c) const
{
	Chunk::Counter result = *this;
	result += c;
	return *this;
}