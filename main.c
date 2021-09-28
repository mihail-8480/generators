#include "generator.h"
#include <stdio.h>

// A fibonacci sequence number generator.
generator(Fib, size_t, n) {
    // Fibonacci sequence initial values.
    size_t current = 0;
    size_t next = 1;

    while (1) {
        // By using the `yield_return` macro, you are returning a value and "pausing" this function.
        yield_return (current);

        // Generate fibonacci sequence numbers until `n` hits 0.
        if (--n == 0) {
            yield_break;
        }

        size_t temp = current;
        current = next;
        next = temp + next;
    }
}


// A function that displays the purpose of `gen_set_finalizer`. It'll be executed after the generator is complete.
void finish(Generator gen, void *args) {
    printf("Generator finished, args = %zu, value = %zu\n", (size_t)args, (size_t)*gen_value(gen));
}

// A simple generator function that counts up to n.
// Created without using any macros.
void
counter_gen (
        // The current generator instance, if you want the utility macros to work, it should be named __gen.
        Generator this,
        // The second argument is the data-pass argument, it's treated as a void pointer internally.
        // Make sure you use a compatible type, otherwise you'll get undefined behavior.
        size_t n
) {
    printf("counter_gen() got called!\n");
    // Count from 1 to n.
    for(size_t i = 1; i <= n; i++) {
        printf("We are inside the for loop!\n");
        // Set the value.
        *gen_value(this) = (void*)i;
        // Return to caller.
        gen_yield(this);
    }
    printf("We are outside of the for loop!\n");
}


int main() {
    // Create a new generator based on Fib with the argument `10`.
    Generator fib = Fib(10);
    // Set its finalizer to `finish()`.
    gen_set_finalizer(fib, finish);

    // Generate until the generator is fully complete.
    while(gen_next(fib)) {
        // Print all the values.
        printf("%zu ",(size_t)*gen_value(fib));
    }


    // Create a generator object manually.
    Generator cnt = alloca(gen_get_size(GEN_DEFAULT_STACK_SIZE)); // You can also use malloc(), mmap() or any allocator.
    // Initialize the generator with "3" as the args and the default stack size.
    gen_init(cnt, counter_gen, (void*)3, GEN_DEFAULT_STACK_SIZE);
    gen_set_finalizer(cnt, finish);

    // Loop until the generator is complete.
    while(gen_next(cnt)) {
        printf("We are inside of main.\n");
        printf("counter_gen() sent us: %zu\n", (size_t)*gen_value(cnt));
    }

    printf("We are outside of the while loop.\n");
}