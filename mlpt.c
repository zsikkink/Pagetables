/*
 * Copyright 2024 Zack Sikkink
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mlpt.h"
#include "config.h"

#define VPN_BITS (POBITS - 3)
#define PAGE_SIZE (1 << POBITS)
#define PTE_VALID_BIT 1UL
#define INVALID_ADDRESS (~0UL)

typedef size_t pte_t;
typedef size_t vpn_t;
typedef size_t ppn_t;

size_t ptbr;

vpn_t get_vpn(size_t virtual_address, int level) {
    size_t vpn_mask = (1 << VPN_BITS) - 1;
    size_t shift = POBITS + (LEVELS - 1 - level) * VPN_BITS;
    return (virtual_address >> shift) & vpn_mask;
}

int is_valid_pte(pte_t pte) {
    return pte & PTE_VALID_BIT;
}

ppn_t get_ppn(pte_t pte) {
    return pte >> POBITS;
}

int is_last_level(int level) {
    return level == LEVELS - 1;
}

size_t translate(size_t va) {
    if (ptbr == 0) {
        return INVALID_ADDRESS;
    }

    size_t page_table_addr = ptbr;
    size_t offset = va & (PAGE_SIZE - 1);  // Extract the offset from the virtual address

    int level = 0;
    while (level < LEVELS) {
        vpn_t vpn = get_vpn(va, level);
        pte_t *pte_ptr = (pte_t *)(page_table_addr + (vpn * sizeof(pte_t)));
        pte_t pte = *pte_ptr;

        if (is_valid_pte(pte)) {
            ppn_t ppn = get_ppn(pte);

            if (is_last_level(level)) {
                return (ppn << POBITS) | offset;  // Combine the physical page number and offset to form the physical address
            } else {
                page_table_addr = ppn << POBITS;  // Move to the next level of the page table
            }
        } else {
            return INVALID_ADDRESS;
        }

        level += 1;
    }

    return INVALID_ADDRESS;
}

void page_allocate(size_t va) {
    if (ptbr == 0) {
        void *page;
        posix_memalign(&page, PAGE_SIZE, PAGE_SIZE);  // Allocate a new page aligned to the page size
        ptbr = (size_t)page;
        memset((void *)ptbr, 0, PAGE_SIZE);  // Initialize the page table to zero
    }

    size_t page_table_addr = ptbr;

    int level = 0;
    while (level < LEVELS) {
        vpn_t vpn = get_vpn(va, level);
        pte_t *pte_ptr = (pte_t *)(page_table_addr + (vpn * sizeof(pte_t)));
        pte_t pte = *pte_ptr;

        if (!is_valid_pte(pte)) {
            void *new_page;
            posix_memalign(&new_page, PAGE_SIZE, PAGE_SIZE);  // Allocate a new page aligned to the page size
            memset(new_page, 0, PAGE_SIZE);  // Initialize the new page to zero

            *pte_ptr = (size_t)new_page | PTE_VALID_BIT;  // Set the PTE to point to the new page and mark it as valid

            if (!is_last_level(level)) {
                page_table_addr = (size_t)new_page;  // Move to the next level of the page table
            }
        } else {
            if (!is_last_level(level)) {
                page_table_addr = get_ppn(pte) << POBITS;  // Move to the next level of the page table
            }
        }

        level += 1;
    }
}
