# Paging: Introduction

How can we virtualize memory with pages, so as to avoid the prob-
lems of segmentation? What are the basic techniques? How do we make
those techniques work well, with minimal space and time overheads?

## Overview

- The OS slices the address space into fixed-size chunks - pages
- When it wants to allocate memory for the process, it just finds the number
  of free pages needed for the address space
- The major role of the page table is to store address translations for each of the
  virtual pages of the address (Page table is per process)
- Address translation: split the virtual address into two components: 1st the cirtual page number,
  and the offset within the page. Using the highest N bits (virtual page number) we find the
  coresponding entry in the page table - physical frame number (PFN). Thus we can translate this
  virtual address by replacing VPN with PFN.

## Where are page tables stored?

- They are very large
- Stored in memory because of size

## What is in the page table?

- Linear page table: array indexed by the VPN and the value is the page table entry (PTE)
- Each PTE has a valid bit (to indicate whether the particular page is valid), protection bits
  (indicating whether the page could be read from, written to, or executed), present bit (if the page
  is in memory or on disk), dirty bit (indicates whether the page has been modified since it was brought into memory),
  reference bit (whether a page has been accessed)

## Paging: Also Too Slow

- First extract VPN from virtual address: `VPN = (virtualAddress & VPN_MASK) >> OFFSET_SIZE_BITS`
- Then find the PTE of the VPN from the page table: `pteAddress = pageTableBaseAddress + (VPN * sizeof(PTE))`
- Then fetch the PTE from memory and extract the PFN
- Calculate offset from virtual address: `offset = virtualAddress & OFFSET_MASK`
- Concatenate offset to PTE: `physicalAddress = (PFN << OFFSET_SIZE_BITS) | offset`
- For every memory reference, paging requires one extra memory reference in order to first fetch the translation
  from the page table.
