#include "generator.h"
#include <ucontext.h>

struct gen {
    int complete;
    ucontext_t self;
    ucontext_t parent;
    ucontext_t fallback;
    void *value;
    gen_function_t *cleanup;
    char fallback_stack[GEN_FALLBACK_STACK_SIZE];
    char stack[GEN_DEFAULT_STACK_SIZE];
};


void __gen_complete(Generator gen, void *args) {
    if (gen->cleanup != (void*)0) {
        gen->cleanup(gen, args);
    }
    *gen_complete(gen) = 1;
}

void gen_init(Generator gen, gen_function_t function, void *args, size_t stack_size) {
    gen->complete = 0;
    gen->cleanup = (void*)0;
    gen->value = (void*)0;
    getcontext(&gen->parent);
    getcontext(&gen->self);
    getcontext(&gen->fallback);
    gen->fallback.uc_stack.ss_size = sizeof gen->fallback_stack;
    gen->fallback.uc_stack.ss_sp = gen->fallback_stack;
    gen->fallback.uc_link = &gen->parent;
    gen->self.uc_stack.ss_size = stack_size;
    gen->self.uc_stack.ss_sp = gen->stack;
    gen->self.uc_link = &gen->fallback;
    makecontext(&gen->fallback, (void(*)()) __gen_complete, 2, gen, args);
    makecontext(&gen->self, (void(*)()) function, 2, gen, args);
}


size_t gen_get_size(size_t stack_size) {
    return (sizeof(struct gen) - GEN_DEFAULT_STACK_SIZE) + stack_size;
}

void gen_set_finalizer(Generator gen, gen_function_t finalizer) {
    gen->cleanup = finalizer;
}

int* gen_complete(Generator gen) {
    return &gen->complete;
}

void **gen_value(Generator gen) {
    return &gen->value;
}

int gen_next(Generator gen) {
    if (!gen->complete) {
        swapcontext(&gen->parent, &gen->self);
    }
    return !gen->complete;
}

void gen_yield(Generator gen) {
    swapcontext(&gen->self, &gen->parent);
}
