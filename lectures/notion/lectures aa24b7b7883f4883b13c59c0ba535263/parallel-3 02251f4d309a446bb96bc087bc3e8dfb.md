# parallel-3

### Некоторые известные задачи

Dining philosophers: [https://en.wikipedia.org/wiki/Dining_philosophers_problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)

RWLock:

[https://www.daemon-systems.org/man/pthread_rwlock.3.html](https://www.daemon-systems.org/man/pthread_rwlock.3.html)

[https://eli.thegreenplace.net/2019/implementing-reader-writer-locks/](https://eli.thegreenplace.net/2019/implementing-reader-writer-locks/)

[https://github.com/catboost/catboost/blob/master/util/system/rwlock.cpp](https://github.com/catboost/catboost/blob/master/util/system/rwlock.cpp)

Producer-consumer queue:

[https://github.com/catboost/catboost/blob/master/util/thread/pool.cpp#L111](https://github.com/catboost/catboost/blob/master/util/thread/pool.cpp#L111)

### Атомарность и кэши

```c
lock add $1, x

          Shared mem
          L3 Unified
---------------------------------
Cache 1     Cache 2      Cache 3
CPU         CPU          CPU

CPU0  // lock incb 0x40403
0x40400 S [ 0003000000000 ] L
0x30280 S [ 0000000000000 ]

CPU1
0x30280 S [ 0000000000000 ]
0x40400 M [ 0003000000000 ]

ARM (RISC)
Load/store architecture

LL r8, x
add r8, 1
SC x, r8

compare_exchange_strong()
compare_exchange_weak()

struct data {
	_Atomic int x;
  char padding[64];
  _Atomic int y;
}

// thread 1
mov $1, x
mfence // flush write
mov y, %eax

// thread 2
mov $1, y
mfence
mov x, %eax
```

[https://en.wikipedia.org/wiki/MESI_protocol](https://en.wikipedia.org/wiki/MESI_protocol)

[https://en.wikipedia.org/wiki/Load-link/store-conditional](https://en.wikipedia.org/wiki/Load-link/store-conditional)

[https://en.wikipedia.org/wiki/ABA_problem](https://en.wikipedia.org/wiki/ABA_problem)