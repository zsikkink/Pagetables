# Multi-Level Page Table (MLPT) Library

This library provides an implementation of a multi-level page table (MLPT) for simulating virtual address translation and page allocation.

## Configuration

The MLPT library can be configured by modifying the `config.h` file. The following constants can be adjusted:

- `LEVELS`: The number of levels in the page table hierarchy. Valid values are between 1 and 6, inclusive.
- `POBITS`: The number of bits used for the page offset. Valid values are between 4 and 18, inclusive.

When choosing values for `LEVELS` and `POBITS`, consider the following guidelines:
- Increasing `LEVELS` allows for a larger virtual address space but increases the overhead of page table lookups.
- Increasing `POBITS` results in larger page sizes, which can reduce the number of page table entries but may lead to internal fragmentation.

## API

The MLPT library provides the following functions:

- `size_t translate(size_t va)`: Translates a virtual address to a physical address. Returns `INVALID_ADDRESS` if the virtual address is not mapped.
- `void page_allocate(size_t va)`: Allocates a page table entry and physical page for the given virtual address, if not already allocated.

## Usage

To use the MLPT library in your project:

1. Include the `mlpt.h` header file in your source code.
2. Compile your source code along with `mlpt.c` and link against the `libmlpt.a` library.
3. Configure the `LEVELS` and `POBITS` constants in `config.h` according to your requirements.
4. Call the `translate` function to translate virtual addresses to physical addresses.
5. Call the `page_allocate` function to allocate page table entries and physical pages for virtual addresses.

Example:
```c
#include "mlpt.h"

int main() {
    size_t va = 0x1234000;
    page_allocate(va);
    size_t pa = translate(va);
    if (pa != INVALID_ADDRESS) {
        // Access the physical memory at address pa
    }
    return 0;
}

## Limitations and Future Work

- The current implementation does not support page deallocation or freeing of memory.
- The Library does not handle concurrent access to the page table. Synchronization mechanisms should be added for multi-threaded environments.
- The performance of the library can be improved b implementing optimizations such as translation lookaside buffers (TLBs).



## Proposed De-allocation Interface

A potential de-allocation interface for the MLPT library could include the following functions:

void page_deallocate(size_t va): Deallocates the physical page and page table entries associated with the given virtual address.

void deallocate_all(): Deallocates all allocated physical pages and page table entries, effectively resetting the MLPT to its initial state.

However, implementing a de-allocation interface in the current design of the MLPT library is not practical due to the following reasons:

1. Changes required in page_allocate and translate:
The page_allocate function would need to keep track of the allocated physical pages and page table entries, possibly using additional data structures like a linked list or a hash table. This would add complexity to the memory management logic.
The translate function would need to check if the requested virtual address is still valid and has not been deallocated. This would require additional checks and potentially impact the performance of the address translation process.

2. Modifications to the library interfaces:
The page_allocate function would need to return a handle or a unique identifier for each allocated page, which can be used later for de-allocation. This would change the function signature and require modifications in the code that uses this function.
The translate function may need to return additional information, such as the validity of the translation or the associated handle, to facilitate de-allocation. This would require changes in the code that relies on the current interface.

3. Insufficient information for safe de-allocation:
The MLPT library, as currently designed, does not have information about the ownership or lifetime of the allocated pages. It cannot determine when a page is no longer needed or when it is safe to deallocate it.
Without additional information from the user of the library, such as explicit de-allocation calls or a reference counting mechanism, the library cannot make reliable decisions about when to deallocate pages.

Consider the following example:
size_t va1 = 0x1234000;
size_t va2 = 0x1234000;
page_allocate(va1);
page_allocate(va2);
// ...
page_deallocate(va1);
size_t pa = translate(va2);
// Invalid memory access, as the page was deallocated


In this case, the library would not have enough information to determine that va1 and va2 refer to the same virtual address. Deallocating the page based on va1 would lead to an invalid memory access when translating va2.

To implement a practical and safe de-allocation interface, the MLPT library would require significant changes to its design and additional information from the user of the library to manage the lifetime and ownership of allocated pages correctly.
