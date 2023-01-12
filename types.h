#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in function return types */
typedef enum
{
	e_success,
	e_failure

} Status;

/* OperationType will be used in fun return type */
typedef enum
{
	e_help,
	e_view,
	e_edit,
	e_unsupported

} OperationType;

#endif

