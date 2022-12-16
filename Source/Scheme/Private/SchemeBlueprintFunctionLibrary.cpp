/* 
 * Copyright (c) 2022 Anders Dahnielson <anders@dahnielson.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in the 
 * Software without restriction, including without limitation the rights to use, copy, 
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, subject to the 
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "SchemeBlueprintFunctionLibrary.h"

#define LISP_NO_SYSTEM_LIB 1
#include "lisp.h"

TMap<FString, float>* map;

extern "C" {

	Lisp sch_get_env(Lisp args, LispError* e, LispContext ctx) {
		Lisp key = lisp_car(args);
		float value = *map->Find(ANSI_TO_TCHAR(lisp_string(key)));
		return lisp_make_real(value);
	}

	Lisp sch_set_env(Lisp args, LispError* e, LispContext ctx) {
		Lisp key = lisp_car(args);
		Lisp value = lisp_car(lisp_cdr(args));
		map->Add(ANSI_TO_TCHAR(lisp_string(key)), lisp_real(value));
		return lisp_make_real(100);
	}

}

float USchemeBlueprintFunctionLibrary::EvaluateLispExpression(TMap<FString, float> environment, FString input, TMap<FString, float>& mutated_environment) {
	map = &environment;	

	LispError error;
	LispContext ctx = lisp_init_lib();

	Lisp env = lisp_env_global(ctx);

	lisp_env_define(env, lisp_make_symbol("GET-ENV", ctx), lisp_make_func(sch_get_env), ctx);
	lisp_env_define(env, lisp_make_symbol("SET-ENV!", ctx), lisp_make_func(sch_set_env), ctx);
		
	Lisp program = lisp_read(TCHAR_TO_ANSI(*input), &error, ctx);
	if (error != LISP_ERROR_NONE) {
		UE_LOG(LogTemp, Warning, TEXT("Lisp: %s"), ANSI_TO_TCHAR(lisp_error_string(error)));
	}

	Lisp result = lisp_eval_opt(program, env, &error, ctx);
	if (error != LISP_ERROR_NONE) {
		UE_LOG(LogTemp, Warning, TEXT("Lisp: %s"), ANSI_TO_TCHAR(lisp_error_string(error)));
	}

	float return_value = 0;

	switch (lisp_type(result)) {
	case LISP_REAL:
		return_value = lisp_real(result);
		break;
	case LISP_INT:
		return_value = lisp_int(result);
		break;
	case LISP_NULL:
		return_value = 0;
		break;
	case LISP_CHAR:
		UE_LOG(LogTemp, Warning, TEXT("Unhandled value of type char from Lisp"));
		break;
	case LISP_PAIR:
		UE_LOG(LogTemp, Warning, TEXT("Unhandled value of type pair from Lisp"));
		break;
	case LISP_SYMBOL:
		UE_LOG(LogTemp, Warning, TEXT("Unhandled value of type symbol from Lisp"));
		break;
	case LISP_STRING:
		UE_LOG(LogTemp, Warning, TEXT("Unhandled value of type string from Lisp"));
		break;
	case LISP_LAMBDA:
		UE_LOG(LogTemp, Warning, TEXT("Unhandled value of type lamda from Lisp"));
		break;
	case LISP_FUNC:
		UE_LOG(LogTemp, Warning, TEXT("Unhandled value of type func from Lisp"));
		break;
	case LISP_TABLE:
		UE_LOG(LogTemp, Warning, TEXT("Unhandled value of type table from Lisp"));
		break;
	case LISP_VECTOR:
		UE_LOG(LogTemp, Warning, TEXT("Unhandled value of type vector from Lisp"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Unhandled value of type %d from Lisp"), (int)lisp_type(result));
		break;
	}

	lisp_shutdown(ctx);
	mutated_environment = *map;

	return return_value;
}
