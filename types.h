#ifndef TYPES_H
#define TYPES_H
/* User defined types */
typedef unsigned int uint;
/* Status will be used in fn. return type */
typedef enum
{
    e_success,
    e_failure
} Status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#define RED "\033[1;31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET "\033[0m"
#define YELLOW "\033[33m"



#endif
