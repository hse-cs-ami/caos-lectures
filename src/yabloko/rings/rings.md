# Кольца защиты

На x86 уровни привилегированности исполняемого кода
называются «кольцами защиты». Их 4 штуки.

В Unix-подобных ОС на x86 используются только два кольца: ядро исполняется в
кольце 0 (наиболее привилегированном), а пользовательские программы (userspace)
— в кольце 3.

**Привилегии колец:**

**Ring 0:**

- Доступ к железу (port mapped IO, memory mapped IO)
- Служебные регистры ([GDTR, IDTR](https://wiki.osdev.org/CPU_Registers_x86#GDTR), [контрольные регистры](https://wiki.osdev.org/CPU_Registers_x86#Control_Registers))
- Служебные инструкции (lgdt, lidt, cli/sti, …)

**Ring 3:**

- Доступ к инструкциям/регистрам общего назначения (полностью самостоятельно
можно только пользоваться доступной памятью и что-то считать, за всем остальным
придется звать ядро).

Актуальный уровень привилегий — CPL (current privilege level) —
хранится в младших двух битах регистра `cs`.

Вообще, селектор сегмента (содержимое сегментного регистра)
всегда устроен подобным образом:
```
┌───────────────────────────┬──┬───────┐
│  Segment descriptor index │  │   PL  │
└───────────────────────────┴──┴───────┘
         13 bits             1b  2 bits
```

Старшие 13 бит — номер сегмента в таблице дескрипторов
(мы используем только одну таблицу дескрипторов — GDT),
младшие 2 бита — уровень привилегий.

В дескрипторе сегмента тоже записан уровень привилегий —
DPL (D = descriptor). Если вы пытаетесь загрузить в сегментный
регистр новый селектор, его PL называется RPL (R = requested).
Загрузить новый селектор получится, если `max(CPL, RPL) <= DPL`.
(Таким образом, вы не можете просто загрузить в `cs` селектор кода ядра
и повысить себе привилегии.)

## GDT в Yabloko

При запуске Яблока создаются 4 базовых сегмента:

```c
#define USER 3
#define KERNEL 0

// SEG(EFLAGS, base, limit, privilege level) - запись сегмента в GDT

void init_seg_desc(void) {
	seg_desc[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, KERNEL) 
	seg_desc[SEG_KDATA] = SEG(STA_W,       0, 0xffffffff, KERNEL)
	seg_desc[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, USER)
	seg_desc[SEG_UDATA] = SEG(STA_W,       0, 0xffffffff, USER)
}
```

### Обработка прерываний

    При возникновении прерывания в кольце 3 (аппаратного или программного),
    процессору надо повысить свой уровень привилегий, обработать прерывание, а
    после понизить его обратно. 

При этом мы не можем рассчитывать, что пользовательский код поддерживает
осмысленное значение регистра `esp`, чтобы нам было куда сохранить регистры
при обрабоке прерывания. Нам придётся сделать отдельный стек в
kernelspace и пользоваться им.
    
Псевдокод обработки прерывания:

```c
vector X; ←- наш вектор прерываний
gate = idt[X]; <-- берём gate из таблицы прерываний по этому вектору
selector = gate.selector;
dpl = selector.pl; <-- уровень привилегий на котором должен быть обработан X

if cpl > dpl { // cpl - current privilege level
	switch_stack();
	change_privilege_level();
}

// обработка прерывания
```

Структура стека в момент начала обработки прерывания (`cpu/isr.h`):
```
    uint32_t eip, cs, eflags;  // Pushed by the processor automatically
    uint32_t useresp, ss;      // Pushed by the processor for userspace interrupts
```

Инструкция `iret` снимает со стека восстанавливаемое значение `cs`,
и если при этом понижается уровень привилегий, то восстанавливает со стека
также `esp` и `ss`.

### Обработка прерываний в Яблоке: 

```c
void trap(registers_t *r) {
	if(r->int_no == T_SYSCALL) { // отдельная проверка на системный вызов - частый частный случай прерывания
		switch(r->eax) {
		
		case SYS_exit:
			if(r->ebx) {
				kprinf("Success\n");
			} else { 
				kprintf("Program finished with exit code %d\n", r->ebx);
			}
			killproc(); // переключение стека без сохранения текущего состояния

		case SYS_greet:
			kprintf("Hello world!\n");
			r->eax = 0;
			break;
		
		default:
			kprintf("Unknown syscall);
			r->eax = -1;
			break;
		}
	}
}
```

### Системный вызов

Инструкция `int $0x84` (Yabloko-specific, int $0x80 для Linux/i368) - обычное прерывание

%eax ← function

%ebx ← arg1

%ecx ← arg2

%edx ← arg3


### Страничная виртуальная память

Страничная виртуальная память - отображение каждого “блока” памяти определенного размера на физическую память внутри процессора

![Screenshot 2023-02-05 193419.png](screenshot.png)

- У каждого процесса свое отображение
- Стандартный размер страницы - 4KiB
- Первые 10 бит адреса - индекс page directory
- Вторые 10 бит - индекс в page table
- Оставшиеся 12 бит - смещение внутри страницы памяти
- Процессор может положить отображение в свой кэш(TLB - translation lookaside buffer) →  отображение 20-битного префикса адреса на физический адрес памяти. Сбрасывается после переключения процесса
