/**
 * @file error.h
 * @author Jakub Havlík (xhavlij00@stud.fit.vutbr.cz)
 * @brief Error handling file
 * @date 2024-09-22
 * @todo specify more errors
 *
 */

#ifndef __ERROR_H__
#define __ERROR_H__

/**
 * @brief enumeration of errors which can happen during compilation
 *
 */
enum error {
	NO_ERRORS,                    // no errors - 0
	ERR_LEX,                      // lexem wrong structure - 1
	ERR_PARSE,                    // program has wrong syntax, missing header - 2
	ERR_SEM_NOT_DEF_FNC_VAR,      // fnc or var not defined - 3
	ERR_SEM_PARAMS,               // wrong number of parameters when calling a fnc - 4
	ERR_SEM_RET_TYPE_DISCARD = 4, // wrong type or illegal discarding of return value - 4
	ERR_SEM_REDEF,                // redefinition of var or fnc - 5
	ERR_SEM_ASSIGN_NON_MODIF = 5, // assignment to non-modifiable var - 5
	ERR_SEM_RET_EXP,              // missing/extra expression in ret from fnc - 6
	ERR_SEM_TYPE_CONTROL,         // type compatibility in arithmetics, assignments ... - 7
	ERR_SEM_TYPE_INFERENCE,       // var type not specified and cant be inferred - 8
	ERR_SEM_UNUSED_VAR,           // unused var in its scope - 9
	ERR_SEM_MODIF_VAR = 9,        // cant modify modifiable var - 9
	ERR_SEM_OTHER,                // other semantic errors - 10
	ERR_INTERN = 99               // internal error (memory etc.) - 99
};

/**
 * @brief macro used to return error code and print out which one it was
 *
 */
#define ERROR_RET(err)                                                                             \
	{                                                                                              \
		fprintf(stderr, "Returned error code: %d\n", err);                                         \
		return err;                                                                                \
	}

#endif /* __ERROR_H__ */