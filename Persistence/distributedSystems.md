# Distributed Systems
How can we build a working system out of parts that don’t work correctly all the time? The basic question should remind you of some of the topics we discussed in RAID storage arrays; however, the problems here tend to be more complex, as are the solutions.

## Communication Basics

- Fundamentally unreliable - packet loss or corruption

## Unreliable Communication Layers

- Some applications know how to deal with packet loss, therefore, we let them communicate over an unreliable messaging layer
- UDP - doesn't guard against packet loss, but guards against corruption by using a checksum

## Reliable Communication Layers

- The sender knows that a receiver has received a message when it receives an acknowledgment (ack) from the receiver. If it has not received an ack by some time (timeout) it retries the send.
- Sometimes, the receiver will get the message, but the ack will fail the transmission, so we need a way for the receiver to know when it gets a duplicate message, so it won't pass it to the application but will retry the ack.
- TCP

## Communication Abstractions

- Distributed shared memory (DSM) - enable processes on different machines to share a large virtual address space. DSM doesn't handle failure well and is very slow

## Remote Procedure Call (RPC)

- Make the process of executing code on a remote machine as simple and straightfoward as calling a local function.

### Stub Generator
- Remove the pain of packing function arguments and results into messages by automating it. It take some interface and generates a few different pieces of code. For the client, a client stub is generated, which contains each of the cuntions specified in the interface, a client program wishing to use this RPC service would link with this client stub and call into it in order to make RPCs.
- For the client, the process is as follows: create a message buffer, pack the needed information into the message buffer, send the message to the destination RPC server, wait for the reply, unpack reeturn code and other arguments and return to the caller.
- For the server: unpack the message, call into the actual function, package the results and send the reply
- How does one package and send a complex data structre?
- Concurrency - handle multiple RPCs concurrently

### Run-Time Library

- How to locate a remote service? Simplest: hostnames and portnumbers, the client most know the hostnames or IP address of the machine running the desired RPC service, as well as the prot number it is using. The protocol suite must then provide a mechanism to route packets to a prticular address from any other machine in the system.
- Which transport layer protocol to use? Using a reliable protocol will cause a lot of inefficiencies. Therefore, the RPC uses UDP and then handles failures in a similar fashion to TCP.

### Other Issues

- What happens if a remote call takes a long time to complete? Use explicit ack when the reply isn't immediately generated, then the client can poll the server to see if it's still working on the RPC.
- Procedure calls with large arguments, larger than what can fit into a single packet.
- Byte ordering - little endian vs big endian, usually specified in the RPC packet
- Should we expose the asynchronous nature of the RPC?