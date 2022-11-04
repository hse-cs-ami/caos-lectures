# parallel-2

### RMW

x86:

```c
[lock](https://www.felixcloutier.com/x86/lock) add/sub/xor/...

xchg %r32, m32  //  exchange; "lock" implied

lock xadd %r32, m32  //  { tmp = m32; m32 += r32; r32 = tmp; }

lock cmpxchg %r32, m32  // CAS — compare and swap
// Compare %eax with m32.
// If equal, set ZF and load r32 into m32.
// Else, clear ZF and load m32 into %eax.
```

C11:

```c
#include <stdatomic.h>

_Bool atomic_compare_exchange_strong(volatile A *object, C *expected, C desired);
_Bool atomic_compare_exchange_weak(volatile A *object, C *expected, C desired);
/*
Atomically, compares the value pointed to by object for equality
with that in expected, and if true, replaces the value pointed to by object with
desired, and if false, updates the value in expected with the value pointed to by
object.
Returns the result of the comparison.
*/

if (weak && !x86) {
	sometimes {
		return false;  // weak-версия иногда фейлится просто так
    // load-linked + store-conditional — напомните рассказать, если время будет
	}
}
atomically {
  bool equal = (*object == *expected);
  *expected = *object;
  if (equal) {
		*object = desired;
	}
	return equal;
}
```

C++: [cppreference](https://en.cppreference.com/w/cpp/atomic).

### Критические секции (мьютексы)

Бывают вещи, которые нужно сделать атомарно, а готовой atomic/RMW операции для этого нет:

```c
struct list {
	int val;
  struct list* next;
}

void push(struct list** head, int value) {
	struct list *node = calloc(1, sizeof(*node));
  node->val = value;
	spin_lock(lock);
  {
    node->next = *head;
    *head = node;
  }
  spin_unlock(lock);
}

/*
Thread 0         Thread 1
*head = node;
lock = 0;  ----> lock == 0
                 node->next = *head;
*/
```

Mutex — от MUTual EXclusion.

Перед тем, как построить мьютекс, построим спинлок.

### Spinlock

```c
int spinlock;
// 0 — unlocked
// 1 — locked
```

Будем пользоваться примитивом Compare-And-Swap:

```c
cmpxchg(var, expected, desired) {
	atomically {
		old = var;
		if (var == expected) {
			var = desired;
		}
		return old;
	}
}
```

Спинлок:

```c
typedef _Atomic int spinlock;

// Неэффективный, но работающий спинлок
void spin_lock(spinlock *s) {
	while (cmpxchg(*s, 0, 1) != 0) { asm volatile ("pause"); }
}

void spin_unlock(spinlock *s) {
	*s = 0;
}
```

Спинлок отлично работает, когда защищает пару инструкций, но бесконечно тратит ресурсы, если нужно подождать подольше. На этот случай нам нужен способ остановиться и подождать, а с этим нам может помочь только ядро.

### Фьютексы

Примитив синхронизации в ядре Linux ([википедия](https://en.wikipedia.org/wiki/Futex#Operations)):

```c
FUTEX_WAIT(addr, val)
If the value stored at the address *addr* is *val*, puts the current thread to sleep.

FUTEX_WAKE(addr, num)
Wakes up *num* threads waiting on the address *addr*.
```

(На любой платформе addr — адрес 32-битного значения.)

(Fast Userspace muTual EXclusion.)

Теперь можно сделать себе (простенький, не production ready) мьютекс:

```c
struct mutex {
	_Atomic uint32_t lock;
	_Atomic uint32_t waiters;
};

void mutex_lock(struct mutex *m) {
	while (cmpxchg(m->lock, 0, 1) != 0) {
		m->waiters++;
		futex_wait(&m->lock, 1);
		m->waiters--;
	}
}

void mutex_unlock(struct mutex *m) {
	m->lock = 0;
  if (m->waiters > 0) {
		futex_wake(&m->lock, 1);
	}
}

struct object {
	struct mutex m;
	int refcount;
}
```

Настоящий мьютекс занимает одно 32-битное значение и немножко ждёт как спинлок в надежде, что не придётся делать futex_wait, но в целом устроен так же.

Сравнить реализации: [мьютекс из musl](https://git.musl-libc.org/cgit/musl/tree/src/thread/pthread_mutex_timedlock.c?h=v1.1.15), [статья Дреппера про фьютексы](https://github.com/tpn/pdfs/blob/master/Futexes%20Are%20Tricky%20-%20Ulrich%20Drepper%20(2011).pdf).

Contention («конкуренция»?). Оптимизируем uncontended case.

### Ну, теперь можно и треды создавать

Threads (нити, треды, легковесные процессы) — потоки исполнения в общем адресном пространстве.

Как сделать себе такую штуку в Linux: [https://man7.org/linux/man-pages/man2/clone.2.html](https://man7.org/linux/man-pages/man2/clone.2.html) (читать про CLONE_THREAD).

[man pthreads](https://man7.org/linux/man-pages/man7/pthreads.7.html)

[pthread_create в musl](https://github.com/cloudius-systems/musl/blob/00733dd1cf791d13ff6155509cf139a5f7b2eecb/src/thread/pthread_create.c)

Thread safety стандартной библиотеки языка Си: [https://man7.org/linux/man-pages/man7/attributes.7.html](https://man7.org/linux/man-pages/man7/attributes.7.html) ([пример про printf](https://man7.org/linux/man-pages/man3/printf.3.html#ATTRIBUTES)).