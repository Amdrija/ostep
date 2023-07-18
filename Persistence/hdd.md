# Hard Disk Drives

How do modern hard-disk drives store data? What is the interface?How is the data actually laid out and accessed? How does disk scheduling improve performance?

## The Interface

- The drive consists of a large number of sectors (n), each of which can be read or written.
- Sectors are numbered from 0 to n-1 - the address space of the drive
- A single sector write is atomic
- Accessing blocks that are near to eachother is faster than accessing blocks which are far.

## Basic Geometry

- Platter - circular surface on which data is stored persistently
- Spindle - connects platters and the motor which spins the disk at a constant rate
- Data is encoded on each surface in concentric circles of sectors; we call one such concentric circle a track
- Disk head - reads and writes on the platter, one per surface
- Disk arm - moves across the surface to position the head over the desired track.

## Simple disk drive

- Rotational delay - the time it takes for the desired sector to rotate under the disk head.
- Seek - move the disk arm to the correct track
- Transfer - reading/writing data

## Disk Scheduling

- Disk scheduler can approximate the time it takes for a read or write to complete
- It will follow the Shortest Job First Approach (SJF)
- Shortest Seek Time First - pick the job that writes to the closest track from the track the disk arm is currently on. However, disk geometry not available to the OS, so it approximates with the nearest-block-first. Starvation is a big problem - nearer blocks will always have priority
- Elevator (SCAN) - moves back and forth across the disk servicing request in order across the tracks. et’s call a single pass across the disk (from outer to inner tracks, or inner to outer) a sweep. Thus, if a request comes for a block on a track that has already been serviced on this sweep of the disk, it is not handled immediately, but rather queued until the next sweep (in the other direction). Similar to how an elevator might work. Doesn't take rotation into account.
- Shortest Positioning Time First - takes into account both seek time and rotation time - even harder to implement, usually performed inside a drive.
- The OS usually picks what it thinks are the best few requests  and issues them all to disk, the disk then uses its internal knowledge of head position and detailed track layoutq information to service said requests in the best possible order.
- I/O merging - merge the requests of subsequent blocks - important at OS level to reduce overhead
- Work-conserving - immediately issue the request to the drive, the disk will never idle if there are requests to serve
- non-work-conserving - wait a bit so that a new and "better" request may arrive at the disk, so that the overall efficiency is increased.