#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdlib.h>

// A generator function.
typedef struct gen * Generator;

// A generator function pointer.
typedef void (gen_function_t)();

// Calculate the generator object size with a specific stack size (so you can allocate it).
size_t gen_get_size(size_t stack_size);

// Initialize the generator object.
void gen_init(Generator gen, gen_function_t function, void *args, size_t stack_size);

// Get a pointer to the current value of the generator.
void **gen_value(Generator gen);

// Set a finalizer to the current generator.
void gen_set_finalizer(Generator gen, gen_function_t finalizer);

// Move forward on a generator, and check if it's complete.
int gen_next(Generator gen);

// Stop executing the current function.
void gen_yield(Generator gen);

// Get a pointer to the complete flag.
int* gen_complete(Generator gen);

// The default generator stack size.
#define GEN_DEFAULT_STACK_SIZE 16384
// The default fallback/finalizer stack size.
#define GEN_FALLBACK_STACK_SIZE 1024

// Utility macros.
#ifndef NO_GEN_UTILITY_MACROS

// Create a generator object using alloca on the stack.
#define gen_create_on_stack(__function__, __arg__, __stack_size__) ({ \
Generator __gen = alloca(gen_get_size(__stack_size__));               \
gen_init(__gen, __function__, (void*)(__arg__), __stack_size__);      \
__gen;                                                                \
})

// Declare a generator (if you declare a generator with name `test` two functions will be created:
// test() that will return a Generator object, and __test_gen() that will be the actual generator function).
#define generator(__name__, __type__,__arg__) static void __##__name__##_gen(Generator __gen, __type__ __arg__); \
__attribute__((always_inline)) static inline Generator __name__(__type__ __arg__) {                              \
return gen_create_on_stack(__##__name__##_gen, (void*)__arg__, GEN_DEFAULT_STACK_SIZE);                          \
}                                                                                                                \
static void __##__name__##_gen(Generator __gen, __type__ __arg__)

// Complete executing the generator.
#define yield_break ({*gen_complete(__gen) = 1; return;})

// Set the generator value pointer to something, and yield.
#define yield_return(__value__) ({ *gen_value(__gen) = (void*)(__value__); gen_yield(__gen); })
#endif


#endif //GENERATOR_H
