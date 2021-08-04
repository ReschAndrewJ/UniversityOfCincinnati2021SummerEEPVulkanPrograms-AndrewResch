#include "SharedStructs.h"


bool SharedStructs::operator<(const bufferSpace& left, const bufferSpace& Right) {
	return left.position < Right.position;
}