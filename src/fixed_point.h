#pragma once

#define FX_INT(v, p)  ((v) >> (p))
#define FX_FRAC(v, p) ((v) & ((1 << (p)) - 1))

#define FX_GTE_P 12

#pragma balls
