# Data Integrity and Protection

How should systems ensure that the data written to storage is protected? What techniques are required? How can such techniques be made efficient, with both low space and time overheads?

## Disk failure modes

- Latent-sector errors - disk sector has been damaged in some way
- Error correcting code (ECC) - determine whether the on-disk bits in a block are good and in some cases can be fixed
- Block corruption - disk block becomes corrupt in a way not detectable by the disk itself. Silent failures.
- Fail-partial - the whole disk can fail, there could be latent-sector errors or the blocks could be corrupt

## Handling LSEs

- Easily fixed with redundancy
- In parity based RAID-4/5 systems, a problem arises when full disk fault and LSEs occur in tandem. When trying to reconstruct a disk, if we wind up on an LSE, we can no longer reconstruct the data.

## Detecting Corrpution: The Checksum

- Checksum - the result of a function that takes a chunk of data as input and computes a function over said data, producig a small summary of the contents of the data.
- XOR - fast, but if two bits in the same position within each cheksummed unit change, the cheksum will not detect the corruption.
- Addition - fast, but not good if data is shifted.
- Fletcher checksum - two check bytes, s1 and s2. Specifically, assume a block D consists of bytes d1 ... dn; s1 is defined as follows: s1 = (s1 + di) mod 255 (computed over all di); s2 in turn is: s2 = (s2 + s1) mod 255 (again over all di) [F04]. The Fletcher checksum is almost as strong as the CRC (see below), detecting all single-bit, double-bit errors, and many burst errors
- Cyclic redundancy check (CRC) - Assume you wish to compute the checksum over a data block D. All you do is treat D as if it is a large binary number (it is just a string of bits after all) and divide it by an agreed upon value (k). The remainder of this division is the value of the CRC. As it turns out, one can implement this binary modulo operation rather efficiently, and hence the popularity of the CRC in networking as well.
- Collision - two data blocks with non-identical contents that have identical checksums.

## Misdirected Writes

- Disk writes the data correctly, but in the wrong location.
- Physical identifier - store the information of the sector and number block which you want to write the data to.

## Lost Writes

- The device informs the upper layer that a write has completed, but in fact it never is persisted
- Write veridy (read-after-write) - immediately read back the data after a write and compare it to the data you wanted to persist - very slow
- Add a checksum elsewhere in the system, so you can compare the checksum from the disk and in the system. Now, only if both writes fail we have a lost write

## Scrubbing

- Periodically read through every block of the system and check whether checksums are still valid.

## Overheads of Checksumming

- Space - each stored checksum takes up room on the disk and in-memory overhead to be able to check the checksums
- Time - the CPU must compute the checksum over each block both when the data is stored and when it is accessed; this can be reduced by combining data copying and checksumming (e.g when copying data from the kernel page cache into a user buffer). Some checksumming schemes can introduce extra I/O overhead when the checksum is stored distinctly from the data and for background scrubbing.