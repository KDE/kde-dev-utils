#include "ktrace.h"

class KTraceActivate 
{
public:
   KTraceActivate() { ktrace(); }
   ~KTraceActivate() { kuntrace(); }
} kTraceActivateInstance;


