[![build](https://img.shields.io/travis/seekaddo/BroadcastMesg.svg)](https://travis-ci.org/seekaddo/BroadcastMesg)
<a href="https://scan.coverity.com/projects/seekaddo-broadcastmesg">
  <img alt="Coverity Scan Build Status"
       src="https://img.shields.io/coverity/scan/14413.svg"/>
</a>

## BroadcastMesg[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/seekaddo/BroadcastMesg/blob/master/LICENSE)
Using Semaphores and Shared memory to simulate Server/Client message(sender/receiver)




Building

```bash
git clone https://github.com/seekaddo/BroadcastMesg.git
cd BroadcastMesg
make all

```

Testing

```bash
make test

```


Usage

```bash
./sender -m <buffer_size> < data
./empfaenger -m <buffer_size>

```


Clean up after Usage

```bash
make clean
make freeshm   // clear all sharedmemory and semaphores created
```


Helpful resources

http://man7.org/tlpi/index.html


