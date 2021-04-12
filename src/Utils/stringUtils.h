#ifndef TD_STRING_UTILS
#define TD_STRING_UTILS

typedef struct td_string *td_string;

td_string createString();

char *toCString(td_string);

void appendString(td_string, char *);

void appendChar(td_string, char);

int getStringLength(td_string);

void destroyString(td_string);

char *stringifyInt(unsigned int);

#endif