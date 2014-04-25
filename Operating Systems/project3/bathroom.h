#ifndef _BATHROOM_
#define _BATHROOM_

#define gender_str(g) (g == male)?"Male":"Female"

typedef enum {male, female} gender;

void Enter(gender g);

void Leave(void);

void Initialize(void);

void Finalize(void);

#endif
