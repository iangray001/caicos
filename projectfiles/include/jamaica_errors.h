#ifndef __JAMAICA_ERRORS__
#define __JAMAICA_ERRORS__

#include "jamaica_global.h"
#include "jamaica_errors_constants.h"
#include "jamaica_vm.h"
#include "jamaica_threads.h"

const char *jamaicaErrors_getText(jamaica_error errorCode);
jamaica_ref jamaicaErrors_getException(jamaica_thread *ct, jamaica_error errorCode);

#endif /* __JAMAICA_ERRORS__ */
