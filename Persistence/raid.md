# Redundant Arrays of Inexpensive Disks (RAIDs)

How can we make a large, fast, and reliable storage system? What are the key techniques? What are trade-offs between different approaches?

- RAID is faster, has more capacity and has redundanchy
- Outside, it looks like just any other disk (it's transparent to the OS)

## Interface and Internals

-When a file system issues a logical I/O to the RAID, the RAID needs to calculate which disk it must access and then issue one or more physical I/Os.
- Specialized computer system, with a processor, memory and disk, but instead of appliations, it runs RAID software.

### Fault model

- Fail-stop - disk can be wither working or failed. If it is working, all blocks can be read or written. If it failed, it is permanently lost. Failure is easily detectable.

### How to Evaluate RAID

- Capacitty - given a set of N disks each with B blocks, how much useful capacity is available to clients of the RAID
- Reliability - How many disk faults can the given design tolerate?
- Performance - single request latency, steady-state throughput - total bandwith of many concurrent requests. Sequential and random workloads.

## RAID-0 (stripping)

- No redundancy
- Spread the blocks of the array across the disks in a round-robin fashion. Most parallelism when requests are made for contiguous chunks of the array.
- Stripe - blocks in the same row
- Small chunk size - increases parallelism of reads and writes, but the positioning time to access blocks across multiple disks increases, because the positioning time for the entire request is determined by the maximum of the positioning times off the request across all drives.
- Big chunk - reduces intra-file parallelism, and thus relises on multiple concurrent requests to achieve high throughput. However, large chunk sizes reduce positioning time; if, for example, a single file fits withing a chunk and thus is placed on a single disk, the positioning time incurred while acccessing it will just be the positioning time of asingle disk.
- Capacity: `N * B`
- Reliability: any disk failure will lead to data loss
- Performance: excellent, all disks are utilized, often in parallel, to service user I/O requests.
- Single request latency - close to the latency of a single disk.
- Steady-state sequential throughput: `N * S` (`S` is steady-state sequential throughput of a single disk).
- Random I/O: `N * R`

## RAID-1 (Mirroring)

- For each RAID block, the system keeps 2 (or more) physical copies of it.
- Reading: from whichever disk contains the block
- Writing: must write to all disks which contain the block, but it can be done in parallel
- Capacity: `N * B / 2`
- Reliability: any one disk can fail, and in some arrangements, even `N / 2` disks can fail without data loss
- Single request latency: read close to the latency of a single disk, write slitghtly higher than a write to a single disk (because we have to write to all disks, we take the worst case rotation + seek)
- Sequential throughput: `N / 2 * S`, because it's like having `N / 2` disks for writing. If we read say block 0 on disk 1, then, the next block is mirrored so we read it in parallel on another disk, and then we read again the block after. In that sense we only read half the sequential blocks, while still needed to use the same sequtnail bandwith, therefore it's `S / 2`
- Random read: `N * R` (because we can distribute the reads to all the disks)
- Random write: `N / 2 * R`

## RAID-4

- Parity block: contains data computed from a stripe such that any one block in the stripe can fail. Usually bitwise XOR of blocks in stripe
- The failed block value is the bitwise XOR of other blocks in a stripe and the parity block.
- Capacity: `(N - 1) * B`
- Reliability: tolerates one disk failure and no more
- Sequntial Read: `(N - 1) * S`
- Sequntial Write: `(N - 1) * S`, full stripe write, you can calculate the parity block, then write it to 1 disk, the other `N - 1` disks can be written to in parallel/
- Random Read: `(N - 1) * R`
- Random Write: additive parity - read all data from other blocks to compute the new parity block. Then write the block and parity block in parallel (scales poorly because of `N - 2` reads). Subtractive parity - First, we read in the old data at `C2 (C2old = 1)` and the old parity `(Pold = 0)`. Then, we compare the old data and the new data; if they are the same (e.g., `C2new = C2old`), then we know the parity bit will also remain the same (i.e., `Pnew = Pold`). If, however, they are different, then we must flip the old parity bit to the opposite of its current state, that is, if `(Pold == 1)`, `Pnew` will be set to `0`; if (`Pold == 0`), `Pnew` will be set to `1`. Or, in other words: `Pnew = (Cold XOR Cnew) XOR Pold`
- Parity disk is a bottle neck under random writes - small write problem
- Random write: `R / 2` (because the parity disk has to do a read and a write for each request).
- Single request latency read: same as for one disk
- Single request latency write: twice of a single disk, because we need 2 reads and 2 writes (which can happen in parallel)

## RAID-5 (Rotating Parity)

- Rotates the aprity block across drives to partially adress the small-write problem
- Capacity: same as RAID-4
- Reliability: same as RAID-4
- Sequential Read: same as RAID-4
- Sequential Write: same as RAID-4
- Random Read: `N * R` - a bit better than RAID-4 because we can utilize all disks
- Random Write: `N / 4 * R` - given a large number of random requests, we can assume that all disks will be equally busy, thus, because we have 4 total operations per write, it's `N * R / 4`.
- Read latency: same as RAID-4
- Write latency: same as RAID-4