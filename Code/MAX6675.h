#include  "avrlibtypes.h"

#ifndef MAX6675_H
#define MAX6675_H

u16 ReadMAX6675 (void);
float CalculateTemp (u16 value);
u08 CheckForThermocoupleOpen (u08 lower);

#endif
